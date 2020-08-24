#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <ws2tcpip.h>

ChatClientWindow::ChatClientWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ChatClientWindow)
{
    ui->setupUi(this);
    s = 0;
    WSADATA wsadata;
    WSAStartup(MAKEWORD(2,2), &wsadata);

    connect(&poke, SIGNAL(timeout()), SLOT(onTimer()));
    poke.start(200);

}

ChatClientWindow::~ChatClientWindow()
{
    delete ui;
    WSACleanup();
}

void ChatClientWindow::on_connect_button_clicked()
{
    if (s != 0)
        return;

    char hostname[256];
    strcpy(hostname, ui->addr_edit->text().toLatin1().data());

    char port[32];
    strcpy(port, ui->port_sb->text().toLatin1().data());

    int err;
    sockaddr_in addr;
    struct addrinfo *result = NULL;
    err = getaddrinfo(hostname, port, NULL, &result);

    if(err == 0)
    {
        memcpy(&addr, result->ai_addr, sizeof(addr));
    }
    freeaddrinfo(result);

    if (err != 0)
    {
        ui->log_edit->append("Unable to lookup address");
        return;
    }

    // Создаем Сокет

    s = socket(AF_INET, SOCK_STREAM, 0);

    //Podkluchaemsa k adresu

    err = ::connect(s, (sockaddr*)&addr, sizeof(addr));
    if (err)
    {
        ui->log_edit->append("Unable to connect");
        on_disconnect_button_clicked();
        return;
    }

    u_long m=1;
    ioctlsocket(s,FIONBIO,&m);

}

void ChatClientWindow::on_disconnect_button_clicked()
{
    closesocket(s);
    s = 0;
    ui->log_edit->append("Socket closed");
}

void ChatClientWindow::on_send_button_clicked()
{
    if (s == 0)
        return;

    char msg[256];
    strcpy(msg, ui->msg_edit->text().toUtf8().data());
    send(s, msg, strlen(msg), 0);

}

void ChatClientWindow::onTimer()
{
    if (s == 0)
        return;

    char msg[256] = {0};
    int res;
    res = recv(s, msg, 256, 0);
    if (res == 0)
    {
        on_disconnect_button_clicked();
        return;
    }

    if (res < 0)
    {
        int err = WSAGetLastError();
        if (err == WSAEWOULDBLOCK)
            return;
        if (err == WSAETIMEDOUT)
            return;
        ui->log_edit->append("Unable to recv");
        on_disconnect_button_clicked();
        return;
    }

    QString s = QString::fromUtf8(msg);
    ui->log_edit->append(s);
}
