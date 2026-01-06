#ifndef PACKETBASE_H
#define PACKETBASE_H
#include <cstdint>
class PacketBase{
public:
    typedef enum{
        Master2Slave,
        Slave2Master,
        Backend2Master,
        Master2Backend,
        Slave2Backend,
    }PacketID;
    PacketBase() = default;
    static uint16_t mtu;
    virtual void handlePacket(uint8_t seq, uint8_t isMore, uint16_t len,const uint8_t* data) = 0;
};

#endif // PACKETBASE_H
