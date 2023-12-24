//���� ����� ��������� ������������
#pragma once

#include <string>

class User
{
    //��������� ������������
    int _id;//id ������������
    std::string _login;//�����
    std::string _password;//������
    std::string _username;//��� ������������
    int _socket;//����� ������������ � ������� ������


public:

    //����������� 
    User(int id, std::string& login, std::string& password, std::string& username, int socket) :_id(id), _login(login), _password(password), _username(username), _socket(socket) {};
    
    //������� ���������� ������������
    int& getUserId() { return _id;};
    std::string& getUserLogin() {return _login;};
    std::string& getUserPassword() {return _password;};
    std::string& getUserName() {return _username;};
    int getUserSocket() { return _socket; };

    //������� ���������� ������������
    void setUserId(int id) { _id = id;};
    void setUserLogin(std::string& login) { _login = login; };
    void setUserPassword(std::string& password) {_password = password;};
    void setUserName(std::string& name) {_username = name;};
    void setUserId(int& socket) { _socket = socket; };
};