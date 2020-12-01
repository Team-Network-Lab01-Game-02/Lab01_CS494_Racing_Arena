#ifndef CLIENT_H
#define CLIENT_H
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>
#include <string>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <thread>
#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QTimer>
#include <QString>
#include <QLabel>
#include <QAbstractSocket> 
#include <QTcpSocket>
#include <QPalette>
#include <QString> 
#include <QtGlobal> 
#include <QTextEdit> 
#include <QListWidget>
#include <vector>
#include <sstream>
#include <map>
#include <algorithm>
using namespace std;

void hello();
int clientTest(char *argv[]);
extern int laststate;
extern int state;

class window : public QWidget {
  Q_OBJECT

  public:
  bool startButtonClicked = 0;
  bool ServerConnected = 0;
  bool ConnectionError;
  bool retryConnect = 0;
  bool ReadyToRead = 0;
  bool GotInput = 0;

  signals:

  public slots:
    void startClicked();
    void serverConnected();
    void connectionError(QAbstractSocket::SocketError);
    void finishDisc();
    void readyToRead();
    void gotInput();
};

class gameLoop : public QWidget {
  Q_OBJECT

  public:
  gameLoop(QWidget *parent) : QWidget(parent) {
  };

  signals:

  public slots:
    void atClkEdge();
};
#endif

