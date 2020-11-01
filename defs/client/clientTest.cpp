#include "client.h"

int clientTest(char *argv[]){
  sockaddr_in serverAddr;
  sockaddr &serverAddrCast = (sockaddr &) serverAddr;
  int sockFd = socket(AF_INET, SOCK_STREAM, 0);
  bzero(&serverAddr, sizeof(serverAddr));
  serverAddr.sin_family = AF_INET;
  hostent *server;
  server = gethostbyname(argv[1]);
  bcopy((char *)server->h_addr,
         (char *)&serverAddr.sin_addr.s_addr,
         server->h_length);
  cout << serverAddr.sin_addr.s_addr << endl;
  int portno = atoi(argv[2]);
  serverAddr.sin_port = htons(portno);
  cout << "Waiting for server to accept" << endl;
  connect(sockFd, &serverAddrCast, sizeof(serverAddr));
  cout << "Accepted. Waiting for server prompt" << endl;
  char buffer[256];
  bzero(buffer, 256);
  recv(sockFd, buffer, 255, 0);
  cout << "Server said to you :" << endl;
  cout << buffer << endl;
  cout << "Enter a message to send to server :" << endl;
  char tmp[256];
  fgets(tmp,255,stdin);
  send(sockFd, tmp, sizeof(tmp), 0);
  cout << endl << "Success" << endl;
  shutdown(sockFd, 2);
  return 0;
}
