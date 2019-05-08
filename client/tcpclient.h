#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QListWidget>
#include <QTime>
#include <QVBoxLayout>
#include <QStackedLayout>
#include <QProgressDialog>
#include <QNetworkConfigurationManager>
#include <map>
#include <QQueue>
#include <QMessageBox>
#include "../common/client_to_server.h"
#include "../common/packet_head.h"
#include "../common/extend_packet.h"
#include "../common/server_to_client.h"
#include "clickablelabel.h"
#include "board.h"


namespace Ui {
class TcpClient;
}

const unsigned int kPacketHeadLen = 8;
const unsigned int kFileDataLen = 4096 + 99 * 1024;
enum ReadState
{
    READ_PACKET_HEAD,
    READ_SERVER_TO_CLIENT_REPORT_SUCCESS,
    READ_SERVER_CLIENT_ONLINE,
    READ_SERVER_CLIENT_OFFLINE,
    READ_SERVER_TO_CLIENT_TEXT_SIMPLE_TEXT,
    READ_SERVER_TO_CLIENT_TEXT_FILE_INFO,
    READ_SERVER_TO_CLIENT_TEXT_FILE_CONTAIN,
    READ_SERVER_TO_CLIENT_USER_SET_UPDATE,
    READ_C2C_FILE_NOTIFY_REQUEST,
    READ_C2C_FILE_NOTIFY_CANCEL_SEND,
    READ_C2C_FILE_NOTIFY_ACCEPT,
    READ_C2C_FILE_NOTIFY_CANCEL_RECV,
    READ_C2C_FILE_DATA,

    /*----------------------------------------------------------------------
    Extend包的额外状态
    -------------------------------------------------------------------------*/
    READ_EXTEND_PREDICT_GUESS,
    READ_EXTEND_PREDICT_JUDGE,
    READ_EXTEND_STATUS_IN_GAME,
    READ_EXTEND_STATUS_OFF_GAME,
    READ_EXTEND_BUILD_AND_DESTROY_INVITE,
    READ_EXTEND_BUILD_AND_DESTROY_ACCEPT,
    READ_EXTEND_BUILD_AND_DESTROY_REJECT,
    READ_EXTEND_BUILD_AND_DESTROY_CANCEL,
    READ_EXTEND_BUILD_AND_DESTROY_NOT_START
};


const QString COLOR[8] = {"black", "red", "grey", "blue", "green", "yellow", "orange", "purple"};

struct fileTrans
{
    FILE* fd = nullptr;
    int blockCnt = 0;
    int len = 0;
    fileTrans() = default;
    fileTrans(FILE* _fd, int _blockCnt, int _len) : fd(_fd), blockCnt(_blockCnt), len(_len) {}
};

class TcpClient : public QMainWindow
{
    Q_OBJECT

public:
    explicit TcpClient(QWidget *parent = nullptr);
    ~TcpClient();

    bool ConnectToHost(const QString& ip, unsigned short port);

    void loginGUI();

    // msg box,
    void errorGUI(const unsigned short & err_type);

    void errorGUI(const QString& err);

    void successGUI(const QString& err);

    void inviteSrcGUI(const QString& name);

    void inviteDstGUI(const QString& name);

    //显示更改密码成功窗口
    void changePwdSuccessGUI();

    void reportSuccess();

    void chatRoomGUI();  //聊天室新窗口



    // two conditions... if it is myself, yor'are out; if it is other, modify GUI
    void offline();

    // only one condition
    void online();



    void showText();

    //显示文件信息
    void showFileInfo();

    //将文件内容从包写入文件，注意在调用该函数前，文件内容已经被写入了ServerToClientTextFileContain包，因此这部分只要显示下载文件完成等即可
    void writeFileContain();


    //单个配置对应的一行String
    std::string singleConfigString(std::string configKey);

    //将config转换成string输出
    std::string configString();



    // read config
    void setConfig();

    void insertListWidget(QString name, bool isOnline, bool isInGame);

    void selectAll();

    void selectNone();

    void getCheckState(QVector<bool>& vecIsChecked,  QVector<QString>& vecName);

    void setUserStatus(QString name, bool isOnline, bool isInGame);

    void showTextImpl(QString name, QString msg, QString tm, bool isMyself=false);

    void tryToSend(QString filename);

    void InitRightLayout();



    void showTryToSend();

    void sendFileData();//发送数据包

    void writeDataAndRequest();//rec从包中获得数据并向send发送新的请求


    void cancelSendFileDataPassive();//send被动取消发送，由对面的取消包触发

    void cancelRecvFileDataPassive();//recv被动取消接收，由对面的取消包触发

    void showFileTransferring(std::string senderName, std::string recvName, std::string fileName, bool isSender);

    void errorFileTransferring(std::string senderName, std::string recvName, std::string fileName);

    void cancelFileTransferring(std::string senderName, std::string recvName, std::string fileName, bool isSender);

    void doneFileTransferring(std::string senderName, std::string recvName, std::string fileName, bool isSender);

    void setConfigImpl(int fontsize, int color);

    void setEnableFileTransfer(bool isEnable);


    bool isConnected();


    void inGame();

    void offGame();



    void showInvitation();

    void sendAcceptInvitationPacket();

    void sendDeclineInvitationPacket();

    void recvAcceptInvitation();

    void recvDeclineInvitation();

    void cancelInvitationPassive();

    void cancelGamePassive();


    void offensive();

    void defensive();

    void gameReady();

    void gameStart();

    void askForPointState();

    void replyPointState();

    void recvReplyPointState(const unsigned short res);

    void assertPlanePos();

    void replyAssertPlanePos();

    void recvReplyAssertPlanePos(const unsigned short res);

    void gameOver(bool isWinner);

private slots:
    // Signal func to handle read event
    //
    void readyRead();

    // Signal func to handle disconnection with Server
    void clientDisconnected();


    // login func
    // send a packet containing and wait
    // get_string write

    void on_loginBtn_clicked();

    void on_signupBtn_clicked();

    void on_sendBtn_clicked();

    // 在登录界面，点击“修改密码”，将显示对话框
    void on_changePwdBtn_clicked();

    // 在修改密码界面，点击“确认”，将发送
    void on_changePwdAckBtn_clicked();

    // 在修改密码界面，点击“取消”，将关闭窗口
    void on_changePwdCancelBtn_clicked();

    // 登录界面，显示密码
    void on_showPwdCheckBox_stateChanged();

    // 点击用户触发函数
    void userLabelClicked();

    // 更新时间
    void timeUpdate();

    void configGUI();

    //向server发送配置包
    void sendConfig();

    void cls();

    //发送请求回看包
    void askForReview();

    void on_fileDialogBtn_clicked();


    void acceptRecv();

    void cancelRecvFileDataActive();//recv主动取消发送 (GUI触发)

    void cancelSendFileDataActive();//send主动取消发送（GUI触发）


    void inviteGame();

    void cancelGameActive();

    void acceptInvitation();

    void declineInvitation();

    void cancelInvitationActive();
private:
    PacketHead my_packet_head;
    ServerToClientReportSuccess my_server_to_client_report_success;
    ServerToClientInform my_server_to_client_inform;
    ServerToClientTextSimpleText my_server_to_client_simple_text;
    ServerToClientTextFileInfo my_server_to_client_file_info;
    ServerToClientTextFileContain my_server_to_client_text_file_contain;
    ServerToClientUserSetUpdate my_server_to_client_user_set_update;
    SenderToReceiverFileNotify my_sender_to_receiver_file_notify;
    SenderToReceiverFileData my_sender_to_receiver_file_data;

    /*----------------------------------------------------------------------
    Extend包的额外报文
    -------------------------------------------------------------------------*/

    ExtendPacketStatus my_extend_packet_status;
    ExtendPacketBuildAndDestroy my_extend_packet_build_and_destroy;
    ExtendPacketPlaying my_extend_packet_playing;

    QTcpSocket *socket;
    Ui::TcpClient *ui;

private:
    QWidget *loginWindow;
    QWidget *chatRoomWindow;
    QWidget *changePwdWindow;
    QWidget *configWindow;
    QWidget *fileWindow;
    QListWidget * userList;
    QTime time;

    QString username;
    QString password;
    QString ip;
    std::map<std::string, std::string> configMap;
    std::map<std::string, fileTrans> sendFile;
    std::map<std::string, fileTrans> recvFile;
    unsigned short port;

    ClickableLabel * preChatter; // 上一个与你对话的用户
    QLabel * curChatter; // 现在与你对话的用户
    QMap <QPushButton*,QString> button2name;
    QStackedLayout * rightStackLayout; // 现在与你对话的用户的聊天窗
    QHBoxLayout * chatRoomMainLayout;
    QMap <QString, int> user2Index;
    int curIndex;

    QProgressDialog* pdlg;

    QString senderName;
    QString recvName;
    QString fileName;
    int fileLen;

    QString opponame;
    QString inviteName;
    QString rejectName;

    bool isOnline;

    Board * my_board;
    Board * oppo_board;
    bool isGaming;
    bool isInviting;

    QSet<QString> invitingName;


    // BOX
    QMessageBox * inviteSrcBox;  // 发出的邀请，一次只能一个
    QMap<QString, QMessageBox*> inviteDstBoxList;   // 邀请列表，能够接收到多个
    QMap <QAbstractButton*, QString> acceptbutton2opponame; // 邀请人的名字
    QMap <QAbstractButton*, QString> rejectbutton2opponame; // 邀请人的名字
};

#endif // TCPCLIENT_H
