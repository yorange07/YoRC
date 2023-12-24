// Client.h
#pragma once

#pragma comment(lib, "ws2_32.lib")
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <thread>
#include <string>
#include "Message.h"
#include "User.h"
#include <mutex>


class Client {
public:

    std::string LOGIN_REQUEST = "1";//Поля: логин|пароль
    std::string REGISTRATION_REQUEST = "2";//Поля: логин|пароль|имя
    std::string JUST_MESSAGE_REQUEST = "3";//Поля: кому|текст
    std::string SHOW_USERS_LIST_REQUEST = "4";//Поля: all/online|
    std::string SHOW_MESSAGE_HISTORY_REQUEST = "5";//Поля: с кем история|
    std::string SIGNAL_REQUEST = "6";
    std::string LOGOUT_REQUEST = "7";
    std::string RIGHT_CODE = "001";
    std::string BAD_CODE = "000";


    Client();
    ~Client();

    void startClient();
    void disconnect();

private:
    std::mutex registerMutex;
    std::condition_variable registerCondition;
    bool registerSuccess = false;
    bool regAttemptCompleted = false;

    std::mutex loginMutex;
    std::condition_variable loginCondition;
    bool loginSuccess = false;
    bool loginAttemptCompleted = false;

    std::mutex otherMessageMutex;
    std::condition_variable otherMessageCondition;
    bool otherMessageSuccess = false;
    bool otherAttemptCompleted = false;

    std::shared_ptr<User> _currentUser = nullptr;
    bool _activUser = false;
    bool _chatActivation = false;//Состояние чата
    bool getChatActivation() const { return _chatActivation; };//Возврат состояния чата 
    void setChatActivation(bool flag) { _chatActivation = flag; };//Установка состояния чата


    void setCurrentUser(User& currentUser) { *_currentUser = currentUser;  };

    void sendMessage(Message& _message);
    void sendMessageToAddress();
    std::string receiveMessage();
    void login();//Вход пользователя
    void logOut();//Выход из аккаунта 
    void signUp();//Регистрация пользователя
    void showChat();//Показать чат
    void showAllUsersList();//Показать список всех пользователей
    void showOnlineUsersList();//Показать список всех пользователей
    void showLoginMenu();//Вход и меню
    void showUserMenu();
    void connectToServer(const std::string& ipAddress, int port);
    int clientSocket = -1;

    void receiveLoop();
    void processServerMessage(std::string receivedMessage);
};

