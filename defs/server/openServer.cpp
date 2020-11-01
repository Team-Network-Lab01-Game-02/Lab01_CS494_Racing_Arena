#include "server.h"

int listenFd;

int openServer(char *portStr){
  int portno = atoi(portStr);
  sockaddr_in serverAddr;
  sockaddr &serverAddrCast = (sockaddr &) serverAddr;
  listenFd = socket(AF_INET, SOCK_STREAM, 0);
  bzero(&serverAddr, sizeof(serverAddr));
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = INADDR_ANY;
  serverAddr.sin_port = htons(portno);
  bind(listenFd, &serverAddrCast, sizeof(serverAddr));
  listen(listenFd, portno);
  return 0;
}
