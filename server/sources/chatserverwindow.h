#ifndef FILEHOSTWINDOW_H
#define FILEHOSTWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDir>
#include <stdlib.h>

namespace Ui {
    class ChatServerWindow;
}

class ChatServerWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ChatServerWindow(QWidget *parent = 0);
    ~ChatServerWindow();

private:
    QTcpServer server;
    QList<QTcpSocket*> clients;
    Ui::ChatServerWindow *ui;

    // Проверяет состояние сервера
    void checkServerState();

    // Закрывает сервер и отключает все соединения
    void closeServer();

private slots:
    void onNewConnection();
    void onSocketDisconnected();
    void onClientDataReady();
    void on_openPort_button_clicked();
};

#endif // FILEHOSTWINDOW_H
