//Этот класс описывает сообщение
#pragma once

#include <string>
#include <vector>




class Message
{
public:

    //Констрктор 
    Message(std::string type, const std::vector<std::string>& data) :_type(type), _data(data) {};



    //Геттеры параметров сообщения
    std::string getType() { return _type; };
    const std::vector<std::string>& getData() const {return _data;};

private:

    //Параметры сообщения
    std::string _type;
    std::vector<std::string> _data;

};

