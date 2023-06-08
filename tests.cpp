#include <iostream>
#include <fstream>
#include <json.hpp>
#include "player.hpp"
using namespace std;

using json = nlohmann::json;

void test_parser();
void test_player();
int main()
{
    test_parser();
    test_player();
    return 0;
}

void test_player(){
    Player P("./test_player.json");
}
void test_parser()
{
    std::ifstream f("./data.json");
    json data = json::parse(f);
    // Access the values existing in JSON data
    string name = data.value("name", "not found");
    string grade = data.value("grade", "not found");
    // Access a value that does not exist in the JSON data
    string email = data.value("email", "not found");
    // Print the values
    cout << "Name: " << name << endl;
    cout << "Grade: " << grade << endl;
    cout << "Email: " << email << endl;
}

