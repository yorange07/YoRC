//Этот класс описывает пользователя
#pragma once

#include <string>

class User
{
    //Параметры пользователя
    int _id;//id пользователя
    std::string _login;//Логин
    std::string _password;//Пароль
    std::string _username;//Имя пользователя
    int _socket;//Сокет пользователя в текущей сессии


public:

    //Конструктор 
    User(int id, std::string& login, std::string& password, std::string& username, int socket) :_id(id), _login(login), _password(password), _username(username), _socket(socket) {};
    
    //Геттеры параметров пользователя
    int& getUserId() { return _id;};
    std::string& getUserLogin() {return _login;};
    std::string& getUserPassword() {return _password;};
    std::string& getUserName() {return _username;};
    int getUserSocket() { return _socket; };

    //Сеттеры параметров пользователя
    void setUserId(int id) { _id = id;};
    void setUserLogin(std::string& login) { _login = login; };
    void setUserPassword(std::string& password) {_password = password;};
    void setUserName(std::string& name) {_username = name;};
    void setUserId(int& socket) { _socket = socket; };
};