#include "server.h"

int openServer(char *portStr){
  int portno = atoi(portStr);
  sockaddr_in serverAddr;
  sockaddr &serverAddrCast = (sockaddr &) serverAddr;
  int listenFd = socket(AF_INET, SOCK_STREAM, 0);
  bzero(&serverAddr, sizeof(serverAddr));
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serverAddr.sin_port = htons(1500);
  bind(listenFd, &serverAddrCast, sizeof(serverAddr));
  listen(listenFd, portno);
  return 0;
}
