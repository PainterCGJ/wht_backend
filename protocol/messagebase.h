#ifndef MESSAGEBASE_H
#define MESSAGEBASE_H

#include <functional>

class MsgBase{
public:
    MsgBase() = default;
    std::vector<uint8_t> msgData;
};


#endif // MESSAGEBASE_H
