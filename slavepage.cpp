#include "slavepage.h"
#include "logger.h"
#include <cstring>
#include "xlsxdocument.h"
#include "./protocol/slave2backend.h"
#include "xlsxworksheet.h"
SlavePage::SlavePage(QTableView* table,QWidget *parent)
    :QWidget(parent),
    m_table(table),
    m_model(new QStandardItemModel(this)) {
    m_model->setColumnCount(COL_MAX);
    m_model->setHorizontalHeaderLabels({"设备id","导通线数","数据长度","删除",});
    // 设置tableView
    m_table->setModel(m_model);

    m_table->verticalHeader()->setDefaultSectionSize(25);

    // 设置行高不可更改，不根据内容调整
    m_table->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    // 设置最小行高（可选，确保不会被压缩）
    m_table->verticalHeader()->setMinimumSectionSize(25);

    // 设置列宽
    m_table->setColumnWidth(DEVICE_ID, 100);
    m_table->setColumnWidth(DATA_SIZE, 60);
    // m_table->horizontalHeader()->setSectionResizeMode(STSTUS_LABLE, QHeaderView::Stretch);
    m_table->setColumnWidth(DELETE_BT, 50);
    m_currentDev = nullptr;
    m_maxFrags = 0;
    connect(&newDeviceDialog,&NewDevice::addDevice,this,&SlavePage::on_addDevice);


}

void SlavePage::addFragColumn(int columnCount) {
    if (!m_model || columnCount <= 0) return;


    // 在 DELETE_BT 列右侧插入 n 列
    // 如果右侧为空，insertColumns 会自动创建
    int curruntCount = m_model->columnCount();
    m_model->insertColumns(curruntCount, columnCount);
    // 设置新列的表头
    for (int i = 0; i < columnCount; i++) {
        int colIndex = curruntCount + i;
        m_model->setHorizontalHeaderItem(colIndex,
                                         new QStandardItem(QString("Frag %1").arg(i + 1)));
    }
}
void SlavePage::deleteColumnsAfter(int columnIndex) {
    if (!m_model) return;

    // 检查列索引是否有效
    if (columnIndex < 0 || columnIndex >= m_model->columnCount()) {
        Logger::appendLog(QString("无效的列索引:%1").arg(columnIndex));
        return;
    }

    int startPos = columnIndex + 1;
    int totalColumns = m_model->columnCount();

    if (startPos < totalColumns) {
        int columnsToDelete = totalColumns - startPos;
        bool success = m_model->removeColumns(startPos, columnsToDelete);

        if (success) {
            Logger::appendLog(QString("成功删除第%1列右侧的%2列").arg(columnIndex).arg(columnsToDelete));
        } else {
            Logger::appendLog("删除列失败");
        }
    } else {
        Logger::appendLog(QString("第%1列右侧没有列可以删除").arg(columnIndex));
    }
}
void SlavePage::on_addDevice(quint32 id, const QVector<QByteArray>& vdata){
    SlaveDevice* newDev = new SlaveDevice(id,vdata,this);
    QString hexText = QString("%1").arg(id, 8, 16, QLatin1Char('0')).toUpper();
    int dataSize = 0;

    for(const QByteArray& array:vdata){
        dataSize += array.size();
    }
    if(newDev->m_fragments > m_maxFrags){
        addFragColumn(newDev->m_fragments - m_maxFrags);
        m_maxFrags = newDev->m_fragments;

    }
    newDev->m_idLable->setText(hexText);
    newDev->m_sizeLable->setText(QString("%1").arg(dataSize));

    m_deviceList.append(newDev);
    int row = m_model->rowCount();
    m_model->insertRow(row);
    m_table->setIndexWidget(m_model->index(row, DEVICE_ID), newDev->m_idLable);
    m_table->setIndexWidget(m_model->index(row, DATA_SIZE), newDev->m_sizeLable);

    for(uint8_t i = 0; i < newDev->m_fragments; i++){
        m_table->setIndexWidget(m_model->index(row, FRAG0 + i), newDev->m_fragmentLable[i]);
    }

    m_table->setIndexWidget(m_model->index(row, DELETE_BT), newDev->m_deleteBt);

    connect(newDev,&SlaveDevice::remove,this,&SlavePage::on_removeDevice);
}
void SlavePage::on_removeDevice(SlaveDevice*dev){
    for (int i = m_deviceList.size() - 1; i >= 0; --i) {
        if (m_deviceList[i] == dev) {
            m_model->removeRow(i);
            SlaveDevice* device = m_deviceList.takeAt(i);
            device->deleteLater();  // 安全删除对象
        }
    }

}
void SlavePage::clearDevlice(){
    while (!m_deviceList.isEmpty()) {
        m_model->removeRow(0);
        SlaveDevice* device = m_deviceList.takeAt(0);
        device->deleteLater();  // 安全删除对象
    }
    deleteColumnsAfter(DELETE_BT);
    m_maxFrags = 0;
}

void SlavePage::openCfgFile(QString& filePath)
{
    cfgFilePath = filePath;
}


int SlavePage::caculateFrags(QXlsx::Document& xlsx,int row){
    int count = 0;

    // 从第2列开始（B列，索引2）
    for (int col = 2; ; col++) {
        // 使用 std::shared_ptr<QXlsx::Cell> 接收
        std::shared_ptr<QXlsx::Cell> cell = xlsx.cellAt(row, col);

        // 如果单元格为空，停止计数
        if (!cell || cell->value().isNull() || cell->value().toString().isEmpty()) {
            break;
        }

        count++;
    }

    return count;
}
void SlavePage::loadCfgFile(){
    QXlsx::Document cfgFile(cfgFilePath);
    if (!cfgFile.load()) {
        Logger::appendLog(QString("无法打开文件:%1").arg(cfgFilePath));
        return;
    }
    clearDevlice();

    int row = cfgFile.dimension().rowCount();
    int col  = cfgFile.dimension().columnCount();
    // int fragCols = col - 1;

    // 设置列数


    Logger::appendLog(QString("当前工作表:%1").arg(cfgFile.currentSheet()->sheetName()));
    Logger::appendLog(QString("设备数量:%1").arg(row - 1));
    Logger::appendLog(QString("列数:%1").arg(col));

    QString hexStr;
    QString cleaned;
    QVector<QByteArray> vdata;
    quint32 idNum;
    for (int i = 2; i <= row; ++i) {
        int frags = caculateFrags(cfgFile,i);
        vdata.resize(frags);
        QVariant id = cfgFile.read(i, 1);
        if (!id.isNull()) {
            hexStr = id.toString().trimmed();
            idNum = hexStr.toInt(nullptr,16);
        }
        Logger::appendLog(QString("find id %1").arg(hexStr));
        for(int j = 0; j < frags; j++){
            QVariant data = cfgFile.read(i, j + 2);
            hexStr = data.toString().trimmed();
            cleaned = hexStr.remove(' ');
            vdata[j] = QByteArray::fromHex(cleaned.toLatin1());
            // QString db = QString("vdata[%1]=%2").arg(j).arg(vdata[j].size());
            // qDebug()<<db;
        }

        on_addDevice(idNum,vdata);

        vdata.clear();
    }
}


void SlavePage::clearBuffer(){
    for (SlaveDevice* dev : m_deviceList) {
        dev->clearBuffer();
    }
}

void SlavePage::handleConductionMsg(uint8_t seq, uint8_t isMore, ConductionDataMsg* msg){
    // 检查数据大小
    if(msg->msgData.size() < 7){
        Logger::appendLog("data size error");
        return;
    }
    
    // 直接构造QByteArray，Qt会自动优化内存分配
    QByteArray byteArray(reinterpret_cast<const char*>(msg->msgData.data()), msg->msgData.size());
    uint32_t devID = 0;
    if(seq == 0){
        byteArray[5] = 0;
        byteArray[6] = 0;
        m_currentDev = nullptr;
        memcpy(&devID,byteArray.data()+1,4);
        Logger::appendLog(QString("dev id %1").arg(devID, 8, 16, QLatin1Char('0')).toUpper());
        for(auto& dev:m_deviceList){
            if(dev->m_id == devID){
                m_currentDev = dev;
            }
        }
        if(m_currentDev == nullptr){
            Logger::appendLog("no found");
        }
        else{
            Logger::appendLog("found");
        }
    }

    if(m_currentDev!=nullptr){
        if(seq >= m_currentDev->m_fragments){
            Logger::appendLog("seq error");
            m_currentDev = nullptr;
            return;
        }


        m_currentDev->setBuffer(seq,byteArray);

        m_currentDev->compareAll();
    }
    // qDebug() << "Data:" << byteArray.toHex(' ').toUpper();
}

