#include "tcpport.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRegExp>
#include <QRegExpValidator>

TcpPort::TcpPort(QWidget* parent)
{
    tcp_port = new QDialog();
    tcp_port->setFixedSize(200, 100);
    auto layout = new QVBoxLayout;

    auto sublayout1 = new QHBoxLayout;
    auto label1 = new QLabel("ip");
    auto ip_edit = new QLineEdit();
    ip_edit->setText("10.60.102.252");
    sublayout1->addWidget(label1);
    sublayout1->addWidget(ip_edit);

    auto sublayout2 = new QHBoxLayout;
    auto label2 = new QLabel("端口");
    auto port_edit = new QLineEdit();
    port_edit->setText("20054");
    QRegExp rx("^\d+$");
    auto pReg = new QRegExpValidator(rx, this);
    port_edit->setValidator(pReg);
    sublayout2->addWidget(label2);
    sublayout2->addWidget(port_edit);

    auto button = new QPushButton("确定");
    layout->addLayout(sublayout1);
    layout->addLayout(sublayout2);
    layout->addWidget(button);
    tcp_port->setLayout(layout);
    connect(button, SIGNAL(clicked()), this, SLOT(on_btn_clicked()));
}

int TcpPort::get_port() {
    auto port_edit = static_cast<QLineEdit*>(tcp_port->layout()->itemAt(1)->layout()->itemAt(1)->widget());
    return port_edit->text().toInt();
}

QString TcpPort::get_ip(){
    auto ip_edit = static_cast<QLineEdit*>(tcp_port->layout()->itemAt(0)->layout()->itemAt(1)->widget());
    return ip_edit->text();
}


void TcpPort::make_exec(){
    tcp_port->exec();
}

TcpPort::~TcpPort(){
    delete tcp_port;
}

void TcpPort::on_btn_clicked(){
    tcp_port->close();
}
