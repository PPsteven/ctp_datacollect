复制文件
ln -s ../DataCollect.h DataCollect.h
ln -s ../libDataCollect.so libDataCollect.so

编译
g++ -o datacollect_cpp main.cpp -L. -lDataCollect -Wl,-rpath,.

运行
sudo ./datacollect_cpp