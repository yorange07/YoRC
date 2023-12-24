// Server.h
#pragma once
#include "User.h"


#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <vector>
#include <stdexcept>
#include "Message.h"
#include <mysql.h>





class Server {
public:

    std::string LOGIN_REQUEST = "1";//Поля: логин|пароль
    std::string REGISTRATION_REQUEST = "2";//Поля: логин|пароль|имя
    std::string JUST_MESSAGE_REQUEST = "3";//Поля: от кого|кому|текст
    std::string SHOW_USERS_LIST_REQUEST = "4";
    std::string SHOW_MESSAGE_HISTORY_REQUEST = "5";
    std::string SIGNAL_REQUEST = "6";
    std::string RIGHT_REG_CODE = "001";
    std::string BAD_REG_CODE = "000";
    std::string RIGHT_LOG_CODE = "101";
    std::string BAD_LOG_CODE = "100";

    Server();
    ~Server();

    void start(int port);
    void stop();
    void startServer(int port);
    


private:
    void sendMessageToClient(int clientSocket, Message& _message);
    std::string receiveMessageFromClient(int clientSocket);
    void sendMessageToAllClients(Message& _message);
    void processClientMessage(int clientSocket, const std::string& message);

    void loginClient(int clientSocket, const std::string& data);
    void logoutClient(int clientSocket);
    void regClient(int clientSocket, const std::string& data);
    void addMessage(int clientSocket, const std::string& data);
    void showUsers(int clientSocket, const std::string& data);
    void showHistory(int clientSocket, const std::string& data);

    std::vector<User> _userList;//Список пользователей в текущей сессии чата
    std::vector<Message> _messageBank;//Все сообшения сессии чата
    std::vector<User>& getAllUsers() { return _userList; };//Возврат списка пользователей
    std::vector<Message>& getAllMessages() { return _messageBank; };//Возврат всех сообщений сессии
    int getUserIdBySocket(int clientSocket);//Возврат Id пользователя по логину
    int getUserSocketById(int id);//Возврат сокета пользователя по логину
    std::string getUserNameById(int id);//Возврат имени пользователя по id

    int serverSocket = -1;
    std::vector<int> clientSockets;
    std::mutex clientSocketsMutex;
    bool stopServer;

    
    void waitForClient();
    void handleClient(int clientSocket);

    MYSQL conn;
    void initDatabaseConnection();
};