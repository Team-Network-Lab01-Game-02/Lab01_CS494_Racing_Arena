#ifndef SERVER_H
#define SERVER_H
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <thread>

using namespace std;

void hello();
// openSever() global variables
extern int listenFd;
int openServer(char *portStr);
// gameLoopS() global variables
extern bool sessionEnd;
extern int count;
int gameLoop();
int serverTest(char *portno);
#endif

