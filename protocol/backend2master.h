#ifndef BACKEND2MASTER_H
#define BACKEND2MASTER_H

#include "messagebase.h"
#include "packetbase.h"

class SlaveCfgMsg : public MsgBase {
public:
    typedef struct {
        uint8_t slave_id;
        uint8_t couductionNum;
        uint8_t resistanceNum;
        uint8_t clipMode;
        uint8_t clipStatus;
    } SlaveCfg;
    SlaveCfg slaveCfg;
    SlaveCfgMsg(SlaveCfg slaveCfg) : slaveCfg(slaveCfg) {
    }
    
};
class Backend2Master : public PacketBase {
public:
    enum MessageType {
        SLAVE_CFG_MSG = 0,
        MODE_CFG_MSG,
        SLAVE_RST_MSG,
        CTRL_MSG,
        INTERVAL_CFG_MSG,
        PONPING_CTRL_MSGG_REPLY,
        DEVICE_LIST_REQ_MSG,
        CLEAR_DEVICE_LIST_MSG,
        SET_UWB_CHAN,
    };
    Backend2Master();
};

#endif // BACKEND2MASTER_H
