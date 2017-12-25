#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QObject>
#include <QTimer>
#include <QtNetwork>
#include <QNetworkInterface>
#include "tool1.h"
#include "pd.h"
#include "camera.h"
class ServerInfoReporter : public QObject{
    Q_OBJECT
public:
    ServerInfoReporter(){
        timer=new QTimer();
        connect(timer,SIGNAL(timeout()),this,SLOT(check_client()));//TODO:maybe replace with readReady signal
        udp_skt = new QUdpSocket();
        udp_skt->bind(Protocol::SERVER_REPORTER_PORT,QUdpSocket::ShareAddress);
        // timer->start(1000);
    }
    ~ServerInfoReporter()
    {
        disconnect(timer);
        delete timer;
        delete udp_skt;
    }
    void start()
    {
        timer->start(1000);
    }

    void stop()
    {
        timer->stop();
    }

public  slots:
    void check_client()
    {
        //   prt(info,"checking client");
        QByteArray client_msg;
        char *msg;
        if(udp_skt->hasPendingDatagrams())
        {
            client_msg.resize((udp_skt->pendingDatagramSize()));
            udp_skt->readDatagram(client_msg.data(),client_msg.size());
            prt(info,"msg :%s",msg=client_msg.data());
            if(!strcmp(msg,"pedestrian"))
                send_buffer_to_client();
            //   udp_skt->flush();
        }else{
            //prt(debug,"searching client on port %d",Protocol::SERVER_REPORTER_PORT)
        }
    }

    void send_buffer_to_client()
    {
        QByteArray datagram;
        datagram.clear();
        QList <QNetworkInterface>list_interface=QNetworkInterface::allInterfaces();
        foreach (QNetworkInterface i, list_interface) {
            if(i.name()!="lo"){
                QList<QNetworkAddressEntry> list_entry=i.addressEntries();
                foreach (QNetworkAddressEntry e, list_entry) {
                    if(e.ip().protocol()==QAbstractSocket::IPv4Protocol)
                    {
                        datagram.append(QString(e.ip().toString())).append(QString(",")).\
                                append(QString(e.netmask().toString())).append(QString(",")).append(QString(e.broadcast().toString()));
                    }

                }
            }
        }
        udp_skt->writeDatagram(datagram.data(), datagram.size(),
                               QHostAddress::Broadcast, Protocol::CLIENT_REPORTER_PORT);
    }
private:
    QTimer *timer;
    QUdpSocket *udp_skt;
};


class CameraManager;
// extern  CameraManager mgr;
class ClientSession:public QObject{
    Q_OBJECT
public:
    ClientSession(QTcpSocket *client_skt,CameraManager *p):skt(client_skt),p_manager(p){
        connect(skt,SIGNAL(readyRead()),this,SLOT(read_all()));
        connect(skt,SIGNAL(disconnected()),this,SLOT(deleteLater()));
        connect(skt,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(socket_error()));
        //  connect(skt,SIGNAL(aboutToClose()),this,SLOT(delete_client()));

        udp_skt=new QUdpSocket();
        // QHostAddress a;
        // udp_skt->bind(a,12349);
        //   timer=new QTimer();
        //  connect(timer,SIGNAL(timeout()),this,SLOT(send_rst_to_client()));
        //    timer->start(1000);
        client_addr=skt->peerAddress();
        //  connect(skt,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(test()));
        //                connect(skt,SIGNAL(aboutToClose()),this,SLOT(test()));
        //                connect(skt,SIGNAL(disconnected()),this,SLOT(test()));



    }
    ~ClientSession()
    {
        //   delete timer;
        //    disconnect(timer,SIGNAL(timeout()),this,SLOT(send_rst_to_client()));
        delete udp_skt;
    }

public slots:
    void socket_error()
    {
        emit socket_error(this);
    }

    void send_rst_to_client(QByteArray ba)
    {
        if(skt->state()==QTcpSocket::ConnectedState)
        {
            udp_skt->writeDatagram(ba,ba.length(),client_addr,Protocol::SERVER_DATA_OUTPUT_PORT);
        }
    }

    //    void simple_reply()
    //    {
    //        QByteArray client_buf=skt->readAll();


    //        QByteArray block;
    //        block.append(client_buf[0]+1);
    //        //        QString str("1234567890");
    //        //        block.append(str);
    //        skt->write(block);
    //        //  skt->disconnectFromHost();
    //    }
    //    void welcom_reply(){

    //        QByteArray block;
    //        QString str("welcom client ");
    //        block.append(str);
    //        skt->write(block);

    //    }

    void read_all(){
        // CameraManager *pa=(CameraManager *)pt;
        //   skt->waitForReadyRead();
        int writes_num=0;

        QByteArray client_buf=skt->readAll();
        //   prt(info,"get config");
        rcv_buf=client_buf.data();
        CameraManager &mgr=CameraManager::GetInstance();
        int ret_size=mgr.handle_cmd(rcv_buf,send_buf,client_buf.size());
        writes_num=skt->write(send_buf,ret_size);
        prt(info,"send %d bytes",writes_num);
        //       int len=client_buf.length();
        //        int ret=0;
        //        int cmd=Protocol::get_operation(client_buf.data());
        //        int pkg_len=Protocol::get_length(client_buf.data());
        //        int cam_index=Protocol::get_cam_index(client_buf.data());
        //        memset(buf,0,Tools::BUF_MAX_LENGTH);
        //        QByteArray bta;
        //        switch (cmd) {
        //        case Protocol::ADD_CAMERA:
        //            prt(info,"protocol :adding   cam");

        //            bta.clear();
        //            bta.append((char *)client_buf.data()+Protocol::HEAD_LENGTH,pkg_len);
        //            p_manager->add_camera(bta);
        //            writes_num=skt->write(buf,ret+Protocol::HEAD_LENGTH);

        //            //     p_manager->add_camera();
        //            break;
        //        case Protocol::GET_CONFIG:
        //            prt(info,"protocol :send config");
        //#if 1
        ////            ret= p_manager->get_config(buf+Protocol::HEAD_LENGTH);
        ////            Protocol::encode_configuration_reply(buf,ret,Protocol::RET_SUCCESS);
        ////            writes_num=skt->write(buf,ret+Protocol::HEAD_LENGTH);
        //#else
        //            ret= p_manager->get_config(buf);
        //            skt->write(buf,ret);
        //#endif
        //            break;
        //        case Protocol::DEL_CAMERA:
        //            prt(info,"protocol :deling    cam %d ",cam_index);
        //            p_manager->del_camera(cam_index);
        //            writes_num=skt->write(buf,ret+Protocol::HEAD_LENGTH);

        //            break;
        //        default:
        //            break;
        //        }

    }

    void displayError(QAbstractSocket::SocketError socketError)
    {
        switch (socketError) {
        case QAbstractSocket::RemoteHostClosedError:
            break;
        case QAbstractSocket::HostNotFoundError:
            prt(info,"err");
            break;
        case QAbstractSocket::ConnectionRefusedError:
            prt(info,"err");
            break;
        default:break;

        }


    }
signals :
    int get_server_config(char *buf);
    void socket_error(ClientSession *c);
private:
    //char buf[Tools::BUF_MAX_LENGTH];
    char *rcv_buf;
    char send_buf[Pd::BUFFER_LENGTH];
    QTcpSocket *skt;
    CameraManager *p_manager;
    QUdpSocket *udp_skt; QTimer *timer;
    QHostAddress client_addr;
};
/*
    server provide
        tcpserver:handle clients session
        clients:hold all ClientSession
        systeminforeporter:recive udp request,and respond with ip ,netmask ,etc...
        camera manager:holds configuration , runing camera,manage add,delete and other operation

*/
class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent=0 ):QObject(parent){
        //cam_manager=new CameraManager((char *)"/root/repo-github/pedestrian-v8/server/config.json");
        //    reporter=new ServerInfoReporter();
        bool ret=false;
        server=new QTcpServer();
        ret=server->listen(QHostAddress::Any,Protocol::SERVER_PORT);
        if(ret){
            prt(info,"ok to listen on port %d",Protocol::SERVER_PORT);
        }else
        {
            prt(info,"err in listen on port %d",Protocol::SERVER_PORT);
            exit(1);
        }
        connect(server, &QTcpServer::newConnection, this, &Server::handle_incomimg_client);
    }
    ~Server(){
        //   delete reporter;
        delete cam_manager;
        delete server;
    }
    void print_server_info()
    {
        //     prt(info,"server started ");
    }

signals:
public slots:
    void handle_incomimg_client()
    {
        QTcpSocket *skt = server->nextPendingConnection();
        connect(skt, SIGNAL(disconnected()),skt, SLOT(deleteLater()));
        QString str(skt->peerAddress().toString());
        prt(info,"client ip %s, port %d connected",str.toStdString().data(),skt->peerPort());
        ClientSession *client=new ClientSession(skt,this->cam_manager);
        connect(client,SIGNAL(socket_error(ClientSession*)),this,SLOT(delete_client(ClientSession*)));
        clients.append(client);
    }
    void delete_client(ClientSession *c)
    {
        delete c ;
        clients.removeOne(c);
    }

    void output_2_client(QByteArray ba)
    {
        foreach (ClientSession  *c , clients) {
            c->send_rst_to_client(ba);
        }
    }

private:
    char recv_buf[Pd::BUFFER_MAX_LENGTH];
    char send_buf[Pd::BUFFER_MAX_LENGTH];
    // mutex mtx;
    CameraManager *cam_manager;//manage all cameras
    //  ServerInfoReporter *reporter;//repy query for system info
    QTcpServer *server;//server for reply all clients request ,execute client cmds,like add cam,del cam, reconfigure cam,etc..
    QList <ClientSession *> clients;//client that connected to server
};

#endif // SERVER_H
