#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "udpmanager.h"
#include "slavepage.h"
#include "protocol/protocol.h"
#include "datapage.h"

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

    void on_udpSendButton_clicked();

    void on_clearUdpSendButton_clicked();

    void on_clearUdpRecvButton_clicked();

    void on_clearLogButton_clicked();

    void on_logCtrlButton_clicked();

private:
    Ui::MainWindow *ui;
    UdpManager* udpManager;
    SlavePage* slavePage;
    Protocol* m_protocol;
    bool isBind;
    bool isLogPaused;  // 日志暂停状态标志
    DataPage* dataPage;
};
#endif // MAINWINDOW_H
