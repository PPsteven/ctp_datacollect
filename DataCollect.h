#ifndef  DATA_COLLECT_H
#define  DATA_COLLECT_H

#define DLL_EXPORT   __declspec(dllexport)

#if defined(IS_WINCOLLECT_LIB) && defined(WIN32)
#ifdef LIB_DATA_COLLECT_API_EXPORT
#define DATA_COLLECT_API_EXPORT __declspec(dllexport)
#else
#define DATA_COLLECT_API_EXPORT __declspec(dllimport)
#endif
#else
#define DATA_COLLECT_API_EXPORT 
#endif


///��ȡAES���ܺ�RSA���ܵ��ն���Ϣ 
///@pSystemInfo ���� �ռ���Ҫ�������Լ����� ����270���ֽ�
///@nLen ���� ��ȡ���Ĳɼ���Ϣ�ĳ���
///�ɼ���Ϣ�ڿ��ܺ��С�\0�� ���������ʹ���ڴ渴��
DATA_COLLECT_API_EXPORT int CTP_GetSystemInfo(char* pSystemInfo, int& nLen);

#endif