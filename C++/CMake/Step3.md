## 练习1	为库添加使用要求

#### 目标	添加库的使用要求

#### 命令

- `target_compile_definitions()`
- `target_compile_options()`
- `target_include_directories()`[相关信息](https://blog.csdn.net/qq_33726635/article/details/121896441)
- `target_link_directories()`
- `target_link_options()`
- `target_precompile_headers()`
- `target_sources()`

```cmake
# MathFunctions/CMakeLists.txt
target_include_directories(MathFunctions
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} # 只包含
)
```

```cmake
# CMakeLists.txt
cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(Tutorial VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# should we use our own math functions
option(USE_MYMATH "Use tutorial provided math implementation" ON)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file(TutorialConfig.h.in TutorialConfig.h)

# TODO 2: Remove EXTRA_INCLUDES list

# add the MathFunctions library
if(USE_MYMATH)
  add_subdirectory(MathFunctions)
  list(APPEND EXTRA_LIBS MathFunctions)
endif()

# add the executable
add_executable(Tutorial tutorial.cxx)

target_link_libraries(Tutorial PUBLIC ${EXTRA_LIBS})

# TODO 3: Remove use of EXTRA_INCLUDES

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
target_include_directories(Tutorial PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           # ${EXTRA_INCLUDES}
                           )
```
