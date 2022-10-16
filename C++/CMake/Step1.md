## 练习1	构建一个基础项目

#### 目标	创建一个简单的CMake项目

完成TODO 1、2、3 

最基本CMake项目是从单个源代码文件构建的可执行文件

#### 命令

- `add_executable()`
- `cmake_minimum_required()`
- `project()`



```cmake
cmake_minimum_required(VERSION 3.10)
project(Tutorial)
add_executable(tutorial tutorial.cxx)
```

```powershell
mkdir build
cd build
```

cmake进行构建

```powershell
cmake .. -G "MinGW Makefiles" #此处windows默认使用nmake
cmake --build
```

## 练习2	指定C++标准

#### 目标	添加C++11功能

完成TODO 4、6

#### 命令

- `CMAKE_CXX_STANDARD`
- `CMAKE_CXX_STANDARD_REQUIRED`
- `set()`

```cmake
cmake_minimum_required(VERSION 3.10)

project(Tutorial)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(Tutorial tutorial.cxx)

```

## 练习3	添加版本号和配置的头文件

#### 目标	定义并报告项目的版本号

完成TODO 7~12

#### 命令

- `_VERSION_MAJOR`
- `_VERSION_MINOR`
- `configure_file()`   [cmake：configure_file](https://zhuanlan.zhihu.com/p/436923370)
- `target_include_directories()`

创建一个定义版本号的输入头文件`TutorialConfig.h.in`，它将接受`configure_file()`

TODO

- 7    `project(Tutorial VERSION 1.0)`

- 8    `configure_file(TutorialConfig.h.in TutorialConfig.h)`	

- 9    

  ```CMAKE
  target_include_directories(
      Tutorial PUBLIC
      ${PROJECT_BINARY_DIR}
  )
  ```

- 10    `TutorialConfig.h.in`是要配置的输入头文件。什么时候`configure_file()`从我们的 中调用，和`CMakeLists.txt`的值将被替换为来自项目中的相应版本号 

  ```c
  // 在 <input> 文件中只使用 @VAR@ 的方式获取CMakelists.txt变量值，不适用 ${VAR} 的方式
  #define Tutorial_VERSION_MAJOR @Tutorial_VERSION_MAJOR@ 
  #define Tutorial_VERSION_MINOR @Tutorial_VERSION_MINOR@
  ```

  构建以后会在build生成一个`TutorialConfig.h`文件如下

  ```c
  #define Tutorial_VERSION_MAJOR 1 // 为project(Tutorial VERSION 1.0)中的1
  #define Tutorial_VERSION_MINOR 0 // 为project(Tutorial VERSION 1.0)中的0
  ```

- 11    `#include "TutorialConfig.h"`

- 12   

  ```cc
  if (argc < 2) {
      // TODO 12: Create a print statement using Tutorial_VERSION_MAJOR
      //          and Tutorial_VERSION_MINOR
      std::cout << "Usage: " << argv[0] << " number" << std::endl;
      std::string s = argv[0];
      std::cout << std::string(s.begin() + s.rfind("\\") + 1,s.end()) << " Version " << Tutorial_VERSION_MAJOR << "."
                << Tutorial_VERSION_MINOR << std::endl;
      return 1;
    }
  ```

  



