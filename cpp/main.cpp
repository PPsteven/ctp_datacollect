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

