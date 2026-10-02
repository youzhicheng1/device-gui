#include <QCoreApplication>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QDataStream>
#include <QTimer>

static const int HEADER_SIZE=4;

QByteArray makePacket(const QByteArray &payload){
    QByteArray packet;
    QDataStream out(&packet,QIODevice::WriteOnly);  //设置流只写
    out.setByteOrder(QDataStream::BigEndian);       //大端
    out<<(quint32)payload.size();                   //向流内写长度
    packet.append(payload);                         //追加内容

    return packet;
}

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
            sock->write(makePacket("OK"));       //正常发送


            // QByteArray burst;
            // burst += makePacket("AAA");
            // burst += makePacket("BBB");
            // burst += makePacket("CCC");
            // sock->write(burst);                  //测试粘包

                                                    //测试半包
            // QByteArray packet = makePacket("HELLO");
            // sock->write(packet.left(3));            // 先发前 3 字节(连长度头都不完整)
            // QTimer::singleShot(200, [sock, packet](){
            //     sock->write(packet.mid(3));         // 200ms 后再发剩下的
            // });
        });
        QObject::connect(sock,&QTcpSocket::disconnected,[sock](){
            qDebug() << "client disconnected";
            sock->deleteLater();
        });
    });

    qDebug() << "packet size:" << makePacket("OK").size();
    qDebug() << "packet hex:"  << makePacket("OK").toHex(' ');
    return QCoreApplication::exec();
}
