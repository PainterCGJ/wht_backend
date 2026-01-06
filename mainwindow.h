#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "udpmanager.h"
#include "slavepage.h"
#include "protocol/protocol.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_bindButton_clicked();

    void on_addDeviceButton_clicked();

    void on_setMtuButton_clicked();

    void on_toolButton_triggered(QAction *arg1);

    void on_toolButton_clicked();

    void on_loadDeviceButton_clicked();

    void on_clearDeviceButton_clicked();

    void on_clearBufferButton_clicked();

    void on_udpReadReady(const QByteArray & readData);

private:
    Ui::MainWindow *ui;
    UdpManager* udpManager;
    SlavePage* slavePage;
    Protocol* m_protocol;
    bool isBind;

};
#endif // MAINWINDOW_H
