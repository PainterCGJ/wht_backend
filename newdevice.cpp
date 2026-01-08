#include "newdevice.h"
#include "ui_newdevice.h"
#include "logger.h"

NewDevice::NewDevice(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::NewDevice)
{
    ui->setupUi(this);


    m_model=(new QStandardItemModel(this));
    m_model->setColumnCount(1);
    m_model->setHorizontalHeaderLabels({"frag","data"});
    // 设置tableView
    ui->tableView->setModel(m_model);

    ui->tableView->verticalHeader()->setDefaultSectionSize(25);

    // 设置行高不可更改，不根据内容调整
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    // 设置最小行高（可选，确保不会被压缩）
    ui->tableView->verticalHeader()->setMinimumSectionSize(25);

    // 设置列宽
    ui->tableView->setColumnWidth(0, 60);
    ui->tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    // 获取当前行数作为起始位置
    int startRow = m_model->rowCount();

    // 插入 n 行
    int rows = 15;
    m_model->insertRows(startRow, rows);

    // 填充第一列数据
    for (int i = 0; i < rows; i++) {
        int row = startRow + i;
        QString fragText = QString("frag %1").arg(i);

        // 创建时直接设置不可编辑
        QStandardItem* fragItem = new QStandardItem(fragText);
        fragItem->setEditable(false);
        fragItem->setTextAlignment(Qt::AlignCenter);

        m_model->setItem(row, 0, fragItem);
    }
}

NewDevice::~NewDevice()
{
    delete ui;
}

void NewDevice::on_buttonBox_accepted()
{
    // QString hexText = ui->dataEdit->toPlainText().remove(" ");
    QVector<QByteArray> data;
    int rowCount = m_model->rowCount();
    for (int row = 0; row < rowCount; ++row) {
        QStandardItem* item = m_model->item(row, 1);
        if (!item) {
            break;
        }

        QString str = item->text().trimmed();
        if (str.isEmpty()) {
            break;
        }

        // 移除空格并转换为十六进制
        str = str.remove(' ');
        QByteArray byteArray = QByteArray::fromHex(str.toLatin1());

        data.append(byteArray);
    }

    quint32 id = ui->idEdit->text().toInt(nullptr, 16);
    Logger::appendLog(QString("new dev id: %1").arg(QString::number(id, 16).toUpper()));
    Logger::appendLog(QString("frag num %1").arg(data.size()));
    emit addDevice(id,data);
}


void NewDevice::on_buttonBox_rejected()
{

}

