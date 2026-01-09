#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logger.h"
#include <QInputDialog>
#include <QFileDialog>
#include <QRegularExpression>
#include <QIcon>
#include <cstdint>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , udpManager(new UdpManager)
{
    ui->setupUi(this);
    dataPage = new DataPage(ui->dataTableWidget,this);
    isBind = false;
    isLogPaused = false;  // 初始状态：日志未暂停（正常输出）
    statusBar()->setVisible(true);    // 显示状态栏
    
    // 初始化日志系统，设置日志输出控件
    Logger::setLogWidget(ui->logEdit);
    
    slavePage = new SlavePage(ui->tableView,this);
    m_protocol = new Protocol;
    connect(udpManager,&UdpManager::readReady,this,&MainWindow::on_udpReadReady);
    m_protocol->slave2BackendPacket.condMsg.setCallback([this](uint8_t seq, uint8_t isMore, ConductionDataMsg* msg){slavePage->handleConductionMsg(seq,isMore,msg);});
    
    Logger::appendLog("系统初始化完成");
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
        Logger::appendLog(QString("尝试绑定本地IP: %1 端口: %2").arg(localIp).arg(port));
        bool isSuccess = udpManager->bindLocal(localIp,port);
        if( isSuccess ){
            isBind = true;  // 只有绑定成功才设置为true
            statusBar()->showMessage(localIp+" 已连接" ,2000);
            Logger::appendLog(QString("成功绑定本地IP: %1 端口: %2").arg(localIp).arg(port));
            ui->ipEdit->setEnabled(false);
            ui->portEdit->setEnabled(false);
            ui->bindButton->setText("取消连接");
        }
        else{
            isBind = false;  // 绑定失败，确保状态为false
            statusBar()->showMessage(localIp+" 连接失败" ,2000);
            Logger::appendLog(QString("绑定失败 - IP: %1 端口: %2").arg(localIp).arg(port));
        }

    }
    else{
        isBind = false;
        Logger::appendLog("取消UDP绑定");
        ui->ipEdit->setEnabled(true);
        ui->portEdit->setEnabled(true);
        udpManager->cancelUdp();
        ui->bindButton->setText("连接");
    }

}

void MainWindow::on_udpReadReady(const QByteArray & readData){
    // 检查数据有效性
    if (readData.isEmpty() || readData.data() == nullptr) {
        QString errorMsg = "接收到空数据或无效数据指针";
        Logger::appendLog(errorMsg);
        return;
    }
    
    uint8_t* data = (uint8_t*)readData.data();
    uint16_t len = static_cast<uint16_t>(readData.size());
    
    // 防止长度溢出（uint16_t最大65535）
    if (readData.size() > 65535) {
        QString errorMsg = QString("数据长度超出限制：%1，截断为65535").arg(readData.size());
        Logger::appendLog(errorMsg);
        len = 65535;
    }
    
    Logger::appendLog(QString("接收到UDP数据，长度: %1 字节").arg(len));
    m_protocol->parse(data, len);
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
        Logger::appendLog("用户取消了文件选择");
        return ;
    }

    // 额外校验：确保文件后缀是.xlsx（防止用户通过"所有文件"选择非xlsx文件）
    if (!filePath.endsWith(".xlsx", Qt::CaseInsensitive)) { // 忽略大小写
        Logger::appendLog(QString("选择的文件不是xlsx格式：%1").arg(filePath));
        return;
    }

    Logger::appendLog(QString("选中的xlsx文件路径：%1").arg(filePath));
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


void MainWindow::on_udpSendButton_clicked()
{
    // 检查UDP是否已绑定（检查用户界面状态，而不是socket状态）
    if (!isBind || !udpManager->isOpen()) {
        Logger::appendLog("UDP未绑定，无法发送数据。请先点击\"连接\"按钮绑定UDP");
        statusBar()->showMessage("请先绑定UDP", 2000);
        return;
    }
    
    // 获取输入框中的文本
    QString inputText = ui->udpSendEdit->toPlainText().trimmed();
    
    // 检查是否为空
    if (inputText.isEmpty()) {
        Logger::appendLog("发送数据为空");
        statusBar()->showMessage("请输入要发送的数据", 2000);
        return;
    }
    
    // 移除所有空格并验证格式
    QString cleanedText = inputText;
    cleanedText = cleanedText.replace(QRegularExpression("\\s+"), " "); // 将多个空格替换为单个空格
    cleanedText = cleanedText.trimmed();
    
    // 按空格分割
    QStringList byteStrings = cleanedText.split(" ", Qt::SkipEmptyParts);
    
    // 验证每个字节字符串
    QByteArray sendData;
    bool isValid = true;
    QString errorMsg;
    
    for (int i = 0; i < byteStrings.size(); ++i) {
        QString byteStr = byteStrings[i].trimmed();
        
        // 检查长度是否为2
        if (byteStr.length() != 2) {
            isValid = false;
            errorMsg = QString("第%1个字节格式错误：\"%2\"，每个字节必须是2个字符（如01、AB）").arg(i + 1).arg(byteStr);
            break;
        }
        
        // 检查每个字符是否为有效的16进制字符（0-9, A-F, a-f）
        bool isHexChar = true;
        for (int j = 0; j < byteStr.length(); ++j) {
            QChar ch = byteStr[j];
            if (!((ch >= '0' && ch <= '9') || 
                  (ch >= 'A' && ch <= 'F') || 
                  (ch >= 'a' && ch <= 'f'))) {
                isHexChar = false;
                break;
            }
        }
        
        if (!isHexChar) {
            isValid = false;
            errorMsg = QString("第%1个字节格式错误：\"%2\"，包含非16进制字符").arg(i + 1).arg(byteStr);
            break;
        }
        
        // 转换为16进制值
        bool ok;
        uint8_t byteValue = static_cast<uint8_t>(byteStr.toUInt(&ok, 16));
        if (!ok) {
            isValid = false;
            errorMsg = QString("第%1个字节格式错误：\"%2\"，转换失败").arg(i + 1).arg(byteStr);
            break;
        }
        
        sendData.append(byteValue);
    }
    
    if (!isValid) {
        Logger::appendLog(errorMsg);
        statusBar()->showMessage(errorMsg, 3000);
        return;
    }
    
    if (sendData.isEmpty()) {
        Logger::appendLog("没有有效的数据可发送");
        statusBar()->showMessage("没有有效的数据可发送", 2000);
        return;
    }
    
    // 获取目标IP和端口（使用绑定的IP和端口，也可以改为广播）
    QString targetIp = ui->ipEdit->text();
    quint16 targetPort = static_cast<quint16>(ui->portEdit->text().toInt());
    
    // 发送数据
    qint64 sentBytes = udpManager->sendData(targetIp, targetPort, sendData);
    
    if (sentBytes > 0) {
        QString hexStr = sendData.toHex(' ').toUpper();
        Logger::appendLog(QString("成功发送UDP数据到 %1:%2，数据: %3，长度: %4 字节")
                         .arg(targetIp).arg(targetPort).arg(hexStr).arg(sentBytes));
        statusBar()->showMessage(QString("成功发送 %1 字节数据").arg(sentBytes), 2000);
    } else {
        Logger::appendLog(QString("UDP发送失败到 %1:%2").arg(targetIp).arg(targetPort));
        statusBar()->showMessage("发送失败", 2000);
    }
}


void MainWindow::on_clearUdpSendButton_clicked()
{

}


void MainWindow::on_clearUdpRecvButton_clicked()
{

}


void MainWindow::on_clearLogButton_clicked()
{
    ui->logEdit->clear();
}


void MainWindow::on_logCtrlButton_clicked()
{
    // 切换日志暂停状态
    isLogPaused = !isLogPaused;
    
    if (isLogPaused) {
        // 暂停日志输出：设置为播放图标，禁用日志输出
        QIcon playIcon = QIcon::fromTheme("media-playback-start");
        if (playIcon.isNull()) {
            // 如果主题图标不可用，使用备用方案
            playIcon = style()->standardIcon(QStyle::SP_MediaPlay);
        }
        ui->logCtrlButton->setIcon(playIcon);
        Logger::setLogEnabled(false);
    } else {
        // 恢复日志输出：设置为暂停图标，启用日志输出
        QIcon pauseIcon = QIcon::fromTheme("media-playback-pause");
        if (pauseIcon.isNull()) {
            // 如果主题图标不可用，使用备用方案
            pauseIcon = style()->standardIcon(QStyle::SP_MediaPause);
        }
        ui->logCtrlButton->setIcon(pauseIcon);
        Logger::setLogEnabled(true);
    }
}

