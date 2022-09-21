## namespace sockets

```cc
namespace sockets
{

///
/// Creates a non-blocking socket file descriptor,
/// abort if any error.
int createNonblockingOrDie(sa_family_t family);

int  connect(int sockfd, const struct sockaddr* addr);
void bindOrDie(int sockfd, const struct sockaddr* addr);
void listenOrDie(int sockfd);
int  accept(int sockfd, struct sockaddr_in6* addr);
ssize_t read(int sockfd, void *buf, size_t count);
ssize_t readv(int sockfd, const struct iovec *iov, int iovcnt);
ssize_t write(int sockfd, const void *buf, size_t count);
void close(int sockfd);
void shutdownWrite(int sockfd);

void toIpPort(char* buf, size_t size,
              const struct sockaddr* addr);
void toIp(char* buf, size_t size,
          const struct sockaddr* addr);

void fromIpPort(const char* ip, uint16_t port,
                struct sockaddr_in* addr);
void fromIpPort(const char* ip, uint16_t port,
                struct sockaddr_in6* addr);

int getSocketError(int sockfd);

const struct sockaddr* sockaddr_cast(const struct sockaddr_in* addr);
const struct sockaddr* sockaddr_cast(const struct sockaddr_in6* addr);
struct sockaddr* sockaddr_cast(struct sockaddr_in6* addr);
const struct sockaddr_in* sockaddr_in_cast(const struct sockaddr* addr);
const struct sockaddr_in6* sockaddr_in6_cast(const struct sockaddr* addr);

struct sockaddr_in6 getLocalAddr(int sockfd);
struct sockaddr_in6 getPeerAddr(int sockfd);
bool isSelfConnect(int sockfd);

}  // namespace sockets
```

```cc
void setNonBlockAndCloseOnExec(int sockfd)
{
  // non-block
  int flags = ::fcntl(sockfd, F_GETFL, 0);
  flags |= O_NONBLOCK;
  int ret = ::fcntl(sockfd, F_SETFL, flags);
  // FIXME check

  // close-on-exec
  flags = ::fcntl(sockfd, F_GETFD, 0);
  flags |= FD_CLOEXEC;
  ret = ::fcntl(sockfd, F_SETFD, flags);
  // FIXME check

  (void)ret;
}

//==================sockaddr_cast========================
const struct sockaddr* sockets::sockaddr_cast(const struct sockaddr_in6* addr)
{
  return static_cast<const struct sockaddr*>(implicit_cast<const void*>(addr));
}

struct sockaddr* sockets::sockaddr_cast(struct sockaddr_in6* addr)
{
  return static_cast<struct sockaddr*>(implicit_cast<void*>(addr));
}

const struct sockaddr* sockets::sockaddr_cast(const struct sockaddr_in* addr)
{
  return static_cast<const struct sockaddr*>(implicit_cast<const void*>(addr));
}

const struct sockaddr_in* sockets::sockaddr_in_cast(const struct sockaddr* addr)
{
  return static_cast<const struct sockaddr_in*>(implicit_cast<const void*>(addr));
}

const struct sockaddr_in6* sockets::sockaddr_in6_cast(const struct sockaddr* addr)
{
  return static_cast<const struct sockaddr_in6*>(implicit_cast<const void*>(addr));
}
//=================socket====================
int sockets::createNonblockingOrDie(sa_family_t family)
{
#if VALGRIND
  int sockfd = ::socket(family, SOCK_STREAM, IPPROTO_TCP);
  if (sockfd < 0)
  {
    LOG_SYSFATAL << "sockets::createNonblockingOrDie";
  }

  setNonBlockAndCloseOnExec(sockfd);
#else
  // IPPROTO_TCP 和 IPPTOTO_UDP，分别表示 TCP 传输协议和 UDP 传输协议。
  int sockfd = ::socket(family, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
  if (sockfd < 0)
  {
    LOG_SYSFATAL << "sockets::createNonblockingOrDie";
  }
#endif
  return sockfd;
}

//===================bind====================
void sockets::bindOrDie(int sockfd, const struct sockaddr* addr)
{
  int ret = ::bind(sockfd, addr, static_cast<socklen_t>(sizeof(struct sockaddr_in6)));
  if (ret < 0)
  {
    LOG_SYSFATAL << "sockets::bindOrDie";
  }
}
//===================listen===================
void sockets::listenOrDie(int sockfd)
{
  int ret = ::listen(sockfd, SOMAXCONN);
  if (ret < 0)
  {
    LOG_SYSFATAL << "sockets::listenOrDie";
  }
}
//===================accept====================
int sockets::accept(int sockfd, struct sockaddr_in6* addr)
{
  socklen_t addrlen = static_cast<socklen_t>(sizeof *addr);
#if VALGRIND || defined (NO_ACCEPT4)
  int connfd = ::accept(sockfd, sockaddr_cast(addr), &addrlen);
  setNonBlockAndCloseOnExec(connfd);
#else
  int connfd = ::accept4(sockfd, sockaddr_cast(addr),
                         &addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
#endif
  if (connfd < 0)
  {
    int savedErrno = errno;
    LOG_SYSERR << "Socket::accept";
    switch (savedErrno)
    {
      case EAGAIN:
      case ECONNABORTED:
      case EINTR:
      case EPROTO: // ???
      case EPERM:
      case EMFILE: // per-process lmit of open file desctiptor ???
        // expected errors
        errno = savedErrno;
        break;
      case EBADF:
      case EFAULT:
      case EINVAL:
      case ENFILE:
      case ENOBUFS:
      case ENOMEM:
      case ENOTSOCK:
      case EOPNOTSUPP:
        // unexpected errors
        LOG_FATAL << "unexpected error of ::accept " << savedErrno;
        break;
      default:
        LOG_FATAL << "unknown error of ::accept " << savedErrno;
        break;
    }
  }
  return connfd;
}

int sockets::connect(int sockfd, const struct sockaddr* addr)
{
  return ::connect(sockfd, addr, static_cast<socklen_t>(sizeof(struct sockaddr_in6)));
}

ssize_t sockets::read(int sockfd, void *buf, size_t count)
{
  return ::read(sockfd, buf, count);
}

ssize_t sockets::readv(int sockfd, const struct iovec *iov, int iovcnt)
{
  return ::readv(sockfd, iov, iovcnt);
}

ssize_t sockets::write(int sockfd, const void *buf, size_t count)
{
  return ::write(sockfd, buf, count);
}

void sockets::close(int sockfd)
{
  if (::close(sockfd) < 0)
  {
    LOG_SYSERR << "sockets::close";
  }
}

void sockets::shutdownWrite(int sockfd)
{
  /*
      关闭与 sockfd 关联的套接字。 
          SHUT_RD 不允许进一步的接收。 
          SHUT_WR 不允许进一步传输。
          SHUT_RDWR 不允许进一步的接收和传输。
  */
  if (::shutdown(sockfd, SHUT_WR) < 0)
  {
    LOG_SYSERR << "sockets::shutdownWrite";
  }
}

//=========根据ip字符串和port填充sockaddr_in(6)
void sockets::fromIpPort(const char* ip, uint16_t port,
                         struct sockaddr_in* addr)
{
  addr->sin_family = AF_INET;
  addr->sin_port = hostToNetwork16(port);
  if (::inet_pton(AF_INET, ip, &addr->sin_addr) <= 0)
  {
    LOG_SYSERR << "sockets::fromIpPort";
  }
}

void sockets::fromIpPort(const char* ip, uint16_t port,
                         struct sockaddr_in6* addr)
{
  addr->sin6_family = AF_INET6;
  addr->sin6_port = hostToNetwork16(port);
  if (::inet_pton(AF_INET6, ip, &addr->sin6_addr) <= 0)
  {
    LOG_SYSERR << "sockets::fromIpPort";
  }
}

int sockets::getSocketError(int sockfd)
{
  int optval;
  socklen_t optlen = static_cast<socklen_t>(sizeof optval);

  if (::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0)
  {
    return errno;
  }
  else
  {
    return optval;
  }
}

struct sockaddr_in6 sockets::getLocalAddr(int sockfd)
{
  struct sockaddr_in6 localaddr;
  memZero(&localaddr, sizeof localaddr);
  socklen_t addrlen = static_cast<socklen_t>(sizeof localaddr);
  if (::getsockname(sockfd, sockaddr_cast(&localaddr), &addrlen) < 0)
  {
    LOG_SYSERR << "sockets::getLocalAddr";
  }
  return localaddr;
}

struct sockaddr_in6 sockets::getPeerAddr(int sockfd)
{
  struct sockaddr_in6 peeraddr;
  memZero(&peeraddr, sizeof peeraddr);
  socklen_t addrlen = static_cast<socklen_t>(sizeof peeraddr);
  if (::getpeername(sockfd, sockaddr_cast(&peeraddr), &addrlen) < 0)
  {
    LOG_SYSERR << "sockets::getPeerAddr";
  }
  return peeraddr;
}

bool sockets::isSelfConnect(int sockfd)
{
  struct sockaddr_in6 localaddr = getLocalAddr(sockfd);
  struct sockaddr_in6 peeraddr = getPeerAddr(sockfd);
  if (localaddr.sin6_family == AF_INET)
  {
    const struct sockaddr_in* laddr4 = reinterpret_cast<struct sockaddr_in*>(&localaddr);
    const struct sockaddr_in* raddr4 = reinterpret_cast<struct sockaddr_in*>(&peeraddr);
    return laddr4->sin_port == raddr4->sin_port
        && laddr4->sin_addr.s_addr == raddr4->sin_addr.s_addr;
  }
  else if (localaddr.sin6_family == AF_INET6)
  {
    return localaddr.sin6_port == peeraddr.sin6_port
        && memcmp(&localaddr.sin6_addr, &peeraddr.sin6_addr, sizeof localaddr.sin6_addr) == 0;
  }
  else
  {
    return false;
  }
}
```



## Socket

```cc
class Socket : noncopyable
{
 public:
  explicit Socket(int sockfd)
    : sockfd_(sockfd)
  { }

  // Socket(Socket&&) // move constructor in C++11
  ~Socket();

  int fd() const { return sockfd_; }
  // return true if success.
  bool getTcpInfo(struct tcp_info*) const;
  bool getTcpInfoString(char* buf, int len) const;

  /// abort if address in use
  void bindAddress(const InetAddress& localaddr);
  /// abort if address in use
  void listen();

  /// On success, returns a non-negative integer that is
  /// a descriptor for the accepted socket, which has been
  /// set to non-blocking and close-on-exec. *peeraddr is assigned.
  /// On error, -1 is returned, and *peeraddr is untouched.
  int accept(InetAddress* peeraddr);

  void shutdownWrite();

  ///
  /// Enable/disable TCP_NODELAY (disable/enable Nagle's algorithm).
  ///
  void setTcpNoDelay(bool on);

  ///
  /// Enable/disable SO_REUSEADDR
  ///
  void setReuseAddr(bool on);

  ///
  /// Enable/disable SO_REUSEPORT
  ///
  void setReusePort(bool on);

  ///
  /// Enable/disable SO_KEEPALIVE
  ///
  void setKeepAlive(bool on);

 private:
  const int sockfd_;
};
```

```cc
Socket::~Socket()
{
  sockets::close(sockfd_);
}

bool Socket::getTcpInfo(struct tcp_info* tcpi) const
{
  socklen_t len = sizeof(*tcpi);
  memZero(tcpi, len);
  return ::getsockopt(sockfd_, SOL_TCP, TCP_INFO, tcpi, &len) == 0;
}

bool Socket::getTcpInfoString(char* buf, int len) const
{
  struct tcp_info tcpi;
  bool ok = getTcpInfo(&tcpi);
  if (ok)
  {
    snprintf(buf, len, "unrecovered=%u "
             "rto=%u ato=%u snd_mss=%u rcv_mss=%u "
             "lost=%u retrans=%u rtt=%u rttvar=%u "
             "sshthresh=%u cwnd=%u total_retrans=%u",
             tcpi.tcpi_retransmits,  // Number of unrecovered [RTO] timeouts
             tcpi.tcpi_rto,          // Retransmit timeout in usec
             tcpi.tcpi_ato,          // Predicted tick of soft clock in usec
             tcpi.tcpi_snd_mss,
             tcpi.tcpi_rcv_mss,
             tcpi.tcpi_lost,         // Lost packets
             tcpi.tcpi_retrans,      // Retransmitted packets out
             tcpi.tcpi_rtt,          // Smoothed round trip time in usec
             tcpi.tcpi_rttvar,       // Medium deviation
             tcpi.tcpi_snd_ssthresh,
             tcpi.tcpi_snd_cwnd,
             tcpi.tcpi_total_retrans);  // Total retransmits for entire connection
  }
  return ok;
}

void Socket::bindAddress(const InetAddress& addr)
{
  sockets::bindOrDie(sockfd_, addr.getSockAddr());
}

void Socket::listen()
{
  sockets::listenOrDie(sockfd_);
}

int Socket::accept(InetAddress* peeraddr)
{
  struct sockaddr_in6 addr;
  memZero(&addr, sizeof addr);
  int connfd = sockets::accept(sockfd_, &addr);
  if (connfd >= 0)
  {
    peeraddr->setSockAddrInet6(addr);
  }
  return connfd;
}

void Socket::shutdownWrite()
{
  sockets::shutdownWrite(sockfd_);
}

// TCP默认开启了Nagle算法。Nagle算法通过减少需要传输的数据包，来优化网络
// 启动TCP_NODELAY，就意味着禁用了Nagle算法，允许小包的发送
void Socket::setTcpNoDelay(bool on)
{
  int optval = on ? 1 : 0;
  ::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY,
               &optval, static_cast<socklen_t>(sizeof optval));
  // FIXME CHECK
}

void Socket::setReuseAddr(bool on)
{
  int optval = on ? 1 : 0;
  ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR,
               &optval, static_cast<socklen_t>(sizeof optval));
  // FIXME CHECK
}

void Socket::setReusePort(bool on)
{
#ifdef SO_REUSEPORT
  int optval = on ? 1 : 0;
  int ret = ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT,
                         &optval, static_cast<socklen_t>(sizeof optval));
  if (ret < 0 && on)
  {
    LOG_SYSERR << "SO_REUSEPORT failed.";
  }
#else
  if (on)
  {
    LOG_ERROR << "SO_REUSEPORT is not supported.";
  }
#endif
}

void Socket::setKeepAlive(bool on)
{
  int optval = on ? 1 : 0;
  ::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE,
               &optval, static_cast<socklen_t>(sizeof optval));
  // FIXME CHECK
}
```

