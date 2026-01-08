#include "udpmanager.h"
#include "logger.h"  // 包含Logger类

UdpManager::UdpManager(QObject *parent)
    : QObject(parent)
{
    // 初始化QUdpSocket
    m_udpSocket = new QUdpSocket(this);
    // 关联数据到来信号与接收槽函数
    connect(m_udpSocket, &QUdpSocket::readyRead, this, &UdpManager::onReadyRead);
}

UdpManager::~UdpManager()
{
    // 关闭UDP套接字
    if (m_udpSocket->isOpen()) {
        m_udpSocket->close();
    }
}

bool UdpManager::bindLocal(const QString &localIp, quint16 localPort)
{
    // 关闭已存在的绑定
    if (m_udpSocket->isOpen()) {
        m_udpSocket->close();
    }

    // 转换本地IP为QHostAddress
    QHostAddress localAddr(localIp);
    // 绑定本地IP和端口，指定ShareAddress（允许多个程序绑定同一端口）
    bool isSuccess = m_udpSocket->bind(localAddr, localPort, QUdpSocket::ShareAddress);
    if (isSuccess) {
        Logger::appendLog(QString("成功绑定本地IP：%1 端口：%2").arg(localIp).arg(localPort));
    } else {
        Logger::appendLog(QString("绑定本地IP：%1 端口：%2 失败！").arg(localIp).arg(localPort));
    }
    return isSuccess;
}

qint64 UdpManager::sendData(const QString &targetIp, quint16 targetPort, const QByteArray &data)
{
    // 转换目标IP为QHostAddress
    QHostAddress targetAddr(targetIp);
    // 发送UDP数据（无连接，直接指定目标IP和端口）
    qint64 sendBytes = m_udpSocket->writeDatagram(data, targetAddr, targetPort);
    if (sendBytes > 0) {
        Logger::appendLog(QString("成功发送 %1 字节数据到目标IP：%2 端口：%3").arg(sendBytes).arg(targetIp).arg(targetPort));
    } else {
        Logger::appendLog(QString("发送数据到目标IP：%1 端口：%2 失败！").arg(targetIp).arg(targetPort));
    }
    return sendBytes;
}

bool UdpManager::isOpen(){
    return m_udpSocket->isOpen();
}

void UdpManager::onReadyRead()
{
    // 读取所有接收到的UDP数据报
    // while (m_udpSocket->hasPendingDatagrams()) {

    //     // 预留足够的缓冲区空间
    //     recvData.resize(m_udpSocket->pendingDatagramSize());
    //     QHostAddress senderAddr; // 发送方IP地址
    //     quint16 senderPort;      // 发送方端口

    //     // 读取数据报，并获取发送方信息
    //     qint64 recvBytes = m_udpSocket->readDatagram(recvData.data(), recvData.size(), &senderAddr, &senderPort);
    //     if (recvBytes > 0) {
    //         qDebug() << "接收到 " << recvBytes;
    //         qDebug() << "发送方IP：" << senderAddr.toString() << " 发送方端口：" << senderPort;
    //     }

    // }
    while (m_udpSocket->hasPendingDatagrams()) {
        QByteArray recvData; // 局部变量，每次循环新建
        
        // 检查数据报大小是否有效（防止异常大的值导致内存问题）
        qint64 pendingSize = m_udpSocket->pendingDatagramSize();
        if (pendingSize <= 0 || pendingSize > 65535) { // UDP最大理论大小65535字节
            QString errorMsg = QString("无效的数据报大小：%1，跳过此数据报").arg(pendingSize);
            Logger::appendLog(errorMsg);
            // 读取并丢弃无效数据报
            QByteArray dummy;
            dummy.resize(65535);
            m_udpSocket->readDatagram(dummy.data(), dummy.size());
            continue;
        }
        
        recvData.resize(static_cast<int>(pendingSize));
        QHostAddress senderAddr;
        quint16 senderPort;

        qint64 recvBytes = m_udpSocket->readDatagram(recvData.data(), recvData.size(),
                                                     &senderAddr, &senderPort);
        if (recvBytes > 0 && recvBytes == pendingSize) {
            Logger::appendLog(QString("接收到 %1 字节数据").arg(recvBytes));

            // 立即发射信号，避免数据竞争
            emit readReady(recvData);
        } else if (recvBytes < 0) {
            Logger::appendLog(QString("读取UDP数据报失败，错误码：%1").arg(recvBytes));
        }
    }


    // emit readReady(recvData);
}

void UdpManager::cancelUdp()
{
    if (m_udpSocket->isOpen()) {
        m_udpSocket->close();
        // 重要：重置socket状态，解除所有绑定
        m_udpSocket->abort();  // 强制立即关闭
        Logger::appendLog("已主动取消UDP通信，关闭套接字");
    } else {
        // 即使没有打开，也尝试重置
        m_udpSocket->abort();
    }

    // 输出状态信息
    Logger::appendLog(QString("Socket状态：%1").arg(m_udpSocket->state()));
    Logger::appendLog(QString("是否打开：%1").arg(m_udpSocket->isOpen() ? "是" : "否"));
    recvData.clear();
}
