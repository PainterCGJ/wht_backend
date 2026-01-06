#ifndef SLAVE2BACKEND_H
#define SLAVE2BACKEND_H
#include "packetbase.h"
#include "messagebase.h"
class ConductionDataMsg: public MsgBase{
public:
    ConductionDataMsg() = default;
    void setCallback(std::function<void(uint8_t seq, uint8_t isMore, ConductionDataMsg* msg)> callback);
    void runCallback(uint8_t seq, uint8_t isMore);
private:
    uint16_t dataLen;
    std::function<void(uint8_t seq, uint8_t isMore, ConductionDataMsg* msg)> m_callback;
};
class ResistanceDataMsg: public MsgBase{
public:
    ResistanceDataMsg() = default;
private:
};
class ClipDataMsg: public MsgBase{
public:
    ClipDataMsg() = default;
private:
};

class Slave2BackendPacket:public PacketBase{
public:
    enum{
        CONDUCTION_DATA_MSG = 0,
        RESISTANCE_DATA_MSG,
        CLIP_DATA_MSG,
    };
#pragma  pack(push,1)
    struct{
        uint8_t msgID;
        uint32_t slaveID;
        struct{
            uint16_t colorSensor:1;
            uint16_t sleeveLimit:1;
            uint16_t unlockButton:1;
            uint16_t batteryAlarm:1;
            uint16_t pressureSensor:1;
            uint16_t lock1:1;
            uint16_t lock2:1;
            uint16_t accessory1:1;
            uint16_t accessory2:1;
        }deviceStatus;
    }head;
#pragma pack(pop)
    Slave2BackendPacket() = default;
    ConductionDataMsg condMsg;
    ClipDataMsg clipMsg;
    ResistanceDataMsg ristMsg;
    void handlePacket(uint8_t seq, uint8_t isMore, uint16_t len,const uint8_t* data) override;
private:


};

#endif //SLAVE2BACKEND_H
