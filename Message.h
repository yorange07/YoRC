//���� ����� ��������� ���������
#pragma once

#include <string>
#include <vector>




class Message
{
public:

    //���������� 
    Message(std::string type, const std::vector<std::string>& data) :_type(type), _data(data) {};



    //������� ���������� ���������
    std::string getType() { return _type; };
    const std::vector<std::string>& getData() const {return _data;};

private:

    //��������� ���������
    std::string _type;
    std::vector<std::string> _data;

};

