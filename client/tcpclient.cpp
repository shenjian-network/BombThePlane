#include "tcpclient.h"
#include "ui_tcpclient.h"

#include <stdio.h>
#include <string.h>
#include <sstream>
#include <QDialog>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QSize>
#include <QMessageBox>
#include <QPainter>
#include <QDir>
#include <QTextBrowser>
#include <QCheckBox>
#include <QRegExpValidator>
#include <QDateTime>
#include <QTimer>
#include <QString>
#include <QFile>
#include <QPlainTextEdit>
#include <QFileDialog>
#include <QHostInfo>
#include <QComboBox>

TcpClient::TcpClient(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TcpClient)
{
    ui->setupUi(this);

    socket = nullptr;
    loginWindow = nullptr;
    chatRoomWindow = nullptr;
    changePwdWindow = nullptr;
    userList = nullptr;
    preChatter = nullptr;
    pdlg = nullptr;
    fileWindow = nullptr;
    configWindow = nullptr;

    curChatter = new QLabel;

    curChatter->setAlignment(Qt::AlignCenter);
    curChatter->setFixedHeight(50);
    curChatter->setStyleSheet("background: white; color: black;border-bottom: 1px solid rgb(230, 230, 255);");
    isOnline = true;
    isGaming = false;
    isInviting = false;

    QFont ft;
    ft.setPointSize(15);
    curChatter->setFont(ft);

    // 右栏，包括消息窗口和发送消息栏
//    rightStackLayout = new QStackedLayout;
//    rightStackLayout->setStackingMode(QStackedLayout::StackOne);

    QTimer *timer=new QTimer(this);
    timer->start(1000); // 每次发射timeout信号时间间隔为1秒
    connect(timer,SIGNAL(timeout()),this,SLOT(timeUpdate()), Qt::QueuedConnection);
    time.start();
}

TcpClient::~TcpClient()
{
    delete ui;

    // 后续的指针，因不知道何时会退出而需要判断是否为空

    if(socket){
        delete socket;
    }

    if(preChatter){
        delete preChatter;
    }

    if(curChatter){
        delete curChatter;
    }

    if(gameWindow){
        delete gameWindow;
    }

    if(loginWindow){
        delete loginWindow;
    }

    if(chatRoomWindow){
        delete chatRoomWindow;
    }

    if(changePwdWindow){
        delete changePwdWindow;
    }

    if(configWindow){
        delete configWindow;
    }

    if(fileWindow){
        delete fileWindow;
    }

    if(userList){
       delete userList;
    }
}


// 连接到服务器 (FINISHED)
bool TcpClient::ConnectToHost(const QString& ip, unsigned short port){
    socket = new QTcpSocket(this);
    this->ip = ip;
    this->port = port;

    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::QueuedConnection);
    connect(socket, SIGNAL(disconnected()), this, SLOT(clientDisconnected()), Qt::QueuedConnection);

    qDebug() << "connecting...";
    socket->connectToHost(ip, port);

    if(!socket->waitForConnected(5000)){
        qDebug() << "Error: " << socket->errorString();
        return false;
    }

    qDebug() << "connect success";

    return true;
}




// 显示登录界面 (FINISHED)
void TcpClient::loginGUI(){
    // login window
    loginWindow = new QWidget;
    loginWindow->setWindowTitle("登录");
    loginWindow->setMaximumSize(500, 300);
    loginWindow->setMinimumSize(500, 300);

    QHBoxLayout * layout1 = new QHBoxLayout;
    QLabel *unameLabel = new QLabel("登录");
    QLineEdit *unameEdit = new QLineEdit();
    unameLabel->setBuddy(unameEdit);
    QRegExp rx("[^\u4e00-\u9fa5]+");
    QRegExpValidator *pReg = new QRegExpValidator(rx, this);
    unameEdit->setValidator(pReg);
    layout1->addWidget(unameLabel);
    layout1->addWidget(unameEdit);
    layout1->setContentsMargins(30, 10, 30, 10);

    QHBoxLayout * layout2 = new QHBoxLayout;
    QLabel *upwdLabel = new QLabel("密码");
    QLineEdit *upwdEdit = new QLineEdit();
    upwdEdit->setEchoMode(QLineEdit::Password);
    upwdEdit->setValidator(pReg);
    unameLabel->setBuddy(upwdEdit);
    layout2->addWidget(upwdLabel);
    layout2->addWidget(upwdEdit);
    layout2->setContentsMargins(30, 10, 30, 10);

    QHBoxLayout * layout3 = new QHBoxLayout;
    QCheckBox * rememberPwd = new QCheckBox;
    rememberPwd->setText("记住密码");   // 还没这个功能
    rememberPwd->setCheckable(true);
    QCheckBox * showPwd = new QCheckBox;
    showPwd->setText("显示密码");
    showPwd->setCheckable(true);
    layout3->addWidget(rememberPwd);
    layout3->addWidget(showPwd);
    layout3->setContentsMargins(100, 10, 100, 10);

    QHBoxLayout * layout4 = new QHBoxLayout;
    QPushButton * login = new QPushButton("登录");
    QPushButton * signup = new QPushButton("注册");
    QPushButton * changePwd = new QPushButton("修改密码");

    login->setStyleSheet("QPushButton{background-color:rgb(0, 150, 255)}"
                         "QPushButton:hover{background-color:rgb(0, 255, 255)}");
    signup->setStyleSheet("QPushButton{background-color:rgb(0, 150, 255)}"
                         "QPushButton:hover{background-color:rgb(0, 255, 255)}");
    changePwd->setStyleSheet("QPushButton{background-color:rgb(0, 150, 255)}"
                         "QPushButton:hover{background-color:rgb(0, 255, 255)}");

    layout4->addWidget(login);
    layout4->addWidget(signup);
    layout4->addWidget(changePwd);
    layout4->setContentsMargins(100, 10, 100, 10);

    QVBoxLayout * mainLayout = new QVBoxLayout;
    mainLayout->addLayout(layout1);
    mainLayout->addLayout(layout2);
    mainLayout->addLayout(layout3);
    mainLayout->addLayout(layout4);
    mainLayout->setMargin(50);

    connect(login, SIGNAL(clicked()), this, SLOT(on_loginBtn_clicked()), Qt::QueuedConnection);
    connect(signup, SIGNAL(clicked()), this, SLOT(on_signupBtn_clicked()), Qt::QueuedConnection);
    connect(changePwd, SIGNAL(clicked()), this, SLOT(on_changePwdBtn_clicked()), Qt::QueuedConnection);
    connect(showPwd, SIGNAL(stateChanged(int)), this, SLOT(on_showPwdCheckBox_stateChanged()), Qt::QueuedConnection);

    loginWindow->setLayout(mainLayout);
    loginWindow->resize(500, 300);
     loginWindow->setAutoFillBackground(true); // 这句要加上, 否则可能显示不出背景图.
     QPalette palette;
     qDebug() << QDir::currentPath()+ "/image/background.jpg";
     QPixmap pix;
     if(pix.load(QDir::currentPath()+ "/image/background.jpg")){
         palette.setBrush(QPalette::Window,
                 QBrush(pix.scaled(// 缩放背景图.
                     loginWindow->size(),
                     Qt::IgnoreAspectRatio,
                     Qt::SmoothTransformation)));             // 使用平滑的缩放方式
         loginWindow->setPalette(palette);                           // 给widget加上背景图
     }

    loginWindow->show();
}



// 显示错误信息窗口 (FINISHED)
void TcpClient::errorGUI(const unsigned short & err_type){
    QMessageBox *errorBox = new QMessageBox();

    switch (err_type) {
    case PacketHead::kS2CReportWrongPwd:
        errorBox->setText("登录失败，密码错误");
        break;
    case PacketHead::kS2CReportNoExist:
        errorBox->setText("登录失败，用户名不存在");
        break;
    case PacketHead::kS2CReportMustUpdate:
        errorBox->setText("登录失败，需要强制改密");
        break;
    case PacketHead::kS2CReportDuplicated:
        errorBox->setText("注册失败，用户名已存在");
        break;
    case PacketHead::kS2CReportNameNotAccess:
        errorBox->setText("注册失败，用户名不符合要求");
        break;
    case PacketHead::kS2CReportPwdNotAccess:
        errorBox->setText("注册失败，密码不符合要求");
        break;
    case PacketHead::kS2CReportLastPwdWrong:
        errorBox->setText("更改密码失败，原密码错误");
        break;
    case PacketHead::kS2CReportNowPwdNotAccess:
        errorBox->setText("更改密码失败，现密码不符合规范");
        break;
    }

    errorBox->show();
}

// 显示错误信息窗口，重载 （FINISHED）
void TcpClient::errorGUI(const QString& err){
    QMessageBox::StandardButton reply;
    reply = QMessageBox::information(this, tr("错误"), err);
}

void TcpClient::successGUI(const QString& err){
    QMessageBox::StandardButton reply;
    reply = QMessageBox::information(this, tr("成功"), err);
}

void TcpClient::inviteSrcGUI(const QString& name) {
    inviteSrcBox = new QMessageBox(QMessageBox::Information, "Waiting...", "正在邀请" + name);
    inviteSrcBox->setStandardButtons(QMessageBox::Cancel);
    connect(inviteSrcBox->button(QMessageBox::Cancel), SIGNAL(clicked()), this, SLOT(cancelInvitationActive()), Qt::QueuedConnection);
    inviteSrcBox->setWindowModality(Qt::NonModal);
    inviteSrcBox->show();
}

void TcpClient::inviteDstGUI(const QString& name){
    auto inviteDstBox = new QMessageBox(QMessageBox::Information, "邀请", name + "邀请你加入游戏");
    inviteDstBox->setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    connect(inviteDstBox->button(QMessageBox::Ok), SIGNAL(clicked()), this, SLOT(acceptInvitation()), Qt::QueuedConnection);
    connect(inviteDstBox->button(QMessageBox::Cancel), SIGNAL(clicked()), this, SLOT(declineInvitation()), Qt::QueuedConnection);
    inviteDstBoxList[name] = inviteDstBox;
    acceptbutton2opponame[inviteDstBox->button(QMessageBox::Ok)] = name;
    rejectbutton2opponame[inviteDstBox->button(QMessageBox::Cancel)] = name;
    inviteDstBox->setWindowModality(Qt::NonModal);
    inviteDstBox->show();
}

// 显示聊天室的新窗口，用于聊天(FINISHED)
void TcpClient::chatRoomGUI(){
    chatRoomWindow = new QWidget;
    chatRoomWindow->setWindowTitle("ChatRoom");
    chatRoomWindow->setFixedSize(494, 800);

    QPalette pal(chatRoomWindow->palette());
    pal.setColor(QPalette::Background, QColor(46, 50, 56)); //设置背景黑色
    chatRoomWindow->setPalette(pal);

    // 左栏，包括登录信息、用户列表
    QVBoxLayout * leftLayout = new QVBoxLayout;

    QPushButton * config = new QPushButton("设置");
    config->setStyleSheet("QPushButton{background: rgb(46, 50, 56); color: white;}");
    leftLayout->addWidget(config);

    connect(config, SIGNAL(clicked()), this, SLOT(configGUI()), Qt::QueuedConnection);

    QHBoxLayout * subsublayout1 = new QHBoxLayout;
    QLabel * curUserLabel = new QLabel("     当前用户");
    QLineEdit * curUsername = new QLineEdit(this->username);
    curUsername->setEnabled(false);
    curUserLabel->setStyleSheet("QLabel{color: white}");
    curUsername->setStyleSheet("QLineEdit{background: rgb(38, 41, 46); color: white; }");
    subsublayout1->addWidget(curUserLabel);
    subsublayout1->addWidget(curUsername);


    QHBoxLayout * subsublayout2 = new QHBoxLayout;
    QLabel * ipLabel = new QLabel("          IP地址");
    QLineEdit * ipaddr = new QLineEdit(this->ip);
    ipaddr->setEnabled(false);
    ipLabel->setStyleSheet("QLabel{color: white}");
    ipaddr->setStyleSheet("QLineEdit{background: rgb(38, 41, 46); color: white; }");
    subsublayout2->addWidget(ipLabel);
    subsublayout2->addWidget(ipaddr);

    QHBoxLayout * subsublayout3 = new QHBoxLayout;
    QLabel * portLabel = new QLabel("          端口号");
    QLineEdit * port = new QLineEdit(QString::number(this->port, 10));
    port->setEnabled(false);
    portLabel->setStyleSheet("QLabel{color: white}");
    port->setStyleSheet("QLineEdit{background: rgb(38, 41, 46); color: white; }");
    subsublayout3->addWidget(portLabel);
    subsublayout3->addWidget(port);

    QHBoxLayout * subsublayout4 = new QHBoxLayout;
    QLabel * loginTimeLabel = new QLabel("     上次登入");
    QLineEdit * loginTime = new QLineEdit(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    loginTime->setEnabled(false);
    loginTimeLabel->setStyleSheet("QLabel{color: white}");
    loginTime->setStyleSheet("QLineEdit{background: rgb(38, 41, 46); color: white; }");
    subsublayout4->addWidget(loginTimeLabel);
    subsublayout4->addWidget(loginTime);

    QHBoxLayout * subsublayout5 = new QHBoxLayout;
    QLabel * onlineTimeLabel = new QLabel("     在线时间");
    QLineEdit * onlineTime = new QLineEdit();
    QLabel * unit = new QLabel("秒");
    onlineTime->setEnabled(false);
    onlineTimeLabel->setStyleSheet("QLabel{color: white}");
    onlineTime->setStyleSheet("QLineEdit{background: rgb(38, 41, 46); color: white; }");
    unit->setStyleSheet("QLabel{color: white}");
    subsublayout5->addWidget(onlineTimeLabel);
    subsublayout5->addWidget(onlineTime);
    subsublayout5->addWidget(unit);

    QHBoxLayout * subsublayout6 = new QHBoxLayout;
    QLabel * statusLabel = new QLabel("     当前状态");
    QLineEdit * status = new QLineEdit("我在线上");
    status->setEnabled(false);
    statusLabel->setStyleSheet("QLabel{color: white}");
    status->setStyleSheet("QLineEdit{background: rgb(38, 41, 46); color: white;}");
    subsublayout6->addWidget(statusLabel);
    subsublayout6->addWidget(status);

    userList = new QListWidget;
    userList->setStyleSheet("QListWidget{background: rgb(46, 50, 56); border: 0px; "
                            "border-top: 1px solid rgb(0, 0, 0);}"
                            "QScrollBar:vertical{background: rgb(46, 50, 56);"
                            "width: 8px;}"
                            "QScrollBar::handle:vertical{background: rgb(58,63,69);"
                            "border: none;"
                            "min-height: 20px;}"
                            "QScrollBar::handle:vertical:hover{background: rgb(128,128,128);"
                            "border: none;"
                            "min-height: 20px;}"
                            "QScrollBar::add-line:vertical{border: none;}"
                            "QScrollBar::sub-line:vertical{border: none;}");
    userList->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    leftLayout->addLayout(subsublayout1);
    leftLayout->addLayout(subsublayout2);
    leftLayout->addLayout(subsublayout3);
    leftLayout->addLayout(subsublayout4);
    leftLayout->addLayout(subsublayout5);
    leftLayout->addLayout(subsublayout6);
    leftLayout->addWidget(userList);


//    rightStackLayout->addWidget(new QWidget);

    chatRoomMainLayout = new QHBoxLayout;
    chatRoomMainLayout->addLayout(leftLayout);
//    chatRoomMainLayout->addLayout(rightStackLayout);
    chatRoomMainLayout->setStretchFactor(leftLayout, 1);
//    chatRoomMainLayout->setStretchFactor(rightStackLayout, 2);
    chatRoomMainLayout->setMargin(0);

    chatRoomWindow->setLayout(chatRoomMainLayout);
    chatRoomWindow->show();

//    /* FOR DEBUG */
//    insertListWidget(QString("zhongyuchen"), true, false);
//    insertListWidget(QString("zhongyuchen2"), true, false);
//    insertListWidget(QString("zhongyuchen3"), false, false);
//    insertListWidget(QString("zhaiyuchen"), true, true);
}

bool TcpClient::isConnected(){
    QHostInfo info = QHostInfo::fromName("www.baidu.com");
      if(info.addresses().isEmpty())
          return false;
      else
          return true;
}


// *****辅助函数***** //

std::string QStringToString(const QString & myQstring)
{
    return std::string(myQstring.toLocal8Bit().constData());
}

std::string stringPadding(std::string myString, const unsigned int & len)
{
    myString.resize(len, 0);
    return myString;
}

//-----------------------//

// 配置界面 （FINISHED）
//下面有个确定键，点击后，会更改GUI界面，并触发槽函数sendConfig
void TcpClient::configGUI(){
    configWindow = new QWidget;
    configWindow->setFixedSize(500, 300);

    QVBoxLayout * layout = new QVBoxLayout;

    QHBoxLayout * sublayout1 = new QHBoxLayout;
    QLabel * label1 = new QLabel("回看数");
    QLineEdit * value1 = new QLineEdit;
    value1->setValidator(new QIntValidator(100, 200, this));
    sublayout1->addWidget(label1);
    sublayout1->addWidget(value1);

    QHBoxLayout * sublayout2 = new QHBoxLayout;
    QLabel * label2 = new QLabel("字体大小");
    QLineEdit * value2 = new QLineEdit;
    value2->setValidator(new QIntValidator(4, 24, this));
    sublayout2->addWidget(label2);
    sublayout2->addWidget(value2);


    QHBoxLayout * sublayout3 = new QHBoxLayout;
    QLabel * label3 = new QLabel("字体颜色");
    QLineEdit * value3 = new QLineEdit;
    value3->setValidator(new QIntValidator(0, 7, this));
    sublayout3->addWidget(label3);
    sublayout3->addWidget(value3);


    QHBoxLayout * sublayout4 = new QHBoxLayout;
    QPushButton * ack = new QPushButton("确定");
    ack->setFixedSize(75, 30);
    sublayout4->addWidget(ack);

    layout->addLayout(sublayout1);
    layout->addLayout(sublayout2);
    layout->addLayout(sublayout3);
    layout->addLayout(sublayout4);
    configWindow->setLayout(layout);

    connect(ack, SIGNAL(clicked()), this, SLOT(sendConfig()), Qt::QueuedConnection);
    configWindow->show();
}


// 下线的操作 （FINISHED）
// 需要熄灭用户栏，如果下线的是自己，会退出
void TcpClient::offline(){
    QString name = my_server_to_client_inform.get_user_name();

    // 1. 下线的是自己，代表被T了
    if(name == this->username){
        chatRoomWindow->close();
        errorGUI("您被T了");
        exit(1);
    } else{
        setUserStatus(name, false, false);
    }
}

// 上线的操作（FINISHED）
// 需要点亮用户栏
void TcpClient::online(){
    QString name = my_server_to_client_inform.get_user_name();

    setUserStatus(name, true, false);
}





std::string getKey(std::string senderNameString, std::string recvNameString, std::string fileNameString)
{
    return stringPadding(senderNameString, 32) + stringPadding(recvNameString, 32) + stringPadding(fileNameString, 64);
}





std::string TcpClient::singleConfigString(std::string configKey)
{
    return configKey + ' ' + configMap[configKey] + '\n';
}

//将config转换成string
std::string TcpClient::configString()
{
    std::string ret;
    ret += singleConfigString("reviewLineCnt");
    //TODO
    /*增加其他的配置项*/
    ret += singleConfigString("fontsize");

    ret += singleConfigString("fontcolor");

    return ret;
}

//发送配置更新包
void TcpClient::sendConfig()
{
    QLineEdit* line1 = static_cast<QLineEdit*>(configWindow->layout()->itemAt(0)->layout()->itemAt(1)->widget());
    qDebug() << line1->text();
    configMap["reviewLineCnt"] = QStringToString(line1->text());

    QLineEdit* line2 = static_cast<QLineEdit*>(configWindow->layout()->itemAt(1)->layout()->itemAt(1)->widget());
    qDebug() << line2->text();
    configMap["fontsize"] = QStringToString(line2->text());

    QLineEdit* line3 = static_cast<QLineEdit*>(configWindow->layout()->itemAt(2)->layout()->itemAt(1)->widget());
    qDebug() << line3->text();
    configMap["fontcolor"] = QStringToString(line3->text());

    // 真正的设置
    setConfigImpl(line2->text().toInt(), line3->text().toInt());

    configWindow->close();

    PacketHead sendPacketHead;

    sendPacketHead.set_packet_type(PacketHead::kC2SUserSet);
    sendPacketHead.set_function_type(PacketHead::kC2SUserSetUpdate);

    std::string myConfigString = configString();
//    printf(myConfigString.c_str());
//    fflush(stdout);

    sendPacketHead.set_length(myConfigString.length());

    ClientToServerUserSetUpdate sendClientToServerUserSetUpdate(sendPacketHead, myConfigString.c_str());

    char* tmpStr = new char[kPacketHeadLen + sendPacketHead.get_length() + 1];
    sendClientToServerUserSetUpdate.get_string(tmpStr);
    socket->write(tmpStr, kPacketHeadLen + sendPacketHead.get_length());

    delete[] tmpStr;
}

//根据初始发送配置包的内容进行设置配置 （FINISHED）
void TcpClient::setConfig(){
    std::string configData = std::string(my_server_to_client_user_set_update.get_user_set_data());
    printf(configData.c_str());

    std::istringstream configDataStream(configData);
    std::string configKey, configValue;
    while(configDataStream >> configKey >> configValue)
        configMap[configKey] = configValue;

    for(auto item: configMap){
        printf("%s, %s\n", item.first.c_str(), item.second.c_str());
        fflush(stdout);
    }


    //TODO
    /*
    根据configMap中的键值对来进行相应的设置
    */
    setConfigImpl(atoi(configMap["fontsize"].c_str()), atoi(configMap[configKey].c_str()));

}

// 向用户列表中添加一项 （FINISHED)
void TcpClient::insertListWidget(QString name, bool isOnline, bool isInGame){
    static int index = 0;
    QListWidgetItem * item = new QListWidgetItem;
    QLabel * userLabel = new QLabel;
    QWidget * user = new QWidget;
    QHBoxLayout * layout = new QHBoxLayout;

    ClickableLabel * userClickLabel = new ClickableLabel(name, userLabel);
    userClickLabel->setAlignment(Qt::AlignVCenter);
    userClickLabel->setMargin(0);
    userClickLabel->setFixedHeight(70);
    item->setSizeHint(userClickLabel->sizeHint());
    user->setMaximumHeight(70);
    user->setMinimumHeight(70);

    QFont ft;
    ft.setPointSize(12);
    userClickLabel->setFont(ft);

    connect(userClickLabel, SIGNAL(clicked()), this, SLOT(userLabelClicked()), Qt::QueuedConnection);

    if(isOnline && !isInGame){
        userClickLabel->setEnabled(true);
        userClickLabel->setStyleSheet("QLabel{background: rgb(46, 50, 56)}"
                                 "QLabel{color: rgb(255, 255, 255)}"
                                 "QLabel:hover{background: rgb(58,63,69)}");
    }else{
        userClickLabel->setEnabled(false);
        userClickLabel->setStyleSheet("QLabel{background: rgb(46, 50, 56)}"
                            "QLabel:hover{background: rgb(58,63,69)}"
                            "QLabel{color: rgb(128, 128, 128)}");
    }
    QPushButton * button = new QPushButton();
    QString show_button_text;
    if(isOnline){
        if(!isInGame) {
            show_button_text = "邀请";
            button->setEnabled(true);
        } else {
            show_button_text = "游戏中";
            button->setEnabled(false);
        }
    } else {
        button->setEnabled(false);
        show_button_text = "离线";
    }

    connect(button, SIGNAL(clicked()), this, SLOT(inviteGame()), Qt::QueuedConnection);

    button->setText(show_button_text);
    button->setFlat(true);
    button->setMaximumHeight(30);
    button->setMinimumHeight(30);
    button->setMaximumWidth(90);
    button->setMinimumWidth(90);
    layout->addWidget(userClickLabel);
    layout->addWidget(button);
    layout->setMargin(0);
    layout->setSpacing(0);
    button2name[button] = name;
    user->setLayout(layout);
//    InitGameWindow();
    user2Index.insert(name, ++index);
    userList->addItem(item);
    userList->setItemWidget(item, user);
}




// 设置用户名的状态 (FINISHED)
void TcpClient::setUserStatus(QString name, bool isOnline, bool isInGame){
    int count = userList->count();

    QListWidgetItem * item;
    QWidget * widget;
    QLabel * box;
    QString curName;
    QPushButton * button;

    qDebug() << "setUserStatus1";
    int flag = false;
    // O(n)遍历
    for(int i = 0;i < count; ++i){
        item = userList->item(i);
        widget = userList->itemWidget(item);
        box = static_cast<QLabel*>(widget->layout()->itemAt(0)->widget());
        button = static_cast<QPushButton*>(widget->layout()->itemAt(1)->widget());
        curName = box->text();

        if(curName == name){
            flag = true;
            if(isOnline){
                if(!isInGame){
                    button->setEnabled(true);
                    button->setText("邀请");
                    box->setEnabled(true);
                    box->setStyleSheet("QLabel{background: rgb(46, 50, 56)}"
                                             "QLabel{color: rgb(255, 255, 255)}"
                                             "QLabel:hover{background: rgb(58,63,69)}");
                } else {
                    button->setEnabled(false);
                    button->setText("游戏中");
                    box->setEnabled(false);
                    box->setStyleSheet("QLabel{background: rgb(46, 50, 56)}"
                                        "QLabel:hover{background: rgb(58,63,69)}"
                                        "QLabel{color: rgb(128, 128, 128)}");
                }

            } else {
                button->setEnabled(false);
                button->setText("离线");
                box->setEnabled(false);
                box->setStyleSheet("QLabel{background: rgb(46, 50, 56)}"
                                    "QLabel:hover{background: rgb(58,63,69)}"
                                    "QLabel{color: rgb(128, 128, 128)}");
            }
            break;
        }
    }
    qDebug() << "setUserStatus2";

    // 如果没找到username, 报错
    if(!flag){
        qDebug() << "Fatal Error: 没有这个用户！" << name;
    }
}



// 更改密码成功GUI （FINISHED）
void TcpClient::changePwdSuccessGUI(){
    errorGUI("修改密码成功");
}



// 报道成功操作 (FINISHED)
// 1. 登录成功， 进入chatRoomGUI
// 2. 注册成功，再登录一次
void TcpClient::reportSuccess(){
    auto length = my_server_to_client_report_success.get_packet_head().get_length();

    // 2. 注册成功包
    if(length == 0){
        errorGUI("注册成功，请重新登录");
        QVector<QObject*> layouts = loginWindow->layout()->children().toVector();
        QLayoutItem * item =  static_cast<QHBoxLayout*>(layouts[1])->itemAt(1);
        QLineEdit* line = static_cast<QLineEdit*>(item->widget());
        line->clear(); // 清空密码就行了
    }
    else{ // 1. 登录成功包
        if(my_server_to_client_report_success.get_packet_head().get_function_type() == PacketHead::kS2CReportSuccessDup){
            errorGUI("您把别人T了");
        }

        qDebug() << "登录成功" << this->username;
        loginWindow->close();

        //显示聊天室GUI
        chatRoomGUI();

        // 显示上次登录时间
        QString time = my_server_to_client_report_success.get_last_login_time();
        QHBoxLayout* layout = static_cast<QHBoxLayout*>(chatRoomMainLayout->itemAt(0)->layout()->itemAt(4));
        QLineEdit* line = static_cast<QLineEdit*>(layout->itemAt(1)->widget());
        line->setText(time);

        int num = my_server_to_client_report_success.get_user_num();
        qDebug() << "所有用户" << num;
        char** uinfo = my_server_to_client_report_success.get_user_status();
        char name[32];
        for(int i = 0;i < num; ++i){
            memcpy(name, uinfo[i], 32);
            qDebug() << name << " " << int(uinfo[i][32]-'0');
            if(name == username)
                continue;
            insertListWidget(QString(name), int(uinfo[i][32]-'0'), false); // TODO: 游戏状态，需要读包
        }

//        insertListWidget(QString("群聊"), true, false); // TODO：游戏状态，需要读包
    }
}


// TODO: 游戏主界面，需要重写
void TcpClient::InitGameWindow(){
      gameWindow = new QWidget;
      gameWindow->setFixedHeight(700);
      gameWindow->setMinimumWidth(1100);

      auto layout = new QVBoxLayout;

      // opponame
      QLabel* label = new QLabel("自己:" + username + "\t对手:" + opponame + "\t");
      label->setAlignment(Qt::AlignCenter);
      label->setFixedHeight(50);
      label->setStyleSheet("background: white; color: black;border-bottom: 1px solid rgb(230, 230, 255);");

      QFont ft;
      ft.setPointSize(15);
      label->setFont(ft);
      layout->addWidget(label);

      // board
      auto board_layout = new QHBoxLayout;
      my_board = new QTableWidget;
      my_board->setRowCount(BOARD_SIZE);
      my_board->setColumnCount(BOARD_SIZE);
//      my_board->setFixedSize(200, 200);
      my_board->setHorizontalHeaderLabels({"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"});
      my_board->setVerticalHeaderLabels({"A", "B", "C", "D", "E", "F", "G", "H", "I", "J"});

      for (int i = 0;i < BOARD_SIZE; ++i) {
        my_board->setRowHeight(i, 50);
        my_board->setColumnWidth(i, 50);
      }

      for(int i = 0;i < BOARD_SIZE; ++i) {
          for(int j = 0;j < BOARD_SIZE; ++j) {
                my_board->setItem(i,j, new QTableWidgetItem);
          }
      }
      my_board->setMouseTracking(true);
      my_board->setEditTriggers(QTableWidget::NoEditTriggers);

      connect(my_board, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(my_board_doubleclicked(int, int)));
      connect(my_board, SIGNAL(cellEntered(int, int)), this, SLOT(my_board_entered(int, int)));
      board_layout->addWidget(my_board);


      oppo_board = new QTableWidget;
      oppo_board->setRowCount(BOARD_SIZE);
      oppo_board->setColumnCount(BOARD_SIZE);
//      my_board->setFixedSize(200, 200);
      oppo_board->setHorizontalHeaderLabels({"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"});
      oppo_board->setVerticalHeaderLabels({"A", "B", "C", "D", "E", "F", "G", "H", "I", "J"});

      for (int i = 0;i < BOARD_SIZE; ++i) {
          oppo_board->setRowHeight(i, 50);
          oppo_board->setColumnWidth(i, 50);
      }
      for(int i = 0;i < BOARD_SIZE; ++i) {
          for(int j = 0;j < BOARD_SIZE; ++j) {
                oppo_board->setItem(i,j, new QTableWidgetItem);
          }
      }
      oppo_board->setEditTriggers(QTableWidget::NoEditTriggers);
      connect(oppo_board, SIGNAL(cellClicked(int, int)), this, SLOT(oppo_board_clicked(int, int)));
      connect(oppo_board, SIGNAL(cellEntered(int, int)), this, SLOT(oppo_board_entered(int, int)));
      oppo_board->setMouseTracking(true);
      oppo_board->setEnabled(false);
      board_layout->addWidget(oppo_board);

      layout->addLayout(board_layout);

      // 控制键
      auto control_layout = new QHBoxLayout;

      // 摆放完成
      auto my_plane_done = new QPushButton("完成摆放");
      my_plane_done->setFixedSize(100, 20);
      my_plane_done->setEnabled(false);
      connect(my_plane_done, SIGNAL(clicked()), this, SLOT(gameReady()), Qt::QueuedConnection);
      control_layout->addWidget(my_plane_done);

      // 上下左右
      direction_index = 0;
      auto direction = new QComboBox;

      direction->setFixedSize(100, 20);
      direction->addItem("上");
      direction->addItem("下");
      direction->addItem("左");
      direction->addItem("右");

      connect(direction, SIGNAL(currentIndexChanged(int)), this, SLOT(direction_on_changed(int)));

      control_layout->addWidget(direction);

      auto ack_button = new QPushButton("确认猜测");
      ack_button->setFixedSize(100, 20);
      ack_button->setEnabled(false);
      connect(ack_button, SIGNAL(clicked()), this, SLOT(on_ackbtn_clicked()), Qt::QueuedConnection);
      control_layout->addWidget(ack_button);

      auto sublayout = new QHBoxLayout;
      auto buttonGroup = new QButtonGroup;
      auto button1 = new QPushButton("猜测");
      auto button2 = new QPushButton("断言");
      button1->setCheckable(true);
      button2->setCheckable(true);
      buttonGroup->setExclusive(true);
      buttonGroup->addButton(button1);
      buttonGroup->addButton(button2);
      button1->setChecked(true);
      button1->setFixedSize(100, 20);
      button2->setFixedSize(100, 20);
      connect(button1, SIGNAL(clicked()), this, SLOT(on_setstate()));
      connect(button2, SIGNAL(clicked()), this, SLOT(on_setstate()));
      sublayout->addWidget(button1);
      sublayout->addWidget(button2);

      control_layout->addLayout(sublayout);



      layout->setSpacing(0);
      layout->addLayout(control_layout);

      gameWindow->setLayout(layout);



      cur_row = -1;
      cur_column = -1;
      original_color = my_board->item(0,0)->backgroundColor();
      pre_not_valid = true;
      pre_not_valid_oppo = true;
      isGuess = true;

      // 初始化
      for(int i = 0;i < BOARD_SIZE; ++i){
          for(int j = 0;j < BOARD_SIZE; ++j){
              valid_board[i][j] = 0;
              guess_board[i][j] = 0;
          }
      }

      my_plane_cnt = 0;
      oppo_plane_cnt = 0;
      guess_right_cnt = 0;

      gameWindow->show();
}

void TcpClient::setPlane(int row, int column){
    qDebug() << "我进来了呀\n";
    cur_row = row;
    cur_column = column;
    auto random_color = PLANE_COLOR[my_plane_cnt];
    // head
    my_board->item(cur_row, cur_column)->setBackgroundColor(random_color);
    valid_board[cur_row][cur_column] = 2;


    int temp_board[BOARD_SIZE][BOARD_SIZE];
    for(int i = 0; i < BOARD_SIZE; ++i) {
        for(int j = 0; j < BOARD_SIZE; ++j){
            temp_board[i][j] = 0;
        }
    }
    temp_board[cur_row][cur_column] = 1;

    // 上
    if(direction_index == 0) {
        // 位置合法
        if(cur_column - 2 >= 0 && cur_column + 2 <= 9 && cur_row + 3 <= 9) {
            temp_board[cur_row+1][cur_column-2] = 1;
            temp_board[cur_row+1][cur_column-1] = 1;
            temp_board[cur_row+1][cur_column] = 1;
            temp_board[cur_row+1][cur_column+1] = 1;
            temp_board[cur_row+1][cur_column+2] = 1;
            temp_board[cur_row+2][cur_column] = 1;
            temp_board[cur_row+3][cur_column-1] = 1;
            temp_board[cur_row+3][cur_column] = 1;
            temp_board[cur_row+3][cur_column+1] = 1;

            int sum = 0;
            for(int i = 0; i < BOARD_SIZE; ++i){
                for(int j = 0;j < BOARD_SIZE; ++j){
                    sum += valid_board[i][j] & temp_board[i][j];
                }
            }

            if(!sum){
                my_board->item(cur_row+1, cur_column-2)->setBackgroundColor(random_color);
                my_board->item(cur_row+1, cur_column-1)->setBackgroundColor(random_color);
                my_board->item(cur_row+1, cur_column)->setBackgroundColor(random_color);
                my_board->item(cur_row+1, cur_column+1)->setBackgroundColor(random_color);
                my_board->item(cur_row+1, cur_column+2)->setBackgroundColor(random_color);
                my_board->item(cur_row+2, cur_column)->setBackgroundColor(random_color);
                my_board->item(cur_row+3, cur_column-1)->setBackgroundColor(random_color);
                my_board->item(cur_row+3, cur_column)->setBackgroundColor(random_color);
                my_board->item(cur_row+3, cur_column+1)->setBackgroundColor(random_color);
                valid_board[cur_row+1][cur_column-2] = 1;
                valid_board[cur_row+1][cur_column-1] = 1;
                valid_board[cur_row+1][cur_column] = 1;
                valid_board[cur_row+1][cur_column+1] = 1;
                valid_board[cur_row+1][cur_column+2] = 1;
                valid_board[cur_row+2][cur_column] = 1;
                valid_board[cur_row+3][cur_column-1] = 1;
                valid_board[cur_row+3][cur_column] = 1;
                valid_board[cur_row+3][cur_column+1] = 1;
                my_plane_loc[my_plane_cnt][0] = cur_row * BOARD_SIZE + cur_column;
                my_plane_loc[my_plane_cnt][1] = (cur_row + 3) * BOARD_SIZE + cur_column;
                my_plane_cnt += 1;
            }else{
                my_board->item(cur_row, cur_column)->setBackgroundColor(original_color);
                valid_board[cur_row][cur_column] = 0;
            }
        } else {
            my_board->item(cur_row, cur_column)->setBackgroundColor(original_color);
            valid_board[cur_row][cur_column] = 0;
        }
    } else if(direction_index == 1) { // 下
        if(cur_column - 2 >= 0 && cur_column + 2 <= 9 && cur_row - 3 >= 0) {
            temp_board[cur_row-1][cur_column-2] = 1;
            temp_board[cur_row-1][cur_column-1] = 1;
            temp_board[cur_row-1][cur_column] = 1;
            temp_board[cur_row-1][cur_column+1] = 1;
            temp_board[cur_row-1][cur_column+2] = 1;
            temp_board[cur_row-2][cur_column] = 1;
            temp_board[cur_row-3][cur_column-1] = 1;
            temp_board[cur_row-3][cur_column] = 1;
            temp_board[cur_row-3][cur_column+1] = 1;

            int sum = 0;
            for(int i = 0;i < BOARD_SIZE; ++i){
                for(int j = 0;j < BOARD_SIZE; ++j){
                    sum += valid_board[i][j] & temp_board[i][j];
                }
            }

            if(!sum){
                my_board->item(cur_row-1, cur_column-2)->setBackgroundColor(random_color);
                my_board->item(cur_row-1, cur_column-1)->setBackgroundColor(random_color);
                my_board->item(cur_row-1, cur_column)->setBackgroundColor(random_color);
                my_board->item(cur_row-1, cur_column+1)->setBackgroundColor(random_color);
                my_board->item(cur_row-1, cur_column+2)->setBackgroundColor(random_color);
                my_board->item(cur_row-2, cur_column)->setBackgroundColor(random_color);
                my_board->item(cur_row-3, cur_column-1)->setBackgroundColor(random_color);
                my_board->item(cur_row-3, cur_column)->setBackgroundColor(random_color);
                my_board->item(cur_row-3, cur_column+1)->setBackgroundColor(random_color);
                valid_board[cur_row-1][cur_column-2] = 1;
                valid_board[cur_row-1][cur_column-1] = 1;
                valid_board[cur_row-1][cur_column] = 1;
                valid_board[cur_row-1][cur_column+1] = 1;
                valid_board[cur_row-1][cur_column+2] = 1;
                valid_board[cur_row-2][cur_column] = 1;
                valid_board[cur_row-3][cur_column-1] = 1;
                valid_board[cur_row-3][cur_column] = 1;
                valid_board[cur_row-3][cur_column+1] = 1;
                my_plane_loc[my_plane_cnt][0] = cur_row * BOARD_SIZE + cur_column;
                my_plane_loc[my_plane_cnt][1] = (cur_row - 3) * BOARD_SIZE + cur_column;
                my_plane_cnt += 1;
            } else{
                my_board->item(cur_row, cur_column)->setBackgroundColor(original_color);
                valid_board[cur_row][cur_column] = 0;
            }
        } else {
            my_board->item(cur_row, cur_column)->setBackgroundColor(original_color);
            valid_board[cur_row][cur_column] = 0;
        }
    } else if(direction_index == 2) { // 左
        if(cur_column + 3 <= 9 && cur_row - 2 >= 0 && cur_row + 2 <= 9) {
            temp_board[cur_row+2][cur_column+1] = 1;
            temp_board[cur_row+1][cur_column+1] = 1;
            temp_board[cur_row][cur_column+1] = 1;
            temp_board[cur_row-1][cur_column+1] = 1;
            temp_board[cur_row-2][cur_column+1] = 1;
            temp_board[cur_row][cur_column+2] = 1;
            temp_board[cur_row-1][cur_column+3] = 1;
            temp_board[cur_row][cur_column+3] = 1;
            temp_board[cur_row+1][cur_column+3] = 1;

            int sum = 0;
            for(int i = 0;i < BOARD_SIZE; ++i){
                for(int j = 0;j < BOARD_SIZE; ++j){
                    sum += valid_board[i][j] & temp_board[i][j];
                }
            }

            if(!sum){
                my_board->item(cur_row+2, cur_column+1)->setBackgroundColor(random_color);
                my_board->item(cur_row+1, cur_column+1)->setBackgroundColor(random_color);
                my_board->item(cur_row, cur_column+1)->setBackgroundColor(random_color);
                my_board->item(cur_row-1, cur_column+1)->setBackgroundColor(random_color);
                my_board->item(cur_row-2, cur_column+1)->setBackgroundColor(random_color);
                my_board->item(cur_row, cur_column+2)->setBackgroundColor(random_color);
                my_board->item(cur_row-1, cur_column+3)->setBackgroundColor(random_color);
                my_board->item(cur_row, cur_column+3)->setBackgroundColor(random_color);
                my_board->item(cur_row+1, cur_column+3)->setBackgroundColor(random_color);
                valid_board[cur_row+2][cur_column+1] = 1;
                valid_board[cur_row+1][cur_column+1] = 1;
                valid_board[cur_row][cur_column+1] = 1;
                valid_board[cur_row-1][cur_column+1] = 1;
                valid_board[cur_row-2][cur_column+1] = 1;
                valid_board[cur_row][cur_column+2] = 1;
                valid_board[cur_row-1][cur_column+3] = 1;
                valid_board[cur_row][cur_column+3] = 1;
                valid_board[cur_row+1][cur_column+3] = 1;
                my_plane_loc[my_plane_cnt][0] = cur_row * BOARD_SIZE + cur_column;
                my_plane_loc[my_plane_cnt][1] = cur_row * BOARD_SIZE + (cur_column + 3);
                my_plane_cnt += 1;
            } else{
                my_board->item(cur_row, cur_column)->setBackgroundColor(original_color);
                valid_board[cur_row][cur_column] = 0;
            }
        } else {
            my_board->item(cur_row, cur_column)->setBackgroundColor(original_color);
            valid_board[cur_row][cur_column] = 0;
        }
    } else if(direction_index == 3) { // 右
        if(cur_column - 3 >= 0 && cur_row - 2 >= 0 && cur_row + 2 <= 9) {
            temp_board[cur_row+2][cur_column-1] = 1;
            temp_board[cur_row+1][cur_column-1] = 1;
            temp_board[cur_row][cur_column-1] = 1;
            temp_board[cur_row-1][cur_column-1] = 1;
            temp_board[cur_row-2][cur_column-1] = 1;
            temp_board[cur_row][cur_column-2] = 1;
            temp_board[cur_row-1][cur_column-3] = 1;
            temp_board[cur_row][cur_column-3] = 1;
            temp_board[cur_row+1][cur_column-3] = 1;

            int sum = 0;
            for(int i = 0;i < BOARD_SIZE; ++i){
                for(int j = 0;j < BOARD_SIZE; ++j){
                    sum += valid_board[i][j] & temp_board[i][j];
                }
            }

            if(!sum){
                my_board->item(cur_row+2, cur_column-1)->setBackgroundColor(random_color);
                my_board->item(cur_row+1, cur_column-1)->setBackgroundColor(random_color);
                my_board->item(cur_row, cur_column-1)->setBackgroundColor(random_color);
                my_board->item(cur_row-1, cur_column-1)->setBackgroundColor(random_color);
                my_board->item(cur_row-2, cur_column-1)->setBackgroundColor(random_color);
                my_board->item(cur_row, cur_column-2)->setBackgroundColor(random_color);
                my_board->item(cur_row-1, cur_column-3)->setBackgroundColor(random_color);
                my_board->item(cur_row, cur_column-3)->setBackgroundColor(random_color);
                my_board->item(cur_row+1, cur_column-3)->setBackgroundColor(random_color);
                valid_board[cur_row+2][cur_column-1] = 1;
                valid_board[cur_row+1][cur_column-1] = 1;
                valid_board[cur_row][cur_column-1] = 1;
                valid_board[cur_row-1][cur_column-1] = 1;
                valid_board[cur_row-2][cur_column-1] = 1;
                valid_board[cur_row][cur_column-2] = 1;
                valid_board[cur_row-1][cur_column-3] = 1;
                valid_board[cur_row][cur_column-3] = 1;
                valid_board[cur_row+1][cur_column-3] = 1;
                my_plane_loc[my_plane_cnt][0] = cur_row * BOARD_SIZE + cur_column;
                my_plane_loc[my_plane_cnt][1] = cur_row * BOARD_SIZE + (cur_column - 3);
                my_plane_cnt += 1;
            } else {
                my_board->item(cur_row, cur_column)->setBackgroundColor(original_color);
                valid_board[cur_row][cur_column] = 0;
            }
        } else {
            my_board->item(cur_row, cur_column)->setBackgroundColor(original_color);
            valid_board[cur_row][cur_column] = 0;
        }
    }

    cur_row = -1;
    cur_column = -1;

    // 摆放结束
    if(my_plane_cnt == 3){
        auto button = static_cast<QPushButton*>(gameWindow->layout()->itemAt(2)->layout()->itemAt(0)->widget());
        button->setEnabled(true);
        my_board->setCurrentCell(-1,-1);
        my_board->setEnabled(false);
    }
}

void TcpClient::setOppoPlane(int row, int column){
        cur_oppo_row = row;
        cur_oppo_column = column;
        if(oppo_plane_cnt >= 3){
            return;
        }
        auto random_color = PLANE_COLOR[oppo_plane_cnt];
        // head
        auto color = oppo_board->item(cur_oppo_row, cur_oppo_column)->backgroundColor();
        oppo_board->item(cur_oppo_row, cur_oppo_column)->setBackgroundColor(random_color);
        guess_board[cur_oppo_row][cur_oppo_column] = 2;


        int temp_board[BOARD_SIZE][BOARD_SIZE];
        for(int i = 0; i < BOARD_SIZE; ++i) {
            for(int j = 0; j < BOARD_SIZE; ++j){
                temp_board[i][j] = 0;
            }
        }
        temp_board[cur_oppo_row][cur_oppo_column] = 1;

        // 上
        if(direction_index == 0) {
            // 位置合法
            if(cur_oppo_column - 2 >= 0 && cur_oppo_column + 2 <= 9 && cur_oppo_row + 3 <= 9) {
                temp_board[cur_oppo_row+1][cur_oppo_column-2] = 1;
                temp_board[cur_oppo_row+1][cur_oppo_column-1] = 1;
                temp_board[cur_oppo_row+1][cur_oppo_column] = 1;
                temp_board[cur_oppo_row+1][cur_oppo_column+1] = 1;
                temp_board[cur_oppo_row+1][cur_oppo_column+2] = 1;
                temp_board[cur_oppo_row+2][cur_oppo_column] = 1;
                temp_board[cur_oppo_row+3][cur_oppo_column-1] = 1;
                temp_board[cur_oppo_row+3][cur_oppo_column] = 1;
                temp_board[cur_oppo_row+3][cur_oppo_column+1] = 1;

                int sum = 0;
                for(int i = 0; i < BOARD_SIZE; ++i){
                    for(int j = 0;j < BOARD_SIZE; ++j){
                        sum += guess_board[i][j] & temp_board[i][j];
                    }
                }
                qDebug() << "Sum:" << sum;


                if(!sum){
                    oppo_board->item(cur_oppo_row+1, cur_oppo_column-2)->setBackgroundColor(random_color);
                    oppo_board->item(cur_oppo_row+1, cur_oppo_column-1)->setBackgroundColor(random_color);
                    oppo_board->item(cur_oppo_row+1, cur_oppo_column)->setBackgroundColor(random_color);
                    oppo_board->item(cur_oppo_row+1, cur_oppo_column+1)->setBackgroundColor(random_color);
                    oppo_board->item(cur_oppo_row+1, cur_oppo_column+2)->setBackgroundColor(random_color);
                    oppo_board->item(cur_oppo_row+2, cur_oppo_column)->setBackgroundColor(random_color);
                    oppo_board->item(cur_oppo_row+3, cur_oppo_column-1)->setBackgroundColor(random_color);
                    oppo_board->item(cur_oppo_row+3, cur_oppo_column)->setBackgroundColor(random_color);
                    oppo_board->item(cur_oppo_row+3, cur_oppo_column+1)->setBackgroundColor(random_color);
                    guess_board[cur_oppo_row+1][cur_oppo_column-2] = 1;
                    guess_board[cur_oppo_row+1][cur_oppo_column-1] = 1;
                    guess_board[cur_oppo_row+1][cur_oppo_column] = 1;
                    guess_board[cur_oppo_row+1][cur_oppo_column+1] = 1;
                    guess_board[cur_oppo_row+1][cur_oppo_column+2] = 1;
                    guess_board[cur_oppo_row+2][cur_oppo_column] = 1;
                    guess_board[cur_oppo_row+3][cur_oppo_column-1] = 1;
                    guess_board[cur_oppo_row+3][cur_oppo_column] = 1;
                    guess_board[cur_oppo_row+3][cur_oppo_column+1] = 1;
                    guess_loc[0] = cur_oppo_row * BOARD_SIZE + cur_oppo_column;
                    guess_loc[1] = (cur_oppo_row + 3) * BOARD_SIZE + cur_oppo_column;
                    oppo_plane_cnt += 1;
                    oppo_board->setEnabled(false);
                }else{
                    oppo_board->item(cur_oppo_row, cur_oppo_column)->setBackgroundColor(color);
                    guess_board[cur_oppo_row][cur_oppo_column] = 0;
                }
            } else {
                oppo_board->item(cur_oppo_row, cur_oppo_column)->setBackgroundColor(color);
                guess_board[cur_oppo_row][cur_oppo_column] = 0;
            }
        } else if(direction_index == 1) { // 下
            if(cur_oppo_column - 2 >= 0 && cur_oppo_column + 2 <= 9 && cur_oppo_row - 3 >= 0) {
                temp_board[cur_oppo_row-1][cur_oppo_column-2] = 1;
                temp_board[cur_oppo_row-1][cur_oppo_column-1] = 1;
                temp_board[cur_oppo_row-1][cur_oppo_column] = 1;
                temp_board[cur_oppo_row-1][cur_oppo_column+1] = 1;
                temp_board[cur_oppo_row-1][cur_oppo_column+2] = 1;
                temp_board[cur_oppo_row-2][cur_oppo_column] = 1;
                temp_board[cur_oppo_row-3][cur_oppo_column-1] = 1;
                temp_board[cur_oppo_row-3][cur_oppo_column] = 1;
                temp_board[cur_oppo_row-3][cur_oppo_column+1] = 1;

                int sum = 0;
                for(int i = 0;i < BOARD_SIZE; ++i){
                    for(int j = 0;j < BOARD_SIZE; ++j){
                        sum += guess_board[i][j] & temp_board[i][j];
                    }
                }

                if(!sum){
                    oppo_board->item(cur_oppo_row-1, cur_oppo_column-2)->setBackgroundColor(random_color);
                    oppo_board->item(cur_oppo_row-1, cur_oppo_column-1)->setBackgroundColor(random_color);
                    oppo_board->item(cur_oppo_row-1, cur_oppo_column)->setBackgroundColor(random_color);
                    oppo_board->item(cur_oppo_row-1, cur_oppo_column+1)->setBackgroundColor(random_color);
                    oppo_board->item(cur_oppo_row-1, cur_oppo_column+2)->setBackgroundColor(random_color);
                    oppo_board->item(cur_oppo_row-2, cur_oppo_column)->setBackgroundColor(random_color);
                    oppo_board->item(cur_oppo_row-3, cur_oppo_column-1)->setBackgroundColor(random_color);
                    oppo_board->item(cur_oppo_row-3, cur_oppo_column)->setBackgroundColor(random_color);
                    oppo_board->item(cur_oppo_row-3, cur_oppo_column+1)->setBackgroundColor(random_color);
                    guess_board[cur_oppo_row-1][cur_oppo_column-2] = 1;
                    guess_board[cur_oppo_row-1][cur_oppo_column-1] = 1;
                    guess_board[cur_oppo_row-1][cur_oppo_column] = 1;
                    guess_board[cur_oppo_row-1][cur_oppo_column+1] = 1;
                    guess_board[cur_oppo_row-1][cur_oppo_column+2] = 1;
                    guess_board[cur_oppo_row-2][cur_oppo_column] = 1;
                    guess_board[cur_oppo_row-3][cur_oppo_column-1] = 1;
                    guess_board[cur_oppo_row-3][cur_oppo_column] = 1;
                    guess_board[cur_oppo_row-3][cur_oppo_column+1] = 1;
                    guess_loc[0] = cur_oppo_row * BOARD_SIZE + cur_oppo_column;
                    guess_loc[1] = (cur_oppo_row - 3) * BOARD_SIZE + cur_oppo_column;
                    oppo_plane_cnt += 1;
                    oppo_board->setEnabled(false);
                } else{
                    oppo_board->item(cur_oppo_row, cur_oppo_column)->setBackgroundColor(color);
                    guess_board[cur_oppo_row][cur_oppo_column] = 0;
                }
            } else {
                oppo_board->item(cur_oppo_row, cur_oppo_column)->setBackgroundColor(color);
                guess_board[cur_oppo_row][cur_oppo_column] = 0;
            }
        } else if(direction_index == 2) { // 左
            if(cur_oppo_column + 3 <= 9 && cur_oppo_row - 2 >= 0 && cur_oppo_row + 2 <= 9) {
                temp_board[cur_oppo_row+2][cur_oppo_column+1] = 1;
                temp_board[cur_oppo_row+1][cur_oppo_column+1] = 1;
                temp_board[cur_oppo_row][cur_oppo_column+1] = 1;
                temp_board[cur_oppo_row-1][cur_oppo_column+1] = 1;
                temp_board[cur_oppo_row-2][cur_oppo_column+1] = 1;
                temp_board[cur_oppo_row][cur_oppo_column+2] = 1;
                temp_board[cur_oppo_row-1][cur_oppo_column+3] = 1;
                temp_board[cur_oppo_row][cur_oppo_column+3] = 1;
                temp_board[cur_oppo_row+1][cur_oppo_column+3] = 1;

                int sum = 0;
                for(int i = 0;i < BOARD_SIZE; ++i){
                    for(int j = 0;j < BOARD_SIZE; ++j){
                        sum += guess_board[i][j] & temp_board[i][j];
                    }
                }

                if(!sum){
                    oppo_board->item(cur_oppo_row+2, cur_oppo_column+1)->setBackgroundColor(random_color);
                    oppo_board->item(cur_oppo_row+1, cur_oppo_column+1)->setBackgroundColor(random_color);
                    oppo_board->item(cur_oppo_row, cur_oppo_column+1)->setBackgroundColor(random_color);
                    oppo_board->item(cur_oppo_row-1, cur_oppo_column+1)->setBackgroundColor(random_color);
                    oppo_board->item(cur_oppo_row-2, cur_oppo_column+1)->setBackgroundColor(random_color);
                    oppo_board->item(cur_oppo_row, cur_oppo_column+2)->setBackgroundColor(random_color);
                    oppo_board->item(cur_oppo_row-1, cur_oppo_column+3)->setBackgroundColor(random_color);
                    oppo_board->item(cur_oppo_row, cur_oppo_column+3)->setBackgroundColor(random_color);
                    oppo_board->item(cur_oppo_row+1, cur_oppo_column+3)->setBackgroundColor(random_color);
                    guess_board[cur_oppo_row+2][cur_oppo_column+1] = 1;
                    guess_board[cur_oppo_row+1][cur_oppo_column+1] = 1;
                    guess_board[cur_oppo_row][cur_oppo_column+1] = 1;
                    guess_board[cur_oppo_row-1][cur_oppo_column+1] = 1;
                    guess_board[cur_oppo_row-2][cur_oppo_column+1] = 1;
                    guess_board[cur_oppo_row][cur_oppo_column+2] = 1;
                    guess_board[cur_oppo_row-1][cur_oppo_column+3] = 1;
                    guess_board[cur_oppo_row][cur_oppo_column+3] = 1;
                    guess_board[cur_oppo_row+1][cur_oppo_column+3] = 1;
                    guess_loc[0] = cur_oppo_row * BOARD_SIZE + cur_oppo_column;
                    guess_loc[1] = cur_oppo_row * BOARD_SIZE + (cur_oppo_column + 3);
                    oppo_plane_cnt += 1;
                    oppo_board->setEnabled(false);
                } else{
                    oppo_board->item(cur_oppo_row, cur_oppo_column)->setBackgroundColor(color);
                    guess_board[cur_oppo_row][cur_oppo_column] = 0;
                }
            } else {
                oppo_board->item(cur_oppo_row, cur_oppo_column)->setBackgroundColor(color);
                guess_board[cur_oppo_row][cur_oppo_column] = 0;
            }
        } else if(direction_index == 3) { // 右
            if(cur_oppo_column - 3 >= 0 && cur_oppo_row - 2 >= 0 && cur_oppo_row + 2 <= 9) {
                temp_board[cur_oppo_row+2][cur_oppo_column-1] = 1;
                temp_board[cur_oppo_row+1][cur_oppo_column-1] = 1;
                temp_board[cur_oppo_row][cur_oppo_column-1] = 1;
                temp_board[cur_oppo_row-1][cur_oppo_column-1] = 1;
                temp_board[cur_oppo_row-2][cur_oppo_column-1] = 1;
                temp_board[cur_oppo_row][cur_oppo_column-2] = 1;
                temp_board[cur_oppo_row-1][cur_oppo_column-3] = 1;
                temp_board[cur_oppo_row][cur_oppo_column-3] = 1;
                temp_board[cur_oppo_row+1][cur_oppo_column-3] = 1;

                int sum = 0;
                for(int i = 0;i < BOARD_SIZE; ++i){
                    for(int j = 0;j < BOARD_SIZE; ++j){
                        sum += guess_board[i][j] & temp_board[i][j];
                    }
                }

                if(!sum){
                    oppo_board->item(cur_oppo_row+2, cur_oppo_column-1)->setBackgroundColor(random_color);
                    oppo_board->item(cur_oppo_row+1, cur_oppo_column-1)->setBackgroundColor(random_color);
                    oppo_board->item(cur_oppo_row, cur_oppo_column-1)->setBackgroundColor(random_color);
                    oppo_board->item(cur_oppo_row-1, cur_oppo_column-1)->setBackgroundColor(random_color);
                    oppo_board->item(cur_oppo_row-2, cur_oppo_column-1)->setBackgroundColor(random_color);
                    oppo_board->item(cur_oppo_row, cur_oppo_column-2)->setBackgroundColor(random_color);
                    oppo_board->item(cur_oppo_row-1, cur_oppo_column-3)->setBackgroundColor(random_color);
                    oppo_board->item(cur_oppo_row, cur_oppo_column-3)->setBackgroundColor(random_color);
                    oppo_board->item(cur_oppo_row+1, cur_oppo_column-3)->setBackgroundColor(random_color);
                    guess_board[cur_oppo_row+2][cur_oppo_column-1] = 1;
                    guess_board[cur_oppo_row+1][cur_oppo_column-1] = 1;
                    guess_board[cur_oppo_row][cur_oppo_column-1] = 1;
                    guess_board[cur_oppo_row-1][cur_oppo_column-1] = 1;
                    guess_board[cur_oppo_row-2][cur_oppo_column-1] = 1;
                    guess_board[cur_oppo_row][cur_oppo_column-2] = 1;
                    guess_board[cur_oppo_row-1][cur_oppo_column-3] = 1;
                    guess_board[cur_oppo_row][cur_oppo_column-3] = 1;
                    guess_board[cur_oppo_row+1][cur_oppo_column-3] = 1;
                    guess_loc[0] = cur_oppo_row * BOARD_SIZE + cur_oppo_column;
                    guess_loc[1] = cur_oppo_row * BOARD_SIZE + (cur_oppo_column - 3);
                    oppo_plane_cnt += 1;
                    oppo_board->setEnabled(false);
                } else {
                    oppo_board->item(cur_oppo_row, cur_oppo_column)->setBackgroundColor(color);
                    guess_board[cur_oppo_row][cur_oppo_column] = 0;
                }
            } else {
                oppo_board->item(cur_oppo_row, cur_oppo_column)->setBackgroundColor(color);
                guess_board[cur_oppo_row][cur_oppo_column] = 0;
            }
        }

        cur_oppo_row = -1;
        cur_oppo_column = -1;
        oppo_board->setCurrentCell(cur_oppo_row, cur_oppo_column);
}

void TcpClient::previewPlane(int row, int column){
    qDebug() << "i am here\n" << row << " " << column;
    if(row != cur_row || column != cur_column) {
        // 需要恢复之前的
        if(!(cur_row == -1 && cur_column == -1) && !pre_not_valid){
            // head
            my_board->item(cur_row, cur_column)->setBackgroundColor(original_color);
            // 上
            if(direction_index == 0) {
                // 位置合法
                if(cur_column - 2 >= 0 && cur_column + 2 <= 9 && cur_row + 3 <= 9) {
                    my_board->item(cur_row+1, cur_column-2)->setBackgroundColor(original_color);
                    my_board->item(cur_row+1, cur_column-1)->setBackgroundColor(original_color);
                    my_board->item(cur_row+1, cur_column)->setBackgroundColor(original_color);
                    my_board->item(cur_row+1, cur_column+1)->setBackgroundColor(original_color);
                    my_board->item(cur_row+1, cur_column+2)->setBackgroundColor(original_color);
                    my_board->item(cur_row+2, cur_column)->setBackgroundColor(original_color);
                    my_board->item(cur_row+3, cur_column-1)->setBackgroundColor(original_color);
                    my_board->item(cur_row+3, cur_column)->setBackgroundColor(original_color);
                    my_board->item(cur_row+3, cur_column+1)->setBackgroundColor(original_color);
                } else {
                }
            } else if(direction_index == 1) { // 下
                if(cur_column - 2 >= 0 && cur_column + 2 <= 9 && cur_row - 3 >= 0) {
                    my_board->item(cur_row-1, cur_column-2)->setBackgroundColor(original_color);
                    my_board->item(cur_row-1, cur_column-1)->setBackgroundColor(original_color);
                    my_board->item(cur_row-1, cur_column)->setBackgroundColor(original_color);
                    my_board->item(cur_row-1, cur_column+1)->setBackgroundColor(original_color);
                    my_board->item(cur_row-1, cur_column+2)->setBackgroundColor(original_color);
                    my_board->item(cur_row-2, cur_column)->setBackgroundColor(original_color);
                    my_board->item(cur_row-3, cur_column-1)->setBackgroundColor(original_color);
                    my_board->item(cur_row-3, cur_column)->setBackgroundColor(original_color);
                    my_board->item(cur_row-3, cur_column+1)->setBackgroundColor(original_color);
                } else {
                }
            } else if(direction_index == 2) { // 左
                if(cur_column + 3 <= 9 && cur_row - 2 >= 0 && cur_row + 2 <= 9) {
                    my_board->item(cur_row+2, cur_column+1)->setBackgroundColor(original_color);
                    my_board->item(cur_row+1, cur_column+1)->setBackgroundColor(original_color);
                    my_board->item(cur_row, cur_column+1)->setBackgroundColor(original_color);
                    my_board->item(cur_row-1, cur_column+1)->setBackgroundColor(original_color);
                    my_board->item(cur_row-2, cur_column+1)->setBackgroundColor(original_color);
                    my_board->item(cur_row, cur_column+2)->setBackgroundColor(original_color);
                    my_board->item(cur_row-1, cur_column+3)->setBackgroundColor(original_color);
                    my_board->item(cur_row, cur_column+3)->setBackgroundColor(original_color);
                    my_board->item(cur_row+1, cur_column+3)->setBackgroundColor(original_color);
                } else {
                }
            } else if(direction_index == 3) { // 右
                if(cur_column - 3 >= 0 && cur_row - 2 >= 0 && cur_row + 2 <= 9) {
                    my_board->item(cur_row+2, cur_column-1)->setBackgroundColor(original_color);
                    my_board->item(cur_row+1, cur_column-1)->setBackgroundColor(original_color);
                    my_board->item(cur_row, cur_column-1)->setBackgroundColor(original_color);
                    my_board->item(cur_row-1, cur_column-1)->setBackgroundColor(original_color);
                    my_board->item(cur_row-2, cur_column-1)->setBackgroundColor(original_color);
                    my_board->item(cur_row, cur_column-2)->setBackgroundColor(original_color);
                    my_board->item(cur_row-1, cur_column-3)->setBackgroundColor(original_color);
                    my_board->item(cur_row, cur_column-3)->setBackgroundColor(original_color);
                    my_board->item(cur_row+1, cur_column-3)->setBackgroundColor(original_color);
                } else {
                }
            }
        }
        cur_row = row;
        cur_column = column;

        int temp_board[BOARD_SIZE][BOARD_SIZE];
        for(int i = 0;i < BOARD_SIZE; ++i){
            for(int j = 0;j < BOARD_SIZE; ++j){
                temp_board[i][j] = 0;
            }
        }
        pre_not_valid = false;

        // head
        auto color = my_board->item(cur_row, cur_column)->backgroundColor();
        my_board->item(cur_row, cur_column)->setBackgroundColor(QColor("gray"));
        temp_board[cur_row][cur_column] = 1;
        // 上
        if(direction_index == 0) {
            // 位置合法
            if(cur_column - 2 >= 0 && cur_column + 2 <= 9 && cur_row + 3 <= 9) {
                temp_board[cur_row+1][cur_column-2] = 1;
                temp_board[cur_row+1][cur_column-1] = 1;
                temp_board[cur_row+1][cur_column] = 1;
                temp_board[cur_row+1][cur_column+1] = 1;
                temp_board[cur_row+1][cur_column+2] = 1;
                temp_board[cur_row+2][cur_column] = 1;
                temp_board[cur_row+3][cur_column-1] = 1;
                temp_board[cur_row+3][cur_column] = 1;
                temp_board[cur_row+3][cur_column+1] = 1;

                int sum = 0;
                for(int i = 0;i < BOARD_SIZE; ++i){
                    for(int j = 0;j < BOARD_SIZE; ++j){
                        sum += valid_board[i][j] & temp_board[i][j];
                    }
                }

                if(!sum){
                    my_board->item(cur_row+1, cur_column-2)->setBackgroundColor(QColor("gray"));
                    my_board->item(cur_row+1, cur_column-1)->setBackgroundColor(QColor("gray"));
                    my_board->item(cur_row+1, cur_column)->setBackgroundColor(QColor("gray"));
                    my_board->item(cur_row+1, cur_column+1)->setBackgroundColor(QColor("gray"));
                    my_board->item(cur_row+1, cur_column+2)->setBackgroundColor(QColor("gray"));
                    my_board->item(cur_row+2, cur_column)->setBackgroundColor(QColor("gray"));
                    my_board->item(cur_row+3, cur_column-1)->setBackgroundColor(QColor("gray"));
                    my_board->item(cur_row+3, cur_column)->setBackgroundColor(QColor("gray"));
                    my_board->item(cur_row+3, cur_column+1)->setBackgroundColor(QColor("gray"));
                } else {
                     my_board->item(cur_row, cur_column)->setBackgroundColor(color);
                     pre_not_valid = true;
                }
            } else {
                my_board->item(cur_row, cur_column)->setBackgroundColor(color);
                pre_not_valid = true;
            }
        } else if(direction_index == 1) { // 下
            if(cur_column - 2 >= 0 && cur_column + 2 <= 9 && cur_row - 3 >= 0) {
                temp_board[cur_row-1][cur_column-2] = 1;
                temp_board[cur_row-1][cur_column-1] = 1;
                temp_board[cur_row-1][cur_column] = 1;
                temp_board[cur_row-1][cur_column+1] = 1;
                temp_board[cur_row-1][cur_column+2] = 1;
                temp_board[cur_row-2][cur_column] = 1;
                temp_board[cur_row-3][cur_column-1] = 1;
                temp_board[cur_row-3][cur_column] = 1;
                temp_board[cur_row-3][cur_column+1] = 1;

                int sum = 0;
                for(int i = 0;i < BOARD_SIZE; ++i){
                    for(int j = 0;j < BOARD_SIZE; ++j){
                        sum += valid_board[i][j] & temp_board[i][j];
                    }
                }

                if(!sum){
                    my_board->item(cur_row-1, cur_column-2)->setBackgroundColor(QColor("gray"));
                    my_board->item(cur_row-1, cur_column-1)->setBackgroundColor(QColor("gray"));
                    my_board->item(cur_row-1, cur_column)->setBackgroundColor(QColor("gray"));
                    my_board->item(cur_row-1, cur_column+1)->setBackgroundColor(QColor("gray"));
                    my_board->item(cur_row-1, cur_column+2)->setBackgroundColor(QColor("gray"));
                    my_board->item(cur_row-2, cur_column)->setBackgroundColor(QColor("gray"));
                    my_board->item(cur_row-3, cur_column-1)->setBackgroundColor(QColor("gray"));
                    my_board->item(cur_row-3, cur_column)->setBackgroundColor(QColor("gray"));
                    my_board->item(cur_row-3, cur_column+1)->setBackgroundColor(QColor("gray"));
                } else {
                    my_board->item(cur_row, cur_column)->setBackgroundColor(color);
                    pre_not_valid = true;
                }
            } else {
                my_board->item(cur_row, cur_column)->setBackgroundColor(color);
                pre_not_valid = true;
            }
        } else if(direction_index == 2) { // 左
            if(cur_column + 3 <= 9 && cur_row - 2 >= 0 && cur_row + 2 <= 9) {
                temp_board[cur_row+2][cur_column+1] = 1;
                temp_board[cur_row+1][cur_column+1] = 1;
                temp_board[cur_row][cur_column+1] = 1;
                temp_board[cur_row-1][cur_column+1] = 1;
                temp_board[cur_row-2][cur_column+1] = 1;
                temp_board[cur_row][cur_column+2] = 1;
                temp_board[cur_row-1][cur_column+3] = 1;
                temp_board[cur_row][cur_column+3] = 1;
                temp_board[cur_row+1][cur_column+3] = 1;

                int sum = 0;
                for(int i = 0;i < BOARD_SIZE; ++i){
                    for(int j = 0;j < BOARD_SIZE; ++j){
                        sum += valid_board[i][j] & temp_board[i][j];
                    }
                }

                if(!sum){
                    my_board->item(cur_row+2, cur_column+1)->setBackgroundColor(QColor("gray"));
                    my_board->item(cur_row+1, cur_column+1)->setBackgroundColor(QColor("gray"));
                    my_board->item(cur_row, cur_column+1)->setBackgroundColor(QColor("gray"));
                    my_board->item(cur_row-1, cur_column+1)->setBackgroundColor(QColor("gray"));
                    my_board->item(cur_row-2, cur_column+1)->setBackgroundColor(QColor("gray"));
                    my_board->item(cur_row, cur_column+2)->setBackgroundColor(QColor("gray"));
                    my_board->item(cur_row-1, cur_column+3)->setBackgroundColor(QColor("gray"));
                    my_board->item(cur_row, cur_column+3)->setBackgroundColor(QColor("gray"));
                    my_board->item(cur_row+1, cur_column+3)->setBackgroundColor(QColor("gray"));
                } else {
                    my_board->item(cur_row, cur_column)->setBackgroundColor(color);
                    pre_not_valid = true;
                }
            } else {
                my_board->item(cur_row, cur_column)->setBackgroundColor(color);
                pre_not_valid = true;
            }
        } else if(direction_index == 3) { // 右
            if(cur_column - 3 >= 0 && cur_row - 2 >= 0 && cur_row + 2 <= 9) {
                temp_board[cur_row+2][cur_column-1] = 1;
                temp_board[cur_row+1][cur_column-1] = 1;
                temp_board[cur_row][cur_column-1] = 1;
                temp_board[cur_row-1][cur_column-1] = 1;
                temp_board[cur_row-2][cur_column-1] = 1;
                temp_board[cur_row][cur_column-2] = 1;
                temp_board[cur_row-1][cur_column-3] = 1;
                temp_board[cur_row][cur_column-3] = 1;
                temp_board[cur_row+1][cur_column-3] = 1;

                int sum = 0;
                for(int i = 0;i < BOARD_SIZE; ++i){
                    for(int j = 0;j < BOARD_SIZE; ++j){
                        sum += valid_board[i][j] & temp_board[i][j];
                    }
                }

                if(!sum){
                    my_board->item(cur_row+2, cur_column-1)->setBackgroundColor(QColor("gray"));
                    my_board->item(cur_row+1, cur_column-1)->setBackgroundColor(QColor("gray"));
                    my_board->item(cur_row, cur_column-1)->setBackgroundColor(QColor("gray"));
                    my_board->item(cur_row-1, cur_column-1)->setBackgroundColor(QColor("gray"));
                    my_board->item(cur_row-2, cur_column-1)->setBackgroundColor(QColor("gray"));
                    my_board->item(cur_row, cur_column-2)->setBackgroundColor(QColor("gray"));
                    my_board->item(cur_row-1, cur_column-3)->setBackgroundColor(QColor("gray"));
                    my_board->item(cur_row, cur_column-3)->setBackgroundColor(QColor("gray"));
                    my_board->item(cur_row+1, cur_column-3)->setBackgroundColor(QColor("gray"));
                } else {
                    my_board->item(cur_row, cur_column)->setBackgroundColor(color);
                    pre_not_valid = true;
                }
            } else {
                my_board->item(cur_row, cur_column)->setBackgroundColor(color);
                pre_not_valid = true;
            }
        }
    }

}

void TcpClient::previewOppoPlane(int row, int column){
    qDebug() << "i am here\n" << row << " " << column;
        if(row != cur_oppo_row || column != cur_oppo_column) {
            // 需要恢复之前的
            if(!(cur_oppo_row == -1 && cur_oppo_column == -1) && !pre_not_valid_oppo){
                // head
                oppo_board->item(cur_oppo_row, cur_oppo_column)->setBackgroundColor(original_color);
                // 上
                if(direction_index == 0) {
                    // 位置合法
                    if(cur_oppo_column - 2 >= 0 && cur_oppo_column + 2 <= 9 && cur_oppo_row + 3 <= 9) {
                        oppo_board->item(cur_oppo_row+1, cur_oppo_column-2)->setBackgroundColor(original_color);
                        oppo_board->item(cur_oppo_row+1, cur_oppo_column-1)->setBackgroundColor(original_color);
                        oppo_board->item(cur_oppo_row+1, cur_oppo_column)->setBackgroundColor(original_color);
                        oppo_board->item(cur_oppo_row+1, cur_oppo_column+1)->setBackgroundColor(original_color);
                        oppo_board->item(cur_oppo_row+1, cur_oppo_column+2)->setBackgroundColor(original_color);
                        oppo_board->item(cur_oppo_row+2, cur_oppo_column)->setBackgroundColor(original_color);
                        oppo_board->item(cur_oppo_row+3, cur_oppo_column-1)->setBackgroundColor(original_color);
                        oppo_board->item(cur_oppo_row+3, cur_oppo_column)->setBackgroundColor(original_color);
                        oppo_board->item(cur_oppo_row+3, cur_oppo_column+1)->setBackgroundColor(original_color);
                    } else {
                    }
                } else if(direction_index == 1) { // 下
                    if(cur_oppo_column - 2 >= 0 && cur_oppo_column + 2 <= 9 && cur_oppo_row - 3 >= 0) {
                        oppo_board->item(cur_oppo_row-1, cur_oppo_column-2)->setBackgroundColor(original_color);
                        oppo_board->item(cur_oppo_row-1, cur_oppo_column-1)->setBackgroundColor(original_color);
                        oppo_board->item(cur_oppo_row-1, cur_oppo_column)->setBackgroundColor(original_color);
                        oppo_board->item(cur_oppo_row-1, cur_oppo_column+1)->setBackgroundColor(original_color);
                        oppo_board->item(cur_oppo_row-1, cur_oppo_column+2)->setBackgroundColor(original_color);
                        oppo_board->item(cur_oppo_row-2, cur_oppo_column)->setBackgroundColor(original_color);
                        oppo_board->item(cur_oppo_row-3, cur_oppo_column-1)->setBackgroundColor(original_color);
                        oppo_board->item(cur_oppo_row-3, cur_oppo_column)->setBackgroundColor(original_color);
                        oppo_board->item(cur_oppo_row-3, cur_oppo_column+1)->setBackgroundColor(original_color);
                    } else {
                    }
                } else if(direction_index == 2) { // 左
                    if(cur_oppo_column + 3 <= 9 && cur_oppo_row - 2 >= 0 && cur_oppo_row + 2 <= 9) {
                        oppo_board->item(cur_oppo_row+2, cur_oppo_column+1)->setBackgroundColor(original_color);
                        oppo_board->item(cur_oppo_row+1, cur_oppo_column+1)->setBackgroundColor(original_color);
                        oppo_board->item(cur_oppo_row, cur_oppo_column+1)->setBackgroundColor(original_color);
                        oppo_board->item(cur_oppo_row-1, cur_oppo_column+1)->setBackgroundColor(original_color);
                        oppo_board->item(cur_oppo_row-2, cur_oppo_column+1)->setBackgroundColor(original_color);
                        oppo_board->item(cur_oppo_row, cur_oppo_column+2)->setBackgroundColor(original_color);
                        oppo_board->item(cur_oppo_row-1, cur_oppo_column+3)->setBackgroundColor(original_color);
                        oppo_board->item(cur_oppo_row, cur_oppo_column+3)->setBackgroundColor(original_color);
                        oppo_board->item(cur_oppo_row+1, cur_oppo_column+3)->setBackgroundColor(original_color);
                    } else {
                    }
                } else if(direction_index == 3) { // 右
                    if(cur_oppo_column - 3 >= 0 && cur_oppo_row - 2 >= 0 && cur_oppo_row + 2 <= 9) {
                        oppo_board->item(cur_oppo_row+2, cur_oppo_column-1)->setBackgroundColor(original_color);
                        oppo_board->item(cur_oppo_row+1, cur_oppo_column-1)->setBackgroundColor(original_color);
                        oppo_board->item(cur_oppo_row, cur_oppo_column-1)->setBackgroundColor(original_color);
                        oppo_board->item(cur_oppo_row-1, cur_oppo_column-1)->setBackgroundColor(original_color);
                        oppo_board->item(cur_oppo_row-2, cur_oppo_column-1)->setBackgroundColor(original_color);
                        oppo_board->item(cur_oppo_row, cur_oppo_column-2)->setBackgroundColor(original_color);
                        oppo_board->item(cur_oppo_row-1, cur_oppo_column-3)->setBackgroundColor(original_color);
                        oppo_board->item(cur_oppo_row, cur_oppo_column-3)->setBackgroundColor(original_color);
                        oppo_board->item(cur_oppo_row+1, cur_oppo_column-3)->setBackgroundColor(original_color);
                    } else {
                    }
                }
            }
            cur_oppo_row = row;
            cur_oppo_column = column;

            int temp_board[BOARD_SIZE][BOARD_SIZE];
            for(int i = 0;i < BOARD_SIZE; ++i){
                for(int j = 0;j < BOARD_SIZE; ++j){
                    temp_board[i][j] = 0;
                }
            }
            pre_not_valid_oppo = false;

            // head
            auto color = oppo_board->item(cur_oppo_row, cur_oppo_column)->backgroundColor();
            oppo_board->item(cur_oppo_row, cur_oppo_column)->setBackgroundColor(QColor("gray"));
            temp_board[cur_oppo_row][cur_oppo_column] = 1;
            // 上
            if(direction_index == 0) {
                // 位置合法
                if(cur_oppo_column - 2 >= 0 && cur_oppo_column + 2 <= 9 && cur_oppo_row + 3 <= 9) {
                    temp_board[cur_oppo_row+1][cur_oppo_column-2] = 1;
                    temp_board[cur_oppo_row+1][cur_oppo_column-1] = 1;
                    temp_board[cur_oppo_row+1][cur_oppo_column] = 1;
                    temp_board[cur_oppo_row+1][cur_oppo_column+1] = 1;
                    temp_board[cur_oppo_row+1][cur_oppo_column+2] = 1;
                    temp_board[cur_oppo_row+2][cur_oppo_column] = 1;
                    temp_board[cur_oppo_row+3][cur_oppo_column-1] = 1;
                    temp_board[cur_oppo_row+3][cur_oppo_column] = 1;
                    temp_board[cur_oppo_row+3][cur_oppo_column+1] = 1;

                    int sum = 0;
                    for(int i = 0;i < BOARD_SIZE; ++i){
                        for(int j = 0;j < BOARD_SIZE; ++j){
                            sum += guess_board[i][j] & temp_board[i][j];
                        }
                    }

                    if(!sum){
                        oppo_board->item(cur_oppo_row+1, cur_oppo_column-2)->setBackgroundColor(QColor("gray"));
                        oppo_board->item(cur_oppo_row+1, cur_oppo_column-1)->setBackgroundColor(QColor("gray"));
                        oppo_board->item(cur_oppo_row+1, cur_oppo_column)->setBackgroundColor(QColor("gray"));
                        oppo_board->item(cur_oppo_row+1, cur_oppo_column+1)->setBackgroundColor(QColor("gray"));
                        oppo_board->item(cur_oppo_row+1, cur_oppo_column+2)->setBackgroundColor(QColor("gray"));
                        oppo_board->item(cur_oppo_row+2, cur_oppo_column)->setBackgroundColor(QColor("gray"));
                        oppo_board->item(cur_oppo_row+3, cur_oppo_column-1)->setBackgroundColor(QColor("gray"));
                        oppo_board->item(cur_oppo_row+3, cur_oppo_column)->setBackgroundColor(QColor("gray"));
                        oppo_board->item(cur_oppo_row+3, cur_oppo_column+1)->setBackgroundColor(QColor("gray"));
                    } else {
                         oppo_board->item(cur_oppo_row, cur_oppo_column)->setBackgroundColor(color);
                         pre_not_valid_oppo = true;
                    }
                } else {
                    oppo_board->item(cur_oppo_row, cur_oppo_column)->setBackgroundColor(color);
                    pre_not_valid_oppo = true;
                }
            } else if(direction_index == 1) { // 下
                if(cur_oppo_column - 2 >= 0 && cur_oppo_column + 2 <= 9 && cur_oppo_row - 3 >= 0) {
                    temp_board[cur_oppo_row-1][cur_oppo_column-2] = 1;
                    temp_board[cur_oppo_row-1][cur_oppo_column-1] = 1;
                    temp_board[cur_oppo_row-1][cur_oppo_column] = 1;
                    temp_board[cur_oppo_row-1][cur_oppo_column+1] = 1;
                    temp_board[cur_oppo_row-1][cur_oppo_column+2] = 1;
                    temp_board[cur_oppo_row-2][cur_oppo_column] = 1;
                    temp_board[cur_oppo_row-3][cur_oppo_column-1] = 1;
                    temp_board[cur_oppo_row-3][cur_oppo_column] = 1;
                    temp_board[cur_oppo_row-3][cur_oppo_column+1] = 1;

                    int sum = 0;
                    for(int i = 0;i < BOARD_SIZE; ++i){
                        for(int j = 0;j < BOARD_SIZE; ++j){
                            sum += guess_board[i][j] & temp_board[i][j];
                        }
                    }

                    if(!sum){
                        oppo_board->item(cur_oppo_row-1, cur_oppo_column-2)->setBackgroundColor(QColor("gray"));
                        oppo_board->item(cur_oppo_row-1, cur_oppo_column-1)->setBackgroundColor(QColor("gray"));
                        oppo_board->item(cur_oppo_row-1, cur_oppo_column)->setBackgroundColor(QColor("gray"));
                        oppo_board->item(cur_oppo_row-1, cur_oppo_column+1)->setBackgroundColor(QColor("gray"));
                        oppo_board->item(cur_oppo_row-1, cur_oppo_column+2)->setBackgroundColor(QColor("gray"));
                        oppo_board->item(cur_oppo_row-2, cur_oppo_column)->setBackgroundColor(QColor("gray"));
                        oppo_board->item(cur_oppo_row-3, cur_oppo_column-1)->setBackgroundColor(QColor("gray"));
                        oppo_board->item(cur_oppo_row-3, cur_oppo_column)->setBackgroundColor(QColor("gray"));
                        oppo_board->item(cur_oppo_row-3, cur_oppo_column+1)->setBackgroundColor(QColor("gray"));
                    } else {
                        oppo_board->item(cur_oppo_row, cur_oppo_column)->setBackgroundColor(color);
                        pre_not_valid_oppo = true;
                    }
                } else {
                    oppo_board->item(cur_oppo_row, cur_oppo_column)->setBackgroundColor(color);
                    pre_not_valid_oppo = true;
                }
            } else if(direction_index == 2) { // 左
                if(cur_oppo_column + 3 <= 9 && cur_oppo_row - 2 >= 0 && cur_oppo_row + 2 <= 9) {
                    temp_board[cur_oppo_row+2][cur_oppo_column+1] = 1;
                    temp_board[cur_oppo_row+1][cur_oppo_column+1] = 1;
                    temp_board[cur_oppo_row][cur_oppo_column+1] = 1;
                    temp_board[cur_oppo_row-1][cur_oppo_column+1] = 1;
                    temp_board[cur_oppo_row-2][cur_oppo_column+1] = 1;
                    temp_board[cur_oppo_row][cur_oppo_column+2] = 1;
                    temp_board[cur_oppo_row-1][cur_oppo_column+3] = 1;
                    temp_board[cur_oppo_row][cur_oppo_column+3] = 1;
                    temp_board[cur_oppo_row+1][cur_oppo_column+3] = 1;

                    int sum = 0;
                    for(int i = 0;i < BOARD_SIZE; ++i){
                        for(int j = 0;j < BOARD_SIZE; ++j){
                            sum += guess_board[i][j] & temp_board[i][j];
                        }
                    }

                    if(!sum){
                        oppo_board->item(cur_oppo_row+2, cur_oppo_column+1)->setBackgroundColor(QColor("gray"));
                        oppo_board->item(cur_oppo_row+1, cur_oppo_column+1)->setBackgroundColor(QColor("gray"));
                        oppo_board->item(cur_oppo_row, cur_oppo_column+1)->setBackgroundColor(QColor("gray"));
                        oppo_board->item(cur_oppo_row-1, cur_oppo_column+1)->setBackgroundColor(QColor("gray"));
                        oppo_board->item(cur_oppo_row-2, cur_oppo_column+1)->setBackgroundColor(QColor("gray"));
                        oppo_board->item(cur_oppo_row, cur_oppo_column+2)->setBackgroundColor(QColor("gray"));
                        oppo_board->item(cur_oppo_row-1, cur_oppo_column+3)->setBackgroundColor(QColor("gray"));
                        oppo_board->item(cur_oppo_row, cur_oppo_column+3)->setBackgroundColor(QColor("gray"));
                        oppo_board->item(cur_oppo_row+1, cur_oppo_column+3)->setBackgroundColor(QColor("gray"));
                    } else {
                        oppo_board->item(cur_oppo_row, cur_oppo_column)->setBackgroundColor(color);
                        pre_not_valid_oppo = true;
                    }
                } else {
                    oppo_board->item(cur_oppo_row, cur_oppo_column)->setBackgroundColor(color);
                    pre_not_valid_oppo = true;
                }
            } else if(direction_index == 3) { // 右
                if(cur_oppo_column - 3 >= 0 && cur_oppo_row - 2 >= 0 && cur_oppo_row + 2 <= 9) {
                    temp_board[cur_oppo_row+2][cur_oppo_column-1] = 1;
                    temp_board[cur_oppo_row+1][cur_oppo_column-1] = 1;
                    temp_board[cur_oppo_row][cur_oppo_column-1] = 1;
                    temp_board[cur_oppo_row-1][cur_oppo_column-1] = 1;
                    temp_board[cur_oppo_row-2][cur_oppo_column-1] = 1;
                    temp_board[cur_oppo_row][cur_oppo_column-2] = 1;
                    temp_board[cur_oppo_row-1][cur_oppo_column-3] = 1;
                    temp_board[cur_oppo_row][cur_oppo_column-3] = 1;
                    temp_board[cur_oppo_row+1][cur_oppo_column-3] = 1;

                    int sum = 0;
                    for(int i = 0;i < BOARD_SIZE; ++i){
                        for(int j = 0;j < BOARD_SIZE; ++j){
                            sum += guess_board[i][j] & temp_board[i][j];
                        }
                    }

                    if(!sum){
                        oppo_board->item(cur_oppo_row+2, cur_oppo_column-1)->setBackgroundColor(QColor("gray"));
                        oppo_board->item(cur_oppo_row+1, cur_oppo_column-1)->setBackgroundColor(QColor("gray"));
                        oppo_board->item(cur_oppo_row, cur_oppo_column-1)->setBackgroundColor(QColor("gray"));
                        oppo_board->item(cur_oppo_row-1, cur_oppo_column-1)->setBackgroundColor(QColor("gray"));
                        oppo_board->item(cur_oppo_row-2, cur_oppo_column-1)->setBackgroundColor(QColor("gray"));
                        oppo_board->item(cur_oppo_row, cur_oppo_column-2)->setBackgroundColor(QColor("gray"));
                        oppo_board->item(cur_oppo_row-1, cur_oppo_column-3)->setBackgroundColor(QColor("gray"));
                        oppo_board->item(cur_oppo_row, cur_oppo_column-3)->setBackgroundColor(QColor("gray"));
                        oppo_board->item(cur_oppo_row+1, cur_oppo_column-3)->setBackgroundColor(QColor("gray"));
                    } else {
                        oppo_board->item(cur_oppo_row, cur_oppo_column)->setBackgroundColor(color);
                        pre_not_valid_oppo = true;
                    }
                } else {
                    oppo_board->item(cur_oppo_row, cur_oppo_column)->setBackgroundColor(color);
                    pre_not_valid_oppo = true;
                }
            }
        }
}

// TODO 实现配置
void TcpClient::setConfigImpl(int fontsize, int color){
//    int num = rightStackLayout->count();
//    QWidget* cur;
//    QTextBrowser* curTextBrowser;
//    for(int i = 1;i < num; ++i){
//        cur = rightStackLayout->itemAt(i)->widget();
//        qDebug() << cur->layout()->count();
//        curTextBrowser = static_cast<QTextBrowser*>(cur->layout()->itemAt(1)->widget());
//        curTextBrowser->setStyleSheet("font-size: " + QString::number(fontsize)
//                                      + "px;" + "color: " + COLOR[color] + ";");
//    }
}


/********************slots*********************************/

// 登录按钮（FINISHED）
void TcpClient::on_loginBtn_clicked()
{
    QVector<QObject*> layouts = loginWindow->layout()->children().toVector();
    QLayoutItem * item =  static_cast<QHBoxLayout*>(layouts[0])->itemAt(1);
    QLineEdit* line = static_cast<QLineEdit*>(item->widget());
    QString username =  line->text();

    item =  static_cast<QHBoxLayout*>(layouts[1])->itemAt(1);
    line = static_cast<QLineEdit*>(item->widget());
    QString password = line->text();

    qDebug() << "username: " << username;
    qDebug() << "password: " << password;

    if(username == "" || password == "" ){
        errorGUI("用户名或密码不能为空");
        return;
    }

    if(username == "群聊"){
        errorGUI("用户名不能为群聊");
        return;
    }

    if(username.size() > 31 || password.size() > 31){
        errorGUI("用户名或密码不能超过32字节");
        return;
    }

    PacketHead sendPacketHead;

    sendPacketHead.set_packet_type(PacketHead::kC2SReport);
    sendPacketHead.set_function_type(PacketHead::kC2SReportLoginIn);

    sendPacketHead.set_length(64);

    /*sendClientToServerReportLogin.set_string(sendPacketHead,
        (stringPadding(QStringToString(username), 32) + stringPadding(QStringToString(password), 32)).c_str());*/
    std::string usernameString = QStringToString(username);
    std::string passwordString = QStringToString(password);

    ClientToServerReportLogin sendClientToServerReportLogin(sendPacketHead,
        stringPadding(usernameString, 32).c_str(), stringPadding(passwordString, 32).c_str());

    char* tmpStr = new char[kPacketHeadLen + sendPacketHead.get_length() + 1];
    sendClientToServerReportLogin.get_string(tmpStr);
     socket->write(tmpStr, kPacketHeadLen + sendPacketHead.get_length());

    delete[] tmpStr;

    this->username = username;
    this->password = password;
}


// 注册按钮 (FINISHED)
void TcpClient::on_signupBtn_clicked()
{
    QVector<QObject*> layouts = loginWindow->layout()->children().toVector();
    QLayoutItem * item =  static_cast<QHBoxLayout*>(layouts[0])->itemAt(1);
    QLineEdit* line = static_cast<QLineEdit*>(item->widget());
    QString username =  line->text();

    item =  static_cast<QHBoxLayout*>(layouts[1])->itemAt(1);
    line = static_cast<QLineEdit*>(item->widget());
    QString password = line->text();

    qDebug() << "username: " << username;
    qDebug() << "password: " << password;

    if(username == "" || password == "" ){
        errorGUI("用户名或密码不能为空");
        return;
    }

    if(username == "群聊"){
        errorGUI("用户名不能为群聊");
        return;
    }

    if(username.size() > 31 || password.size() > 31){
        errorGUI("用户名或密码不能超过32字节");
        return;
    }

    PacketHead sendPacketHead;


    sendPacketHead.set_packet_type(PacketHead::kC2SReport);
    sendPacketHead.set_function_type(PacketHead::kC2SReportRegister);  //注册

    sendPacketHead.set_length(64);
    /*sendClientToServerReportLogin.set_string(sendPacketHead,
        (stringPadding(QStringToString(username), 32) + stringPadding(QStringToString(password), 32)).c_str());*/

    std::string usernameString = QStringToString(username);
    std::string passwordString = QStringToString(password);

    ClientToServerReportLogin sendClientToServerReportLogin(sendPacketHead,
        stringPadding(usernameString, 32).c_str(), stringPadding(passwordString, 32).c_str());

    char* tmpStr = new char[kPacketHeadLen + sendPacketHead.get_length() + 1];
    sendClientToServerReportLogin.get_string(tmpStr);
     socket->write(tmpStr, kPacketHeadLen + sendPacketHead.get_length());

    delete[] tmpStr;
}


// 断线处理 (FINISHED)
void TcpClient::clientDisconnected(){
    qDebug() << "您断线了，请检查网络设置";
    errorGUI("您断线了，请检查网络设置");
}


// 修改密码按钮 (FINISHED)
void TcpClient::on_changePwdBtn_clicked(){
    changePwdWindow = new QWidget;
    changePwdWindow->setWindowTitle("修改密码");
    changePwdWindow->setFixedSize(QSize(500, 300));

    QHBoxLayout * layout0 = new QHBoxLayout;
    QLabel * usernameLabel = new QLabel("          用户名");
    QLineEdit * user = new QLineEdit;
    QRegExp rx("[^\u4e00-\u9fa5]+");
    QRegExpValidator *pReg = new QRegExpValidator(rx, this);
    user->setValidator(pReg);
    layout0->addWidget(usernameLabel);
    layout0->addWidget(user);
    layout0->setContentsMargins(50, 10, 50, 10);

    QHBoxLayout * layout1 = new QHBoxLayout;
    QLabel * originalPwdLabel = new QLabel("     原始密码");
    QLineEdit * originalPwd = new QLineEdit;
    originalPwd->setEchoMode(QLineEdit::Password);
    layout1->addWidget(originalPwdLabel);
    layout1->addWidget(originalPwd);
    layout1->setContentsMargins(50, 10, 50, 10);

    QHBoxLayout * layout2 = new QHBoxLayout;
    QLabel * newPwdLabel = new QLabel("          新密码");
    QLineEdit * newPwd = new QLineEdit;
    newPwd->setEchoMode(QLineEdit::Password);
    layout2->addWidget(newPwdLabel);
    layout2->addWidget(newPwd);
    layout2->setContentsMargins(50, 10, 50, 10);

    QHBoxLayout * layout3 = new QHBoxLayout;
    QLabel * acknewPwdLabel = new QLabel("确认新密码");
    QLineEdit * acknewPwd = new QLineEdit;
    acknewPwd->setEchoMode(QLineEdit::Password);
    layout3->addWidget(acknewPwdLabel);
    layout3->addWidget(acknewPwd);
    layout3->setContentsMargins(50, 10, 50, 10);

    QHBoxLayout * layout4 = new QHBoxLayout;
    QPushButton * ack = new QPushButton("确认");
    QPushButton * cancel = new QPushButton("取消");
    ack->setFixedSize(QSize(75,30));
    cancel->setFixedSize(QSize(75,30));
    layout4->addWidget(ack);
    layout4->addWidget(cancel);
    layout4->setContentsMargins(50, 10, 50, 10);


    QVBoxLayout * mainlayout = new QVBoxLayout;
    mainlayout->addLayout(layout0);
    mainlayout->addLayout(layout1);
    mainlayout->addLayout(layout2);
    mainlayout->addLayout(layout3);
    mainlayout->addLayout(layout4);

    connect(ack, SIGNAL(clicked()), this, SLOT(on_changePwdAckBtn_clicked()), Qt::QueuedConnection);
    connect(cancel, SIGNAL(clicked()), this, SLOT(on_changePwdCancelBtn_clicked()), Qt::QueuedConnection);

    changePwdWindow->setLayout(mainlayout);
    changePwdWindow->show();
}


// 在修改密码界面，点击“确认”，将发送 (FINISHED)
void TcpClient::on_changePwdAckBtn_clicked(){
    QVector<QObject*> layouts = changePwdWindow->layout()->children().toVector();
    QLayoutItem * item =  static_cast<QHBoxLayout*>(layouts[0])->itemAt(1);
    QLineEdit* line = static_cast<QLineEdit*>(item->widget());
    QString user = line->text();

    item = static_cast<QHBoxLayout*>(layouts[1])->itemAt(1);
    line = static_cast<QLineEdit*>(item->widget());
    QString originalPwd = line->text();

    item = static_cast<QHBoxLayout*>(layouts[2])->itemAt(1);
    line = static_cast<QLineEdit*>(item->widget());
    QString newPwd = line->text();

    item = static_cast<QHBoxLayout*>(layouts[3])->itemAt(1);
    line = static_cast<QLineEdit*>(item->widget());
    QString ackNewPwd = line->text();

    if(user == "" || originalPwd == "" || newPwd == ""){
        errorGUI("用户名或密码不能为空");
        return;
    }

    if(newPwd != ackNewPwd){
        errorGUI("新密码不一致");
        return;
    }

    // TODO 发送修改密码消息
    qDebug() << "用户名" << user;
    qDebug() << "原始密码" << originalPwd;
    qDebug() << "新密码" << newPwd;
    qDebug() << "确认新密码" << ackNewPwd;

    PacketHead sendPacketHead;

    sendPacketHead.set_packet_type(PacketHead::kC2SReport);
    sendPacketHead.set_function_type(PacketHead::kC2SReportUpdate);

    sendPacketHead.set_length(96);

    std::string userString = QStringToString(user);
    std::string originalPwdString = QStringToString(originalPwd);
    std::string newPwdString = QStringToString(newPwd);

    ClientToServerReportUpdate sendClientToServerReportUpdate(sendPacketHead,
        stringPadding(userString, 32).c_str(), stringPadding(originalPwdString, 32).c_str(), stringPadding(newPwdString, 32).c_str());

    char* tmpStr = new char[kPacketHeadLen + sendPacketHead.get_length() + 1];
    sendClientToServerReportUpdate.get_string(tmpStr);
     socket->write(tmpStr, kPacketHeadLen + sendPacketHead.get_length());

    delete[] tmpStr;

    changePwdWindow->close();
}


// 在修改密码界面，点击“取消”，将关闭窗口 (FINISHED)
void TcpClient::on_changePwdCancelBtn_clicked(){
    changePwdWindow->close();
}


// 登录界面，显示密码 (FINISHED)
void TcpClient::on_showPwdCheckBox_stateChanged(){
    QVector<QObject*> layouts = loginWindow->layout()->children().toVector();
    QLayoutItem * item =  static_cast<QHBoxLayout*>(layouts[1])->itemAt(1);
    QLayoutItem * item2 =  static_cast<QHBoxLayout*>(layouts[2])->itemAt(1);
    QCheckBox * box = static_cast<QCheckBox*>(item2->widget());
    QLineEdit* line = static_cast<QLineEdit*>(item->widget());

    if(box->isChecked()){
        line->setEchoMode(QLineEdit::Normal);
    }else{
        line->setEchoMode(QLineEdit::Password);
    }
}

 // 时间中断，每秒一次，更新时间 （FINISHED）
void TcpClient::timeUpdate(){
    if(chatRoomWindow){
        QLineEdit* line = static_cast<QLineEdit*>(chatRoomWindow->layout()->itemAt(0)->
                                                  layout()->itemAt(5)->layout()->itemAt(1)->widget());
        line->setText(QString::number(time.elapsed() / 1000));
    }

    if(!isConnected()){
        isOnline = false;
        // TODO 我离线了
        QHBoxLayout* layout = static_cast<QHBoxLayout*>(chatRoomMainLayout->itemAt(0)->layout()->itemAt(6));
        QLineEdit* line = static_cast<QLineEdit*>(layout->itemAt(1)->widget());
        line->setText("已离线");
    }else{
        if(!isOnline){
            isOnline = true;
            QHBoxLayout* layout = static_cast<QHBoxLayout*>(chatRoomMainLayout->itemAt(0)->layout()->itemAt(6));
            QLineEdit* line = static_cast<QLineEdit*>(layout->itemAt(1)->widget());
            line->setText("我在线上");
            time.restart();
        }
    }

}



// 点击用户的事件
void TcpClient::userLabelClicked(){
    QObject * object = QObject::sender();
    ClickableLabel * user = static_cast<ClickableLabel*>(qobject_cast<QWidget*>(object));
    if(preChatter){
        if(preChatter->isEnabled()){
            // 撤销高亮
            preChatter->setStyleSheet("QLabel{background: rgb(46, 50, 56);}"
                                     "QLabel{color: rgb(255, 255, 255);}"
                                     "QLabel:hover{background: rgb(58,63,69);}");
        }else{
            // 撤销高亮
            preChatter->setStyleSheet("QLabel{background: rgb(46, 50, 56);}"
                                     "QLabel{color: rgb(128, 128, 128);}"
                                     "QLabel:hover{background: rgb(58,63,69);}");
        }

    }

    if(user->isEnabled()){
        // 高亮
        user->setStyleSheet("QLabel{background: rgb(58,63,69);}"
                                 "QLabel{color: rgb(255, 255, 255);}"
                                 "QLabel:hover{background: rgb(58,63,69);}");
    } else{
        // 高亮
        user->setStyleSheet("QLabel{background: rgb(58,63,69);}"
                                 "QLabel{color: rgb(128, 128, 128);}"
                                 "QLabel:hover{background: rgb(58,63,69);}");
    }


    preChatter = user;
    curChatter->setText(user->text());

    int index = user2Index[user->text()];
    curIndex = index;
//    rightStackLayout->setCurrentIndex(index);
    qDebug() << index;
//    QLabel* label = static_cast<QLabel*>(rightStackLayout->currentWidget()->layout()->itemAt(0)->widget());
//    label->setText(user->text());

    qDebug() << "点击了" << user->text() << " Index:" << index;
}






/*---------------------------------------------------------------------------------
以下部分是关于邀请游戏和左侧显示栏的部分
-----------------------------------------------------------------------------------*/

//TODO
void TcpClient::inGame()
{
    /*
    提示某两个人进入了游戏（左侧用户栏显示在游戏中以及对象并隐藏或冻结邀请按钮），
    然后如果其中一个人是你邀请的对象而另一个人不是，那么当前邀请的弹窗关闭，显示“对方已经接受别的邀请”
    */
    //注意：处于邀请他人的状态使用isInviting来标志，邀请对象为opponame
    QString player_one_name = my_extend_packet_status.get_player_one_name();
    QString player_two_name = my_extend_packet_status.get_player_two_name();
    
    if(isInviting && (player_one_name != username && player_two_name == opponame) || (player_one_name == opponame && player_two_name != username))
    {
        //TODO
        //GUI部分，其中一个人是你邀请的对象而另一个人不是，那么当前邀请的弹窗关闭，显示“对方已经接受别的邀请”
        inviteSrcBox->close();
        errorGUI("对方已接受别人的邀请");
    }
    else
    {   
        //TODO
        //GUI部分，显示player1和player2的游戏状态，冻结邀请按钮（注意其中一人是自己的特殊情况）
        if(player_one_name != username)
            setUserStatus(player_one_name, true, false);
        if(player_two_name != username)
            setUserStatus(player_two_name, true, false);
    }
}

//TODO
void TcpClient::offGame()
{
    /*
    提示某两个人结束了游戏（对于两人中的任意一个，若仍在线，左侧用户栏恢复其按钮）
    */
    QString player_one_name = my_extend_packet_status.get_player_one_name();
    QString player_two_name = my_extend_packet_status.get_player_two_name();

    //TODO
    /*GUI部分，恢复两个人的邀请按钮, 注意特判其中一个人是自己的情况*/
    if(player_one_name != username)
        setUserStatus(player_one_name, true, false);
    if(player_two_name != username)
        setUserStatus(player_one_name, true, false);
}

//这是一个槽函数, 绑定的是邀请按钮
//TODO
void TcpClient::inviteGame()
{
    //TODO
    /*
    GUI部分，获得对方用户名opponame
    */
    QObject * object = QObject::sender();
    QPushButton * button = static_cast<QPushButton*>(qobject_cast<QWidget*>(object));

    opponame = button2name[button];

    //寻找是否有你邀请的对象，对于不是你邀请的对象，全部发送拒绝包
    bool findOpponame = false;
    for(auto name : invitingName)
    {
        qDebug() << name << "_invitingName\n";
        if(name == opponame)
            findOpponame = true;
        else
        {
            rejectName = name;
            sendDeclineInvitationPacket();
        }
    }

    invitingName.clear();
    //TODO
    //GUI部分，关闭所有邀请的弹窗
    for(auto it = inviteDstBoxList.begin(); it != inviteDstBoxList.end(); ++it){
        it.value()->close();
        delete it.value();
    }
    inviteDstBoxList.clear();

    if(findOpponame)
    {
        //由于你邀请的人也邀请了你，因此直接接受邀请
        sendAcceptInvitationPacket();
        return;
    }
    else
    {
        isInviting = true;//正在邀请opponame

        //发送一个请求对战包

        PacketHead sendPacketHead;

        sendPacketHead.set_packet_type(PacketHead::kExtendBuildAndDestroy);
        sendPacketHead.set_function_type(PacketHead::kExtendBuildAndDestroyInvite);
        sendPacketHead.set_length(64);

        //首先取出文本信息以便计算包长度
        std::string usernameString = QStringToString(username);
        std::string opponameString = QStringToString(opponame);

        ExtendPacketBuildAndDestroy sendExtendPacketBuildAndDestroy(sendPacketHead,
            stringPadding(usernameString, 32).c_str(), stringPadding(opponameString, 32).c_str());

        char* tmpStr = new char[kPacketHeadLen + sendPacketHead.get_length() + 1];
        sendExtendPacketBuildAndDestroy.get_string(tmpStr);
        socket->write(tmpStr, kPacketHeadLen + sendPacketHead.get_length());

        delete[] tmpStr;

        //FINISH
        //GUI部分，隐藏主界面，并显示“正在邀请opponame", 要提供相应的选项
        inviteSrcGUI(opponame);
    }
}

//TODO
void TcpClient::showInvitation()
{
     qDebug() << inviteName << "邀请1" << "\n";
    //首先判断自己是否在游戏中(用状态isGaming标志），如果在游戏中那么就丢弃这个包，因为发送者会接收到inGame包，然后就知道接受了别人的邀请
    if(isGaming) return;
     qDebug() << inviteName << "邀请2" << "\n";

    //在邀请opponame的时候，有人邀请你，默认拒绝
    inviteName = my_extend_packet_build_and_destroy.get_send_name();
    if(isInviting)
    {
        rejectName = inviteName;
        sendDeclineInvitationPacket();
        return;
    }
     qDebug() << inviteName << "邀请3" << "\n";

    invitingName.insert(inviteName);
    //TODO
    //GUI部分，显示弹窗，同意或拒绝

    inviteDstGUI(inviteName);
}


/*---------------------------------------------------------------*/

//向对端发送同意邀请包（这时候server会向所有用户发送ingame包）
void TcpClient::sendAcceptInvitationPacket()
{
    PacketHead sendPacketHead;

    sendPacketHead.set_packet_type(PacketHead::kExtendBuildAndDestroy);
    sendPacketHead.set_function_type(PacketHead::kExtendBuildAndDestroyAccept);

    //首先取出文本信息以便计算包长度
    std::string usernameString = QStringToString(username);
    std::string opponameString = QStringToString(opponame);
    sendPacketHead.set_length(64);

    ExtendPacketBuildAndDestroy sendExtendPacketBuildAndDestroy(sendPacketHead,
        stringPadding(usernameString, 32).c_str(), stringPadding(opponameString, 32).c_str());

    char* tmpStr = new char[kPacketHeadLen + sendPacketHead.get_length() + 1];
    sendExtendPacketBuildAndDestroy.get_string(tmpStr);
    socket->write(tmpStr, kPacketHeadLen + sendPacketHead.get_length());

    delete[] tmpStr;
}

//这是一个槽函数，绑定的是“同意邀请”的按钮，另外inviteGame中也会调用
//TODO
void TcpClient::acceptInvitation()
{
    isInviting = false;
    isGaming = true;

    //TODO
    //GUI部分，获得对方用户名opponame，关闭所有邀请的弹窗，游戏开始，可以摆放飞机
    QObject * object = QObject::sender();
    QPushButton * button = static_cast<QPushButton*>(qobject_cast<QWidget*>(object));

    opponame = acceptbutton2opponame[button];
    for(auto it = inviteDstBoxList.begin(); it != inviteDstBoxList.end(); ++it){
        it.value()->close();
        delete it.value();
    }
    inviteDstBoxList.clear();


    sendAcceptInvitationPacket();

    InitGameWindow();
}

/*---------------------------------------------------------------*/



/*---------------------------------------------------------------*/

void TcpClient::sendDeclineInvitationPacket()
{
    //向对端发送拒绝邀请包
    PacketHead sendPacketHead;

    sendPacketHead.set_packet_type(PacketHead::kExtendBuildAndDestroy);
    sendPacketHead.set_function_type(PacketHead::kExtendBuildAndDestroyReject);
    sendPacketHead.set_length(64);

    //首先取出文本信息以便计算包长度
    std::string senderString = QStringToString(username);
    std::string recvString = QStringToString(rejectName);

    ExtendPacketBuildAndDestroy sendExtendPacketBuildAndDestroy(sendPacketHead,
        stringPadding(senderString, 32).c_str(), stringPadding(recvString, 32).c_str());

    char* tmpStr = new char[kPacketHeadLen + sendPacketHead.get_length() + 1];
    sendExtendPacketBuildAndDestroy.get_string(tmpStr);
    socket->write(tmpStr, kPacketHeadLen + sendPacketHead.get_length());

    delete[] tmpStr;
}

//这是一个槽函数，绑定的是“拒绝邀请”的按钮，另外inviteGame中也会调用
//TODO
void TcpClient::declineInvitation()
{
    //TODO
    //GUI部分，获得对方的用户名rejectName
    QObject * object = QObject::sender();
    QPushButton * button = static_cast<QPushButton*>(qobject_cast<QWidget*>(object));
    rejectName = rejectbutton2opponame[button];
    
    invitingName.erase(invitingName.find(rejectName));
    sendDeclineInvitationPacket();
}

/*---------------------------------------------------------------*/

//TODO
void TcpClient::recvAcceptInvitation()
{
    //接收到同意邀请包
    isInviting = false;
    isGaming = true;

    //TODO
    //GUI部分，关闭所有邀请的弹窗，游戏开始，可以摆放飞机
    inviteSrcBox->close();
    delete inviteSrcBox;

    InitGameWindow();
 }

//TODO
void TcpClient::recvDeclineInvitation()
{
    //接收到拒绝邀请包
    isInviting = false;

    //TODO
    //GUI部分，关闭“正在邀请opponame”的弹窗，然后回到主界面
    inviteSrcBox->close();
    delete inviteSrcBox;
    errorGUI(opponame + "拒绝了您的邀请");
}


//TODO
// 槽函数
void TcpClient::cancelGameActive()
{
    isGaming = false;

    /*
    主动取消游戏
    回到主界面，恢复opponame的按钮
    */
    qDebug() << "cancle 成功" << "\n";

    //向对端发送主动取消游戏包（这时候server会向所有用户发送offgame包）
    PacketHead sendPacketHead;

    sendPacketHead.set_packet_type(PacketHead::kExtendBuildAndDestroy);
    sendPacketHead.set_function_type(PacketHead::kExtendBuildAndDestroyCancel);
    sendPacketHead.set_length(64);

    //首先取出文本信息以便计算包长度
    std::string usernameString = QStringToString(username);
    std::string opponameString = QStringToString(opponame);

    ExtendPacketBuildAndDestroy sendExtendPacketBuildAndDestroy(sendPacketHead,
        stringPadding(usernameString, 32).c_str(), stringPadding(opponameString, 32).c_str());

    char* tmpStr = new char[kPacketHeadLen + sendPacketHead.get_length() + 1];
    sendExtendPacketBuildAndDestroy.get_string(tmpStr);
    socket->write(tmpStr, kPacketHeadLen + sendPacketHead.get_length());

    delete[] tmpStr;
}

//TODO
void TcpClient::cancelGamePassive()
{
    isGaming = false;
    /*
    被动取消游戏
    GUI部分，显示“对方退出了游戏”
    然后回到主界面，恢复opponame的按钮
    */
}

//槽函数，主动取消邀请包
//TODO
void TcpClient::cancelInvitationActive()
{
    isInviting = false;
    //取消的对象是oppoName
    //发送cancelInvitation包
    PacketHead sendPacketHead;

    sendPacketHead.set_packet_type(PacketHead::kExtendBuildAndDestroy);
    sendPacketHead.set_function_type(PacketHead::kExtendBuildAndDestroyNotStart);
    sendPacketHead.set_length(64);

    //首先取出文本信息以便计算包长度
    std::string usernameString = QStringToString(username);
    std::string opponameString = QStringToString(opponame);

    ExtendPacketBuildAndDestroy sendExtendPacketBuildAndDestroy(sendPacketHead,
        stringPadding(usernameString, 32).c_str(), stringPadding(opponameString, 32).c_str());

    char* tmpStr = new char[kPacketHeadLen + sendPacketHead.get_length() + 1];
    sendExtendPacketBuildAndDestroy.get_string(tmpStr);
    socket->write(tmpStr, kPacketHeadLen + sendPacketHead.get_length());

    delete[] tmpStr;

    //GUI部分，关闭“正在邀请opponame”弹窗，回到主界面
    inviteSrcBox->close();
    delete inviteSrcBox;
}

//TODO
void TcpClient::cancelInvitationPassive()
{
    QString cancelInviteName = my_extend_packet_build_and_destroy.get_send_name();
    invitingName.erase(invitingName.find(cancelInviteName));
    /*GUI部分，被动取消邀请，需要关闭相应的弹窗并给出提示*/

    inviteDstBoxList[cancelInviteName]->close();
    delete inviteDstBoxList[cancelInviteName];
    inviteDstBoxList.erase(inviteDstBoxList.find(cancelInviteName));
    errorGUI(cancelInviteName + "取消了对您的邀请");
}

/*---------------------------------------------------------------------------------
以下部分是摆飞机的部分
左边显示的是oppoBoard，表示自己猜对方棋盘的进度
右边显示的是myBoard，表示对方猜测自己棋盘的进度
-----------------------------------------------------------------------------------*/

//TODO
void TcpClient::offensive()
{
    /*GUI部分，将状态转为先手，然后恢复oppoBoard棋盘（取消冻结）*/
    oppo_board->setEnabled(true);
    errorGUI("your turn");
}

void TcpClient::defensive()
{
    /*GUI部分，将状态转为后手，然后冻结oppoBoard棋盘*/
    oppo_board->setEnabled(false);
}

//TODO
/*点击摆放完成按钮触发*/
void TcpClient::gameReady()
{
    //GUI部分，摆放好了以后，首先先要判断三个位置是否合法，如果不合法要提示错误
    // 需要disable完成摆放
    auto button = static_cast<QPushButton*>(gameWindow->layout()->itemAt(2)->layout()->itemAt(0)->widget());
    button->setEnabled(false);

    //GUI部分，三个位置坐标, 装在下面的数组里

    static const int loc_num=3;
    unsigned short loc_small[loc_num];
    unsigned short loc_big[loc_num];

    for(int i = 0;i < loc_num; ++i) {
        loc_small[i] = my_plane_loc[i][0];
        loc_big[i] = my_plane_loc[i][1];
    }
    qDebug() << "进入gaemReady\n";


    //如果合法，发送gameReady包
    PacketHead sendPacketHead;

    sendPacketHead.set_packet_type(PacketHead::kExtendReady);
    sendPacketHead.set_function_type(PacketHead::kExtendReadyPlayer);
    sendPacketHead.set_length(12);

    ExtendPacketReady sendExtendPacketReady(sendPacketHead,
        loc_small, loc_big);

    char* tmpStr = new char[kPacketHeadLen + sendPacketHead.get_length() + 1];
    sendExtendPacketReady.get_string(tmpStr);
    socket->write(tmpStr, kPacketHeadLen + sendPacketHead.get_length());

    delete[] tmpStr;
}

//TODO
void TcpClient::gameStart(bool isOffensive)
{
    //GUI部分，接收到server发送的gameStart包，冻结或隐藏右侧的摆放栏，并提示游戏开始以及先后手，如果自己是后手，冻结棋盘
    qDebug() << "进入gameStart\n";
    if(isOffensive) {
        errorGUI("您是先手");
    } else {
        errorGUI("您是后手");
    }

}


/*---------------------------------------------------------------------------------
以下部分是对战的部分
左边显示的是oppoBoard，表示自己猜对方棋盘的进度
右边显示的是myBoard，表示对方猜测自己棋盘的进度
-----------------------------------------------------------------------------------*/

//TODO
void TcpClient::askForPointState()
{
    //GUI部分，点击棋盘上的一个按钮，获取位置loc_small
    unsigned short loc_small;
    unsigned short loc_big = 0;
    loc_small = cur_oppo_row * BOARD_SIZE + cur_oppo_column;
    guess_loc[0] = loc_small;
    qDebug() << "猜测的坐标是:" << loc_small;


    //发送请求包，然后转为后手
    PacketHead sendPacketHead;

    sendPacketHead.set_packet_type(PacketHead::kExtendPredict);
    sendPacketHead.set_function_type(PacketHead::kExtendPredictGuess);
    sendPacketHead.set_length(4);

    ExtendPacketPlaying sendExtendPacketPlaying(sendPacketHead,
        loc_small, loc_big);

    char* tmpStr = new char[kPacketHeadLen + sendPacketHead.get_length() + 1];
    sendExtendPacketPlaying.get_string(tmpStr);
    socket->write(tmpStr, kPacketHeadLen + sendPacketHead.get_length());

    delete[] tmpStr;
    
    //首先获取并保存棋盘的坐标cord

    defensive();
}

//TODO
void TcpClient::replyPointState()
{
    unsigned short loc_small = my_extend_packet_playing.get_loc_small();
    //GUI部分，myBoard显示对方猜的位置和结果
    int row = static_cast<int>(loc_small / BOARD_SIZE);
    int column = static_cast<int>(loc_small % BOARD_SIZE);
    qDebug() << "收到的位置" << loc_small;
    my_board->item(row, column)->setText("X");
    QFont ft;
    ft.setPointSize(50);
    my_board->item(row, column)->setFont(ft);
    my_board->item(row, column)->setTextColor(QColor("white"));

    unsigned short PointState;

    /*
    static const unsigned short kExtendReplyPreNo=0x00; // 猜测未命中
    static const unsigned short kExtendReplyPreHurt=0x01; // 猜测伤飞机
    static const unsigned short kExtendReplyPreDestroy=0x02; // 猜测毁飞机
    */
    PointState = valid_board[row][column];

    //回复棋盘坐标和相应的状态PointState，然后转为先手
    PacketHead sendPacketHead;

    sendPacketHead.set_packet_type(PacketHead::kExtendReplyPre);
    sendPacketHead.set_function_type(PointState);
    sendPacketHead.set_length(0);

    char* tmpStr = new char[kPacketHeadLen + sendPacketHead.get_length() + 1];
    sendPacketHead.get_string(tmpStr);
    socket->write(tmpStr, kPacketHeadLen + sendPacketHead.get_length());

    delete[] tmpStr;

    offensive();
}

//TODO
void TcpClient::recvReplyPointState(const unsigned short res)
{
    //GUI部分，oppoBoard显示自己猜测的结果
//    static const unsigned short kExtendReplyPreNo=0x00; // 猜测未命中
//    static const unsigned short kExtendReplyPreHurt=0x01; // 猜测伤飞机
//    static const unsigned short kExtendReplyPreDestroy=0x02; // 猜测毁飞机
    int row = static_cast<int>(guess_loc[0] / BOARD_SIZE);
    int column = static_cast<int>(guess_loc[0] % BOARD_SIZE);
    if(res == PacketHead::kExtendReplyPreNo){
        oppo_board->item(row, column)->setText("O");
        QFont ft;
        ft.setPointSize(50);
        oppo_board->item(row, column)->setFont(ft);
        oppo_board->item(row, column)->setTextColor(QColor("blue"));
    } else if(res == PacketHead::kExtendReplyPreHurt) {
        oppo_board->item(row, column)->setText("X");
        QFont ft;
        ft.setPointSize(50);
        oppo_board->item(row, column)->setFont(ft);
        oppo_board->item(row, column)->setTextColor(QColor("white"));
    } else if(res == PacketHead::kExtendReplyPreDestroy){
        oppo_board->item(row, column)->setText("+");
        QFont ft;
        ft.setPointSize(50);
        oppo_board->item(row, column)->setFont(ft);
        oppo_board->item(row, column)->setTextColor(QColor("white"));
    }
}

//TODO
//点击“断言”按钮后触发
void TcpClient::assertPlanePos()
{
    //GUI部分，获取飞机的位置
    unsigned short loc_small;
    unsigned short loc_big;
    loc_small = guess_loc[0];
    loc_big = guess_loc[1];


    //发送断言包
    PacketHead sendPacketHead;

    sendPacketHead.set_packet_type(PacketHead::kExtendPredict);
    sendPacketHead.set_function_type(PacketHead::kExtendPredictJudge);
    sendPacketHead.set_length(4);

    ExtendPacketPlaying sendExtendPacketPlaying(sendPacketHead,
        loc_small, loc_big);

    char* tmpStr = new char[kPacketHeadLen + sendPacketHead.get_length() + 1];
    sendExtendPacketPlaying.get_string(tmpStr);
    socket->write(tmpStr, kPacketHeadLen + sendPacketHead.get_length());

    delete[] tmpStr;


    defensive();
}

//TODO
void TcpClient::replyAssertPlanePos()
{
    unsigned short loc_small = my_extend_packet_playing.get_loc_small();
    unsigned short loc_big = my_extend_packet_playing.get_loc_big();
    //GUI部分，显示对方的断言位置和结果


    unsigned short assertRes = 4;
    /*
    static const unsigned short kExtendReplyPreFail=0x03; // 断言未中
    static const unsigned short kExtendReplyPreSuccess=0x04; // 断言命中
    */
    for(int i = 0;i < 3; ++i){
        if(my_plane_loc[i][0] == loc_small && my_plane_loc[i][1] == loc_big){
            assertRes = 3;
            break;
        }
    }

    // 命中
    int direction_index;
    int row = static_cast<int>(loc_small / BOARD_SIZE);
    int column = static_cast<int>(loc_small % BOARD_SIZE);
    if(assertRes == 3){
        guess_right_cnt += 1;
        // 上 左
        if(loc_big > loc_small) {
            if(loc_small % BOARD_SIZE == loc_big % BOARD_SIZE){
                direction_index = 0;
            } else {
                direction_index = 2;
            }
        } else {
            if(loc_small % BOARD_SIZE == loc_big % BOARD_SIZE){
                direction_index = 1;
            } else {
                direction_index = 3;
            }
        }

        valid_board[row][column] = 0;
        if(direction_index == 0) {
                            my_board->item(row+1, column-2)->setBackgroundColor(original_color);
                            my_board->item(row+1, column-1)->setBackgroundColor(original_color);
                            my_board->item(row+1, column)->setBackgroundColor(original_color);
                            my_board->item(row+1, column+1)->setBackgroundColor(original_color);
                            my_board->item(row+1, column+2)->setBackgroundColor(original_color);
                            my_board->item(row+2, column)->setBackgroundColor(original_color);
                            my_board->item(row+3, column-1)->setBackgroundColor(original_color);
                            my_board->item(row+3, column)->setBackgroundColor(original_color);
                            my_board->item(row+3, column+1)->setBackgroundColor(original_color);
                            valid_board[row+1][column-2] = 0;
                            valid_board[row+1][column-1] = 0;
                            valid_board[row+1][column] = 0;
                            valid_board[row+1][column+1] = 0;
                            valid_board[row+1][column+2] = 0;
                            valid_board[row+2][column] = 0;
                            valid_board[row+3][column-1] = 0;
                            valid_board[row+3][column] = 0;
                            valid_board[row+3][column+1] = 0;

                } else if(direction_index == 1) { // 下

                            my_board->item(row-1, column-2)->setBackgroundColor(original_color);
                            my_board->item(row-1, column-1)->setBackgroundColor(original_color);
                            my_board->item(row-1, column)->setBackgroundColor(original_color);
                            my_board->item(row-1, column+1)->setBackgroundColor(original_color);
                            my_board->item(row-1, column+2)->setBackgroundColor(original_color);
                            my_board->item(row-2, column)->setBackgroundColor(original_color);
                            my_board->item(row-3, column-1)->setBackgroundColor(original_color);
                            my_board->item(row-3, column)->setBackgroundColor(original_color);
                            my_board->item(row-3, column+1)->setBackgroundColor(original_color);
                            valid_board[row-1][column-2] = 0;
                            valid_board[row-1][column-1] = 0;
                            valid_board[row-1][column] = 0;
                            valid_board[row-1][column+1] = 0;
                            valid_board[row-1][column+2] = 0;
                            valid_board[row-2][column] = 0;
                            valid_board[row-3][column-1] = 0;
                            valid_board[row-3][column] = 0;
                            valid_board[row-3][column+1] = 0;



                } else if(direction_index == 2) { // 左

                            my_board->item(row+2, column+1)->setBackgroundColor(original_color);
                            my_board->item(row+1, column+1)->setBackgroundColor(original_color);
                            my_board->item(row, column+1)->setBackgroundColor(original_color);
                            my_board->item(row-1, column+1)->setBackgroundColor(original_color);
                            my_board->item(row-2, column+1)->setBackgroundColor(original_color);
                            my_board->item(row, column+2)->setBackgroundColor(original_color);
                            my_board->item(row-1, column+3)->setBackgroundColor(original_color);
                            my_board->item(row, column+3)->setBackgroundColor(original_color);
                            my_board->item(row+1, column+3)->setBackgroundColor(original_color);
                            valid_board[row+2][column+1] = 0;
                            valid_board[row+1][column+1] = 0;
                            valid_board[row][column+1] = 0;
                            valid_board[row-1][column+1] = 0;
                            valid_board[row-2][column+1] = 0;
                            valid_board[row][column+2] = 0;
                            valid_board[row-1][column+3] = 0;
                            valid_board[row][column+3] = 0;
                            valid_board[row+1][column+3] = 0;


                } else if(direction_index == 3) { // 右

                            my_board->item(row+2, column-1)->setBackgroundColor(original_color);
                            my_board->item(row+1, column-1)->setBackgroundColor(original_color);
                            my_board->item(row, column-1)->setBackgroundColor(original_color);
                            my_board->item(row-1, column-1)->setBackgroundColor(original_color);
                            my_board->item(row-2, column-1)->setBackgroundColor(original_color);
                            my_board->item(row, column-2)->setBackgroundColor(original_color);
                            my_board->item(row-1, column-3)->setBackgroundColor(original_color);
                            my_board->item(row, column-3)->setBackgroundColor(original_color);
                            my_board->item(row+1, column-3)->setBackgroundColor(original_color);
                            valid_board[row+2][column-1] = 0;
                            valid_board[row+1][column-1] = 0;
                            valid_board[row][column-1] = 0;
                            valid_board[row-1][column-1] = 0;
                            valid_board[row-2][column-1] = 0;
                            valid_board[row][column-2] = 0;
                            valid_board[row-1][column-3] = 0;
                            valid_board[row][column-3] = 0;
                            valid_board[row+1][column-3] = 0;

                }

    }

    //判断是否猜中并发送回复断言包
    PacketHead sendPacketHead;

    sendPacketHead.set_packet_type(PacketHead::kExtendReplyPre);
    sendPacketHead.set_function_type(assertRes);
    sendPacketHead.set_length(0);


    char* tmpStr = new char[kPacketHeadLen + sendPacketHead.get_length() + 1];
    sendPacketHead.get_string(tmpStr);
    socket->write(tmpStr, kPacketHeadLen + sendPacketHead.get_length());

    delete[] tmpStr;

    bool isWinner = false;
    //TODO,判断对方是否猜中了三次
    if(guess_right_cnt == 3){
        isWinner = true;
    }

    
    if(isWinner)//对方胜利了，发送胜利包
    {
        sendPacketHead.set_packet_type(PacketHead::kExtendGameOver);
        sendPacketHead.set_function_type(PacketHead::kExtendGameOverReply);
        sendPacketHead.set_length(0);


        char* tmpStr = new char[kPacketHeadLen + sendPacketHead.get_length() + 1];
        sendPacketHead.get_string(tmpStr);
        socket->write(tmpStr, kPacketHeadLen + sendPacketHead.get_length());

        delete[] tmpStr;
    }

    offensive();
}

//TODO
void TcpClient::recvReplyAssertPlanePos(const unsigned short res)
{
    //GUI部分，显示断言结果，如果猜中要将整个飞机显示出来，否则显示断言错误（比如闪烁几下）
    /*
    static const unsigned short kExtendReplyPreFail=0x03; // 断言未中
    static const unsigned short kExtendReplyPreSuccess=0x04; // 断言命中
    */
    if(res == 3){
        // do nothing
        errorGUI("断言成功!");
    } else if(res == 4){
        // 需要去掉
        int row = static_cast<int>(guess_loc[0]);
        int column = static_cast<int>(guess_loc[0]);
        if(direction_index == 0) {
                            oppo_board->item(row+1, column-2)->setBackgroundColor(original_color);
                            oppo_board->item(row+1, column-1)->setBackgroundColor(original_color);
                            oppo_board->item(row+1, column)->setBackgroundColor(original_color);
                            oppo_board->item(row+1, column+1)->setBackgroundColor(original_color);
                            oppo_board->item(row+1, column+2)->setBackgroundColor(original_color);
                            oppo_board->item(row+2, column)->setBackgroundColor(original_color);
                            oppo_board->item(row+3, column-1)->setBackgroundColor(original_color);
                            oppo_board->item(row+3, column)->setBackgroundColor(original_color);
                            oppo_board->item(row+3, column+1)->setBackgroundColor(original_color);
                            guess_board[row+1][column-2] = 0;
                            guess_board[row+1][column-1] = 0;
                            guess_board[row+1][column] = 0;
                            guess_board[row+1][column+1] = 0;
                            guess_board[row+1][column+2] = 0;
                            guess_board[row+2][column] = 0;
                            guess_board[row+3][column-1] = 0;
                            guess_board[row+3][column] = 0;
                            guess_board[row+3][column+1] = 0;

                } else if(direction_index == 1) { // 下

                            oppo_board->item(row-1, column-2)->setBackgroundColor(original_color);
                            oppo_board->item(row-1, column-1)->setBackgroundColor(original_color);
                            oppo_board->item(row-1, column)->setBackgroundColor(original_color);
                            oppo_board->item(row-1, column+1)->setBackgroundColor(original_color);
                            oppo_board->item(row-1, column+2)->setBackgroundColor(original_color);
                            oppo_board->item(row-2, column)->setBackgroundColor(original_color);
                            oppo_board->item(row-3, column-1)->setBackgroundColor(original_color);
                            oppo_board->item(row-3, column)->setBackgroundColor(original_color);
                            oppo_board->item(row-3, column+1)->setBackgroundColor(original_color);
                            guess_board[row-1][column-2] = 0;
                            guess_board[row-1][column-1] = 0;
                            guess_board[row-1][column] = 0;
                            guess_board[row-1][column+1] = 0;
                            guess_board[row-1][column+2] = 0;
                            guess_board[row-2][column] = 0;
                            guess_board[row-3][column-1] = 0;
                            guess_board[row-3][column] = 0;
                            guess_board[row-3][column+1] = 0;



                } else if(direction_index == 2) { // 左

                            oppo_board->item(row+2, column+1)->setBackgroundColor(original_color);
                            oppo_board->item(row+1, column+1)->setBackgroundColor(original_color);
                            oppo_board->item(row, column+1)->setBackgroundColor(original_color);
                            oppo_board->item(row-1, column+1)->setBackgroundColor(original_color);
                            oppo_board->item(row-2, column+1)->setBackgroundColor(original_color);
                            oppo_board->item(row, column+2)->setBackgroundColor(original_color);
                            oppo_board->item(row-1, column+3)->setBackgroundColor(original_color);
                            oppo_board->item(row, column+3)->setBackgroundColor(original_color);
                            oppo_board->item(row+1, column+3)->setBackgroundColor(original_color);
                            guess_board[row+2][column+1] = 0;
                            guess_board[row+1][column+1] = 0;
                            guess_board[row][column+1] = 0;
                            guess_board[row-1][column+1] = 0;
                            guess_board[row-2][column+1] = 0;
                            guess_board[row][column+2] = 0;
                            guess_board[row-1][column+3] = 0;
                            guess_board[row][column+3] = 0;
                            guess_board[row+1][column+3] = 0;


                } else if(direction_index == 3) { // 右

                            oppo_board->item(row+2, column-1)->setBackgroundColor(original_color);
                            oppo_board->item(row+1, column-1)->setBackgroundColor(original_color);
                            oppo_board->item(row, column-1)->setBackgroundColor(original_color);
                            oppo_board->item(row-1, column-1)->setBackgroundColor(original_color);
                            oppo_board->item(row-2, column-1)->setBackgroundColor(original_color);
                            oppo_board->item(row, column-2)->setBackgroundColor(original_color);
                            oppo_board->item(row-1, column-3)->setBackgroundColor(original_color);
                            oppo_board->item(row, column-3)->setBackgroundColor(original_color);
                            oppo_board->item(row+1, column-3)->setBackgroundColor(original_color);
                            guess_board[row+2][column-1] = 0;
                            guess_board[row+1][column-1] = 0;
                            guess_board[row][column-1] = 0;
                            guess_board[row-1][column-1] = 0;
                            guess_board[row-2][column-1] = 0;
                            guess_board[row][column-2] = 0;
                            guess_board[row-1][column-3] = 0;
                            guess_board[row][column-3] = 0;
                            guess_board[row+1][column-3] = 0;

                }

        errorGUI("断言错误!");
    }
}

//TODO
void TcpClient::gameOver(bool isWinner)
{
    isGaming = false;
    /*
    GUI部分，游戏结束状态
    */
    if(isWinner)
    {
        errorGUI("您赢了");
    }
    else
    {
        errorGUI("您输了");
    }
}




// 接受到包的处理，状态机 （FINISHED)
void TcpClient::readyRead(){
    qDebug() << "reading...";

    static ReadState current_read_state = READ_PACKET_HEAD;
    static QByteArray current_byte_array;
    static QByteArray set_byte_array;
    static unsigned int current_byte_num_to_read = kPacketHeadLen;
    // TODO state machine
    current_byte_array += socket->readAll();
    while(current_byte_array.size() >= current_byte_num_to_read)
    {
        if(current_byte_num_to_read)
        {
            set_byte_array = current_byte_array.left(current_byte_num_to_read);
            current_byte_array.remove(0, current_byte_num_to_read);
        }

        else
        {
            set_byte_array = "";
        }

        switch(current_read_state)
        {
            case READ_PACKET_HEAD://获得报头，将相应的字符串传进报头对应的类中，然后根据类型来做选择
                my_packet_head.set_string(set_byte_array.constData());
                switch(my_packet_head.get_packet_type())
                {
                    case PacketHead::kS2CReport:
                        switch(my_packet_head.get_function_type())
                        {
                            case PacketHead::kS2CReportSuccess:
                            case PacketHead::kS2CReportSuccessDup:
                                //登录/注册成功，进入READ_SERVER_TO_CLIENT_REPORT_SUCCESS状态准备读取后续信息
                                current_read_state = READ_SERVER_TO_CLIENT_REPORT_SUCCESS;
                                current_byte_num_to_read = my_packet_head.get_length();

                                break;
                            case PacketHead::kS2CReportUpdateSucess:
                                //更改密码成功，这个时候状态机仍然处于等待下一个packet head读入的状态
                                changePwdSuccessGUI();
                                break;
                            case PacketHead::kS2CReportWrongPwd:
                            case PacketHead::kS2CReportNoExist:
                            case PacketHead::kS2CReportMustUpdate:
                            case PacketHead::kS2CReportDuplicated:
                            case PacketHead::kS2CReportNameNotAccess:
                            case PacketHead::kS2CReportPwdNotAccess:
                            case PacketHead::kS2CReportLastPwdWrong:
                            case PacketHead::kS2CReportNowPwdNotAccess:
                                //登陆失败和注册失败，这个时候状态机仍然处于等待下一个packet head读入的状态
                                errorGUI(my_packet_head.get_function_type());
                                break;
                            default:
                                qDebug() << "switch kS2CReport my_packet_head.get_packet_type() case lost";
                        }
                        break;
                    case PacketHead::kS2CInform:
                        switch(my_packet_head.get_function_type())
                        {
                            case PacketHead::kS2CInformOnline:
                                //提示上线,进入online状态
                                current_read_state = READ_SERVER_CLIENT_ONLINE;
                                current_byte_num_to_read = my_packet_head.get_length();
                                break;
                            case PacketHead::kS2CInformOffline:
                                //提示下线，进入offline状态
                                current_read_state = READ_SERVER_CLIENT_OFFLINE;
                                current_byte_num_to_read = my_packet_head.get_length();
                                break;
                            default:
                                qDebug() << "switch kS2CInform my_packet_head.get_packet_type() case lost";
                        }
                        break;
                    case PacketHead::kS2CText:
                        switch(my_packet_head.get_function_type())
                        {
                            case PacketHead::kS2CTextSimpleText:
                                qDebug() << "get here";
                                //文本信息包
                                current_read_state = READ_SERVER_TO_CLIENT_TEXT_SIMPLE_TEXT;
                                current_byte_num_to_read = my_packet_head.get_length();
                                break;
                            case PacketHead::kS2CTextFileInfo:
                                //文本信息包
                                current_read_state = READ_SERVER_TO_CLIENT_TEXT_FILE_INFO;
                                current_byte_num_to_read = my_packet_head.get_length();
                                break;
                            /*
                                这个现在已经被notify替代了，相应的状态也不存在了
                            case PacketHead::kS2CTextFileContain:
                                //文本内容包
                                current_read_state = READ_SERVER_TO_CLIENT_TEXT_FILE_CONTAIN;
                                current_byte_num_to_read = my_packet_head.get_length();
                                break;
                            */
                            case PacketHead::kS2CTextAskForClr:
                                //清屏，这个时候状态机仍然处于等待下一个packet head读入的状态
                                // cls();
                                break;// two conditions... if it is myself, yor'are out; if it is other, modify GUI
                            default:
                                qDebug() << "switch kS2CText my_packet_head.get_packet_type() case lost";
                        }
                        break;
                    //收到用户设置包，进入相应的状态
                    case PacketHead::kS2CUserSet:
                        current_read_state = READ_SERVER_TO_CLIENT_USER_SET_UPDATE;
                        current_byte_num_to_read = my_packet_head.get_length();
                        break;
                    //在线文件通知包
                    case PacketHead::kC2CFileNotify:
                        switch(my_packet_head.get_function_type())
                        {
                            case PacketHead::kC2CFileNotifyRequest:
                                current_read_state = READ_C2C_FILE_NOTIFY_REQUEST;
                                current_byte_num_to_read = my_packet_head.get_length();
                                break;
                            case PacketHead::kC2CFileNotifyCancelSend:
                                current_read_state = READ_C2C_FILE_NOTIFY_CANCEL_SEND;
                                current_byte_num_to_read = my_packet_head.get_length();
                                break;
                            case PacketHead::kC2CFileNotifyAccept:
                                current_read_state = READ_C2C_FILE_NOTIFY_ACCEPT;
                                current_byte_num_to_read = my_packet_head.get_length();
                                break;
                            case PacketHead::kC2CFileNotifyCancelRecv:
                                current_read_state = READ_C2C_FILE_NOTIFY_CANCEL_RECV;
                                current_byte_num_to_read = my_packet_head.get_length();
                                break;
                            default:
                                qDebug() << "switch kC2CFileNotify my_packet_head.get_packet_type() case lost";
                        }
                        break;
                    case PacketHead::kC2CFileData:
                        qDebug() << "kc2cfiledata";
                        current_read_state = READ_C2C_FILE_DATA;
                        current_byte_num_to_read = my_packet_head.get_length();
                        break;

                    /*----------------------------------------------------------------------
                    Extend包的报头读入
                    -------------------------------------------------------------------------*/

                    case PacketHead::kExtendStatus://游戏状态包
                        switch(my_packet_head.get_function_type())
                        {
                            case PacketHead::kExtendStatusInGame://ingame包
                                current_read_state = READ_EXTEND_STATUS_IN_GAME;
                                current_byte_num_to_read = my_packet_head.get_length();
                                break;
                            case PacketHead::kExtendStatusOffGame://offgame包
                                current_read_state = READ_EXTEND_STATUS_OFF_GAME;
                                current_byte_num_to_read = my_packet_head.get_length();
                                break;
                            default:
                                qDebug() << "switch kExtendStatus my_packet_head.get_function_type() case lost";
                        }
                        break;

                    case PacketHead::kExtendBuildAndDestroy://玩家邀请创建对局或者中途退出销毁对局时使用
                        switch(my_packet_head.get_function_type())
                        {
                            case PacketHead::kExtendBuildAndDestroyInvite://发起对局邀请包
                                current_read_state = READ_EXTEND_BUILD_AND_DESTROY_INVITE;
                                current_byte_num_to_read = my_packet_head.get_length();
                                break;
                            case PacketHead::kExtendBuildAndDestroyAccept://接受对局邀请包
                                current_read_state = READ_EXTEND_BUILD_AND_DESTROY_ACCEPT;
                                current_byte_num_to_read = my_packet_head.get_length();
                                break;
                            case PacketHead::kExtendBuildAndDestroyReject://拒绝对局邀请包
                                current_read_state = READ_EXTEND_BUILD_AND_DESTROY_REJECT;
                                current_byte_num_to_read = my_packet_head.get_length();
                                break;
                            case PacketHead::kExtendBuildAndDestroyCancel://中途取消对局包
                                current_read_state = READ_EXTEND_BUILD_AND_DESTROY_CANCEL;
                                current_byte_num_to_read = my_packet_head.get_length();
                                break;
                            case PacketHead::kExtendBuildAndDestroyNotStart://主动取消邀请包
                                current_read_state = READ_EXTEND_BUILD_AND_DESTROY_NOT_START;
                                current_byte_num_to_read = my_packet_head.get_length();
                                break;
                            default:
                                qDebug() << "switch kExtendBuildAndDestroy my_packet_head.get_function_type() case lost";
                        }
                    break;

                    case PacketHead::kExtendBegin://准备游戏包
                        switch(my_packet_head.get_function_type())
                        {
                            case PacketHead::kExtendBeginOffensive://先手开始游戏包
                                gameStart(true);
                                offensive();
                                break;
                            case PacketHead::kExtendReadyDeffensive://后手开始游戏包
                                gameStart(false);
                                defensive();
                                break;
                            default:
                                qDebug() << "switch kExtendReady my_packet_head.get_function_type() case lost";
                        }
                    break;
                    
                    case PacketHead::kExtendPredict://猜测包
                        switch(my_packet_head.get_function_type())
                        {
                            case PacketHead::kExtendPredictGuess://猜测位置包
                                current_read_state = READ_EXTEND_PREDICT_GUESS;
                                current_byte_num_to_read = my_packet_head.get_length();
                                break;
                            case PacketHead::kExtendPredictJudge://断言位置包
                                current_read_state = READ_EXTEND_PREDICT_JUDGE;
                                current_byte_num_to_read = my_packet_head.get_length();
                                break;
                            default:
                                qDebug() << "switch kExtendPredict my_packet_head.get_function_type() case lost";
                        }
                    break;

                    case PacketHead::kExtendReplyPre://回复预测包
                        switch(my_packet_head.get_function_type())
                        {
                            case PacketHead::kExtendReplyPreNo://猜测未命中
                            case PacketHead::kExtendReplyPreHurt://猜测伤飞机
                            case PacketHead::kExtendReplyPreDestroy://猜测毁飞机
                                recvReplyPointState(my_packet_head.get_function_type());
                                break;
                            case PacketHead::kExtendReplyPreFail://断言未中
                            case PacketHead::kExtendReplyPreSuccess://断言命中
                                recvReplyAssertPlanePos(my_packet_head.get_function_type());
                            break;
                            default:
                                 qDebug() << "switch kExtendReplyPre my_packet_head.get_function_type() case lost";
                        }
                    break;

                    case PacketHead::kExtendGameOver://游戏结束（胜利）包
                        gameOver(true);
                        //之后继续进入read packet head状态
                    break;

                    default:
                        qDebug() << "switch my_packet_head.get_packet_type() case lost";

                }
                break;
            case READ_SERVER_TO_CLIENT_REPORT_SUCCESS://报道成功,收到额外信息（见协议栈），进行进一步判断
                my_server_to_client_report_success.set_string(my_packet_head, set_byte_array.constData());
                reportSuccess();
                current_read_state = READ_PACKET_HEAD;
                current_byte_num_to_read = kPacketHeadLen;
                break;
            case READ_SERVER_CLIENT_ONLINE://上线包
                my_server_to_client_inform.set_string(my_packet_head, set_byte_array.constData());
                online();
                current_read_state = READ_PACKET_HEAD;
                current_byte_num_to_read = kPacketHeadLen;
                break;
            case READ_SERVER_CLIENT_OFFLINE://下线包
                my_server_to_client_inform.set_string(my_packet_head, set_byte_array.constData());
                offline();
                current_read_state = READ_PACKET_HEAD;
                current_byte_num_to_read = kPacketHeadLen;
                break;
//            case READ_SERVER_TO_CLIENT_TEXT_SIMPLE_TEXT://收到文本信息，将文本信息放在相应的报头中，然后调用显示文本信息
//                my_server_to_client_simple_text.set_string(my_packet_head, set_byte_array.constData());
//                showText();
//                current_read_state = READ_PACKET_HEAD;
//                current_byte_num_to_read = kPacketHeadLen;
//                break;
//            case READ_SERVER_TO_CLIENT_TEXT_FILE_INFO://收到文件相关信息，将相应信息放入报头，然后显示文件信息,这一步只有回看的时候收到
//                my_server_to_client_file_info.set_string(my_packet_head, set_byte_array.constData());
//                showFileInfo();
//                current_read_state = READ_PACKET_HEAD;
//                current_byte_num_to_read = kPacketHeadLen;
//                break;
            /*
            这一步不需要了，被notify包所取代
            case READ_SERVER_TO_CLIENT_TEXT_FILE_CONTAIN://收到文件内容信息，将相应信息放入报头，然后进行下载操作
                my_server_to_client_text_file_contain.set_string(my_packet_head, set_byte_array.constData());
                writeFileContain();
                current_read_state = READ_PACKET_HEAD;
                current_byte_num_to_read = kPacketHeadLen;
                break;
            */

            case READ_SERVER_TO_CLIENT_USER_SET_UPDATE://收到设置用户信息，将相应信息放入报头，然后做相应的设置操作
                my_server_to_client_user_set_update.set_string(my_packet_head, set_byte_array.constData());
                setConfig();
                current_read_state = READ_PACKET_HEAD;
                current_byte_num_to_read = kPacketHeadLen;
                break;

//            case READ_C2C_FILE_NOTIFY_REQUEST://recv收到请求用户发送包
//                my_sender_to_receiver_file_notify.set_string(my_packet_head, set_byte_array.constData());
//                showTryToSend();
//                current_read_state = READ_PACKET_HEAD;
//                current_byte_num_to_read = kPacketHeadLen;
//                break;

//            case READ_C2C_FILE_NOTIFY_ACCEPT://sender收到同意请求包
//                my_sender_to_receiver_file_notify.set_string(my_packet_head, set_byte_array.constData());
//                sendFileData();//向recv发送数据包
//                current_read_state = READ_PACKET_HEAD;
//                current_byte_num_to_read = kPacketHeadLen;
//                break;

//            case READ_C2C_FILE_DATA://recv收到数据包
//                qDebug() << "READ_C2C_FILE_DATA";
//                my_sender_to_receiver_file_data.set_string(my_packet_head, set_byte_array.constData());
//                writeDataAndRequest();//写数据并发送请求用户发送包
//                current_read_state = READ_PACKET_HEAD;
//                current_byte_num_to_read = kPacketHeadLen;
//                break;

//            case READ_C2C_FILE_NOTIFY_CANCEL_RECV://send收到取消接收包
//                my_sender_to_receiver_file_notify.set_string(my_packet_head, set_byte_array.constData());
//                cancelSendFileDataPassive();//取消发送
//                current_read_state = READ_PACKET_HEAD;
//                current_byte_num_to_read = kPacketHeadLen;
//                break;

//            case READ_C2C_FILE_NOTIFY_CANCEL_SEND://recv收到取消发送包
//                my_sender_to_receiver_file_notify.set_string(my_packet_head, set_byte_array.constData());
//                cancelRecvFileDataPassive();//取消接收
//                current_read_state = READ_PACKET_HEAD;
//                current_byte_num_to_read = kPacketHeadLen;
//                break;

            /*----------------------------------------------------------------------
            Extend包的额外状态
            -------------------------------------------------------------------------*/

            case READ_EXTEND_STATUS_IN_GAME:
                my_extend_packet_status.set_string(my_packet_head, set_byte_array.constData());
                inGame();//某两个人进入游戏
                current_read_state = READ_PACKET_HEAD;
                current_byte_num_to_read = kPacketHeadLen;
                break;

            case READ_EXTEND_STATUS_OFF_GAME:
                my_extend_packet_status.set_string(my_packet_head, set_byte_array.constData());
                offGame();//某两个人退出游戏
                current_read_state = READ_PACKET_HEAD;
                current_byte_num_to_read = kPacketHeadLen;
                break;

            case READ_EXTEND_BUILD_AND_DESTROY_INVITE:
                my_extend_packet_build_and_destroy.set_string(my_packet_head, set_byte_array.constData());
                showInvitation();//展示邀请
                current_read_state = READ_PACKET_HEAD;
                current_byte_num_to_read = kPacketHeadLen;
                break;

            case READ_EXTEND_BUILD_AND_DESTROY_ACCEPT:
                my_extend_packet_build_and_destroy.set_string(my_packet_head, set_byte_array.constData());
                recvAcceptInvitation();//对方接收了您的邀请
                current_read_state = READ_PACKET_HEAD;
                current_byte_num_to_read = kPacketHeadLen;
                break;

            case READ_EXTEND_BUILD_AND_DESTROY_REJECT:
                my_extend_packet_build_and_destroy.set_string(my_packet_head, set_byte_array.constData());
                recvDeclineInvitation();//对方拒绝了您的邀请
                current_read_state = READ_PACKET_HEAD;
                current_byte_num_to_read = kPacketHeadLen;
                break;
                
            case READ_EXTEND_BUILD_AND_DESTROY_CANCEL:
                my_extend_packet_build_and_destroy.set_string(my_packet_head, set_byte_array.constData());
                cancelGamePassive();//对方取消了游戏
                current_read_state = READ_PACKET_HEAD;
                current_byte_num_to_read = kPacketHeadLen;
                break;
            
            case READ_EXTEND_BUILD_AND_DESTROY_NOT_START:
                my_extend_packet_build_and_destroy.set_string(my_packet_head, set_byte_array.constData());
                cancelInvitationPassive();//对方主动取消了邀请
                current_read_state = READ_PACKET_HEAD;
                current_byte_num_to_read = kPacketHeadLen;
                break;

            case READ_EXTEND_PREDICT_GUESS:
                my_extend_packet_playing.set_string(my_packet_head, set_byte_array.constData());
                replyPointState();//收到了猜测位置包
                current_read_state = READ_PACKET_HEAD;
                current_byte_num_to_read = kPacketHeadLen;
                break;
            
            case READ_EXTEND_PREDICT_JUDGE:
                my_extend_packet_playing.set_string(my_packet_head, set_byte_array.constData());
                replyAssertPlanePos();//收到了断言位置包
                current_read_state = READ_PACKET_HEAD;
                current_byte_num_to_read = kPacketHeadLen;
                break;

            default:
                qDebug() << "switch current_read_state case lost";
        }
    }
}


// TODO
void TcpClient::direction_on_changed(int index){
    qDebug() << index << "\n";
    direction_index = index;
}


// TODO
void TcpClient::my_board_doubleclicked(int row, int column){
    qDebug() << "double clicked —— colunm:" << column << " row:" << row << "\n";

    // setPlane
    setPlane(row, column);
}

void TcpClient::oppo_board_clicked(int row, int column) {
    qDebug() << "colunm:" << column << " row:" << row << "\n";
    cur_oppo_row = row;
    cur_oppo_column = column;

    auto button = static_cast<QPushButton*>(gameWindow->layout()->itemAt(2)->layout()->itemAt(2)->widget());
    button->setEnabled(true);

    if(!isGuess){
        setOppoPlane(row, column);
    }
}

void TcpClient::oppo_board_entered(int row, int column) {
    if(!isGuess) {
        // preview
        previewOppoPlane(row, column);
    }
}

void TcpClient::my_board_entered(int row, int column){
    qDebug() << "clicked —— colunm:" << column << " row:" << row << "\n";

    // previewPlane
    previewPlane(row, column);
}

void TcpClient::on_setstate(){
    QObject * object = QObject::sender();
    auto button = static_cast<QPushButton*>(qobject_cast<QWidget*>(object));
    if(button->text() == "猜测"){
        isGuess = true;
    } else if(button->text() == "断言"){
        isGuess = false;
    }
}

void TcpClient::on_ackbtn_clicked(){
    if(isGuess) {
        askForPointState();
    } else {
        assertPlanePos();
    }
}
