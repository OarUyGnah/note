## 练习1	使用接口库设置C++标准

#### 目标	添加`INTERFACE`库目标以指定所需C++标准

#### 相关命令

- `add_library()`
- `target_compile_features()`
- `target_link_libraries()`

```cmake
# MathFunctions/CMakeLists.txt
add_library(MathFunctions mysqrt.cxx)

# state that anybody linking to us needs to include the current source dir
# to find MathFunctions.h, while we don't.
target_include_directories(MathFunctions
          INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
          )

# TODO 3: Link to tutorial_compiler_flags
target_link_libraries(
    MathFunctions   tutorial_compiler_flags
)
```

```cmake
# CMakeLists.txt
add_library(
  tutorial_compiler_flags INTERFACE 
)
target_compile_features(
  tutorial_compiler_flags INTERFACE cxx_std_11
)


target_link_libraries(Tutorial PUBLIC ${EXTRA_LIBS} tutorial_compiler_flags)
```



## 练习2	使用生成器表达式添加编译器警告标志

#### 目标	构建时添加编译器警告标志，而不是为已安装的版本

#### 相关命令

- `cmake-generator-expressions(7)`
- `cmake_minimum_required()`
- `set()`
- `target_compile_options()`