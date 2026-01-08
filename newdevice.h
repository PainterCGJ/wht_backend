#ifndef NEWDEVICE_H
#define NEWDEVICE_H

#include <QDialog>
#include <QByteArray>
#include <QStandardItemModel>
#include <QTableView>
#include <QHeaderView>

namespace Ui {
class NewDevice;
}

class NewDevice : public QDialog
{
    Q_OBJECT

public:
    explicit NewDevice(QWidget *parent = nullptr);
    ~NewDevice();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();
signals:
    void addDevice(quint32 id, const QVector<QByteArray>& data);

private:

    QStandardItemModel *m_model;
    Ui::NewDevice *ui;
};

#endif // NEWDEVICE_H
