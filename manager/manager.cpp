#include "manager.hpp"
#include <thread>
#include <crow_all.h>
#include <chrono>
#include <stdio.h>
#include "player.hpp"
#include <fstream>
#include <curl/curl.h>
#include <vector>
using namespace cv;
using namespace std;

std::string character_name = "animus";
std::string IP_Address = "192.168.1.105";
std::atomic_bool alive;
void register_func(Player &P);
int main(int argc, char** argv )
{
    
    Player P("./test_player.json");
    alive = true;
    std::thread register_to_server = std::thread(&register_func, std::ref(P));
    crow::SimpleApp app;

    CROW_ROUTE(app, "/")
    ([&]() {

           return crow::response{"application working"};
    });

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
        std::cout<<"got here"<<std::endl;
        auto raw = crow::json::load(req.body);
        if (!raw)
            return crow::response(400);
        ofstream file1("./test_player.json");
        file1 << raw << endl;
        file1.close();                
        P.Restart(); 
        return crow::response{"loaded"};
    });
    CROW_ROUTE(app, "/json_get")
        .methods("GET"_method)
    ([&](const crow::request& req){
        std::cout<<"go get the json"<<std::endl;
            std::string register_json;
    register_json.reserve(1000);
    register_json.append(P.c.tostring());

        return crow::response{register_json};
    });
    P.Start();
    int count =0;
    app.port(P.c.app.PORT).run();
    alive = false;
    P.Stop();
    std::this_thread::sleep_for (std::chrono::seconds(1));
    return 0;
}
void register_func(Player &P){

    CURLcode ret;
    CURL *hnd;
    struct curl_slist *slist1;    
    std::cout<<"is it alive:"<<(int)alive<<std::endl;
    std::string register_json;
    register_json.reserve(1000);
    register_json.append(P.c.tostring());

    std::cout<<"json:"<<register_json.c_str()<<std::endl;
    while(alive)
    {
        //curl -X POST https://reqbin.com/echo/post/json -H 'Content-Type: application/json' -d '{"login":"my_login","password":"my_password"}' --libcurl post_json.c

        
        slist1 = NULL;
        slist1 = curl_slist_append(slist1, "Content-Type: application/json");

        hnd = curl_easy_init();
        curl_easy_setopt(hnd, CURLOPT_BUFFERSIZE, 102400L);
        curl_easy_setopt(hnd, CURLOPT_URL, "http://192.168.1.104/device_register");
        curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 1L);


        curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, register_json.c_str());
        curl_easy_setopt(hnd, CURLOPT_POSTFIELDSIZE_LARGE, (curl_off_t)register_json.size());
        curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);
        curl_easy_setopt(hnd, CURLOPT_USERAGENT, "curl/7.68.0");
        curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
        curl_easy_setopt(hnd, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_2TLS);
        curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "POST");
        curl_easy_setopt(hnd, CURLOPT_FTP_SKIP_PASV_IP, 1L);
        curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);


        ret = curl_easy_perform(hnd);
        std::cout<<" Returned value is:"<<(int)ret<<std::endl;
        curl_easy_cleanup(hnd);
        hnd = NULL;
        curl_slist_free_all(slist1);
        slist1 = NULL;

        if((int)ret == 0)
        {
            std::cout<<"manager did log to the server"<<std::endl;
            break;
        }
        else
        {
            std::cout<<"manager did not find the server try again in 1 sec"<<std::endl;
            std::this_thread::sleep_for (std::chrono::seconds(1));
        }
    }
}
