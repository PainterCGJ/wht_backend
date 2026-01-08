#include "protocol.h"
#include "packetbase.h"
#include <cstring>  // 添加 memcpy 支持
#include "../logger.h"  // 包含Logger类（protocol目录需要向上查找）
void Protocol::parse(const uint8_t* data,uint16_t len){
    uint16_t recvDelimiter;
    std::memcpy(&recvDelimiter, data, 2);
    if(len < sizeof(head) + 2){
        return;
    }
    if(recvDelimiter!=0xCDAB){
        return;
    }
    std::memcpy(&head, data + 2, sizeof(head));
    qDebug()<<"packetID "<<head.packetID;
    if(m_nextSeq > head.seq){
        // 丢失最后一包
        m_waitFirstFragment = 1;
        if(head.seq != 0){
            // 不是接收到第一包
            m_nextSeq = 0; // 期望接收第一包
            return;
        }

    }
    else{
        if(head.isMore == 1){
            // 接收下一包
            m_nextSeq = head.seq + 1;
        }
        else{
            m_nextSeq = 0;
        }
    }
    switch(head.packetID){
    case PacketBase::Slave2Backend:{
        slave2BackendPacket.handlePacket(head.seq,head.isMore,head.len,data+2+sizeof(head));
        break;
    }
    default:break;
    }
    // parseByte(data,len);

}

void Protocol::parseByte(const uint8_t* pdata,uint16_t len){
    // 检查输入参数有效性
    if (pdata == nullptr || len == 0) {
        Logger::appendLog("parseByte: 无效的输入参数");
        return;
    }
    
    // 定义最大缓冲区大小（防止恶意数据导致内存耗尽）
    const size_t MAX_BUFFER_SIZE = 1024 * 1024; // 1MB限制
    
    // 预分配缓冲区容量，减少重新分配次数
    if (m_buffer.capacity() < 1024) {
        m_buffer.reserve(1024);
    }
    
    uint8_t data;
    for(uint16_t i = 0; i < len; i++){
        data = pdata[i];
        
        // 检查缓冲区大小，防止无限增长
        if (m_buffer.size() >= MAX_BUFFER_SIZE) {
            Logger::appendLog(QString("缓冲区大小超过限制：%1，重置解析状态").arg(MAX_BUFFER_SIZE));
            parseStatus = FindDelimiter;
            m_buffer.clear();
            m_buffer.shrink_to_fit();  // 释放多余内存
            return;
        }
        
        m_buffer.push_back(data);
        switch(parseStatus){
        case FindDelimiter:
            if(m_buffer.size() == 2){
                uint16_t recvDelimiter;

                memcpy(&recvDelimiter,m_buffer.data(),2);
                if(recvDelimiter == 0xCDAB){
                    parseStatus = ParseHead;
                }else{
                    QString errorMsg = QString("分隔符错误: 0x%1").arg(QString::number(recvDelimiter, 16).toUpper());
                    Logger::appendLog(errorMsg);
                    // 分隔符错误，重置状态（保留最后一个字节，可能是下一个包的开始）
                    if (m_buffer.size() > 1) {
                        m_buffer.erase(m_buffer.begin());
                    } else {
                        m_buffer.clear();
                    }
                }
            }
            break;
        case ParseHead:
            if(m_buffer.size() == sizeof(head) + 2){
                memcpy(&head,m_buffer.data() + 2,sizeof(head));
                
                // 验证head.len的合理性（防止异常大的值）
                const uint16_t MAX_PAYLOAD_SIZE = 65535 - sizeof(head) - 2; // 考虑UDP最大大小
                if (head.len > MAX_PAYLOAD_SIZE) {
                    QString errorMsg = QString("head.len异常大：%1，重置解析状态").arg(head.len);
                    Logger::appendLog(errorMsg);
                    parseStatus = FindDelimiter;
                    m_buffer.clear();
                    m_buffer.shrink_to_fit();  // 释放多余内存
                    break;
                }
                
                parseStatus = ReadPayload;
            }
            break;
        case ReadPayload:
            {
                // 计算期望的总大小
                size_t expectedSize = static_cast<size_t>(head.len) + sizeof(head) + 2;
                
                // 再次验证head.len的合理性（防止整数溢出）
                if (expectedSize < sizeof(head) + 2 || expectedSize > MAX_BUFFER_SIZE) {
                    Logger::appendLog(QString("计算出的数据包大小异常：%1，重置解析状态").arg(expectedSize));
                    parseStatus = FindDelimiter;
                    m_buffer.clear();
                    m_buffer.shrink_to_fit();  // 释放多余内存
                    break;
                }
                
                if(m_buffer.size() == expectedSize){
                    uint8_t isHandle = 1;
                if(m_nextSeq > head.seq){
                    // 丢失最后一包
                    Logger::appendLog("loss last pkt");
                    m_waitFirstFragment = 1;
                    if(head.seq != 0){
                        // 不是接收到第一包

                        isHandle = 0;
                        Logger::appendLog("loss last pkt, but recv first pkt");
                    }
                    m_nextSeq = 0; // 期望接收第一包

                }

                    if(isHandle){
                        Logger::appendLog(QString("handle pkt %1").arg(head.seq));
                        if(head.isMore == 1){
                            // 接收下一包
                            m_nextSeq = head.seq + 1;
                        }
                        else{
                            m_nextSeq = 0;
                        }
                        switch(head.packetID){
                        case PacketBase::Slave2Backend:{
                            // 验证数据指针有效性
                            const uint8_t* payloadData = m_buffer.data() + 2 + sizeof(head);
                            if (payloadData >= m_buffer.data() && 
                                payloadData + head.len <= m_buffer.data() + m_buffer.size()) {
                                slave2BackendPacket.handlePacket(head.seq,head.isMore,head.len,payloadData);
                            } else {
                                Logger::appendLog("数据指针越界，跳过此数据包");
                            }
                            break;
                        }
                        default:break;
                        }
                    }


                    parseStatus = FindDelimiter;
                    m_buffer.clear();
                    m_buffer.shrink_to_fit();  // 释放多余内存
                }
            }
            break;
        default:
            // 未知状态，重置
            Logger::appendLog(QString("未知的解析状态：%1").arg(static_cast<int>(parseStatus)));
            parseStatus = FindDelimiter;
            m_buffer.clear();
            m_buffer.shrink_to_fit();  // 释放多余内存
            break;
        }
    }
}
