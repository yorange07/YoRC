#include<iostream>
#include<thread>
#include<condition_variable>
#include "Server.h"

using namespace std;

int main()
{
    setlocale(LC_ALL, "rus");

    Server server;
    server.start(12345);

    return 0;
}