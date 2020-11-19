#include "client.h"

int lastState = -1;
int state = 0;
QApplication *app;
window *win;
QTcpSocket *sock;
char buffer[512];
QString inputText;

void nextState(int next) {
  lastState = state;
  state = next;
}

void startingScreen() {
  static QPushButton *startButton;
  if (state == 0) {
    if (lastState == -1) {
      cout << "At starting screen\n";
      startButton = new QPushButton();
      startButton->setObjectName("Start Button");
      startButton->setParent(win);
      startButton->setText("Start");
      startButton->setGeometry(400, 300, 200, 200);
      QObject::connect(startButton, SIGNAL(clicked()), win, SLOT(startClicked()));
      startButton->show();
      nextState(0);
    } else {
      if (win->startButtonClicked) {
	startButton->close();
	win->startButtonClicked = 0;
	nextState(1);
      } else {
	nextState(0);
      }
    }
  } 
}

void connectingScreen() {
  static QLabel *connectingText;
  static QWidget *loadingDot;
  static int *dotPos;
  static QWidget *loadingBar;
  static int *waitTime;
  static int *holdTimer;
  if (state == 1) {
    if (lastState == 0) {
      cout << "At connecting screen\n";
      connectingText = new QLabel();
      connectingText->setObjectName("Connecting Text");
      connectingText->setParent(win);
      connectingText->setGeometry(400, 300, 200, 200);
      connectingText->setText("Connecting to server\n");
      connectingText->setAlignment(Qt::AlignCenter);
      loadingBar = new QWidget();
      loadingBar->setObjectName("Loading Bar");
      loadingBar->setParent(win);
      loadingBar->setGeometry(440, 500, 120, 20);
      loadingBar->setPalette(Qt::white);
      loadingBar->setAutoFillBackground(true);
      loadingDot = new QWidget();
      loadingDot->setObjectName("Loading Dot");
      loadingDot->setParent(loadingBar);
      loadingDot->setGeometry(0, 0, 20, 20);
      loadingDot->setPalette(QPalette(Qt::black));
      loadingDot->setAutoFillBackground(true);
      dotPos = new int(1);
      win->ConnectionError = 0;
      holdTimer = new int(1);
      connectingText->show();
      loadingBar->show();
      loadingDot->show();
      nextState(1);
    } else if (*holdTimer < 5) {
      *holdTimer = *holdTimer + 1;
      *dotPos = (*dotPos + 1)%11;
      loadingDot->move(0 + *dotPos*10, 0);
      nextState(1);
    } else if (*holdTimer == 5) {
      waitTime = new int(1);
      sock = new QTcpSocket(win);
      QObject::connect(sock, SIGNAL(connected()), win, SLOT(serverConnected()));
      QObject::connect(sock, SIGNAL(readyRead()), win, SLOT(readyToRead()));
      sock->connectToHost(QString("localhost"), quint16(9000));
      *holdTimer = 6;
      *dotPos = (*dotPos + 1)%11;
      loadingDot->move(0 + *dotPos*10, 0);
      nextState(1);
    } else {
      if(win->ServerConnected) {
	win->ServerConnected = 0;
        connectingText->close();
        loadingBar->close();
	loadingDot->close();
	delete dotPos;
	delete waitTime;
	delete holdTimer;
        nextState(2);
      } else {
	if (win->ConnectionError) {
	  if (win->retryConnect) {
            cout << "Time to retry\n";
	    win->retryConnect = 0;
            sock->connectToHost(QString("localhost"), quint16(9000));
	    win->ConnectionError = 0;
	  }
	} else {
          if (*waitTime > 10) {
            cout << "Timeout\n";
            sock->disconnectFromHost();
	    win->ConnectionError = 1;
	    win->retryConnect = 1;
            *waitTime = 0;
	  }
	  *waitTime = *waitTime + 1;
	}
        *dotPos = (*dotPos + 1)%11;
        loadingDot->move(0 + *dotPos*10, 0);
	nextState(1);
      }
    }
  }
}

void connectedScreen() {
  static QLabel *connectedText;
  static int *holdTimer;
  if (state==2) {
    if (lastState==1) {
      connectedText = new QLabel();
      connectedText->setObjectName("Connected Text");
      connectedText->setParent(win);
      connectedText->setGeometry(400, 300, 200, 200);
      connectedText->setText("Connected");
      connectedText->setAlignment(Qt::AlignCenter);
      holdTimer = new int(1);
      connectedText->show();
      nextState(2);
    } else if (*holdTimer <= 5) {
      *holdTimer = *holdTimer + 1;
      nextState(2);
    } else {
      connectedText->close();
      delete holdTimer;
      nextState(3);
    }
  }
}

void waitScreen() {
  static QLabel *waitPrompt;
  static int *holdTimer;
  static int *dotPos;
  static QWidget *loadingBar;
  static QWidget *loadingDot;
  if (state == 3) {
    if (lastState != 3) {
      waitPrompt = new QLabel();
      waitPrompt->setObjectName("Wait Prompt Text");
      waitPrompt->setParent(win);
      waitPrompt->setGeometry(400, 300, 200, 200);
      waitPrompt->setText("Waiting for Server Prompt");
      waitPrompt->setAlignment(Qt::AlignCenter);
      holdTimer = new int(1);
      loadingBar = new QWidget();
      loadingBar->setObjectName("Loading Bar");
      loadingBar->setParent(win);
      loadingBar->setGeometry(440, 500, 120, 20);
      loadingBar->setPalette(Qt::white);
      loadingBar->setAutoFillBackground(true);
      loadingDot = new QWidget();
      loadingDot->setObjectName("Loading Dot");
      loadingDot->setParent(loadingBar);
      loadingDot->setGeometry(0, 0, 20, 20);
      loadingDot->setPalette(QPalette(Qt::black));
      loadingDot->setAutoFillBackground(true);
      dotPos = new int(1);
      loadingBar->show();
      loadingDot->show();
      waitPrompt->show();
      nextState(3);
    } else if (*holdTimer < 5) {
      *holdTimer = *holdTimer + 1;
      *dotPos = (*dotPos + 1)%11;
      loadingDot->move(0 + *dotPos*10, 0);
      nextState(3);
    } else if (*holdTimer == 5) {
      bzero(buffer, 512);
      sock->read(buffer, 511);
      *holdTimer = 6;
      *dotPos = (*dotPos + 1)%11;
      loadingDot->move(0 + *dotPos*10, 0);
      nextState(3);
    } else if (win->ReadyToRead) {
      cout << "Ready to read\n";
      win->ReadyToRead = 1;
      cout << buffer << endl;
      waitPrompt->close();
      loadingBar->close();
      loadingDot->close();
      delete holdTimer;
      nextState(4);
    } else {
      *dotPos = (*dotPos + 1)%11;
      loadingDot->move(0 + *dotPos*10, 0);
      nextState(3);
    }
  }
}

void inputScreen() {
  static QLabel *dummy;
  static QTextEdit *inputField;
  static QPushButton *confirm;
  if (state == 4) {
    if (lastState != 4) {
      dummy = new QLabel();
      dummy->setObjectName("Dummy Text");
      dummy->setParent(win);
      dummy->setGeometry(0, 100, 1000, 200);
      dummy->setText(buffer);
      dummy->setAlignment(Qt::AlignCenter);
      inputField = new QTextEdit();
      inputField->setObjectName("user Input");
      inputField->setParent(win);
      inputField->setGeometry(300, 500, 400, 200);
      confirm = new QPushButton();
      confirm->setObjectName("Confirm Button");
      confirm->setParent(win);
      confirm->setText("Confirm");
      confirm->setGeometry(300, 700, 400, 50);
      QObject::connect(confirm, SIGNAL(clicked()), win, SLOT(gotInput()));
      confirm->show();
      dummy->show();
      inputField->show();
      nextState(4);
    } else if (win->GotInput) {
      win->GotInput = 0;
      inputText = inputField->toPlainText();
      string stdInputText = inputText.toStdString();
      cout << inputText.toStdString() << endl;
      sock->write(stdInputText.c_str());
      dummy->close();
      inputField->close();
      confirm->close();
      nextState(3);
    } else {
      nextState(4);
    }
  }
}

void gameLoop::atClkEdge() {
  startingScreen();
  connectingScreen();
  connectedScreen();
  waitScreen();
  inputScreen();
}

void initGameLoop() {
  win = new window();
  win->setFixedSize(1000, 800);
  static QTimer *timer;
  static gameLoop *gLoop;
  gLoop = new gameLoop(win);
  timer = new QTimer(gLoop);
  QObject::connect(timer, SIGNAL(timeout()), gLoop, SLOT(atClkEdge()));
  timer->start(100); 
  win->show();
}

int main(int argc, char *argv[]) {
  app = new QApplication(argc, argv);
  initGameLoop();
  return app->exec();
}
