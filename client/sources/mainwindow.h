#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <winsock2.h>
#include <QTimer>

namespace Ui {
    class ChatClientWindow;
}

class ChatClientWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ChatClientWindow(QWidget *parent = 0);
    ~ChatClientWindow();

private slots:
    void on_connect_button_clicked();
    void on_disconnect_button_clicked();
    void on_send_button_clicked();
    void onTimer();

private:
    Ui::ChatClientWindow *ui;

    SOCKET s;
    QTimer poke;
};

#endif // MAINWINDOW_H
