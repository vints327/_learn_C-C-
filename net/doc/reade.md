

g++ "${workspaceFolder}/net/icmp-test.cpp" -o "${workspaceFolder}/net/icmp-test" -I D:/vcpkg/installed/x64-windows/include -L D:/vcpkg/installed/x64-windows/lib -lws2_32 -lwpcap -lPacket


 g++ "icmp-test.cpp" -o "icmp-test" -I D:/vcpkg/installed/x64-windows/include -L D:/vcpkg/installed/x64-windows/lib -I E:/repository/_code-source/_C_C++/_learn/npcap-sdk-1.13/Include -L E:/repository/_code-source/_C_C++/_learn/npcap-sdk-1.13/Lib/x64 -lws2_32 -lwpcap -lPacket


 g++ "icmp-main.cpp" "icmp-utils.cpp" "packet-handler.cpp" -o "npcap-demo" -I D:/vcpkg/installed/x64-windows/include -L D:/vcpkg/installed/x64-windows/lib -I E:/repository/_code-source/_C_C++/_learn/npcap-sdk-1.13/Include -L E:/repository/_code-source/_C_C++/_learn/npcap-sdk-1.13/Lib/x64 -lws2_32 -lwpcap -lPacket



  g++ "capture.cpp"  -o "capture" -I D:/vcpkg/installed/x64-windows/include -L D:/vcpkg/installed/x64-windows/lib -I E:/repository/_code-source/_C_C++/_learn/npcap-sdk-1.13/Include -L E:/repository/_code-source/_C_C++/_learn/npcap-sdk-1.13/Lib/x64 -lws2_32 -lwpcap -lPacket