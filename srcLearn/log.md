### class

- Logger
- LoggerManager
- LogLevel
- LogEvent
- LogEventWrap
- LogFormatter
- LogAppender



### Logger

日志器

```cc
// 写日志
void log(LogLevel::Level level, LogEvent::ptr event);
void debug(LogEvent::ptr event);
void info(LogEvent::ptr event);
void warn(LogEvent::ptr event);
void error(LogEvent::ptr event);
void fatal(LogEvent::ptr event);
// LogAppender
void addAppender(LogAppender::ptr appender);
void delAppender(LogAppender::ptr appender);
void clearAppenders();
// 
LogLevel::Level getLevel() const { return m_level;}
void setLevel(LogLevel::Level val) { m_level = val;}
const std::string& getName() const { return m_name;}
// 设置LogFormatter
void setFormatter(LogFormatter::ptr val);
// 设置日志格式
void setFormatter(const std::string& val);
LogFormatter::ptr getFormatter();
private:
    /// 日志名称
    std::string m_name;
    /// 日志级别
    LogLevel::Level m_level;
    /// Mutex
    MutexType m_mutex;
    /// 日志目标集合
    std::list<LogAppender::ptr> m_appenders;
    /// 日志格式器
    LogFormatter::ptr m_formatter;
    /// 主日志器
    Logger::ptr m_root;

======================
void Logger::log(LogLevel::Level level, LogEvent::ptr event) {
    if(level >= m_level) {
        auto self = shared_from_this();
        MutexType::Lock lock(m_mutex);
        if(!m_appenders.empty()) {
            for(auto& i : m_appenders) {
                i->log(self, level, event);
            }
        } else if(m_root) {
            m_root->log(level, event);
        }
    }
}
```

### LoggerManager

日志器管理

```
private:
    /// Mutex
    Spinlock  m_mutex;
    /// 日志器容器
    std::map<std::string, Logger::ptr> m_loggers;
    /// 主日志器
    Logger::ptr m_root;
```



### LogEventWrap 

绑定一个LogEvent，获取其日志流内容(eventStream)

### LogAppender

```cc
protected:
	/// 日志级别
    LogLevel::Level m_level = LogLevel::DEBUG;
    /// 是否有自己的日志格式器
    bool m_hasFormatter = false;
    /// Mutex
    MutexType m_mutex;
    /// 日志格式器
    LogFormatter::ptr m_formatter;包含一个mutex 一个LogFormatter、LogLevel
```

两个继承类 FileAppender StdoutAppender

```cc
class FileAppender : public LogAppender {
public:
  using fileAppenderPtr = std::shared_ptr<FileAppender>;

  FileAppender(std::string filename) : _filename(filename) {
  }

  void log(std::shared_ptr<Logger> logger, LogLevel::Level level,
           LogEvent::eventPtr event);

  bool reopen();

private:
  std::ofstream _ofs;
  std::string _filename;
  uint64_t _lastTime;
};

class StdoutAppender : public LogAppender {
public:
  using StdoutAppenderPtr = std::shared_ptr<StdoutAppender>;
  StdoutAppender() = default;
  void log(std::shared_ptr<Logger> logger, LogLevel::Level level,
           LogEvent::eventPtr event);
};
```



