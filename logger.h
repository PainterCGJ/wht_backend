#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QPlainTextEdit>

/**
 * @brief 日志工具类，提供全局静态日志输出功能
 * 
 * 使用前需要先调用 setLogWidget() 设置日志输出控件
 * 之后可以在任何地方调用 appendLog() 输出日志
 */
class Logger
{
public:
    /**
     * @brief 设置日志输出控件
     * @param logWidget 用于显示日志的QPlainTextEdit控件指针
     */
    static void setLogWidget(QPlainTextEdit* logWidget);
    
    /**
     * @brief 追加带时间戳的日志
     * @param message 日志消息内容
     */
    static void appendLog(const QString& message);
    
    /**
     * @brief 清空日志
     */
    static void clearLog();
    
    /**
     * @brief 检查日志控件是否已设置
     * @return 如果日志控件已设置返回true，否则返回false
     */
    static bool isLogWidgetSet();

private:
    static QPlainTextEdit* s_logWidget;  // 静态日志控件指针
};

#endif // LOGGER_H

