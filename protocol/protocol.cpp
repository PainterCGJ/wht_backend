#include "protocol.h"
#include <algorithm>
#include "packetbase.h"
#include <cstring>  // 添加 memcpy 支持
#include <QDebug>
void Protocol::parse(const uint8_t* data,uint16_t len){
    // uint16_t recvDelimiter;
    // std::memcpy(&recvDelimiter, data, 2);
    // if(len < sizeof(head) + 2){
    //     return;
    // }
    // if(recvDelimiter!=0xCDAB){
    //     return;
    // }
    // std::memcpy(&head, data + 2, sizeof(head));
    // qDebug()<<"packetID "<<head.packetID;
    // if(m_nextSeq > head.seq){
    //     // 丢失最后一包
    //     m_waitFirstFragment = 1;
    //     if(head.seq != 0){
    //         // 不是接收到第一包
    //         m_nextSeq = 0; // 期望接收第一包
    //         return;
    //     }

    // }
    // else{
    //     if(head.isMore == 1){
    //         // 接收下一包
    //         m_nextSeq = head.seq + 1;
    //     }
    //     else{
    //         m_nextSeq = 0;
    //     }
    // }
    // switch(head.packetID){
    // case PacketBase::Slave2Backend:{
    //     slave2BackendPacket.handlePacket(head.seq,head.isMore,head.len,data+2+sizeof(head));
    //     break;
    // }
    // default:break;
    // }
    parseByte(data,len);

}

void Protocol::parseByte(const uint8_t* pdata,uint16_t len){
    uint8_t data;
    for(uint16_t i = 0; i < len; i++){
        data = pdata[i];
        m_buffer.push_back(data);
        switch(parseStatus){
        case FindDelimiter:
            if(m_buffer.size() == 2){
                uint16_t recvDelimiter;

                memcpy(&recvDelimiter,m_buffer.data(),2);
                if(recvDelimiter == 0xCDAB){
                    parseStatus = ParseHead;
                }else{
                    qWarning()<<"recvDelimiter error";
                }
            }
            break;
        case ParseHead:
            if(m_buffer.size() == sizeof(head) + 2){
                memcpy(&head,m_buffer.data() + 2,sizeof(head));
                parseStatus = ReadPayload;
            }
            break;
        case ReadPayload:
            if(m_buffer.size()==head.len+sizeof(head)+2){
                uint8_t isHandle = 1;
                if(m_nextSeq > head.seq){
                    // 丢失最后一包
                    qDebug()<<"loss last pkt";
                    m_waitFirstFragment = 1;
                    if(head.seq != 0){
                        // 不是接收到第一包

                        isHandle = 0;
                        qDebug()<<"loss last pkt, but recv first pkt";
                    }
                    m_nextSeq = 0; // 期望接收第一包

                }

                if(isHandle){
                    qDebug("handle pkt %u",head.seq);
                    if(head.isMore == 1){
                        // 接收下一包
                        m_nextSeq = head.seq + 1;
                    }
                    else{
                        m_nextSeq = 0;
                    }
                    switch(head.packetID){
                    case PacketBase::Slave2Backend:{
                        slave2BackendPacket.handlePacket(head.seq,head.isMore,head.len,m_buffer.data()+2+sizeof(head));
                        break;
                    }
                    default:break;
                    }
                }


                parseStatus = FindDelimiter;
                m_buffer.clear();
            }
            break;
        default:
            break;
        }
    }
}
