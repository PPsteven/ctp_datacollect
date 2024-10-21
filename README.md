# CTP看穿式监管-数据采集模块调用
使用CTP看穿式监管，期货公司需要正确采集到客户端的机器信息，并进行验证。
经过沟通了解，无论是直连类型还是中继类型的方式链接，均需要加载采集链接库，并将采集到的信息上传。
<!--more-->
## 环境
- 操作系统：Linux
- CTP版本：6.7.0_P1_CP_LinuxDataCollect
- 文件：
    - 采集链接库：6.7.0_P1_CP_LinuxDataCollect.so
    - 采集链接库头文件：6.7.0_P1_CP_LinuxDataCollect.h

## CPP调用
> 因为 so 文件本身就是C++编译的，所以可以直接调用。

```cpp
#include <iostream>
#include <cstring>
#include "DataCollect.h"

int main() {
    char systemInfo[270];  // 根据注释，分配270字节的空间
    int len = 0;

    int result = CTP_GetSystemInfoUnAesEncode(systemInfo, len);

    if (result == 0) {  // 假设0表示成功，您可能需要根据实际情况调整
        std::cout << "获取系统信息成功！" << std::endl;
        std::cout << "信息长度: " << len << std::endl;
        std::cout << "系统信息: " << systemInfo << std::endl;
    } else {
        std::cout << "获取系统信息失败，错误代码: " << result << std::endl;
    }

    return 0;
}
```

编译流程
```bash
g++ -o datacollect_cpp main.cpp -L. -lDataCollect -Wl,-rpath,.
```
下面解释一下编译参数
- `-o datacollect_cpp` 指定编译后的可执行文件名
- `-L.` 指定链接库路径, 会从当前路径下找libDataCollect.so
- `-lDataCollect` 指定链接库名称, 默认会去链接库路径下找libDataCollect.so
- `-Wl,-rpath,.` 指定链接库路径, 否则运行时会报错找不到链接库。或者通过在运行时指定 `LD_LIBRARY_PATH` 环境变量

运行
```bash
sudo ./datacollect_cpp
```
如果没有指定rpath，运行时需要指定 `LD_LIBRARY_PATH` 环境变量

```bash
sudo LD_LIBRARY_PATH=$LD_LIBRARY_PATH:. ./datacollect_cpp
```

## Go调用-使用wrapper将C++的so包装成C的so
> 因为 go 语言是跨平台的，所以需要使用 cgo 来调用 C 语言编写的链接库。
> 使用wrapper将C++的so包装成C的so，然后go调用C的so。

### 创建C++ Wrapper

为了让Go能够调用C++库，我们需要创建一个C++ wrapper。这个wrapper会将C++函数转换为C风格的函数，这样Go就可以通过cgo来调用它们。

1. 创建 `DataCollectWrapper.h`:

```c
#ifndef DATA_COLLECT_WRAPPER_H
#define DATA_COLLECT_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

EXPORT int CTP_GetSystemInfo_C(char* pSystemInfo, int* nLen);

#ifdef __cplusplus
}
#endif

#endif // DATA_COLLECT_WRAPPER_H
```

2. 创建 `DataCollectWrapper.cpp`:

```cpp
#include "DataCollectWrapper.h"
#include "DataCollect.h"

extern "C" {

EXPORT int CTP_GetSystemInfo_C(char* pSystemInfo, int* nLen) {
    int cpp_nLen = *nLen;
    int result = CTP_GetSystemInfo(pSystemInfo, cpp_nLen);
    *nLen = cpp_nLen;
    
    return result;
}

}
```

### 编译C++ Wrapper

使用以下命令编译C++ wrapper:

```bash
g++ -shared -fPIC -o libDataCollectWrapper.so DataCollectWrapper.cpp -L. -lDataCollect
```
上述的命令的作用是生成 `libDataCollectWrapper.so` 文件，这是一个可以被Go程序调用的C共享库。

- `-shared` 选项告诉编译器生成一个共享库
- `-fPIC` 选项告诉编译器生成位置无关代码，这是生成共享库时必需的
- `-o libDataCollectWrapper.so` 指定输出文件的名称
- `DataCollectWrapper.cpp` 是我们要编译的源文件
- `-L. -lDataCollect` 指定链接库路径和库名称

### 创建Go程序

创建一个名为 `main.go` 的Go文件，内容如下：

```go
package main

/*
#cgo LDFLAGS: -L. -lDataCollectWrapper
#include "DataCollectWrapper.h"
*/
import "C"
import (
    "fmt"
    "unsafe"
)

func main() {
    systemInfo := make([]byte, 270)
    length := C.int(len(systemInfo))

    result := C.CTP_GetSystemInfo_C((*C.char)(unsafe.Pointer(&systemInfo[0])), &length)

    if result == 0 {
        fmt.Println("获取系统信息成功！")
        fmt.Printf("信息长度: %d\n", length)
        fmt.Printf("系统信息: %s\n", string(systemInfo[:length]))
    } else {
        fmt.Printf("获取系统信息失败，错误代码: %d\n", result)
    }
}
```

### 编译和运行Go程序

使用以下命令编译Go程序：

```bash
go build -o datacollect_go -ldflags "-r ." main.go 
```

运行程序：

```bash
sudo ./datacollect_go 
```

注意：如果遇到找不到共享库的问题，可以设置 `LD_LIBRARY_PATH` 环境变量：

```bash
sudo LD_LIBRARY_PATH=$LD_LIBRARY_PATH:. ./datacollect_go
```

通过这种方式，我们成功地使用Go调用了原始的C++共享库。这个方法的优点是它允许我们在不修改原始C++库的情况下，为Go程序提供一个干净的接口。

## Go调用-使用cgo直接调用C++的so

除了使用 C++ wrapper 的方法，我们还可以使用 cgo 直接调用 C++ 的共享库。这种方法更加直接，但需要注意一些细节。

### 查看共享库中的真实函数名

> 在使用 cgo 直接调用 C++ 共享库时，我们需要使用 `nm` 命令来查看共享库中的真实函数名。这是因为 C++ 编译器会对函数名进行名称修饰（name mangling）。
> 名称修饰是 C++ 编译器用来支持函数重载和命名空间等特性的一种技术。它会将函数的参数类型、命名空间等信息编码到函数名中，以区分同名但参数不同的函数。这导致 C++ 函数在编译后的二进制文件中的实际名称与源代码中的名称不同。例如，`CTP_GetSystemInfo` 函数在编译后可能变成 `_Z17CTP_GetSystemInfoPcRi`。这个修饰后的名称包含了函数名、参数数量和类型等信息。使用 `nm` 命令可以查看共享库中的符号表，从而找到这些被修饰过的函数名。这对于正确地在 Go 代码中声明和调用这些函数是必要的，因为 cgo 需要知道确切的函数名才能正确链接。

```bash
nm libDataCollect.so | grep CTP_GetSystemInfo
```

输出应该类似于：

```
0000000000085790 T _Z17CTP_GetSystemInfoPcRi
0000000000085640 T _Z28CTP_GetSystemInfoUnAesEncodePcRi
```

这里我们可以看到 `CTP_GetSystemInfo` 和 `CTP_GetSystemInfoUnAesEncode` 的真实函数名。

### 创建 Go 程序

创建一个名为 `main.go` 的文件，内容如下：

```go
package main

/*
#cgo CFLAGS: -I.
#cgo LDFLAGS: -L. -lDataCollect
#include <stdlib.h>
extern int _Z17CTP_GetSystemInfoPcRi(char* pSystemInfo, int* nLen);

int CTP_GetSystemInfo_Wrapper(char* pSystemInfo, int* nLen) {
    return _Z17CTP_GetSystemInfoPcRi(pSystemInfo, nLen);
}

*/
import "C"
import (
	"fmt"
	"unsafe"
)

func main() {
	// 分配内存来存储系统信息
	bufferSize := C.int(270) // 根据注释，我们需要至少270字节
	buffer := make([]byte, bufferSize)

	// 调用C函数
	result := C.CTP_GetSystemInfo_Wrapper((*C.char)(unsafe.Pointer(&buffer[0])), &bufferSize)

	if result == 0 {
		// 成功获取系统信息
		systemInfo := C.GoString((*C.char)(unsafe.Pointer(&buffer[0])))
		fmt.Println("获取系统信息成功！")
		fmt.Printf("信息长度: %d\n", int(bufferSize))
		fmt.Printf("系统信息: %s\n", systemInfo)
	} else {
		fmt.Printf("获取系统信息失败，错误代码: %d\n", int(result))
	}
}

```

在这个程序中，我们直接声明了 C++ 函数的真实名称，并通过 cgo 调用它们。

### 编译和运行

使用以下命令编译 Go 程序：

```bash
go build -o datacollect_go -ldflags "-r ." main.go
```

运行程序：

```bash
sudo ./datacollect_go
```

# 参考资料
- [GO/C语言直接调用C++ 库简单方法](https://blog.csdn.net/pinke/article/details/141072533)
- [Golang cgo调用c++动态库so文件](https://aibenlin.com/golang/2019/09/05/golang-cgo_call_c++.html)
- [看穿式监管数据采集说明](https://ctpapi.jedore.top/6.7.2/CTSJGSJCJJK/_CTSJGSJCJJK/#1)