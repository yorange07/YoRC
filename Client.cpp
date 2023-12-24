// Client.cpp
#include "Client.h"


Client::Client() {
}

Client::~Client() {
    disconnect();
}

void Client::startClient() {
    connectToServer("127.0.0.1", 12345);//Подключение к серверу с чатом
    std::cout << "!!!";
    std::cout << _chatActivation;
    if (_chatActivation == true)
        showLoginMenu();//Чат приветствует и показывает меню входа/регистрации
    else std::cout << "Failed to connect to server";
}

void Client::connectToServer(const std::string& ipAddress, int port) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        throw std::runtime_error("Failed to initialize Winsock");
    }

    struct sockaddr_in serverAddr;

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        throw std::runtime_error("Error creating socket");
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);

    std::wstring wideIpAddress(ipAddress.begin(), ipAddress.end());

    if (InetPton(AF_INET, wideIpAddress.c_str(), &(serverAddr.sin_addr)) <= 0) {
        throw std::runtime_error("Invalid address/Address not supported");
    }

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        throw std::runtime_error("Error connecting to server");
    }

    setChatActivation(true);

    // Запускаем цикл получения сообщений в отдельном потоке
    std::thread(&Client::receiveLoop, this).detach();
}

void Client::showLoginMenu() {
    char action;

    std::cout << "////// Welcome to YoRC-chat! //////" << std::endl << std::endl;

    do
    {
        std::cout << "MAIN MENU" << std::endl;
        std::cout << "- press 1 to login" << std::endl;
        std::cout << "- press 2 to sign up" << std::endl;
        std::cout << "- press 0 to exit" << std::endl << std::endl;
        std::cout << ">> ";
        std::cin >> action;
        switch (action)
        {
        case '1':
            login();
            break;
        case '2':
            signUp();
            break;
        case '0':
            std::cout << "We will miss you..." << std::endl;
            _chatActivation = false;
            _currentUser = nullptr;
            logOut();
            disconnect();
            break;
        default:
            std::cout << "Such a command does not exist. Try again!" << std::endl;
            break;
        }
    } while (_currentUser == nullptr && _chatActivation);
}

void Client::showUserMenu() {
    char action;


    while (_currentUser != nullptr)
    {
        std::cout << std::endl;
        std::cout << "Choose an action:" << std::endl
            << "- press 1 to show chat" << std::endl
            << "- press 2 to send a message" << std::endl
            << "- press 3 to show all users" << std::endl
            << "- press 4 to show online users" << std::endl
            << "- press 0 to logout";

        std::cout << std::endl << std::endl;
        std::cout << ">> ";
        std::cin >> action;
        switch (action)
        {
        case '1':
            showChat();
            break;
        case '2':
            sendMessageToAddress();
            break;
        case '3':
            showAllUsersList();
            break;
        case '4':
            showOnlineUsersList();
            break;
        case '0':
            _currentUser = nullptr;
            logOut();
            showLoginMenu();
            break;
        default:
            std::cout << "Such a command does not exist. Try again!" << std::endl;
            break;
        }
    }
}

void Client::signUp()
{
    std::string login, password, name;
    char action;
    do {
       
        std::cout << std::endl;
        std::cout << "Enter your login >> ";
        std::cin >> login;
        std::cout << "Enter your password >> ";
        std::cin >> password;
        std::cout << "Enter your name >> ";
        std::cin >> name;


        Message signUpMessage(REGISTRATION_REQUEST, { login, password, name });
        sendMessage(signUpMessage);

        std::unique_lock<std::mutex> lock(registerMutex);
        registerCondition.wait(lock, [this] { return regAttemptCompleted; });

        if (registerSuccess) {
            // Регистрация успешна
            std::cout << "Registration successful!" << std::endl;
            User currenetUser(1, login, password, name, clientSocket);
            _currentUser = std::make_shared<User>(currenetUser);
        }
        else {
            // Ошибка регистрации
            std::cout << "Registration failed." << std::endl;
        }
        if (_currentUser == nullptr)
        {
            _currentUser == nullptr;
            std::cout << "Registration error" << std::endl;
            std::cout << "Press 0 to exit or any key to repeat" << std::endl;
            std::cout << ">> ";
            std::cin >> action;
            if (action == '0')
                break;
        }
    } while (!_currentUser);
    if (_currentUser) {
        showUserMenu();
        std::cout << std::endl;
        std::cout << "Hi, " << _currentUser->getUserName() << "! " << "We are glad to see you!" << std::endl;
    }
};

void Client::login()
{
    std::string login, password;
    std::string Me = "Me";
    char action;
    std::cout << std::endl;
    do
    {
        std::cout << "Enter your login >> ";
        std::cin >> login;
        std::cout << "Enter your password >> ";
        std::cin >> password;

        Message loginMessage(LOGIN_REQUEST, { login,password});
        sendMessage(loginMessage);

        // Ожидание ответа
        std::unique_lock<std::mutex> lock(loginMutex);
        loginCondition.wait(lock, [this] { return loginAttemptCompleted; });
        

        if (loginSuccess == true) {
            // Вход успешен
            User currenetUser(1, login, password, Me, clientSocket);
            _currentUser = std::make_shared<User>(currenetUser);
        }
        else {
            // Ошибка входа
            std::cout << "Login error:";
        }

        if (_currentUser == nullptr )
        {
            _currentUser == nullptr;
            std::cout << "User with this login was not found." << std::endl;
            std::cout << "Press 0 to exit or any key to repeat" << std::endl;
            std::cout << ">> ";
            std::cin >> action;
            if (action == '0')
                break;
        }
    } while (!_currentUser);
    if (_currentUser) {
        showUserMenu();
        std::cout << std::endl;
        std::cout << "Hi, " << _currentUser->getUserName() << "! " << "We are glad to see you!" << std::endl;
    }
};

void Client::logOut() {
    Message _message(LOGOUT_REQUEST, { "SIMPLE TEXT" });
    sendMessage(_message);
}

void Client::disconnect() {
    closesocket(clientSocket);
    registerSuccess = false; 
    regAttemptCompleted = false;
    loginSuccess = false;
    loginAttemptCompleted = false;
    _currentUser = nullptr;
    WSACleanup();
}

void Client::showChat() {

    std::string name;
    std::cout << std::endl;
    std::cout << "Enter address name";
    std::cin >> name;

    Message _message(SHOW_MESSAGE_HISTORY_REQUEST, { name });
    sendMessage(_message);

    // Ожидание ответа
    std::unique_lock<std::mutex> lock(otherMessageMutex);
    otherMessageCondition.wait(lock, [this] { return otherAttemptCompleted; });

    if (otherMessageSuccess == true) {
        // Запрос удовлетворен и мы сбрасываем флаг 
        otherMessageSuccess = false;
        otherAttemptCompleted = false;

    }
    else {
        // Запрос не удовлетворен
        std::cout << "Request error" << std::endl;
    }
    lock.unlock();
    showUserMenu();
};

void Client::showAllUsersList() {
    Message _message(SHOW_USERS_LIST_REQUEST, { "all"});
    sendMessage(_message);

    // Ожидание ответа
    std::unique_lock<std::mutex> lock(otherMessageMutex);
    otherMessageCondition.wait(lock, [this] { return otherAttemptCompleted; });

    if (otherMessageSuccess == true) {
        // Запрос удовлетворен и мы сбрасываем флаг 
        otherMessageSuccess = false;
        otherAttemptCompleted = false;

    }
    else {
        // Запрос не удовлетворен
        std::cout << "Request error" << std::endl;
    }
    lock.unlock();
    showUserMenu();
}

void Client::showOnlineUsersList() {
    Message _message(SHOW_USERS_LIST_REQUEST, { "online" });
    sendMessage(_message);

    // Ожидание ответа
    std::unique_lock<std::mutex> lock(otherMessageMutex);
    otherMessageCondition.wait(lock, [this] { return otherMessageSuccess; });

    if (otherMessageSuccess == true) {
        // Запрос удовлетворен и мы сбрасываем флаг 
        otherMessageSuccess = false;
        otherMessageSuccess = false;
    }
    else {
        // Запрос не удовлетворен
        std::cout << "Request error" << std::endl;
    }

    lock.unlock();
    showUserMenu();
}

void Client::sendMessage(Message& _message) {
    std::string message = _message.getType()+"|";
    for (int i = 0; i < _message.getData().size(); i++) {
        message = message + _message.getData()[i] + "|";
    }
    send(clientSocket, message.c_str(), message.size(), 0);
}

void Client::sendMessageToAddress() {
    std::string address, text;
    std::cout << std::endl;
    std::cout << "Enter address name";
    std::cin >> address;
    std::cout << "Enter your message";
    std::cin >> text;

    Message _message(JUST_MESSAGE_REQUEST, { address,text });

    sendMessage(_message);
}

std::string Client::receiveMessage() {
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));

    int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

    if (bytesRead <= 0) {
        // Обработка отключения сервера
        disconnect();
        return "";
    }

    return std::string(buffer, bytesRead);
}

void Client::receiveLoop() {
    while (true) {
        // Реализуйте цикл получения сообщений от сервера
        std::string receivedMessage = receiveMessage();
        if (receivedMessage.empty()) {
            break;  // Если сервер отключен, завершаем цикл
        }

        // Обработка полученного сообщения
        processServerMessage(receivedMessage);
    }
}

void Client::processServerMessage(std::string receivedMessage) {
    // Извлекаем тип сообщения до первого разделителя
    size_t delimiterPos = receivedMessage.find('|');
    if (delimiterPos == std::string::npos) {
        // Некорректное сообщение, разделитель не найден
        std::cerr << "Error: Invalid message format." << std::endl;
        return;
    }

    std::string receivedType = receivedMessage.substr(0, delimiterPos);

    // Остальные данные после разделителя
    std::string dataAfterDelimiter = receivedMessage.substr(delimiterPos + 1);

    // В зависимости от типа сообщения вызываем соответствующую функцию

        if (receivedType == "001") {
            std::lock_guard<std::mutex> lock(registerMutex);
            registerSuccess = true;
            regAttemptCompleted = true;
            registerCondition.notify_one();
        }
        else if (receivedType == "000") {
            std::lock_guard<std::mutex> lock(registerMutex);
            registerSuccess = false;
            regAttemptCompleted = true;
            registerCondition.notify_one();
        }

        if (receivedType == "101") {
            std::lock_guard<std::mutex> lock(loginMutex);
            loginSuccess = true;
            loginAttemptCompleted = true;
            loginCondition.notify_one();
        }
        else if (receivedType == "100") {
            std::lock_guard<std::mutex> lock(loginMutex);
            loginSuccess = false;
            loginAttemptCompleted = true;
            loginCondition.notify_one();
        }
 
    else {
            std::lock_guard<std::mutex> lock(otherMessageMutex);
            otherMessageSuccess = true;
            otherAttemptCompleted = true;
            otherMessageCondition.notify_one();
        std::cout << "Received message from server: " << dataAfterDelimiter << std::endl;
    }
};