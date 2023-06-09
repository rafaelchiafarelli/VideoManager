#include "container.hpp"
#include <iostream>
#include <fstream>
bool container::parser(std::string filename)
{
bool ret = false;
    /*
    if file exists try parse

    */
    std::ifstream f(filename);
    if(f.good())
    {
        try
        {
            raw_data = json::parse(f);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }

        if(raw_data.contains("window"))
        {
            if(raw_data["window"].type() == json::value_t::array)
            {
                if(raw_data["window"].size() == 2)
                {
                    if(raw_data["window"][0].type() == json::value_t::number_unsigned)
                        app.window_x = raw_data["window"][0];
                    if(raw_data["window"][1].type() == json::value_t::number_unsigned)                        
                        app.window_y = raw_data["window"][1];
                }
            }
        }
        if(raw_data.contains("window_number"))
        {
            if(raw_data["window_number"].type() == json::value_t::number_unsigned)
                {
                    app.window_number = raw_data["window_number"];
                }
        }
        if(raw_data.contains("step_ms"))
        {
            if(raw_data["step_ms"].type() == json::value_t::number_unsigned)
            {
                app.step_ms = raw_data["step_ms"];
            }
        }
        if(raw_data.contains("ondemand_max"))
        {
            if(raw_data["ondemand_max"].type() == json::value_t::number_unsigned)
            {
                app.ondemand_max = raw_data["ondemand_max"];
            }
        }
        if(raw_data.contains("sequences"))
        {
            if(raw_data["sequences"].type() == json::value_t::array)
            {
                if(raw_data["sequences"].size() > 0)
                {
                    PlayImage tmp; //used to load the images
                    for(auto &sequence: raw_data["sequences"])
                    {
                        tmp.clear();
                        if(sequence.contains("name"))
                        {    if(sequence["name"].type() == json::value_t::string)
                            {
                                tmp.name = sequence["name"];
                            }
                            else
                            {
                                std::cout<<"error: name not string"<<std::endl;
                                continue;
                            }
                        }
                        else
                        {
                            std::cout<<"error: name not in sequence"<<std::endl;
                            continue;
                        }

                        if(sequence.contains("repeat"))
                        {
                            if(sequence["repeat"].type()==json::value_t::number_unsigned)
                            {
                                tmp.repeat = sequence["repeat"];
                            }
                            else
                            {
                                tmp.repeat = 0;
                            }
                        }
                        else
                        {
                            std::cout<<"error pos does not contains y coordinates"<<std::endl;
                            continue;
                        }

                        if(sequence.contains("type"))
                        {
                            if(sequence["type"].type() == json::value_t::string)
                            {
                                if(std::string("permanent").compare(sequence["type"]) == 0)
                                {
                                    tmp.type = TypeOfImage::permanent;
                                }
                                else if(std::string("ondemand").compare(sequence["type"]) == 0)
                                {
                                    tmp.type = TypeOfImage::ondemand;
                                }
                                else
                                {
                                    std::cout<<"error: type:"<<sequence["type"]<<" invalid"<<std::endl;
                                    continue;
                                }
                            }
                        }
                        else
                        {
                            std::cout<<"error: type not recognized"<<std::endl;
                            continue;
                        }
                        if(sequence.contains("pos"))
                        {
                            if(sequence["pos"].type() == json::value_t::object)
                            {
                                if(sequence["pos"].contains("x"))
                                {
                                    if(sequence["pos"]["x"].type()==json::value_t::number_unsigned)
                                    {
                                        tmp.x = sequence["pos"]["x"];
                                    }
                                    else
                                    {
                                        std::cout<<"error X is not integer:"<<sequence["pos"]["x"]<<std::endl;
                                        continue;
                                    }
                                }
                                else
                                {
                                    std::cout<<"error pos does not contains X coordinates"<<std::endl;
                                    continue;
                                }
                                if(sequence["pos"].contains("y"))
                                {
                                    if(sequence["pos"]["y"].type()==json::value_t::number_unsigned)
                                    {
                                        tmp.y = sequence["pos"]["y"];
                                    }
                                    else
                                    {
                                        std::cout<<"error Y is not integer"<<std::endl;
                                        continue;
                                    }
                                }
                                else
                                {
                                    std::cout<<"error pos does not contains y coordinates"<<std::endl;
                                    continue;
                                }
                                if(sequence["pos"].contains("h"))
                                {                                                                    
                                    if(sequence["pos"]["h"].type()==json::value_t::number_unsigned)
                                    {
                                        tmp.h = sequence["pos"]["h"];
                                    }
                                    else
                                    {
                                        std::cout<<"error H is not integer"<<std::endl;
                                        continue;
                                    }
                                }
                                else
                                {
                                    std::cout<<"error pos does not contains h coordinates"<<std::endl;
                                    continue;
                                }
                                if(sequence["pos"].contains("w"))
                                {                                                                    
                                    if(sequence["pos"]["w"].type()==json::value_t::number_unsigned)
                                    {
                                        tmp.w = sequence["pos"]["w"];
                                    }
                                    else
                                    {
                                        std::cout<<"error W is not integer"<<std::endl;
                                        continue;
                                    }
                                }
                                else
                                {
                                    std::cout<<"error pos does not contains w coordinates"<<std::endl;
                                    continue;
                                }
                            }
                            else
                            {
                                std::cout<<"error pos not found"<<std::endl;
                                continue;
                            }
                        }
                        else
                        {
                            std::cout<<"error does not contains pos"<<std::endl;
                            continue;
                        }
                        if(sequence.contains("names"))
                        {
                            if(sequence["names"].type() == json::value_t::array)
                            {
                                for(auto &f_name: sequence["names"])
                                {
                                    if(f_name.type() == json::value_t::string)
                                    {
                                        tmp.names.push_back(f_name);
                                    }
                                    else
                                    {
                                        std::cout<<"error: file name is not string"<<std::endl;
                                    }
                                }
                            }
                        }
                        else
                        {
                            std::cout<<"error: sequence does not contains files names"<<std::endl;
                        }
                        if(sequence.contains("last_one"))
                        {
                            if(sequence["last_one"].type() == json::value_t::string)
                            {
                                tmp.last_one = sequence["last_one"];
                            }
                            else
                            {
                                std::cout<<"error: last_one not a string"<<std::endl;
                                continue;

                            }
                        }
                        else

                        {
                            std::cout<<"error: sequence does not contains last_one"<<std::endl;
                            continue;
                        }
                    //can load the images now
                    //copy the image to a map (name , PlayImage) and then start's loading the images
                    tmp.region = cv::Rect(tmp.x,tmp.y,tmp.w,tmp.h);
                    sequences.emplace(std::make_pair(tmp.name,tmp));                        
                    
                    }
                    ret = true;
                }
                else
                {
                    std::cout<<"error: sequence has 0 inputs"<<std::endl;
                }
            }
            else
            {
                std::cout<<"error: sequence is not an array"<<std::endl;
            }
        }
    }

return ret;
}


