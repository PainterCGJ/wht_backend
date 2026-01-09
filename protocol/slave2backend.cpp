#include "slave2backend.h"
#include "../logger.h" // 包含Logger类（protocol目录需要向上查找）
#include <cstring>
void ConductionDataMsg::setCallback(std::function<void(uint8_t seq, uint8_t isMore, ConductionDataMsg* msg)> callback)
{
    m_callback = callback;
}
void ConductionDataMsg::runCallback(uint8_t seq, uint8_t isMore)
{
    if (m_callback != nullptr) {
        m_callback(seq, isMore, this);
    }
}
void Slave2BackendPacket::handlePacket(uint8_t seq, uint8_t isMore, uint16_t len, const uint8_t* data)
{
    // 检查输入参数有效性
    if (data == nullptr) {
        Logger::appendLog("handlePacket: 数据指针为空");
        return;
    }

    if (len == 0) {
        Logger::appendLog("handlePacket: 数据长度为0");
        return;
    }

    // 定义最大消息大小限制（防止异常大的数据）
    const uint16_t MAX_MSG_SIZE = 65535;
    if (len > MAX_MSG_SIZE) {
        Logger::appendLog(QString("handlePacket: 数据长度异常：%1，截断为%2").arg(len).arg(MAX_MSG_SIZE));
        len = MAX_MSG_SIZE;
    }

    uint8_t msgID = data[0];
    if (msgID == CONDUCTION_DATA_MSG) {
        // 优化内存管理：先reserve再resize，减少重新分配
        if (len > 0) {
            condMsg.msgData.reserve(len); // 预分配容量
            condMsg.msgData.resize(len);
            memcpy(condMsg.msgData.data(), data, len);
        }
        else {
            condMsg.msgData.clear();
        }
        condMsg.runCallback(seq, isMore);
        // 回调完成后，如果数据不再需要，可以释放内存
        if (!isMore) {
            condMsg.msgData.clear();
            condMsg.msgData.shrink_to_fit(); // 释放多余内存
        }
    }
    else {
        Logger::appendLog(QString("handlePacket: 未知的消息ID：%1").arg(msgID));
    }
}
