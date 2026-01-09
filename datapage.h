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
void setTableView(QTableView* tableView);
private:
    enum DataPageCol {
        COL_DEVICE_ID = 0,
        COL_DEVICE_FRAG,
        COL_DEVICE_DATA,

        COL_NUMS,
    };
    QTableView* m_tableView;
    QStandardItemModel* m_model;
};

#endif // DATAPAGE_H
