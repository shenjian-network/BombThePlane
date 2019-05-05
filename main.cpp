#include "tcpclient.h"
#include <QApplication>
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TcpClient w;

    string hostAddr;
    int hostPort;
    
    std::ifstream finHostConf("host.conf")
    if(!finHostConf.open()){
        hostAddr = "10.60.102.252";
        hostPort = 25998;
    }
    else
    {
        finHostConf >> hostAddr >> hostPort;
        if(finHostConf.fail())
        {
            hostAddr = "10.60.102.252";
            hostPort = 25998;
        }  
    }

    if(!w.ConnectToHost(hostAddr.c_str(), hostPort)){
        w.errorGUI("网络连接异常");
        return 0;
    }

    // 显示登录GUI
    w.loginGUI();


    return a.exec();
}
