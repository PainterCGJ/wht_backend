#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QInputDialog>
#include <QFileDialog>
#include <cstdint>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , udpManager(new UdpManager)
{
    ui->setupUi(this);
    isBind = false;
    statusBar()->setVisible(true);    // 显示状态栏
    slavePage = new SlavePage(ui->tableView,this);
    m_protocol = new Protocol;
    connect(udpManager,&UdpManager::readReady,this,&MainWindow::on_udpReadReady);
    m_protocol->slave2BackendPacket.condMsg.setCallback([this](uint8_t seq, uint8_t isMore, ConductionDataMsg* msg){slavePage->handleConductionMsg(seq,isMore,msg);});
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_bindButton_clicked()
{
    QString localIp = ui->ipEdit->text();
    qint16 port = ui->portEdit->text().toInt();
    if(!isBind)
    {
        isBind = true;
        qDebug()<<"绑定";
        bool isSuccess = udpManager->bindLocal(localIp,port);
        if( isSuccess ){
            statusBar()->showMessage(localIp+" 已连接" ,2000);
            ui->ipEdit->setEnabled(false);
            ui->portEdit->setEnabled(false);
            ui->bindButton->setText("取消连接");
        }
        else{
            statusBar()->showMessage(localIp+" 连接失败" ,2000);
        }

    }
    else{
        isBind = false;
        qDebug()<<"取消绑定";
        ui->ipEdit->setEnabled(true);
        ui->portEdit->setEnabled(true);
        udpManager->cancelUdp();
        ui->bindButton->setText("连接");
    }

}

void MainWindow::on_udpReadReady(const QByteArray & readData){
    uint8_t* data = (uint8_t*)readData.data();
    uint16_t len = readData.size();
    qDebug()<<"udp len "<<len;
    m_protocol->parse(data,len);
}


void MainWindow::on_addDeviceButton_clicked()
{
    slavePage->newDeviceDialog.show();
}


void MainWindow::on_setMtuButton_clicked()
{
    bool ok;
    QString text = QInputDialog::getText(this,
                                         "输入十进制MTU",
                                         "MTU:",
                                         QLineEdit::Normal,
                                         "800",
                                         &ok);

    if (ok && !text.isEmpty()) {
        // 转换为32位整数
        uint32_t value = text.toUInt(nullptr);
        ui->mtuLable->setText(text);

    }
}


void MainWindow::on_toolButton_triggered(QAction *arg1)
{

}


void MainWindow::on_toolButton_clicked()
{
    // 1. 配置文件对话框：标题、默认路径、文件类型过滤
    // 过滤规则格式："显示名称(*.后缀名)"，多个类型用;;分隔（如"Excel文件(*.xlsx);;所有文件(*.*)"）

    QString filter = "Excel 2007+文件 (*.xlsx)";
    // 获取用户选择的文件路径（静态方法，直接弹出对话框）
    QString filePath = QFileDialog::getOpenFileName(
        nullptr,                  // 父窗口（可为nullptr，对话框居中显示）
        "选择Excel文件",          // 对话框标题
        QDir::homePath(),        // 默认打开路径（QDir::homePath()为用户主目录）
        filter                   // 文件类型过滤
        );

    // 2. 校验选择结果（用户点击取消则返回空字符串）
    if (filePath.isEmpty()) {
        qDebug() << "用户取消了文件选择";
        return ;
    }

    // 额外校验：确保文件后缀是.xlsx（防止用户通过"所有文件"选择非xlsx文件）
    if (!filePath.endsWith(".xlsx", Qt::CaseInsensitive)) { // 忽略大小写
        qDebug() << "选择的文件不是xlsx格式：" << filePath;
        return;
    }

    qDebug() << "选中的xlsx文件路径：" << filePath;
    ui->cfgFileEdit->setText(filePath);
    slavePage->openCfgFile(filePath);
}


void MainWindow::on_loadDeviceButton_clicked()
{
    slavePage->loadCfgFile();
}


void MainWindow::on_clearDeviceButton_clicked()
{
    slavePage->clearDevlice();
}


void MainWindow::on_clearBufferButton_clicked()
{
    slavePage->clearBuffer();
}

