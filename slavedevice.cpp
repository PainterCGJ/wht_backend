#include "slavedevice.h"

SlaveDevice::SlaveDevice(quint32 id, const QVector<QByteArray>& vfrags, QWidget *parent)
    :QWidget(parent)
    ,m_statusLable (new QLabel("异常",this))
    ,m_idLable (new QLabel("00000000",this))
    ,m_sizeLable(new QLabel("0",this))
    ,m_deleteBt (new  QPushButton("删除",this))
{
    m_id = id;
    setIdStyle(StatusError);
    connect(m_deleteBt,&QPushButton::clicked,this,&SlaveDevice::on_deleteBt);
    m_vfrags = vfrags;

    m_fragments = m_vfrags.size();
    qDebug()<<"device"<<id<<"frags"<<m_fragments;
    m_fragmentLable.clear();
    m_fragmentLable.resize(m_fragments);
    m_vbuffer.resize(m_fragments);
    for(uint8_t i=0;i<m_fragments;i++){
        QLabel* label = new QLabel("", this);
        m_fragmentLable[i] = label;
        setFragmentsLableStyle(i, StatusLoss);

    }
}
QString SlaveDevice::getStyle(StatusType type){
    QString styleSheet;
    switch (type) {
    case StatusOK:
        styleSheet =
            "QLabel {"
            "    background-color: #d4edda;"      // 浅绿色不透明背景
            "    color: #155724;"                 // 深绿色文字
            "    border-radius: 8px;"
            "    padding: 4px 8px;"
            "    border: 2px solid #155724;"      // 深绿色边框
            "    font-weight: bold;"
            "}";
        break;
    case StatusError:
        styleSheet =
            "QLabel {"
            "    background-color: #f8d7da;"      // 浅红色不透明背景
            "    color: #721c24;"                 // 深红色文字
            "    border-radius: 8px;"
            "    padding: 4px 8px;"
            "    border: 2px solid #721c24;"      // 深红色边框
            "    font-weight: bold;"
            "}";
        break;
    case StatusLoss:
        styleSheet =
            "QLabel {"
            "    background-color: #fff3cd;"      // 浅黄色不透明背景
            "    color: #856404;"                 // 深黄色文字
            "    border-radius: 8px;"
            "    padding: 4px 8px;"
            "    border: 2px solid #856404;"      // 深黄色边框
            "    font-weight: bold;"
            "}";
        break;
    case StatusNone:
        styleSheet =
            "QLabel {"
            "    background-color: #e0e0e0;"      // 灰色背景
            "    color: #666666;"                 // 深灰色文字
            "    border-radius: 8px;"
            "    padding: 4px 8px;"
            "    border: 2px solid #b0b0b0;"      // 灰色边框
            "    font-weight: bold;"
            "}";
        break;
    }
    return styleSheet;
}
void SlaveDevice::setIdStyle(StatusType type) {
    m_idLable->setStyleSheet(getStyle(type));
    m_idLable->setAlignment(Qt::AlignCenter);
}

void SlaveDevice::setFragmentsLableStyle(uint8_t i, StatusType type) {
    if(i >= m_fragmentLable.size() || m_fragmentLable[i] == nullptr){
        return;
    }
    switch (type) {
    case StatusOK:
        m_fragmentLable[i]->setText("OK");
        break;
    case StatusError:
        m_fragmentLable[i]->setText("ERROR");
        break;
    case StatusLoss:
        m_fragmentLable[i]->setText("LOSS");
        break;
    case StatusNone:
        m_fragmentLable[i]->setText(" ");
        break;
    }
    m_fragmentLable[i]->setStyleSheet(getStyle(type));
    m_fragmentLable[i]->setAlignment(Qt::AlignCenter);
}
void SlaveDevice::on_deleteBt(){
    emit remove(this);
}
bool SlaveDevice::compareFrag(int i){
    bool res = false;
    if(m_vbuffer[i].isWrite){
        res = m_vfrags[i] == m_vbuffer[i].data;
        if(res){
            setFragmentsLableStyle(i , StatusOK);
        }
        else{
            setFragmentsLableStyle(i , StatusError);
        }
    }
    else{
        setFragmentsLableStyle(i , StatusLoss);
    }

    return res;
}
bool SlaveDevice::compareAll(){
    bool res = true;
    for(int i = 0; i < m_fragments; i++){
        res &= compareFrag(i);
    }
    if(res){
        setIdStyle(StatusOK);
    }
    return res;
}



void SlaveDevice::clearBuffer(){
    for(auto& buffer:m_vbuffer){
        buffer.data.clear();
        buffer.isWrite = 0;
    }
    for(uint8_t i = 0;i<m_fragments;i++){
        setFragmentsLableStyle(i , StatusLoss);
    }
}
void SlaveDevice::setBuffer(int i, QByteArray& data){
    m_vbuffer[i].data = data;
    m_vbuffer[i].isWrite = 1;
}

