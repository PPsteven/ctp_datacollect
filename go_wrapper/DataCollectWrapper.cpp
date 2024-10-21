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
