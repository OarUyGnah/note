### SubTask ParallelTask

ParallelTask继承SubTask

内部包含一个SubTask数组

```cc
class SubTask
{
public:
	virtual void dispatch() = 0;

private:
	virtual SubTask *done() = 0;

protected:
	void subtask_done();

public:
	void *get_pointer() const { return this->pointer; }
	void set_pointer(void *pointer) { this->pointer = pointer; }

private:
	ParallelTask *parent;
	void *pointer;

public:
	SubTask()
	{
		this->parent = NULL;
		this->pointer = NULL;
	}

	virtual ~SubTask() { }
	friend class ParallelTask;
};

class ParallelTask : public SubTask
{
public:
	virtual void dispatch();

protected:
	SubTask **subtasks;
	size_t subtasks_nr;

private:
	size_t nleft;

public:
	ParallelTask(SubTask **subtasks, size_t n)
	{
		this->subtasks = subtasks;
		this->subtasks_nr = n;
	}

	virtual ~ParallelTask() { }
	friend class SubTask;
};
```

### msgqueue

一个简易的消息队列

有消息就通知consumer

```cc
typedef struct __msgqueue msgqueue_t;

msgqueue_t *msgqueue_create(size_t maxlen, int linkoff);
void msgqueue_put(void *msg, msgqueue_t *queue);
void *msgqueue_get(msgqueue_t *queue);
void msgqueue_set_nonblock(msgqueue_t *queue);
void msgqueue_set_block(msgqueue_t *queue);
void msgqueue_destroy(msgqueue_t *queue);

struct __msgqueue
{
	size_t msg_max; // 阻塞模式
	size_t msg_cnt;
	int linkoff; // 偏移量
	int nonblock; // 非阻塞1 阻塞0
	void *head1;
	void *head2;
	void **get_head;
	void **put_head;
	void **put_tail;
	pthread_mutex_t get_mutex;
	pthread_mutex_t put_mutex;
	pthread_cond_t get_cond;
	pthread_cond_t put_cond;
};
```

### thrdpool

线程池

```cc
typedef struct __thrdpool thrdpool_t;

struct thrdpool_task
{
	void (*routine)(void *);
	void *context;
};

thrdpool_t *thrdpool_create(size_t nthreads, size_t stacksize);
int thrdpool_schedule(const struct thrdpool_task *task, thrdpool_t *pool);
int thrdpool_increase(thrdpool_t *pool);
int thrdpool_in_pool(thrdpool_t *pool);
void thrdpool_destroy(void (*pending)(const struct thrdpool_task *),
					  thrdpool_t *pool);

struct __thrdpool
{
	msgqueue_t *msgqueue; // 非阻塞
	size_t nthreads;
	size_t stacksize;
	pthread_t tid;
	pthread_mutex_t mutex;
	pthread_key_t key; // 创建一个键，然后可以使用pthread_setspecific函数将特定于线程的值与该键关联起来。
	pthread_cond_t *terminate;
};

struct __thrdpool_task_entry
{
	void *link;
	struct thrdpool_task task;
};

static pthread_t __zero_tid;

// 每个线程内部函数
static void *__thrdpool_routine(void *arg);
```

### list

包含双向、单向链表

```c
struct list_head {
	struct list_head *next, *prev;
};

struct slist_node {
	struct slist_node *next;
};

struct slist_head {
	struct slist_node first, *last;
};
```

封装了一下宏定义如for循环变量list等

```c
#define list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)

#define list_for_each_prev(pos, head) \
	for (pos = (head)->prev; pos != (head); pos = pos->prev)

#define list_for_each_safe(pos, n, head) \
	for (pos = (head)->next, n = pos->next; pos != (head); \
		 pos = n, n = pos->next)
```

`list_entry`使用空指针获取member在结构体的偏移量，并根据当前指针位置获得结构体的地址

```cc
#define list_entry(ptr, type, member) \
	((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))
```

### poller

底层为epoll