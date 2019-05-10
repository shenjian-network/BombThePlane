#include "tcpport.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

TcpPort::TcpPort(QWidget* parent)
{
    tcp_port = new QDialog();
    tcp_port->setFixedSize(200, 100);
    auto layout = new QHBoxLayout;
    auto label = new QLabel("端口");
    auto port_edit = new QLineEdit();
    auto button = new QPushButton("确定");
    layout->addWidget(label);
    layout->addWidget(port_edit);
    layout->addWidget(button);
    tcp_port->setLayout(layout);
    connect(button, SIGNAL(clicked()), this, SLOT(on_btn_clicked()));
}

int TcpPort::get_port() {
    auto port_edit = static_cast<QLineEdit*>(tcp_port->layout()->itemAt(1)->widget());
    return port_edit->text().toInt();
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
