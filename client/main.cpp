#include "tcpclient.h"
#include <QApplication>
#include "tcpport.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TcpClient w;

    TcpPort tcp_port;
    tcp_port.make_exec();

    int port = tcp_port.get_port();
    QString ip = tcp_port.get_ip();

    qDebug() << ip;
    if(!w.ConnectToHost(ip, port)){
        w.errorGUI("网络连接异常");
        return 0;
    }

    //    // 显示登录GUI
    w.loginGUI();
    //    w.chatRoomGUI();
    //w.InitGameWindow();


    return a.exec();


}
