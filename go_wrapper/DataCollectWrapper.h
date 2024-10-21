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
