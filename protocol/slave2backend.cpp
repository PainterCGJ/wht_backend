#include "slave2backend.h"
#include <cstring>
#include <QDebug>
void ConductionDataMsg::setCallback(std::function<void(uint8_t seq, uint8_t isMore, ConductionDataMsg* msg)> callback){
     m_callback = callback;
}
void ConductionDataMsg::runCallback(uint8_t seq, uint8_t isMore){
    if(m_callback!= nullptr){
        m_callback(seq , isMore, this);
    }
}
void Slave2BackendPacket::handlePacket(uint8_t seq, uint8_t isMore, uint16_t len, const uint8_t* data) {
    uint8_t msgID = data[0];
    if(msgID==CONDUCTION_DATA_MSG){
        condMsg.msgData.clear();
        if(len > 0){
            condMsg.msgData.resize(len);
            memcpy(condMsg.msgData.data(),data,len);
        }
        condMsg.runCallback(seq,isMore);
    }
}
