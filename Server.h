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

    std::string LOGIN_REQUEST = "1";//����: �����|������
    std::string REGISTRATION_REQUEST = "2";//����: �����|������|���
    std::string JUST_MESSAGE_REQUEST = "3";//����: �� ����|����|�����
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

    std::vector<User> _userList;//������ ������������� � ������� ������ ����
    std::vector<Message> _messageBank;//��� ��������� ������ ����
    std::vector<User>& getAllUsers() { return _userList; };//������� ������ �������������
    std::vector<Message>& getAllMessages() { return _messageBank; };//������� ���� ��������� ������
    int getUserIdBySocket(int clientSocket);//������� Id ������������ �� ������
    int getUserSocketById(int id);//������� ������ ������������ �� ������
    std::string getUserNameById(int id);//������� ����� ������������ �� id

    int serverSocket = -1;
    std::vector<int> clientSockets;
    std::mutex clientSocketsMutex;
    bool stopServer;

    
    void waitForClient();
    void handleClient(int clientSocket);

    MYSQL conn;
    void initDatabaseConnection();
};