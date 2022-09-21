## Poller

抽象类，派生类分别实现了poll epoll

poller内部监听读写时间，将事件返回到Channel中，Channel对应放在了EventLoop的Channel数组中

EventLoop内部的loop在每次循环过程中遍历Channel数组去处理相应的事件

处理时间时根据不同情况触发不同的回调函数

```cc
class Poller : noncopyable
{
 public:
  typedef std::vector<Channel*> ChannelList;

  Poller(EventLoop* loop);
  virtual ~Poller();

  /// Polls the I/O events.
  /// Must be called in the loop thread.
  virtual Timestamp poll(int timeoutMs, ChannelList* activeChannels) = 0;

  /// Changes the interested I/O events.
  /// Must be called in the loop thread.
  virtual void updateChannel(Channel* channel) = 0;

  /// Remove the channel, when it destructs.
  /// Must be called in the loop thread.
  virtual void removeChannel(Channel* channel) = 0;

  virtual bool hasChannel(Channel* channel) const;

  static Poller* newDefaultPoller(EventLoop* loop);

  void assertInLoopThread() const
  {
    ownerLoop_->assertInLoopThread();
  }

 protected:
  // K->fd	V->Channel*
  typedef std::map<int, Channel*> ChannelMap;
  ChannelMap channels_;

 private:
  EventLoop* ownerLoop_;
};

Poller::Poller(EventLoop* loop)
  : ownerLoop_(loop)
{
}

Poller::~Poller() = default;

bool Poller::hasChannel(Channel* channel) const
{
  assertInLoopThread();
  ChannelMap::const_iterator it = channels_.find(channel->fd());
  return it != channels_.end() && it->second == channel;
}
```

## PollPoller

```cc
class PollPoller : public Poller
{
 public:

  PollPoller(EventLoop* loop);
  ~PollPoller() override;

  Timestamp poll(int timeoutMs, ChannelList* activeChannels) override;
  // 将channel中存储的信息转化为新的pollfd放入pollfds_中
  void updateChannel(Channel* channel) override;
  void removeChannel(Channel* channel) override;

 private:
  // 调用poll后返回的pollfds_的内容(fd revents)放到activeChannels中
  void fillActiveChannels(int numEvents,
                          ChannelList* activeChannels) const;

  typedef std::vector<struct pollfd> PollFdList;
  PollFdList pollfds_;
};
```

## EPollPoller

```cc
class EPollPoller : public Poller
{
 public:
  EPollPoller(EventLoop* loop);
  ~EPollPoller() override;

  Timestamp poll(int timeoutMs, ChannelList* activeChannels) override;
  void updateChannel(Channel* channel) override;
  void removeChannel(Channel* channel) override;

 private:
  static const int kInitEventListSize = 16;

  static const char* operationToString(int op);

  void fillActiveChannels(int numEvents,
                          ChannelList* activeChannels) const;
  void update(int operation, Channel* channel);

  typedef std::vector<struct epoll_event> EventList;

  int epollfd_;
  EventList events_;
};
```

