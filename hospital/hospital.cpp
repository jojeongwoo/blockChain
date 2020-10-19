#include <eosio/eosio.hpp>


using namespace eosio;

class [[eosio::contract("hospital")]] hospital : public eosio::contract {

    public:

    hospital(name receiver, name code,  datastream<const char*> ds): contract(receiver, code, ds) {}
   
    // upsert action 정의 - 테이블에 정보를 등록하고 동일한 user가 할 경우 수정함
    [[eosio::action]]
    void upsert(name user, std::string doctor, std::string patient, std::string diseaseCode) {
        require_auth( user );
        hospital_index hospital( get_self(), get_first_receiver().value );
        auto iterator = hospital.find(user.value);
       
        //테이블에 새개체를 추가하는 메소드 emplace를 활용
        if(iterator == hospital.end()) {
            hospital.emplace(user, [&]( auto& row ) {
                row.key = user;
                row.doctor = doctor;
                row.patient = patient;
                row.diseaseCode = diseaseCode;
            });
        } else {
            //테이블에 개체를 수정하는 메소드 modify를 활용
            hospital.modify(iterator, user, [&]( auto& row ){
                row.key = user;
                row.doctor = doctor;
                row.patient = patient;
                row.diseaseCode = diseaseCode;
            });
        }
    }

    //erase action을 정의 - 테이블의 user가 같은 곳을 find 메소드로 찾아서 erase명령으로 지움
    [[eosio::action]]
    void eraser(name user) {
        require_auth(user);
        hospital_index hospital( get_self(), get_first_receiver().value);
        auto iterator = hospital.find(user.value);
        check(iterator != hospital.end(), "Record does not exist");
        hospital.erase(iterator);
    }
  
    //show action을 정의 - 테이블의 user를 찾아서 기록된 정보를 출력함
    [[eosio::action]]
    void show(name user){
        
        hospital_index hospital( get_self(), get_first_receiver().value);
        auto iterator = hospital.find(user.value);
        check(iterator != hospital.end(),"Record does not exist");
        hospital.modify(iterator, user, [&](auto& row){
        print("등록한 사용자 이름 : ", row.key, " " ,"진료 의사 이름 : ", row.doctor , " " ,  "환자 이름 : ", row.patient , " " , "질병 코드 : ", row.diseaseCode);
        });
    }


    [[eosio::action]]
    void pay(name user){
        hospital_index hospital( get_self(), get_first_receiver().value);
        auto iterator = hospital.find(user.value);
        check(iterator != hospital.end(),"Record does not exist");
        int cash;
        hospital.modify(iterator, user, [&](auto& row){
            if (row.diseaseCode == "headache")
            {
                cash = 10;
                print("환자 이름 : ",row.patient," ", "질병 코드 : ", row.diseaseCode, " " , "지불할 금액 : ",cash, "CASH") ;
            }else if (row.diseaseCode == "stomache")
            {
                cash = 20;
                print("환자 이름 : ",row.patient," ", "질병 코드 : ", row.diseaseCode, " " , "지불할 금액 : ",cash, "CASH");
            }else if (row.diseaseCode == "cancer")
            {
                cash = 100;
                print("환자 이름 : ",row.patient," ", "질병 코드 : ", row.diseaseCode, " " , "지불할 금액 : ",cash, "CASH");
            }else
            {
                print("질병 코드가 잘못 입력되었습니다.");
            }
        });
    }

    


    






    //person이라는 struct에 people이라는 테이블로 정보들을 저장할 구조를 생성
    private:
        struct [[eosio::table]] prescriptions {
            name key;
            std::string doctor;
            std::string patient;
            std::string diseaseCode;
            uint64_t primary_key() const { return key.value; }
        };
        using hospital_index = eosio::multi_index<"prescription"_n, prescriptions>;
};