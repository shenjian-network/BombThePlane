#ifndef TCPPORT_H
#define TCPPORT_H

#include <QDialog>

class TcpPort: public QDialog
{
    Q_OBJECT
public:
    explicit TcpPort(QWidget* parent = nullptr);
    ~TcpPort();

    int get_port();

    QString get_ip();

    void make_exec();
private slots:
    void on_btn_clicked();
private:
    QDialog* tcp_port;
};

#endif // TCPPORT_H
