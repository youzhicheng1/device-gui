#include <QCoreApplication>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>

// 本地模拟设备:监听 8888,收到数据回 "OK"
// 用途:上位机(Device_gui)在没有真实硬件时的通信测试对手
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QTcpServer server;
    if(!server.listen(QHostAddress::LocalHost,8888)){
        qDebug()<<"listen failed:"<<server.errorString();
        return 1;
    }
    qDebug()<<"sim device listening on 8888";

    QObject::connect(&server,&QTcpServer::newConnection,[&server](){
        QTcpSocket *sock=server.nextPendingConnection();
        qDebug()<<"client connected"<<sock->peerAddress().toString();

        QObject::connect(sock,&QTcpSocket::readyRead,[sock](){
            QByteArray data=sock->readAll();
            qDebug()<<"recv: "<<data;
            sock->write("OK\n");
        });
        QObject::connect(sock,&QTcpSocket::disconnected,[sock](){
            qDebug() << "client disconnected";
            sock->deleteLater();
        });
    });
    return QCoreApplication::exec();
}
