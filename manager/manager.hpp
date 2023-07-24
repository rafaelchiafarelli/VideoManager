#pragma once

#include <string>
#include <atomic>
#include <chrono>
std::string character_name = "animus";
std::string IP_Address = "192.168.1.106";
std::atomic_bool alive;
void register_func();