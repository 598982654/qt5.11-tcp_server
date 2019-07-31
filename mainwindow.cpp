#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //界面初始化
    ui->ipaddr_TextEdit->setText(QNetworkInterface().allAddresses().at(1).toString());//获取本地IP
    ui->ipport_TextEdit->setText("8080");
    ui->listen_Button->setEnabled(true);
    ui->dispbuffclear_Button->setEnabled(true);
    ui->sendbuffclear_Button->setEnabled(true);
    ui->sendbuffsend_Button->setDisabled(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}
//
void MainWindow::new_connection_slot()
{
    current_tcp_client = tcp_server->nextPendingConnection();
    connected_tcp_client.append(current_tcp_client);//有新的链接，添加到列表里去
    ui->connectedtcpclient_Box->addItem(tr("%1:%2").arg(current_tcp_client->peerAddress().toString().split("::ffff:")[1])\
            .arg(current_tcp_client->peerPort()));
    connect(current_tcp_client, SIGNAL(readyRead()), this, SLOT(tcp_server_readdata()));
    connect(current_tcp_client, SIGNAL(disconnected()), this, SLOT(disconnected_slot()));
}
//实现槽函数功能
void MainWindow::tcp_server_readdata()
{
    int i = 0;
    static QString ip_port;
    static QString ip_port_prev;
    for(i = 0; i < connected_tcp_client.length(); i ++)//遍历所有连接的客户端信息
    {
        QByteArray receive_buff = connected_tcp_client[i]->readAll();
        if(receive_buff.isEmpty())
            continue;
        ip_port = tr("[%1:%2]:").arg(connected_tcp_client[i]->peerAddress().toString().split("::ffff:")[1])\
                .arg(connected_tcp_client[i]->peerPort());
        if(ip_port != ip_port_prev)//此次的端口地址和上次的不同，则打印此次客户端的地址和端口号
        {
            ui->dispbuff_textBrowser->append(ip_port);
        }
        ui->dispbuff_textBrowser->append(receive_buff);//显示客户端发送的信息
        //更新端口号
        ip_port_prev = ip_port;
    }
}
void MainWindow::disconnected_slot()
{
    int i = 0;
    for(i = 0; i < connected_tcp_client.length(); i ++)
    {
        if(connected_tcp_client[i]->state() == QAbstractSocket::UnconnectedState)
        {
            //删除box的客户端信息
            ui->connectedtcpclient_Box->removeItem(ui->connectedtcpclient_Box->findText(tr("%1:%2")\
                                                   .arg(connected_tcp_client[i]->peerAddress().toString().split("::ffff:")[1])\
                                                   .arg(connected_tcp_client[i]->peerPort())));
            //删除存储在connected_tcp_client列表中的信息
            connected_tcp_client[i]->destroyed();
            connected_tcp_client.removeAt(i);
        }
    }
}
//以下函数，系统自动生成
void MainWindow::on_listen_Button_clicked()
{

    if(ui->listen_Button->text() == "开始监听")
    {
        //创建一个tcp服务器对象
        tcp_server = new QTcpServer(this);
        //绑定槽
        connect(tcp_server,SIGNAL(newConnection()),this,SLOT(new_connection_slot()));
        bool listen_ok = tcp_server->listen(QHostAddress::Any,ui->ipport_TextEdit->toPlainText().toInt());
        if(listen_ok)
        {
            ui->listen_Button->setText("停止监听");
            ui->sendbuffsend_Button->setEnabled(true);//发送使能
        }
    }
    else
    {
        //断开所有连接
        int i = 0;
        printf("connected_tcp_client.length()=%d\r\n",connected_tcp_client.length());
        qDebug() << connected_tcp_client.length();
        for(i = 0; i < connected_tcp_client.length(); i ++)
        {
            connected_tcp_client[i]->disconnectFromHost();
            connected_tcp_client.removeAt(i);

        }
        tcp_server->close();//关闭服务器
        ui->listen_Button->setText("开始监听");
        ui->sendbuffsend_Button->setDisabled(true);//发送失能
        ui->connectedtcpclient_Box->clear();//清空列表
        ui->connectedtcpclient_Box->insertItem(0,"全部连接");//添加显示项
        delete tcp_server;//删除tcp_server
    }
}

void MainWindow::on_sendbuffsend_Button_clicked()
{
    QString send_data = ui->sendbuff_TextEdit->toPlainText();
    if(send_data == "")
    {
        return;
    }
    //全部连接
    if(ui->connectedtcpclient_Box->currentIndex() == 0)
    {
        int i = 0;
        for(i = 0; i < connected_tcp_client.length(); i ++)
        {
            connected_tcp_client[i]->write(send_data.toLatin1());
        }
    }
    else//指定连接
    {
        QString tcp_client_ip = ui->connectedtcpclient_Box->currentText().split(":")[0];
        int tcp_client_port = ui->connectedtcpclient_Box->currentText().split(":")[1].toInt();
        int i = 0;
        for(i = 0; i < connected_tcp_client.length(); i++)
        {
            if(connected_tcp_client[i]->peerAddress().toString().split("::ffff:")[1]==tcp_client_ip\
                                        && connected_tcp_client[i]->peerPort()==tcp_client_port)
            {
                connected_tcp_client[i]->write(send_data.toLatin1());//发送完成后退出
                return;
            }
        }


    }
}

void MainWindow::on_sendbuffclear_Button_clicked()
{
    ui->sendbuff_TextEdit->clear();
}

void MainWindow::on_dispbuffclear_Button_clicked()
{
    ui->dispbuff_textBrowser->clear();
}
