#include "chatserverwindow.h"
#include "ui_chatserverwindow.h"

#include <QFileDialog>
#include <QStringList>
#include <math.h>
#include <time.h>

ChatServerWindow::ChatServerWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ChatServerWindow)
{
    ui->setupUi(this);

    connect(&server, SIGNAL(newConnection()), SLOT(onNewConnection()));
    checkServerState();
}

ChatServerWindow::~ChatServerWindow()
{
    server.close();
    for(int a=0; a<clients.size(); a++)
    {
        clients[a]->deleteLater();
    }
    clients.clear();
    delete ui;
}

void ChatServerWindow::on_openPort_button_clicked()
{
    if(server.isListening())
    {
        server.close();
        ui->log_edit->append(QString::fromUtf8("Порт закрыт."));
    }
    else
    {
        server.listen(QHostAddress::Any, ui->port_sb->value());
        ui->log_edit->append(QString::fromUtf8("Сервер запущен на порту ")
                             + QString::number(ui->port_sb->value()));
    }
    checkServerState();
}

void ChatServerWindow::checkServerState()
{
    if(server.isListening())
        ui->openPort_button->setText(QString::fromUtf8("Закрыть порт"));
    else
        ui->openPort_button->setText(QString::fromUtf8("Открыть порт"));
}

void ChatServerWindow::onNewConnection()
{
    QTcpSocket* s = server.nextPendingConnection();
    if(s == 0)
        return;
    ui->log_edit->append(QString::fromUtf8("Подключен клиент: ")
                         + s->peerAddress().toString() + ":"
                         + QString::number(s->peerPort()));

    connect(s, SIGNAL(disconnected()), SLOT(onSocketDisconnected()));
    connect(s, SIGNAL(readyRead()), SLOT(onClientDataReady()));

    clients.append(s);
}

void ChatServerWindow::onClientDataReady()
{
    QObject *s = sender();
    if(s == 0)
        return;
    if(!s->inherits("QTcpSocket"))
        return;

    QTcpSocket *ss = (QTcpSocket*)s;
    QByteArray ar = ss->readAll();
    QString sign = ss->peerAddress().toString() + ":"
                   + QString::number(ss->peerPort()) + " | ";
    ui->log_edit->append(sign + QString::fromUtf8(ar));

    // Рассылаем сообщение всем
    for(int a=0; a<clients.size(); a++)
    {
        clients[a]->write(sign.toUtf8() + ar);
    }
}

void ChatServerWindow::onSocketDisconnected()
{
    QObject *s = sender();
    if(s->inherits("QTcpSocket"))
    {
        QTcpSocket *ss = (QTcpSocket*)s;
        ss->disconnectFromHost();
        for(int a=0; a<clients.size(); a++)
        {
            if(clients[a] == ss)
            {
                clients.removeAt(a);
                break;
            }
        }
        ss->deleteLater();
    }
    ui->log_edit->append(QString::fromUtf8("Клиент отключился"));
}
