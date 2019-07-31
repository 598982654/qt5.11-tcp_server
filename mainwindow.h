#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
//添加网络头文件
#include <QTcpServer>
#include <QTcpSocket>
#include <QNetworkInterface>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    //按键事件，自动生成
    void on_listen_Button_clicked();

    void on_sendbuffsend_Button_clicked();

    void on_sendbuffclear_Button_clicked();

    void on_dispbuffclear_Button_clicked();
    //tcp服务器相关的slot,需要进行链接
    void new_connection_slot(void);//与信号槽tcpServer newConnection()链接
    void disconnected_slot(void);//与信号槽currentClient disconnected()链接
    void tcp_server_readdata(void);//与信号槽currentClient readyRead()链接
private:
    Ui::MainWindow *ui;
    QTcpServer *tcp_server;//tcp服务器
    QList<QTcpSocket*> connected_tcp_client;//连接tcp服务器的客户端列表
    QTcpSocket  *current_tcp_client;//当前连接服务器的客户端

};

#endif // MAINWINDOW_H
