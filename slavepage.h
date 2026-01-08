#ifndef SLAVEPAGE_H
#define SLAVEPAGE_H
#include <QStandardItemModel>
#include <QTableView>
#include <QWidget>
#include <QList>
#include <QHeaderView>
#include "slavedevice.h"
#include "newdevice.h"
#include "xlsxdocument.h"
#include "./protocol/slave2backend.h"

class SlavePage: public QWidget
{
    Q_OBJECT
public:
    SlavePage(QTableView* table,QWidget *parent = nullptr);
    NewDevice newDeviceDialog;

    void openCfgFile(QString& filePath);
    void loadCfgFile();
    void clearDevlice();
    void clearBuffer();
    void handleConductionMsg(uint8_t seq, uint8_t isMore, ConductionDataMsg* msg);
private:
    typedef enum{
        DEVICE_ID,
        WIRE_NUMS,
        DATA_SIZE,
        DELETE_BT,
        FRAG0,
        COL_MAX = FRAG0,
    }TableCol;
    QTableView* m_table;
    QStandardItemModel *m_model;
    QList<SlaveDevice*> m_deviceList;
    QString cfgFilePath;
    SlaveDevice* m_currentDev;
    uint8_t m_maxFrags;
    void addFragColumn(int columnCount);
    void deleteColumnsAfter(int columnIndex);
    int caculateFrags(QXlsx::Document& xlsx, int row);


private slots:
    void on_addDevice(quint32 id, const QVector<QByteArray>& vdata);
    void on_removeDevice(SlaveDevice*dev);
};

#endif // SLAVEPAGE_H
