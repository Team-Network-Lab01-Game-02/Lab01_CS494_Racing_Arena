#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <string>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <thread>

using namespace std;

void hello();
int openServer(char *portStr);
