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

    std::string LOGIN_REQUEST = "1";//Ïîëÿ: ëîãèí|ïàðîëü
    std::string REGISTRATION_REQUEST = "2";//Ïîëÿ: ëîãèí|ïàðîëü|èìÿ
    std::string JUST_MESSAGE_REQUEST = "3";//Ïîëÿ: êîìó|òåêñò
    std::string SHOW_USERS_LIST_REQUEST = "4";//Ïîëÿ: all/online|
    std::string SHOW_MESSAGE_HISTORY_REQUEST = "5";//Ïîëÿ: ñ êåì èñòîðèÿ|
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
    bool _chatActivation = false;//Ñîñòîÿíèå ÷àòà
    bool getChatActivation() const { return _chatActivation; };//Âîçâðàò ñîñòîÿíèÿ ÷àòà 
    void setChatActivation(bool flag) { _chatActivation = flag; };//Óñòàíîâêà ñîñòîÿíèÿ ÷àòà


    void setCurrentUser(User& currentUser) { *_currentUser = currentUser;  };

    void sendMessage(Message& _message);
    void sendMessageToAddress();
    std::string receiveMessage();
    void login();//Âõîä ïîëüçîâàòåëÿ
    void logOut();//Âûõîä èç àêêàóíòà 
    void signUp();//Ðåãèñòðàöèÿ ïîëüçîâàòåëÿ
    void showChat();//Ïîêàçàòü ÷àò
    void showAllUsersList();//Ïîêàçàòü ñïèñîê âñåõ ïîëüçîâàòåëåé
    void showOnlineUsersList();//Ïîêàçàòü ñïèñîê âñåõ ïîëüçîâàòåëåé
    void showLoginMenu();//Âõîä è ìåíþ
    void showUserMenu();
    void connectToServer(const std::string& ipAddress, int port);
    int clientSocket = -1;

    void receiveLoop();
    void processServerMessage(std::string receivedMessage);
};

