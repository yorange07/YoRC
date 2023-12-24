// Server.cpp
#include "Server.h"

Server::Server() {
    stopServer = false;
}

Server::~Server() {
    stop();
}

void Server::initDatabaseConnection() {

    mysql_init(&conn);

    // �������� ����� MYSQL_OPT_SSL_MODE � ���������� �������� SSL_MODE_DISABLED
    const int ssl_mode = SSL_MODE_DISABLED;
    mysql_options(&conn, MYSQL_OPT_SSL_MODE, &ssl_mode);

    
    if (&conn == NULL) {
        std::cout << "MySQL initialization failed" << std::endl;
        exit(1);
    }
    if (!mysql_real_connect(&conn, "localhost", "root", "VovaL4604", "mydb", 0, NULL, 0)) {
        std::cerr << "Database connection failed: " << mysql_error(&conn) << std::endl;
        exit(1);
    }
}


void Server::start(int port) {
    initDatabaseConnection();
    startServer(port);

        while (!stopServer) {
            waitForClient();
        }

}

void Server::stop() {
    stopServer = true;
    closesocket(serverSocket);

    // ��������� ��� ���������� ������
    for (int clientSocket : clientSockets) {
        closesocket(clientSocket);
    }

    WSACleanup();
}

void Server::sendMessageToClient(int clientSocket, Message& _message) {
    std::string message = _message.getType() + "|";
    for (int i = 0; i < _message.getData().size(); i++) {
        message = message + _message.getData()[i] + "|";
    }
    send(clientSocket, message.c_str(), message.size(), 0);
}

std::string Server::receiveMessageFromClient(int clientSocket) {
    char buffer[1024];
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

    if (bytesRead > 0) {
        return std::string(buffer, bytesRead);
    }
    else {
        // � ������ ������ ��� ���� ���������� �������, ���������� ������ ������
        return std::string();
    }
}

void Server::sendMessageToAllClients(Message& _message) {
    std::lock_guard<std::mutex> lock(clientSocketsMutex);

    for (int clientSocket : clientSockets) {
        sendMessageToClient(clientSocket, _message);
    }
}

void Server::startServer(int port) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        throw std::runtime_error("Failed to initialize Winsock");
    }

    struct sockaddr_in serverAddr;
    int clientLen = sizeof(struct sockaddr_in);

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        throw std::runtime_error("Error creating socket");
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        throw std::runtime_error("Error binding");
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        throw std::runtime_error("Error listening");
    }

    waitForClient();
}

void Server::waitForClient() {
    struct sockaddr_in clientAddr;
    int clientLen = sizeof(clientAddr);

    std::cout << "Waiting for a client to connect..." << std::endl;

    int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);

    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Error accepting connection: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    std::lock_guard<std::mutex> lock(clientSocketsMutex);
    clientSockets.push_back(clientSocket);

    std::cout << "Client connected!" << std::endl;

    // ������� ����� ����� ��� ��������� �������
    std::thread clientThread([this, clientSocket]() {
        handleClient(clientSocket);
        });

    // ����������� �����, ����� �� ��� ����������� ����������
    clientThread.detach();
}

void Server::handleClient(int clientSocket) {
    while (true) {
        std::string receivedMessage = receiveMessageFromClient(clientSocket);

        // ���������, ���� �� ���������� ������� ��� ��������� ������
        if (receivedMessage.empty()) {
            std::cout << "Client disconnected or error occurred." << std::endl;
            break;
        }

        // ��������� ��������� � ����������� �� ��� ����
        processClientMessage(clientSocket, receivedMessage);
    }

    // �������� ����������� ������
    closesocket(clientSocket);
}

void Server::processClientMessage(int clientSocket, const std::string& message) {

    // ��������� ��� ��������� �� ������� �����������
    size_t delimiterPos = message.find('|');
    if (delimiterPos == std::string::npos) {
        // ������������ ���������, ����������� �� ������
        std::cerr << "Error: Invalid message format." << std::endl;
        return;
    }

    std::string receivedType = message.substr(0, delimiterPos);

    // ��������� ������ ����� �����������
    std::string dataAfterDelimiter = message.substr(delimiterPos + 1);

    // � ����������� �� ���� ��������� �������� ��������������� �������
    if (receivedType == "1") {
        loginClient(clientSocket, dataAfterDelimiter);
    }
    else if (receivedType == "2") {
        regClient(clientSocket, dataAfterDelimiter);
    }
    else if (receivedType == "3") {
        addMessage(clientSocket, dataAfterDelimiter);
    }
    else if (receivedType == "4") {
        showUsers(clientSocket, dataAfterDelimiter);
    }
    else if (receivedType == "5") {
        showHistory(clientSocket, dataAfterDelimiter);
    }
    else if (receivedType == "7") {
        logoutClient(clientSocket);
    }
    else {
        std::cerr << "Error: Unknown message type." << std::endl;
    }
}

void Server::loginClient(int clientSocket, const std::string& data) {
    size_t pos = 0;
    std::vector<std::string> parts;
    std::string token;
    std::string delimiter = "|";

    std::string dataCopy = data;
    while ((pos = dataCopy.find(delimiter)) != std::string::npos) {
        token = dataCopy.substr(0, pos);
        parts.push_back(token);
        dataCopy.erase(0, pos + delimiter.length());
    }

    if (parts.size() < 2) {
        std::cerr << "Error: Insufficient data for login" << std::endl;
        return;
    }


    std::string login = parts[0];
    std::string password = parts[1];
    std::string loginFlag;

    // ���������� SQL-������ ��� ��������� user_id � user_name
    std::string query = "SELECT User_reg.User_id, User_data.User_name "
                        "FROM User_reg INNER JOIN User_data ON User_reg.User_id = User_data.User_id "
                        "WHERE User_reg.User_login = '" + login + "' AND User_reg.User_password = '" + password + "'";
    
    if (mysql_query(&conn, query.c_str())) {
        std::cerr << "Query Execution Error: " << mysql_error(&conn) << std::endl;
        Message signUpMessageToClient(BAD_LOG_CODE, { "Query Execution Error" });
        sendMessageToClient(clientSocket, signUpMessageToClient);
        return;
    }

    MYSQL_RES *result = mysql_store_result(&conn);
    if (result == NULL) {
        std::cerr << "Error storing result: " << mysql_error(&conn) << std::endl;
        Message signUpMessageToClient(BAD_LOG_CODE, { "Error storing result" });
        sendMessageToClient(clientSocket, signUpMessageToClient);
        return;
    }

    // ���������� ��� �������� ��������� ��������
    int id = -1;
    std::string name;

    if (mysql_num_rows(result) > 0) {
        MYSQL_ROW row = mysql_fetch_row(result);
        id = std::stoi(row[0]); // �������������� user_id � int
        name = row[1]; // user_name ��� � ������� std::string
        loginFlag = name;
        Message loginMessageToClient(RIGHT_LOG_CODE, { loginFlag });
        sendMessageToClient(clientSocket, loginMessageToClient);
        _userList.push_back(User(id, login, password, name, clientSocket));
    } else {
        loginFlag = "Login unsuccessful";
        Message loginMessageToClient(BAD_LOG_CODE, { loginFlag });
        sendMessageToClient(clientSocket, loginMessageToClient);
    }

    mysql_free_result(result);


}

void Server::logoutClient(int clientSocket) {
    auto it = std::remove_if(_userList.begin(), _userList.end(),
        [clientSocket](User& user) {
            return user.getUserSocket() == clientSocket;
        });

    _userList.erase(it, _userList.end());

}

void Server::regClient(int clientSocket, const std::string& data) {
    size_t pos = 0;
    std::string token;
    std::vector<std::string> parts;
    std::string delimiter = "|";

    std::string dataCopy = data;
    while ((pos = dataCopy.find(delimiter)) != std::string::npos) {
        token = dataCopy.substr(0, pos);
        parts.push_back(token);
        dataCopy.erase(0, pos + delimiter.length());
    }

    if (parts.size() < 3) {
        std::cerr << "Error: Insufficient data for registration" << std::endl;
        return;
    }

    std::string login = parts[0];
    std::string password = parts[1];
    std::string name = parts[2];

    // ���������� � ���������� SQL-������� ��� User_reg
    std::string queryUserReg = "INSERT INTO User_reg (User_login, User_password) VALUES ('" + login + "', '" + password + "')";
    if (mysql_query(&conn, queryUserReg.c_str())) {
        std::cerr << "Query Execution Error: " << mysql_error(&conn) << std::endl;
        // ��������� ������...
        Message signUpMessageToClient(BAD_REG_CODE, { "Query Execution Error" });
        sendMessageToClient(clientSocket, signUpMessageToClient);
        return;
    }

    // ��������� ID ������������ ������������
    int user_id = static_cast<int>(mysql_insert_id(&conn));

    // ���������� � ���������� SQL-������� ��� User_data
    std::string queryUserData = "INSERT INTO User_data (User_id, User_name) VALUES (" + std::to_string(user_id) + ", '" + name + "')";
    if (mysql_query(&conn, queryUserData.c_str())) {
        std::cerr << "Query Execution Error: " << mysql_error(&conn) << std::endl;
        // ��������� ������...
        Message signUpMessageToClient(BAD_REG_CODE, { "Query Execution Error" });
        sendMessageToClient(clientSocket, signUpMessageToClient);
        return;
    }

    //� ������ �������� ����������� - � ������ �������� ������� ������ ����������� �����
    _userList.push_back(User(user_id, login, name, password, clientSocket));

    Message signUpMessageToClient(RIGHT_REG_CODE, { name });
    sendMessageToClient(clientSocket, signUpMessageToClient);
}

void Server::addMessage(int clientSocket, const std::string& data) {
    std::string delimiter = "|";
    size_t pos = 0;
    std::vector<std::string> parts;
    std::string dataCopy = data;

    while ((pos = dataCopy.find(delimiter)) != std::string::npos) {
        parts.push_back(dataCopy.substr(0, pos));
        dataCopy.erase(0, pos + delimiter.length());
    }

    if (parts.size() < 2) {
        std::cerr << "Error: Insufficient data for adding message" << std::endl;
        return;
    }

    std::string address = parts[0];
    std::string text = parts[1];

    // ��������� sender_id
    int sender_id = getUserIdBySocket(clientSocket);

    // ����� address_id
    std::string query = "SELECT User_id FROM User_data WHERE User_name = '" + address + "'";
    if (mysql_query(&conn, query.c_str())) {
        std::cerr << "Query Execution Error: " << mysql_error(&conn) << std::endl;
        return;
    }

    MYSQL_RES* result = mysql_store_result(&conn);
    if (result == NULL) {
        std::cerr << "Error storing result: " << mysql_error(&conn) << std::endl;
        return;
    }

    int address_id = -1;
    if (mysql_num_rows(result) > 0) {
        MYSQL_ROW row = mysql_fetch_row(result);
        address_id = std::stoi(row[0]);
    }
    else {
        std::cerr << "Address user not found" << std::endl;
    }
    mysql_free_result(result);

    if (address_id == -1) {
        //������� ������� ������ � ��������������� ��������
        Message SignalMessageToClient(SIGNAL_REQUEST, { "Address not found" });
        sendMessageToClient(clientSocket, SignalMessageToClient);
        return;
    }

    // ������������ � ���������� SQL-������� ��� ���������� ���������
    std::string insertQuery = "INSERT INTO Message (Sender_id, Address_id, Message_text, Message_date) VALUES ("
        + std::to_string(sender_id) + ", " + std::to_string(address_id) + ", '"
        + text + "', NOW())";

    //������� �������� ���������
    //1. ������� ����� �������� �� id
    //2. ���� ����� ����������, �� ������� � ������� ��������� �� �����
    int addressSocket = getUserSocketById(address_id);
    if (addressSocket != -1) {
        std::string senderName = getUserNameById(sender_id);
        Message Message(JUST_MESSAGE_REQUEST, { senderName, text });
        sendMessageToClient(addressSocket, Message);
    }


    if (mysql_query(&conn, insertQuery.c_str())) {
        std::cerr << "Query Execution Error: " << mysql_error(&conn) << std::endl;
        return;
    }
}

void Server::showUsers(int clientSocket, const std::string& data) {
    std::string delimiter = "|";
    size_t pos = data.find(delimiter);
    std::string list;

    if (pos == std::string::npos) {
        std::cerr << "Error: Invalid data format for showUsers" << std::endl;
        return;
    }

    std::string flag = data.substr(0, pos);

    if (flag == "all") {
        std::string query = "SELECT User_name FROM User_data";
        if (mysql_query(&conn, query.c_str())) {
            std::cerr << "Query Execution Error: " << mysql_error(&conn) << std::endl;
            return;
        }

        MYSQL_RES* result = mysql_store_result(&conn);
        if (result == NULL) {
            std::cerr << "Error storing result: " << mysql_error(&conn) << std::endl;
            return;
        }

      
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(result)) != NULL) {
            if (!list.empty()) {
                list += delimiter;
            }
            list += row[0];
        }
        mysql_free_result(result);

        //��������� list �������
        Message showUsers(SHOW_USERS_LIST_REQUEST, { list });
        sendMessageToClient(clientSocket, showUsers);

    }
    else if (flag == "online") {
        //������� ����� ������������� �� ������� ������������� �������� ������ � �������� � list
        //��������� list �������
        for (auto& user : _userList) {
            list += user.getUserName() + delimiter;
        }
        Message showUsers(SHOW_USERS_LIST_REQUEST, { list });
        sendMessageToClient(clientSocket, showUsers);

    }
}

void Server::showHistory(int clientSocket, const std::string& data) {
    std::string delimiter = "|";
    size_t pos = data.find(delimiter);

    if (pos == std::string::npos) {
        std::cerr << "Error: Invalid data format for showHistory" << std::endl;
        return;
    }

    std::string addressName = data.substr(0, pos);

    int senderId = getUserIdBySocket(clientSocket);

    // ��������� addressId
    std::string query = "SELECT User_id FROM User_data WHERE User_name = '" + addressName + "'";
    if (mysql_query(&conn, query.c_str())) {
        std::cerr << "Query Execution Error: " << mysql_error(&conn) << std::endl;
        return;
    }

    MYSQL_RES* result = mysql_store_result(&conn);
    if (result == NULL) {
        std::cerr << "Error storing result: " << mysql_error(&conn) << std::endl;
        return;
    }

    if (mysql_num_rows(result) == 0) {
        std::cerr << "Address user not found" << std::endl;
        mysql_free_result(result);
        return;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    int addressId = std::stoi(row[0]);
    mysql_free_result(result);

    // ��������� ������� ���������
    query = "SELECT Message_text, Message_date FROM Message WHERE (Sender_id = "
        + std::to_string(senderId) + " AND Address_id = " + std::to_string(addressId)
        + ") OR (Sender_id = " + std::to_string(addressId) + " AND Address_id = "
        + std::to_string(senderId) + ")";

    if (mysql_query(&conn, query.c_str())) {
        std::cerr << "Query Execution Error: " << mysql_error(&conn) << std::endl;
        return;
    }

    result = mysql_store_result(&conn);
    if (result == NULL) {
        std::cerr << "Error storing result: " << mysql_error(&conn) << std::endl;
        return;
    }

    std::string list;
    while ((row = mysql_fetch_row(result)) != NULL) {
        if (!list.empty()) {
            list += delimiter;
        }
        list += std::string(row[0]) + " " + row[1]; // Message_text � Message_date
    }
    mysql_free_result(result);

    Message showHistory(SHOW_MESSAGE_HISTORY_REQUEST, { list });
    sendMessageToClient(clientSocket, showHistory);
}

int Server::getUserIdBySocket(int clientSocket) {
    for (auto& user : _userList)
    {
        if (clientSocket == user.getUserSocket())
            return user.getUserId();
    }
    return -1;
}

int Server::getUserSocketById(int id) {
    for (auto& user : _userList)
    {
        if (id == user.getUserId())
            return user.getUserSocket();
    }
    return -1;
}

std::string Server::getUserNameById(int id) {
    for (auto& user : _userList)
    {
        if (id == user.getUserId())
            return user.getUserName();
    }
    return " ";
}