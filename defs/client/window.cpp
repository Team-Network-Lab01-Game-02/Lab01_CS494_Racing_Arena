#include "client.h"

extern int laststate;
extern int state;

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
}
