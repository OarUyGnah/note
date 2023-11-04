

history_list 优先放入该队列

cache_list 如果调用次数>=k，则将其放入缓存队列

```cc
class LRUKReplacer {
 public:
  explicit LRUKReplacer(size_t num_frames, size_t k);

  DISALLOW_COPY_AND_MOVE(LRUKReplacer);

  ~LRUKReplacer() = default;
  // 驱逐一个 优先删除history_list_中evictable == true的，没有则删除cache_list_中evictable的
  // 并获取对应的帧id
  auto Evict(frame_id_t *frame_id) -> bool;
  // 调用一次
  void RecordAccess(frame_id_t frame_id);

  void SetEvictable(frame_id_t frame_id, bool set_evictable);

  void Remove(frame_id_t frame_id);
  // 可驱逐的数量
  auto Size() -> size_t;
  // 0 => 不在 1 => 在history  2 => 在cache
  auto AlreadyInList(frame_id_t frame_id) -> int;

 private:
  // bool代表
  std::list<std::tuple<frame_id_t, bool, size_t>> history_list_{};  // waiting to be removed list
  std::list<std::tuple<frame_id_t, bool, size_t>> cache_list_{};
  size_t replacer_size_;
  size_t k_;
  std::mutex latch_;
};
```

