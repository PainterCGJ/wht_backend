#include "datapage.h"
#include <QLabel>
#include "slavepage.h"

DataPage::DataPage(QTableView* tableView, QWidget *parent) : QWidget(parent) {
    m_tableView = tableView;
    m_model = new QStandardItemModel(this);
    m_model->setColumnCount(COL_NUMS);
    m_model->setHorizontalHeaderLabels({QString("设备id"),QString("分包序号"),QString("数据"),});
    m_tableView->setModel(m_model);
    
    // 设置表格支持文本换行
    m_tableView->setWordWrap(true);
    
    // 设置行高根据内容自动调整
    m_tableView->verticalHeader()->setDefaultSectionSize(25);
    m_tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_tableView->verticalHeader()->setMinimumSectionSize(25);
    
    m_tableView->setColumnWidth(COL_DEVICE_ID, 100);
    m_tableView->setColumnWidth(COL_DEVICE_FRAG, 60);
    m_tableView->horizontalHeader()->setSectionResizeMode(COL_DEVICE_DATA, QHeaderView::Stretch);
    m_deviceList = nullptr;
}

DataPage::~DataPage() {
}

void DataPage::setTable(QList<SlaveDevice*>* deviceList){
    m_deviceList = deviceList;
}

void DataPage::on_setTable(QList<SlaveDevice*>* deviceList){
    setTable(deviceList);
    refreshTable();
}

void DataPage::refreshTable(){
    if(m_deviceList == nullptr) return;
    
    // 清空表格
    m_model->removeRows(0, m_model->rowCount());
    
    // 重新填充表格，直接使用 deviceList 中的 widget 和 item
    for(auto dev : *m_deviceList){
        // 为每个设备添加行，每个分包一行
        for(quint32 fragIndex = 0; fragIndex < dev->m_fragments; fragIndex++){
            int row = m_model->rowCount();
            m_model->insertRow(row);
            
            // 第一列：只在第一个分包（fragIndex == 0）时显示设备ID，其他行第一列为空
            if(fragIndex == 0){
                m_tableView->setIndexWidget(m_model->index(row, COL_DEVICE_ID), dev->m_idLable);
            } else {
                // 其他行第一列留空，不设置任何内容
                QStandardItem* emptyItem = new QStandardItem("");
                emptyItem->setEditable(false);
                m_model->setItem(row, COL_DEVICE_ID, emptyItem);
            }
            
            // 第二列：分包序号（从1开始）
            QStandardItem* fragItem = new QStandardItem(QString::number(fragIndex + 1));
            fragItem->setTextAlignment(Qt::AlignCenter);
            m_model->setItem(row, COL_DEVICE_FRAG, fragItem);
            
            // 第三列：直接使用 dataItem（不创建新对象，直接使用已有的 QStandardItem）
            QStandardItem* dataItem = dev->getDataItem(fragIndex);
            if(dataItem != nullptr){
                // 设置文本对齐方式（左对齐，垂直居中）
                dataItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
                // 文本换行由 QTableView::setWordWrap(true) 控制，不需要在 item 上设置
                // 直接使用 dataItem，当 dataItem 的数据变化时，需要调用 refreshTable 来更新
                m_model->setItem(row, COL_DEVICE_DATA, dataItem);
            }
        }
    }
}
