#include "client.h"

int timeInMilli;
int lastState = -1;
int state = 0;
QApplication *app;
window *win;
QTcpSocket *sock;
char buffer[512];
QString inputText;
QTimer *aTimer;
//int timeInMilli = 5;
bool mathQuestion = 0;
int counter = 0;
QLabel *timeBox;

//main screen Information
vector<string> listUser;
vector<pair<int,int> > score;
string serverMessage = "";
int yourScore = 0;
int maxScore = 0;
map<int,bool> removeUser;

int w_win = 600;
int h_win = 400;

float rate_w =  float(w_win)/1000;
float rate_h =  float(h_win)/800;

void HandleMessage();

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
      connectingText->setGeometry(0.25*w_win, 0.4*h_win, 0.5*w_win, 0.07*h_win);
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
      cout<<"Setup success"<<endl;
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
      sock->connectToHost(QString("localhost"), quint16(6969));
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
            sock->connectToHost(QString("localhost"), quint16(6969));
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
        
      cout<<"connectScreen is running"<<endl;
      connectedText = new QLabel();
      connectedText->setObjectName("Connected Text");
      connectedText->setParent(win);
      connectedText->setGeometry(0.25*w_win, 0.4*h_win, 0.5*w_win, 0.05*h_win);
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
      cout<<"connectedScreen is finishing"<<endl;
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
      cout<<"waitScreen is running"<<endl;
      waitPrompt = new QLabel();
      waitPrompt->setObjectName("Wait Prompt Text");
      waitPrompt->setParent(win);
      waitPrompt->setGeometry(0.25*w_win, 0.4*h_win, 0.5*w_win, 0.05*h_win);
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
    } else if (*holdTimer%5==0) {
      cout<<"Read buffer is running"<<endl;
      bzero(buffer, 512);
      sock->read(buffer, 511);
      *holdTimer = 6;
      *dotPos = (*dotPos + 1)%11;
      loadingDot->move(0 + *dotPos*10, 0);
      nextState(3);
      cout<<"Read buffer is complete"<<endl;
    } else if (win->ReadyToRead) {
      cout << "Ready to read\n";
      win->ReadyToRead = 1;
      waitPrompt->close();
      loadingBar->close();
      loadingDot->close();
      nextState(4);
    } else {
      *dotPos = (*dotPos + 1)%11;
      loadingDot->move(0 + *dotPos*10, 0);
      nextState(3);
    }
  }
}
/*
vector<string> listUser;
vector<pair<int,int> > score;
string serverMessage = "";
int yourScore = 0;
int maxScore = 0;
map<int,bool> removeUser;
*/

string score2Road(int sc){
    string s(maxScore*3,'.');
    s[sc*3+1] = 'O';
    s[sc*3+0] = 'o';
    s[sc*3+2] = 'o';
    return s;
}

void drawBackGround(bool close){
    static QListWidget *list_userUI;
    static QListWidget *scoreUI;
    static QLabel *serverMessageUI;
    static QLabel *yourScoreUI;
    static QLabel *maxScoreUI;
    if(close){
      scoreUI -> close();
      serverMessageUI -> close();
      list_userUI->close();
      yourScoreUI->close();
      maxScoreUI->close();
      return;
    }
    
    cout<<"drawBackGround is running..."<<endl;
    
    yourScoreUI = new QLabel();
    yourScoreUI -> setObjectName("Your score");
    yourScoreUI -> setParent(win);
    yourScoreUI -> setGeometry(0.1*w_win,0.65*h_win,0.2*w_win,0.05*h_win);
    string ys = "The answer: " + to_string(yourScore); 
    yourScoreUI -> setText(ys.c_str());
    
    maxScoreUI = new QLabel();
    maxScoreUI -> setObjectName("Max score");
    maxScoreUI -> setParent(win);
    maxScoreUI -> setGeometry(0.1*w_win,0.75*h_win,0.2*w_win,0.05*h_win);
    string ms = "Max score: " + to_string(maxScore);
    maxScoreUI -> setText(ms.c_str());
    
    serverMessageUI = new QLabel();
    serverMessageUI -> setObjectName("Server Message");
    serverMessageUI -> setParent(win);
    serverMessageUI -> setGeometry(0.1*w_win, 0.55*h_win, 0.8*w_win, 0.05*w_win);
    serverMessage = "Server: " + serverMessage;
    cout<<serverMessage<<endl;
    serverMessageUI -> setText(serverMessage.c_str());
    serverMessage = "";
    list_userUI = new QListWidget();  
    list_userUI->setObjectName("List User");
    list_userUI->setParent(win);
    list_userUI->setGeometry(0.1*w_win, 0.1*h_win, 0.6*w_win, 0.4*h_win);
    
    string rsize(maxScore*3,'.');
    vector<string>  ro(listUser.size(),rsize);
    for(int i=0 ; i<(int)score.size();++i){
        string tmp = score2Road(score[i].first);
        ro[score[i].second] = tmp;
    }
    string headRoad = "UserID/UserName"+string(20,'.')+"Road"+string(20,'.');
    list_userUI->addItem(headRoad.c_str());
    for(int i =  0;i<(int)listUser.size();++i){
      int sz = 40;
      string item = listUser[i]+": "+string(sz-listUser[i].size(),' ') +ro[i];
    }
    for(int i =  0;i<(int)listUser.size();++i){
      int sz = 15;
      string item = listUser[i]+": "+string(sz-listUser[i].size(),' ')+'\t' +ro[i];
      list_userUI->addItem(item.c_str());
    }
    scoreUI = new QListWidget();
    scoreUI -> setObjectName("List Score");
    scoreUI -> setParent(win);
    scoreUI -> setGeometry(0.7*w_win,0.1*h_win,0.2*w_win, 0.4*h_win);
    scoreUI -> addItem("LeaderBoard");
    for(int i=0;i<(int)score.size();++i){
        string item = listUser[score[i].second]+'\t'+to_string(score[i].first);
        scoreUI -> addItem(item.c_str());
    }
    list_userUI -> show();
    serverMessageUI->show();
    scoreUI -> show();
    yourScoreUI->show();
    maxScoreUI -> show();
    cout<<"drawBackGround is finishing..."<<endl;    
}

void inputScreen() {
  //static QLabel *dummy;
  
  
  static QTextEdit *inputField;
  static QPushButton *confirm;
  if (state == 4) {
    if (lastState != 4) {
      HandleMessage();
      if(state == 3 ){
          return;
      }
      /*
        dummy = new QLabel();
      dummy->setObjectName("Dummy Text");
      dummy->setParent(win);
      dummy->setGeometry(0, 100, 1000, 200);
      dummy->setText(buffer);
      dummy->setAlignment(Qt::AlignCenter);
      */
      cout<<"inputScreen is running"<<endl;
      inputField = new QTextEdit();
      inputField->setObjectName("user Input");
      inputField->setParent(win);
      inputField->setGeometry(0.3*w_win, 0.7*h_win, 0.4*w_win, 0.15*w_win);
      confirm = new QPushButton();
      confirm->setObjectName("Confirm Button");
      confirm->setParent(win);
      confirm->setText("Next");
      confirm->setGeometry(0.7*w_win, 0.7*h_win, 0.1*w_win, 0.1*h_win);
      QObject::connect(confirm, SIGNAL(clicked()), win, SLOT(gotInput()));
      confirm->show();
      //dummy->show();
      inputField->show();
      
     
      drawBackGround(0);
     
      nextState(4);
    } else if (!mathQuestion && win->GotInput) {
      cout << "Enter checkpoint 2\n";
      win->GotInput = 0;
      inputText = inputField->toPlainText();
      string stdInputText = inputText.toStdString();
      if(stdInputText.size()==0){
          stdInputText = "OK";
      }
      stdInputText.push_back('\n');
      cout << inputText.toStdString() << endl;
      sock->write(stdInputText.c_str());
      //dummy->close();
      
      inputField->close();
      confirm->close();
    
      drawBackGround(1);
      
      nextState(3);
      cout << "Exit checkpoint 2\n";
      
    } else if (mathQuestion && win->TimeOut) {
	cout << "win->TimeOut:" << win->TimeOut <<endl;
	cout << "Enter checkpoint 1\n";
	mathQuestion = 0;
	win->TimeOut = 0;
	cout << "Enter checkpoint 1a\n";
	win->GotInput = 0;
        inputText = inputField->toPlainText();
	string stdInputText = inputText.toStdString();
        if(stdInputText.size()==0){
          stdInputText = "0";
        }
	stdInputText.push_back(',');
	stdInputText += to_string(timeInMilli);
        stdInputText.push_back('\n');
        cout << inputText.toStdString() << endl;
        sock->write(stdInputText.c_str());
	cout << "Exit checkpoint 1a\n";
	aTimer->stop();
	timeBox->close();
        inputField->close();
	confirm->close();
	drawBackGround(1);
	nextState(3);
	cout << "Exit checkpoint 1\n";
    } else {
      if (mathQuestion) {	
	timeInMilli = aTimer->remainingTime();
	cout << timeInMilli << endl;
        timeBox->setText(QString(to_string(timeInMilli).c_str()));
      }
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
  aTimer = new QTimer(win);
  aTimer->setSingleShot(1);
  aTimer->setTimerType(Qt::PreciseTimer);
  QObject::connect(aTimer, SIGNAL(timeout()), win, SLOT(timeOut()));
  timer->start(100); 
  win->show();
}

int main(int argc, char *argv[]) {
  app = new QApplication(argc, argv);
  initGameLoop();
  return app->exec();
}


vector<string> findListFromMessage(vector<string> ms,int index){
    vector<string> res;
    string end = "end";
    string start = "start";
    int start_index = ms.size();
    for(int i = index ; i<(int)ms.size();++i){
        if(ms[i]==start){
            start_index = i+1;
            break;
        }
    }
    for(int i = start_index ; i<(int)ms.size();++i){
        if(ms[i]=="end") break;
        res.push_back(ms[i]);
    }
    return res;
}

pair<int, int> score2Pair(string s){
    pair<int,int> re;
    int id = 0;
    int sc = 0;
    bool  is_id = true;
    for(int i=0;i<(int)s.size();++i){
        if(s[i]==' '){
            is_id = false;
            continue;
        }
        if(is_id){
            id = id*10 + (s[i]-'0');
        }
        else {
            sc = sc*10 + (s[i]-'0');
        }
    }
    re = make_pair(sc,id);
    return re;
}

void HandleMessage(){
    cout<<"handle_message is running..."<<endl;
    string m(buffer);
    if(m.size()==0)  {
        nextState(3);
        return;
    }
    m.pop_back();
    vector<string> listM;
    listM = split(m);
    string c = listM[0];
    cout<<"Message contains: ";
    cout<<listM.size()<<endl;
    for(int i=0;i<(int)listM.size();++i){
        cout<<listM[i]<<' '<<listM[i].size()<<endl;
    }

    if(c == "username_request"){
        serverMessage = "enter username";
    }else if(c == "username_request_status"){
        if(listM[1]=="Success"){
            serverMessage = "Registration completed successfully";
        }
        else{
            serverMessage = "Registration failure!!";
        }
    }else if(c == "username_id_mapping"){
        serverMessage = "User info is updated. ";
        listUser = findListFromMessage(listM,0);
    }else if(c == "score_info"){
        serverMessage = "Score is updated. ";
        yourScore = atoi(listM[1].c_str());
        maxScore = atoi(listM[2].c_str());
    }else if (c=="wait_ready"){
        serverMessage ="Are you ready? ";
    }else if(c == "question"){
        cout << "Enter checkpoint 3\n";
        serverMessage = listM[1];
	aTimer->start(10000);
        mathQuestion = 1;
	timeBox =  new QLabel(win);

	timeBox->setGeometry(0.9*w_win, 0*h_win, 0.1*w_win, 0.1*h_win);
        cout << "Enter checkpoint 4\n";
	timeBox->setAlignment(Qt::AlignCenter);
	timeBox->setText("10000");
        cout << "Exit checkpoint 4\n";
	timeBox->show();
        cout << "Enter checkpoint 3\n";
    }else if(c == "answer_info"){
        serverMessage = "Your  answer is " + listM[2] +" and " + listM[3] +",  you are " + listM[4];
        vector<string> listRemove = findListFromMessage(listM,0);
        vector<string> listScore = findListFromMessage(listM,7+listRemove.size());
        yourScore = atoi(listM[1].c_str());
        cout<<"listScore size: "<<listScore.size()<<endl;
        cout<<"listRemove size:"<<listRemove.size()<<endl;
        for(int i = 0 ;i<(int)listRemove.size();++i){
         removeUser[atoi(listRemove[i].c_str())] = true;   
        }
        score.clear();
        for(int i = 0 ;i<(int)listScore.size();++i){
            score.push_back(score2Pair(listScore[i]));
        }
        sort(score.begin(),score.end());
        reverse(score.begin(),score.end());
        
    }
    else if(c == "continue"){
        if(listM[1]=="n"){
            serverMessage = "Game Over!! The winner is: ";
            sort(score.begin(),score.end());
            reverse(score.begin(),score.end());
            serverMessage += listUser[score[0].second];
        }
        else{
            serverMessage = "Game continue, wait for the next question...";
        }
    }
}
