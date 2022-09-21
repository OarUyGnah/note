## Buffer

**两个位置`readerIndex` `writerIndex_`分别为已读到的位置和已写到的位置**

二者中间的内容是进行操作的地方

两种append方式，一种小端方式，一种大端方式

小端：`append()`

大端：`appendInt64()...` 先用`hostToNetwork16`将主机字节序转换为网络字节序再`append`



```cc
namespace net
{
    /// +-------------------+------------------+------------------+
    /// | prependable bytes |  readable bytes  |  writable bytes  |
    /// |                   |     (CONTENT)    |                  |
    /// +-------------------+------------------+------------------+
    /// |                   |                  |                  |
    /// 0      <=      readerIndex   <=   writerIndex    <=     size
    class Buffer : public muduo::copyable
    {
    public:
      static const size_t kCheapPrepend = 8;
      static const size_t kInitialSize = 1024;
       ...
    private:
      std::vector<char> buffer_;
      size_t readerIndex_;
      size_t writerIndex_;

      static const char kCRLF[];
    };
    const char Buffer::kCRLF[] = "\r\n";
    
}
```

```cc
size_t readableBytes() const { 
	return writerIndex_ - readerIndex_; 
}

size_t writableBytes() const { 
    return buffer_.size() - writerIndex_; 
}

size_t prependableBytes() const { 
    return readerIndex_; 
}

const char* peek() const { 
    return begin() + readerIndex_; 
}

void retrieve(size_t len) {
	assert(len <= readableBytes());
    if (len < readableBytes())
    {
      readerIndex_ += len;
    }
    else
    {
      retrieveAll();
    }
}
```

