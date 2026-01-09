#ifndef MESSAGEBASE_H
#define MESSAGEBASE_H

#include <cstdint>
#include <functional>
#include <vector>

class MsgBase {
public:
    MsgBase() = default;
    std::vector<uint8_t> msgData;
private:
    uint8_t msgType;
};

#endif // MESSAGEBASE_H
