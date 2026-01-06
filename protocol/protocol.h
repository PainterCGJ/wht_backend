#ifndef PROTOCOL_H
#define PROTOCOL_H
#include "slave2backend.h"
#include <cstdint>
class Protocol
{
public:
    Protocol() =default;
    uint16_t delimiter = 0xCDAB;
#pragma  pack(push,1)
    struct{
        uint8_t packetID;
        uint8_t seq;
        uint8_t isMore;
        uint16_t len;
    }head;
#pragma pack(pop)


    void parse(const uint8_t* data,uint16_t len);
    Slave2BackendPacket slave2BackendPacket;
private:
    enum ParseStatus{
        FindDelimiter,
        ParseHead,
        ReadPayload,
    };
    ParseStatus parseStatus = FindDelimiter;
    uint8_t m_nextSeq = 0;
    uint8_t m_waitFirstFragment = 1;
    std::vector<uint8_t> m_buffer;
    uint32_t m_recvLen = 0;
    void parseByte(const uint8_t* data,uint16_t len);
};

#endif // PROTOCOL_H
