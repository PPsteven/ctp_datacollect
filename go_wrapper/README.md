g++ -shared -fPIC -o libDataCollectWrapper.so DataCollectWrapper.cpp 
go build -o datacollect_go -ldflags "-r ." main.go 

sudo ./datacollect_go 