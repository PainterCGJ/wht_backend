#include "udpmanager.h"
#include <QDebug>

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
        qDebug() << "成功绑定本地IP：" << localIp << " 端口：" << localPort;
    } else {
        qDebug() << "绑定本地IP：" << localIp << " 端口：" << localPort << " 失败！";
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
        qDebug() << "成功发送 " << sendBytes << " 字节数据到目标IP：" << targetIp << " 端口：" << targetPort;
    } else {
        qDebug() << "发送数据到目标IP：" << targetIp << " 端口：" << targetPort << " 失败！";
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
        recvData.resize(m_udpSocket->pendingDatagramSize());
        QHostAddress senderAddr;
        quint16 senderPort;

        qint64 recvBytes = m_udpSocket->readDatagram(recvData.data(), recvData.size(),
                                                     &senderAddr, &senderPort);
        if (recvBytes > 0) {
            qDebug() << "接收到 " << recvBytes;

            // 立即发射信号，避免数据竞争
            emit readReady(recvData);
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
        qDebug() << "已主动取消UDP通信，关闭套接字";
    } else {
        // 即使没有打开，也尝试重置
        m_udpSocket->abort();
    }

    // 输出状态信息
    qDebug() << "Socket状态：" << m_udpSocket->state();
    qDebug() << "是否打开：" << m_udpSocket->isOpen();
    recvData.clear();
}
