#include "logger.h"
#include <QDateTime>
#include <QScrollBar>
#include <QDebug>

// 初始化静态成员变量
QPlainTextEdit* Logger::s_logWidget = nullptr;
bool Logger::s_logEnabled = true;  // 默认启用日志输出

void Logger::setLogWidget(QPlainTextEdit* logWidget)
{
    s_logWidget = logWidget;
}

void Logger::appendLog(const QString& message)
{
    // 如果日志输出被禁用，直接返回
    if (!s_logEnabled) {
        return;
    }
    
    // 获取当前时间戳（格式：yyyy-MM-dd hh:mm:ss.zzz）
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    
    // 格式化日志消息：[时间戳] 消息内容
    QString logMessage = QString("[%1] %2").arg(timestamp, message);
    
    // 同时输出到qDebug
    qDebug() << logMessage;
    
    // 检查日志控件是否已设置
    if (!s_logWidget) {
        // 如果未设置，只输出到qDebug
        return;
    }
    
    // 追加到文本编辑框
    s_logWidget->appendPlainText(logMessage);
    
    // 自动滚动到底部，确保最新日志可见
    QScrollBar* scrollBar = s_logWidget->verticalScrollBar();
    if (scrollBar) {
        scrollBar->setValue(scrollBar->maximum());
    }
}

void Logger::clearLog()
{
    if (s_logWidget) {
        s_logWidget->clear();
    }
}

bool Logger::isLogWidgetSet()
{
    return s_logWidget != nullptr;
}

void Logger::setLogEnabled(bool enabled)
{
    s_logEnabled = enabled;
}

bool Logger::isLogEnabled()
{
    return s_logEnabled;
}

