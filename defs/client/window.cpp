#include "client.h"

extern int laststate;
extern int state;
extern QTimer *aTimer;

void window::startClicked() {
  cout << "Start Button clicked\n";
  startButtonClicked = 1;
}

void window::serverConnected() {
  cout << "Connected to Server\n";
  ServerConnected = 1;
}

void window::connectionError(QAbstractSocket::SocketError) {
  cout << "Couldn't connect\n";
  ConnectionError = 1;
}

void window::finishDisc() {
  cout << "Finished disconnecting\n";
  retryConnect = 1;
}

void window::readyToRead() {
  cout << "I'm ready to read\n";
  ReadyToRead = 1;
}

void window::gotInput() {
  cout << "Got your input. Echoing: \n";
  GotInput = 1;
  timeInMilli = aTimer->remainingTime();
  TimeOut = 1;
}

void window::timeOut() {
  cout << "Time out\n";
  TimeOut = 1;
  timeInMilli = aTimer->remainingTime();
  cout << "THE TIMING: " << timeInMilli << endl;
  timeInMilli = -1;
}
