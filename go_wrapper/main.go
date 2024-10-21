package main

/*
#cgo LDFLAGS: -L. -lDataCollectWrapper -lDataCollect
#include "DataCollectWrapper.h"
#include <stdlib.h>
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
	result := C.CTP_GetSystemInfo_C((*C.char)(unsafe.Pointer(&buffer[0])), &bufferSize)

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
