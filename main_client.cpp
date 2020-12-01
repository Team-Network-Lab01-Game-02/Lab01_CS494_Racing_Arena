#include "client.h"

int lastState = -1;
int state = 0;
QApplication *app;
window *win;
QTcpSocket *sock;
char buffer[512];
QString inputText;

int w_win = 600;
int h_win = 400;

float rate_w =  float(w_win)/1000;
float rate_h =  float(h_win)/800;

void HandleMessage(bool is_close);

vector<string> split(string str){
    vector<string> v;
    stringstream ss(str);
    while(ss.good()){
        string substr;
        getline(ss,substr,',');
        v.push_back(substr);
    }
    return  v;
}

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
      startButton->setGeometry(int(400 * rate_w), int(300*rate_h), int(200*rate_w), int(200*rate_h));
      //startButton->setGeometry(200,200,100,100);
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
      connectingText->setGeometry(int(400 * rate_w), int(300*rate_h), int(200*rate_w), int(200*rate_h));
      connectingText->setText("Connecting to server\n");
      connectingText->setAlignment(Qt::AlignCenter);
      loadingBar = new QWidget();
      loadingBar->setObjectName("Loading Bar");
      loadingBar->setParent(win);
      loadingBar->setGeometry(int(440 * rate_w), int(500*rate_h), int(120*rate_w), int(20*rate_h));
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
      connectedText->setGeometry(int(400 * rate_w), int(300*rate_h), int(200*rate_w), int(200*rate_h));
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
      waitPrompt->setGeometry(int(400 * rate_w), int(300*rate_h), int(200*rate_w), int(200*rate_h));
      waitPrompt->setText("Waiting for Server Prompt");
      waitPrompt->setAlignment(Qt::AlignCenter);
      holdTimer = new int(1);
      loadingBar = new QWidget();
      loadingBar->setObjectName("Loading Bar");
      loadingBar->setParent(win);
      loadingBar->setGeometry(int(440 * rate_w), int(500*rate_h), int(120*rate_w), int(20*rate_h));
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
      win->ReadyToRead = 0;
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

void drawBackGround(bool close){
    if(close){
    }
}

void inputScreen() {
  //static QLabel *dummy;
  static QTextEdit *inputField;
  static QPushButton *confirm;
  if (state == 4) {
    if (lastState != 4) {
      /*
        dummy = new QLabel();
      dummy->setObjectName("Dummy Text");
      dummy->setParent(win);
      dummy->setGeometry(0, 100, 1000, 200);
      dummy->setText(buffer);
      dummy->setAlignment(Qt::AlignCenter);
      */
      inputField = new QTextEdit();
      inputField->setObjectName("user Input");
      inputField->setParent(win);
      inputField->setGeometry(int(300 * rate_w), int(500*rate_h), int(400*rate_w), int(200*rate_h));
      confirm = new QPushButton();
      confirm->setObjectName("Confirm Button");
      confirm->setParent(win);
      confirm->setText("Confirm");
      confirm->setGeometry(int(300 * rate_w), int(700*rate_h), int(400*rate_w), int(50*rate_h));
      QObject::connect(confirm, SIGNAL(clicked()), win, SLOT(gotInput()));
      confirm->show();
      //dummy->show();
      inputField->show();
  
      drawBackGround(0);
      HandleMessage(0);
      nextState(4);
    } else if (win->GotInput) {
      win->GotInput = 0;
      inputText = inputField->toPlainText();
      string stdInputText = inputText.toStdString();
      cout << inputText.toStdString() << endl;
      sock->write(stdInputText.c_str());
      //dummy->close();
      
      inputField->close();
      confirm->close();
    
      drawBackGround(1);
      HandleMessage(1);
      
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
  win->setFixedSize(w_win, h_win);
  //win->setWindowState(Qt::WindowFullScreen);
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

void username_request(bool close){

   static QLabel *dummy;
      if(close){
      dummy->close();
      return;
  }
  dummy = new QLabel();
  dummy->setObjectName("Dummy Text");
  dummy->setParent(win);
  dummy->setGeometry(int(0 * rate_w), int(100*rate_h), int(1000*rate_w), int(200*rate_h));
  dummy->setText("Enter username");
  dummy->setAlignment(Qt::AlignCenter);
  dummy->show();

}

void username_request_status(bool is_success,bool close){
  static QLabel *dummy;
  if(close){
      dummy->close();
      return;
  }
  dummy = new QLabel();
  dummy->setObjectName("Dummy Text");
  dummy->setParent(win);
  dummy->setGeometry(int(0 * rate_w), int(100*rate_h), int(1000*rate_w), int(200*rate_h));
  if(is_success){
    dummy->setText("Registration success");
  }else{
    dummy->setText("Registration failure");  
  }
  dummy->setAlignment(Qt::AlignCenter);
  dummy->show();
}

void username_id_mapping(vector<string> l_item,bool close){

  static QListWidget *list_user;
  static QLabel *dummy;
  if(close){
      cout<<"Close user_id_mapping..."<<endl;
      dummy->close();
      list_user->close();
      return;
  }
  dummy = new QLabel();
  dummy->setObjectName("List User Name");
  dummy->setParent(win);
  dummy->setGeometry(int(w_win * 0.8), int(100*rate_h), int(1000*rate_w), int(200*rate_h));
  dummy->setText("List User Name");
  dummy->setAlignment(Qt::AlignCenter);
  dummy->show();
  list_user = new QListWidget();  
  list_user->setObjectName("List User");
  list_user->setParent(dummy);
  list_user->setGeometry(0, 0.1*h_win, int(200*rate_w), int(200*rate_h));
  for(int i =  0;i<l_item.size();++i){
      list_user->addItem(QString::number(i)+" "+l_item[i].c_str());
  }
  list_user->show();
}

void score_info(bool close){
  static QLabel *dummy;
   if(close){
      dummy->close();
      return;
  }
  dummy = new QLabel();
  dummy->setObjectName("Dummy Text");
  dummy->setParent(win);
  dummy->setGeometry(int(0 * rate_w), int(100*rate_h), int(1000*rate_w), int(200*rate_h));
  dummy->setText("Enter you nickname: 4");
  dummy->setAlignment(Qt::AlignCenter);
}

void question(string str,bool close){
  static QLabel *dummy;
    if(close){
      dummy->close();
      return;
  }
  dummy = new QLabel();
  dummy->setObjectName("Question: ");
  dummy->setParent(win);
  dummy->setGeometry(int(0 * rate_w), int(100*rate_h), int(1000*rate_w), int(200*rate_h));
  str = "Question: " + str;
  dummy->setText(str.c_str());
  dummy->setAlignment(Qt::AlignCenter);
  dummy->show();

}

void answer_info(bool close){
      static QLabel *dummy;
        if(close){
      dummy->close();
      return;
        }
      dummy = new QLabel();
      dummy->setObjectName("Dummy Text");
      dummy->setParent(win);
      dummy->setGeometry(int(0 * rate_w), int(100*rate_h), int(1000*rate_w), int(200*rate_h));
      dummy->setText("Enter you nickname: 6");
      dummy->setAlignment(Qt::AlignCenter);

}

vector<string> findListFromMessage(vector<string> ms){
    vector<string> res;
    string end = "end";
    string start = "start";
    int start_index = ms.size();
    for(int i = 0 ; i<ms.size();++i){
        if(ms[i]==start){
            start_index = i+1;
            break;
        }
    }
    for(int i = start_index ; i<ms.size();++i){
        if(ms[i]=="end") break;
        res.push_back(ms[i]);
    }
    return res;
}

void HandleMessage(bool is_close){
    string m(buffer);
    vector<string> listM;
    listM = split(m);
    string c = listM[0];
    cout<<"Message contains: ";
    cout<<listM.size()<<endl;
    for(int i=0;i<listM.size();++i){
        cout<<listM[i]<<endl;
    }
    cout<<"handle_message is running..."<<endl;
    if(c == "username_request"){
        cout<<"handle_messages  for username request"<<endl;
        username_request(is_close);
    }else if(c == "username_request_status"){
        cout<<"handle_messages  for username request status: "<<listM[1]<<endl;
        username_request_status(listM[1]=="Success",is_close);
    }else if(c == "username_id_mapping"){
        cout<<"handle_messages  for username id mapping"<<endl;
        username_id_mapping(findListFromMessage(listM),is_close);
    }else if(c == "score_info"){
        cout<<"handle_messages  for score_info"<<endl;
        score_info(is_close);
    }else if(c == "question"){
        cout<<"handle_messages  for question"<<endl;
        question(listM[1],is_close);
    }else if(c == "answer_info"){
        cout<<"handle_messages  for answer_info"<<endl;
        answer_info(is_close);
    }
}