#ifndef UDPMANAGER_H
#define UDPMANAGER_H

#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>
#include <QByteArray>

class UdpManager : public QObject
{
    Q_OBJECT
public:
    explicit UdpManager(QObject *parent = nullptr);
    ~UdpManager() override;

    /**
     * @brief 绑定本地IP地址和端口（用于接收UDP数据）
     * @param localIp 本地IP地址（如"192.168.1.100"）
     * @param localPort 本地端口（如8888）
     * @return 绑定成功返回true，失败返回false
     */
    bool bindLocal(const QString &localIp, quint16 localPort);

    /**
     * @brief 发送UDP数据到目标IP和端口
     * @param targetIp 目标IP地址（如"192.168.1.101"）
     * @param targetPort 目标端口（如8888）
     * @param data 要发送的数据
     * @return 发送成功返回发送的字节数，失败返回-1
     */
    qint64 sendData(const QString &targetIp, quint16 targetPort, const QByteArray &data);

    /**
     * @brief 关闭套接字，取消绑定/虚拟连接，释放资源
     */
    void cancelUdp();

    bool isOpen();
signals:
    void readReady(const QByteArray & readData);

private slots:
    // 接收UDP数据的槽函数（当有数据到来时触发）
    void onReadyRead();

private:
    QByteArray recvData;
    QUdpSocket *m_udpSocket; // UDP通信核心对象
};

#endif // UDPMANAGER_H
