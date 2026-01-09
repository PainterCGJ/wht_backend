#ifndef DATAPAGE_H
#define DATAPAGE_H

#include "slavedevice.h"
#include <QHeaderView>
#include <QList>
#include <QStandardItemModel>
#include <QTableView>
#include <QWidget>


class DataPage : public QWidget {
    Q_OBJECT
public:
    DataPage(QTableView* tableView, QWidget* parent = nullptr);
    ~DataPage();
    void setTable(QList<SlaveDevice*>* deviceList);  // 改为指针，以便自动更新
    void refreshTable();  // 刷新表格

public slots:
    void on_setTable(QList<SlaveDevice*>* deviceList);
private:
    enum DataPageCol {
        COL_DEVICE_ID = 0,
        COL_DEVICE_FRAG,
        COL_DEVICE_DATA,

        COL_NUMS,
    };
    QTableView* m_tableView;
    QStandardItemModel* m_model;
    QList<SlaveDevice*>* m_deviceList;  // 保存 deviceList 的指针
};

#endif // DATAPAGE_H
