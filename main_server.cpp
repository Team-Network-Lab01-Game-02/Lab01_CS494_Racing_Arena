#include "server.h"

int main(int argc, char *argv[]){
  openServer(argv[1]);
  cout << "Waiting for client to connect" << endl;
  int connectFd = accept(listenFd, (sockaddr *) NULL, NULL);
  cout << "Client connected. Sending welcome message" << endl;
  char buffer[256];
  send(connectFd, "Welcome to the RACE\n", 19, 0);
  bzero(buffer, 256);
  cout << "Waiting for a message from client" << endl;
  recv(connectFd, buffer, 255, 0);
  cout << "Got the message :" << endl;
  cout << buffer << endl;
  shutdown(connectFd, 2);
  shutdown(listenFd, 2);
  return 0;
}

