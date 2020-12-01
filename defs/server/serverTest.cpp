#include "server.h"

int serverTest(char *portno){
  openServer(portno);
  cout << "Waiting for client to connect" << endl;
  int connectFd = accept(listenFd, (sockaddr *) NULL, NULL);
  cout << "Client connected. Sending welcome message" << endl;
  char buffer[256];
  send(connectFd, "username_id_mapping,start,abc,bcde,gdbc,end", 512, 0);
  //username_request
  //username_request_status,Success
  //username_request_status,Failure
  //username_id_mapping,start,abc,bcde,gdbc,end
  //question,1+2
  bzero(buffer, 256);
  cout << "Waiting for a message from client" << endl;
  recv(connectFd, buffer, 255, 0);
  cout << "Got the message :" << endl;
  cout << buffer << endl;
  shutdown(connectFd, 2);
  shutdown(listenFd, 2);
  return 0;
}