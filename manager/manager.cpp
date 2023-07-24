#include "manager.hpp"
#include <thread>
#include <crow_all.h>
#include <chrono>
#include <stdio.h>
#include "player.hpp"
#include <fstream>
#include <curl/curl.h>
using namespace cv;
using namespace std;
int main(int argc, char** argv )
{
    
    Player P("./test_player.json");
    std::thread register_to_server = std::thread(&register_func,this).detach();
    crow::SimpleApp app;

    CROW_ROUTE(app, "/")
    ([&]() {
        P.OnDemand("nome");
        return "Hello world!";
    });
    /*
    {
        "ondemand":name_of_sequence
    }
    
    */
    CROW_ROUTE(app, "/ondemand")
        .methods("POST"_method)
    ([&](const crow::request& req){
        auto raw = crow::json::load(req.body);
        if (!raw)
            return crow::response(400);
        
        if(raw.has("ondemand"))
        {
            if(raw["ondemand"].t() == crow::json::type::String)
            {
                std::string name = raw["ondemand"].s();
                if(!P.OnDemand(name)){
                    return crow::response(404);
                }
            }
        }
        return crow::response{"loaded"};
    });

    CROW_ROUTE(app, "/json_store")
        .methods("POST"_method)
    ([&](const crow::request& req){
        auto raw = crow::json::load(req.body);
        if (!raw)
            return crow::response(400);
        ofstream file1("./test_player.json");
        file1 << raw << endl;
        file1.close();                
        alive = false;
        return crow::response{"loaded"};
    });

    P.Start();
    int count =0;
    app.port(18080).run();
    alive = false;
    P.Stop();
    std::this_thread::sleep_for (std::chrono::seconds(1));
    return 0;
}
void register_func(Player P){

    CURLcode ret;
    CURL *hnd;
    struct curl_slist *slist1;    
    while(alive)
    {
        //curl -X POST https://reqbin.com/echo/post/json -H 'Content-Type: application/json' -d '{"login":"my_login","password":"my_password"}' --libcurl post_json.c


        slist1 = NULL;
        slist1 = curl_slist_append(slist1, "Content-Type: application/json");

        hnd = curl_easy_init();
        curl_easy_setopt(hnd, CURLOPT_BUFFERSIZE, 102400L);
        curl_easy_setopt(hnd, CURLOPT_URL, "https://192.168.1.104:5000/device_register");
        curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 1L);
        std::string register_json = "{\"character\":\"";
        register_json += character_name;
        register_json+="\",\"IP\":\"";
        register_json+=IP_Address;
        register_json+="\",\"PORT\":\"18080\",\"type\":\"magistrade\",\"endpoints\":[\"ondemand\",\"json_store\"],\"animations\":[";
        int i =0;
        for(std::string name : P.OnDemandListName)
        {
            register_json+="\"";
            register_json+=name;
            if(i < P.OnDemandListName.size()-1)
            {
                register_json+="\",";
            }
            else
            {
                register_json+="\"";
            }
            i++;
        }
        register_json+="]}";


        curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, register_json);
        curl_easy_setopt(hnd, CURLOPT_POSTFIELDSIZE_LARGE, (curl_off_t)45);
        curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);
        curl_easy_setopt(hnd, CURLOPT_USERAGENT, "curl/7.68.0");
        curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
        curl_easy_setopt(hnd, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_2TLS);
        curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "POST");
        curl_easy_setopt(hnd, CURLOPT_FTP_SKIP_PASV_IP, 1L);
        curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);


        ret = curl_easy_perform(hnd);
        printf(" Returned value is: %d", (int)ret);
        curl_easy_cleanup(hnd);
        hnd = NULL;
        curl_slist_free_all(slist1);
        slist1 = NULL;

        if((int)ret == 200)
        {
            break;
        }
        else
        {
            std::this_thread::sleep_for (std::chrono::seconds(1));
        }
    }
}
