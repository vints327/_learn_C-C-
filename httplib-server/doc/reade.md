// g++ E:\repository_code-source_C_C++\_learn\http-server\main.cpp -o main -I"D:/vcpkg/installed/x64-windows/include" -lws2_32


## mvc 编译命令
g++ "main.cpp" "View.cpp" "Controller.cpp" "Model.cpp" -o "main" -I "D:\vcpkg\installed\x64-windows\include" -L "D:\vcpkg\installed\x64-windows\lib" -lws2_32


g++ "ping.cpp"  -o "main" -I "D:\vcpkg\installed\x64-windows\include" -L "D:\vcpkg\installed\x64-windows\lib" -lws2_32
g++ "ping.cpp" -o "ping" -I "D:\vcpkg\installed\x64-windows\include" -L "D:\vcpkg\installed\x64-windows\lib" -lws2_32 -limp