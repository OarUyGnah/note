### 最大值

无符号类型最大值

```c
(size_t)(~(size_t)0)
```

有无符号类型皆可

```cc
std::numeric_limit<T>()::max();
```

```cc
template<typename T>
T getMaxNum() {
    return std::is_signed<T>() ? std::numeric_limits<T>::max() : ((T)(~(T)0));
}
```

