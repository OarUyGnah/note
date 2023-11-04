ExtendibleHashTable记录 页和帧的对应关系

LRUKReplacer 如果free_list_ 没有多余的帧，则lru_k中删除一个

std::list<frame_id_t> free_list_ 存放空闲帧

Page *pages_数组放入页

DiskManager *disk_manager_ 负责将页内容写入磁盘

```cc
class BufferPoolManagerInstance : public BufferPoolManager {
 public:
  BufferPoolManagerInstance(size_t pool_size, DiskManager *disk_manager, size_t replacer_k = LRUK_REPLACER_K,
                            LogManager *log_manager = nullptr);

  ~BufferPoolManagerInstance() override;

  auto GetPoolSize() -> size_t override { return pool_size_; }

  auto GetPages() -> Page * { return pages_; }

 protected:
  // 初始将所有page都设置为不可替换 即 replacer_->SetEvictable(frame_id, false);
  auto NewPgImp(page_id_t *page_id) -> Page * override;

  // 找到了但有人正在用即pin_count_>0 返回nullptr,如果没人使用则返回指定的,如果没有则创建新的
  auto FetchPgImp(page_id_t page_id) -> Page * override;

  // 将当前page标记为已不使用，如果是写操作(is_dirty == true)则用diskmanager将其写入磁盘
  auto UnpinPgImp(page_id_t page_id, bool is_dirty) -> bool override;

  auto FlushPgImp(page_id_t page_id) -> bool override;

  
  void FlushAllPgsImp() override;

  // 如果page_id的pin_count_ == 0，才删除
  auto DeletePgImp(page_id_t page_id) -> bool override;

  const size_t pool_size_;

  std::atomic<page_id_t> next_page_id_ = 0;

  const size_t bucket_size_ = 4;


  Page *pages_;

  DiskManager *disk_manager_ __attribute__((__unused__));

  LogManager *log_manager_ __attribute__((__unused__));

  ExtendibleHashTable<page_id_t, frame_id_t> *page_table_;
 
  LRUKReplacer *replacer_;
  
  std::list<frame_id_t> free_list_;

  std::mutex latch_;

  auto AllocatePage() -> page_id_t;

  void DeallocatePage(__attribute__((unused)) page_id_t page_id) {

  }

};
```

