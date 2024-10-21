复制文件
ln -s ../DataCollect.h DataCollect.h
ln -s ../libDataCollect.so libDataCollect.so

真实函数名
nm libDataCollect.so | grep CTP_GetSystemInfo
0000000000085790 T _Z17CTP_GetSystemInfoPcRi
0000000000085640 T _Z28CTP_GetSystemInfoUnAesEncodePcRi

编译
go build -o datacollect_go -ldflags "-r ." main.go

运行
sudo ./datacollect_cpp