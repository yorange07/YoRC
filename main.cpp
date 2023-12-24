#include<iostream>
#include<thread>
#include<condition_variable>
#include "Client.h" 

using namespace std;

int main()
{
    setlocale(LC_ALL, "rus");

    Client client;
    client.startClient();

    return 0;
}