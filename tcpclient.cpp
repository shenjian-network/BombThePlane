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

    QFont ft;
    ft.setPointSize(15);
    curChatter->setFont(ft);

    // 右栏，包括消息窗口和发送消息栏
    rightStackLayout = new QStackedLayout;
    rightStackLayout->setStackingMode(QStackedLayout::StackOne);

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

    if(rightStackLayout){
        delete rightStackLayout;
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

// 显示聊天室的新窗口，用于聊天(FINISHED)
void TcpClient::chatRoomGUI(){
    chatRoomWindow = new QWidget;
    chatRoomWindow->setWindowTitle("ChatRoom");
    chatRoomWindow->setFixedSize(800, 630);

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


    rightStackLayout->addWidget(new QWidget);

    chatRoomMainLayout = new QHBoxLayout;
    chatRoomMainLayout->addLayout(leftLayout);
    chatRoomMainLayout->addLayout(rightStackLayout);
    chatRoomMainLayout->setStretchFactor(leftLayout, 1);
    chatRoomMainLayout->setStretchFactor(rightStackLayout, 2);
    chatRoomMainLayout->setMargin(0);

    chatRoomWindow->setLayout(chatRoomMainLayout);
    chatRoomWindow->show();
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

//TODO
// 下线的操作 （FINISHED）
// 需要熄灭用户栏，如果下线的是自己，会退出
/*如果下线的人你正在邀请，那么弹窗关闭，显示“对方已下线”*/
void TcpClient::offline(){
    QString name = my_server_to_client_inform.get_user_name();

    // 1. 下线的是自己，代表被T了
    if(name == this->username){
        chatRoomWindow->close();
        errorGUI("您被T了");
        exit(1);
    } else{
        //如果当前我在传输文件，并且下线的人正好是我传输的那个人，那么终止传输
        if(recvFile.size()){
            if(senderName == name){
                cancelRecvFileDataPassive();
            }
        }

        setUserStatus(name, false);
    }
}

//TODO
/*还需要有一个尚未冻结的邀请按钮*/
// 上线的操作（FINISHED）
// 需要点亮用户栏
void TcpClient::online(){
    QString name = my_server_to_client_inform.get_user_name();

    setUserStatus(name, true);
}

void TcpClient::setEnableFileTransfer(bool isEnable){
    int num = rightStackLayout->count();
    QWidget* cur;
    QPushButton* button;
    for(int i = 1;i < num; ++i){
        cur = rightStackLayout->itemAt(i)->widget();
        qDebug() << cur->layout()->itemAt(1)->widget()->children().count();
        button = static_cast<QPushButton*>(cur->layout()->itemAt(2)->widget()->children().at(3));
        button->setEnabled(isEnable);
    }
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
    //TODO
    /*这部分改成只有背景颜色的选项*/
    //TODO

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
    printf("%s\n", configData.c_str());

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
    setConfigImpl(atoi(configMap["bgcolor"].c_str()));

}

// 向用户列表中添加一项 （FINISHED)
void TcpClient::insertListWidget(QString name, bool isOnline){
    static int index = 0;
    QListWidgetItem * item = new QListWidgetItem;
    QLabel * userLabel = new QLabel;
    ClickableLabel * user = new ClickableLabel(name, userLabel);
    user->setAlignment(Qt::AlignVCenter);
    user->setFixedHeight(65);
    user->setMargin(23);
    item->setSizeHint(user->sizeHint());

    QFont ft;
    ft.setPointSize(12);
    user->setFont(ft);

    connect(user, SIGNAL(clicked()), this, SLOT(userLabelClicked()), Qt::QueuedConnection);

    if(isOnline){
        user->setEnabled(true);
        user->setStyleSheet("QLabel{background: rgb(46, 50, 56)}"
                                 "QLabel{color: rgb(255, 255, 255)}"
                                 "QLabel:hover{background: rgb(58,63,69)}");
    }else{
        user->setEnabled(false);
        user->setStyleSheet("QLabel{background: rgb(46, 50, 56)}"
                            "QLabel:hover{background: rgb(58,63,69)}"
                            "QLabel{color: rgb(128, 128, 128)}");
    }

    InitRightLayout();

    user2Index.insert(name, ++index);

    userList->addItem(item);
    userList->setItemWidget(item, user);
}

// 设置用户名的状态 (FINISHED)
void TcpClient::setUserStatus(QString name, bool isOnline){
    int count = userList->count();

    QListWidgetItem * item;
    QWidget * widget;
    QLabel * box;
    QString curName;

    qDebug() << "setUserStatus1";
    int flag = false;
    // O(n)遍历
    for(int i = 0;i < count; ++i){
        item = userList->item(i);
        widget = userList->itemWidget(item);
        box = static_cast<QLabel*>(widget);
        curName = box->text();

        if(curName == name){
            flag = true;
            if(isOnline){
                box->setEnabled(true);
                box->setStyleSheet("QLabel{background: rgb(46, 50, 56)}"
                                         "QLabel{color: rgb(255, 255, 255)}"
                                         "QLabel:hover{background: rgb(58,63,69)}");
            } else {
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
            insertListWidget(QString(name), int(uinfo[i][32]-'0'));
        }

        insertListWidget(QString("群聊"), true);
    }
}

// 显示消息，带GUI
void TcpClient::showTextImpl(QString name, QString msg, QString tm, bool isMyself){
    QWidget * cur = rightStackLayout->itemAt(user2Index[name])->widget();
    QTextBrowser* curTextBrowser = static_cast<QTextBrowser*>(cur->layout()->itemAt(1)->widget());
    if(isMyself){
        name = username;
    }
    curTextBrowser->append(name + " <" + tm + "> ");
    curTextBrowser->append(msg);
}


void TcpClient::InitRightLayout(){
    QWidget * right = new QWidget;

    QVBoxLayout* rightLayout = new QVBoxLayout;
    // 右栏，包括消息窗口和发送消息栏

    QLabel* label = new QLabel;
    label->setAlignment(Qt::AlignCenter);
    label->setFixedHeight(50);
    label->setStyleSheet("background: white; color: black;border-bottom: 1px solid rgb(230, 230, 255);");

    QFont ft;
    ft.setPointSize(15);
    label->setFont(ft);

    rightLayout->addWidget(label);

    QTextBrowser * textfield = new QTextBrowser;
    textfield->setStyleSheet("border:0px; border-bottom: 1px solid rgb(230,230,250);");
    rightLayout->addWidget(textfield);

    QWidget * textEditArea = new QWidget;

    QPlainTextEdit * textedit = new QPlainTextEdit;
    textedit->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    textedit->setStyleSheet("border:0px; color: black;");
    textedit->setParent(textEditArea);
    textedit->setGeometry(0, 0, 530, 200);
    ft.setPointSize(12);
    textedit->setFont(ft);

    QPushButton * send = new QPushButton("Send");
    send->setStyleSheet("QPushButton:hover{background: rgb(248, 248, 248);}");
    send->setFixedSize(95, 30);
    send->setGeometry(415, 130, 95, 30);
    send->setParent(textEditArea);
    send->setShortcut(Qt::Key_F1);

    connect(send, SIGNAL(clicked()),  this, SLOT(on_sendBtn_clicked()), Qt::QueuedConnection);

    QPushButton * review = new QPushButton("Review");
    review->setStyleSheet("QPushButton:hover{background: rgb(248, 248, 248);}");
    review->setFixedSize(95, 30);
    review->setGeometry(300, 130, 95, 30);
    review->setParent(textEditArea);

    connect(review, SIGNAL(clicked()),  this, SLOT(cls()), Qt::QueuedConnection);

    QPushButton * file = new QPushButton("File");
    file->setStyleSheet("QPushButton:hover{background: rgb(248, 248, 248);}");
    file->setFixedSize(95, 30);
    file->setGeometry(185, 130, 95, 30);
    file->setParent(textEditArea);

    connect(file, SIGNAL(clicked()),  this, SLOT(on_fileDialogBtn_clicked()), Qt::QueuedConnection);

    rightLayout->addWidget(textEditArea);
    rightLayout->setStretchFactor(textfield, 2);
    rightLayout->setStretchFactor(textEditArea, 1);
    rightLayout->setMargin(0);
    rightLayout->setSpacing(0);

    right->setLayout(rightLayout);
    // stack
    rightStackLayout->addWidget(right);
}


void TcpClient::setConfigImpl(int bgcolor){

    //TODO
    /*这部分只修改背景颜色*/
    int num = rightStackLayout->count();
    QWidget* cur;
    QTextBrowser* curTextBrowser;
    for(int i = 1;i < num; ++i){
        cur = rightStackLayout->itemAt(i)->widget();
        qDebug() << cur->layout()->count();
        curTextBrowser = static_cast<QTextBrowser*>(cur->layout()->itemAt(1)->widget());
        curTextBrowser->setStyleSheet("font-size: " + QString::number(fontsize)
                                      + "px;" + "color: " + COLOR[color] + ";");
    }
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
}

//TODO
void TcpClient::offGame()
{
    /*
    提示某两个人结束了游戏（对于两人中的任意一个，若仍在线，左侧用户栏恢复其按钮）
    如果其中有一个人是自己，那么根据结果弹窗“祝贺您胜利了”或“很遗憾您输了”，然后关闭这个弹窗以后回到主界面
    */
}

//TODO
void TcpClient::inviteGame()
{
    //获得对方用户名oppoName

   //TODO
   //发送一个请求对战包
}

//TODO
void TcpClient::showInvitation()
{
    //TODO
    //首先判断自己是否在游戏中，如果在游戏中那么就丢弃这个包

    //GUI部分，显示弹窗，同意或拒绝
}

//TODO
void TcpClient::acceptInvitation()
{
    //向对端发送同意邀请包（这时候server会向所有用户发送ingame包*/
}

//TODO
void TcpClient::declineInvitation()
{
    //向对端发送拒绝邀请包
}

//TODO
void TcpClient::cancelGameActive()
{
    //向对端发送主动取消游戏包（这时候server会向所有用户发送offgame包）
}

//TODO
void TcpClient::cancelGamePassive()
{
    /*
    被动取消游戏
    显示“对方退出了游戏”
    */
}

/*---------------------------------------------------------------------------------
以下部分是摆飞机的部分
左边显示的是oppoBoard，表示自己猜对方棋盘的进度
右边显示的是myBoard，表示对方猜测自己棋盘的进度
-----------------------------------------------------------------------------------*/

//TODO
void TcpClient::offensive()
{
    /*将状态转为先手，然后恢复oppoBoard棋盘（取消冻结）*/
}

void TcpClient::defensive()
{
    /*将状态转为后手，然后冻结oppoBoard棋盘*/
}

//TODO
/*点击摆放完成按钮触发*/
void TcpClient::gameReady()
{
    //摆放好了以后，首先先要判断三个位置是否合法，如果不合法要提示错误


    //如果合法，发送gameReady包，表示三个的位置
}

//TODO
void TcpClient::gameStart()
{
    //接收到server发送的gameStart包，冻结或隐藏右侧的摆放栏，并提示游戏开始以及先后手，如果自己是后手，冻结棋盘
}


/*---------------------------------------------------------------------------------
以下部分是对战的部分
左边显示的是oppoBoard，表示自己猜对方棋盘的进度
右边显示的是myBoard，表示对方猜测自己棋盘的进度
-----------------------------------------------------------------------------------*/

//TODO
void TcpClient::askForPointState()
{
    //点击棋盘上的一个按钮，发送请求包

    //首先获取棋盘的坐标cord

}

//TODO
void TcpClient::replyPointState()
{
    //GUI部分，myBoard显示对方猜的位置和结果
    //回复棋盘坐标和相应的状态，然后转为先手
}

//TODO
void TcpClient::recvReplyPointState()
{
    //GUI部分，oppoBoard显示自己猜测的结果，然后转为后手
}

//TODO
//点击“断言”按钮后触发
void TcpClient::assertPlanePos()
{
    //GUI部分，获取飞机的位置

    //发送断言包
}

//TODO
void TcpClient::replyAssertPlanePos()
{
    //GUI部分，显示对方的断言位置

    //判断是否猜中并发送回复断言包
}

//TODO
void TcpClient::recvReplyAssertPlanePos()
{
    //GUI部分，显示断言结果，如果猜中要将整个飞机显示出来，否则显示断言错误（比如闪烁几下）

}

//TODO
void TcpClient:

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
    ClickableLabel * user = qobject_cast<ClickableLabel*>(object);
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
    rightStackLayout->setCurrentIndex(index);
    qDebug() << index;
    QLabel* label = static_cast<QLabel*>(rightStackLayout->currentWidget()->layout()->itemAt(0)->widget());
    label->setText(user->text());

    qDebug() << "点击了" << user->text() << " Index:" << index;
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
                    //收到用户设置包，进入相应的状态
                    case PacketHead::kS2CUserSet:
                        current_read_state = READ_SERVER_TO_CLIENT_USER_SET_UPDATE;
                        current_byte_num_to_read = my_packet_head.get_length();
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

            case READ_SERVER_TO_CLIENT_USER_SET_UPDATE://收到设置用户信息，将相应信息放入报头，然后做相应的设置操作
                my_server_to_client_user_set_update.set_string(my_packet_head, set_byte_array.constData());
                setConfig();
                current_read_state = READ_PACKET_HEAD;
                current_byte_num_to_read = kPacketHeadLen;
                break;


            default:
                qDebug() << "switch current_read_state case lost";
        }
    }
}