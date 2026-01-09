#ifndef SLAVEDEVICE_H
#define SLAVEDEVICE_H

#include <QObject>
#include <QLabel.h>
#include <QVector>
#include <QPushButton>
#include <QStandardItem>
class SlaveDevice : public QWidget
{
    Q_OBJECT
public:
    enum StatusType {
        StatusError,
        StatusOK,
        StatusLoss,
        StatusNone
    };


    SlaveDevice(quint32 id, const QVector<QByteArray>& vdata, QWidget *parent = nullptr);


    quint32 m_id;
    quint32 m_dataLen;
    quint32 m_fragments;
    QLabel* m_statusLable;
    QLabel* m_idLable;
    QLabel* m_sizeLable;
    QVector<QLabel*> m_fragmentLable;
    QPushButton* m_deleteBt;

    bool compareFrag(int i);
    bool compareAll();

    void clearBuffer();
    void setBuffer(int i, const QByteArray& data);  // 使用内存拷贝，先resize再复制
    QStandardItem* getDataItem(int i);  // 获取指定索引的 dataItem

signals:
    void remove(SlaveDevice*dev);




private slots:
    void on_deleteBt();

private:

    typedef struct sBuffer{
        QByteArray data;
        uint8_t isWrite;
        QStandardItem* dataItem;  // 改为指针，因为 QStandardItem 不能被拷贝
        sBuffer(){
            isWrite = 0;
            dataItem = nullptr;
        }
        ~sBuffer(){
            if(dataItem != nullptr){
                delete dataItem;
            }
        }
    }Buffer;
    QVector<Buffer> m_vbuffer;
    QVector<QByteArray> m_vfrags;

    QString getStyle(StatusType type);
    void setIdStyle(StatusType type);
    void setFragmentsLableStyle(uint8_t i, StatusType type);

};

#endif // SLAVEDEVICE_H
