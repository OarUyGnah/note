Logfile 内部包含一个`std::unique_ptr<FileUtil::AppendFile> file_`

`FileUtil::AppendFile`内部包含一个`FILE*`

`AsyncLogging`通过在threadFunc中构造一个Logfile（output），并通过缓冲区进行log的输出

调用Logfile::append

`AsyncLogging::append`内部通过Buffer的append，底层调用memcpy进行填充缓冲区

```cpp
// AsyncLogging.h
typedef muduo::detail::FixedBuffer<muduo::detail::kLargeBuffer> Buffer;
//FixedBuffer内部的append调用memcpy
//muduo::detail::kLargeBuffer = 4000 * 1000;
//内含char data_[muduo::detail::kLargeBuffer]数组


//BufferVector内部存放Buffer的unique_ptr
typedef std::vector<std::unique_ptr<Buffer>> BufferVector;
//using BufferPtr = unique_ptr<Buffer>
typedef BufferVector::value_type BufferPtr;

  const int flushInterval_;
  std::atomic<bool> running_;
  const string basename_;
  const off_t rollSize_;
  muduo::Thread thread_;
  muduo::CountDownLatch latch_;
  muduo::MutexLock mutex_;
  muduo::Condition cond_ GUARDED_BY(mutex_);
  BufferPtr currentBuffer_ GUARDED_BY(mutex_);
  BufferPtr nextBuffer_ GUARDED_BY(mutex_);
  BufferVector buffers_ GUARDED_BY(mutex_);




typedef muduo::detail::FixedBuffer<muduo::detail::kLargeBuffer> Buffer;



typedef std::vector<std::unique_ptr<Buffer>> BufferVector;


typedef BufferVector::value_type BufferPtr; 
```

DACAB