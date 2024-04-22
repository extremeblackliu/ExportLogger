# ExportLogger

Log `GetProcAddress` Get and Call

# Features
- Sequence Number, see example output.
- Call Stack, not enabled by default.
- Global Record, not enabled by default. (BUG)

# Usage
this project used version.dll hijack, put compiled binary to same folder of target executable and launch the executable.

then you should have file in the folder like `Log_[2024-4-23]-[7-7-12].txt`

# TODO
- x64 support
- Fix crash with global record enabled
- Capable to hook manually resolve pe header export table.

# Note

this is not reliable solution if you are using for reverse engineer purpose, at least so far. 

hooking `GetProcAddress` is pointless if they can have own `GetProcAddress` (walking export table through pe header).

# Requirements
- MSVC (Visual Studio 2022 v143)
- Windows

read `dllmain.cpp` to understand what are the defines for

# Example Output
call stack off, global record off.
```
==============================================
ExporLogger made by Extr3lackLiu
https://github.com/extremeblackliu/ExportLogger
==============================================
[*][GET] 7:7:12 export: Create_HP_TcpClientListener [Seq:1]
[*][CALL] 7:7:12 export: Create_HP_TcpClientListener [Seq:1]
[*][GET] 7:7:12 export: Create_HP_TcpPackClient [Seq:2]
[*][CALL] 7:7:12 export: Create_HP_TcpPackClient [Seq:2]
[*][GET] 7:7:12 export: HP_Set_FN_Client_OnReceive [Seq:3]
[*][CALL] 7:7:12 export: HP_Set_FN_Client_OnReceive [Seq:3]
[*][GET] 7:7:12 export: HP_Set_FN_Client_OnClose [Seq:4]
[*][CALL] 7:7:12 export: HP_Set_FN_Client_OnClose [Seq:4]
[*][GET] 7:7:12 export: HP_Client_HasStarted [Seq:5]
[*][CALL] 7:7:12 export: HP_Client_HasStarted [Seq:5]
[*][GET] 7:7:12 export: HP_Client_Start [Seq:6]
[*][CALL] 7:7:12 export: HP_Client_Start [Seq:6]
[*][CALL] 7:7:12 export: Create_HP_TcpClientListener [Seq:1]
[*][CALL] 7:7:12 export: Create_HP_TcpPackClient [Seq:2]
[*][GET] 7:7:12 export: HP_Set_FN_Client_OnConnect [Seq:7]
[*][CALL] 7:7:12 export: HP_Set_FN_Client_OnConnect [Seq:7]
[*][CALL] 7:7:12 export: HP_Set_FN_Client_OnReceive [Seq:3]
[*][CALL] 7:7:12 export: HP_Set_FN_Client_OnClose [Seq:4]
[*][GET] 7:7:12 export: HP_TcpClient_SetKeepAliveInterval [Seq:8]
[*][CALL] 7:7:12 export: HP_TcpClient_SetKeepAliveInterval [Seq:8]
[*][GET] 7:7:12 export: HP_TcpClient_SetKeepAliveTime [Seq:9]
[*][CALL] 7:7:12 export: HP_TcpClient_SetKeepAliveTime [Seq:9]
[*][GET] 7:7:12 export: HP_Client_Send [Seq:10]
[*][CALL] 7:7:12 export: HP_Client_Send [Seq:10]
[*][CALL] 7:7:12 export: HP_Client_HasStarted [Seq:5]
[*][CALL] 7:7:12 export: HP_Client_Start [Seq:6]
[*][CALL] 7:7:12 export: HP_Client_Send [Seq:10]
[*][CALL] 7:7:12 export: HP_Client_HasStarted [Seq:5]
[*][CALL] 7:7:12 export: HP_Client_HasStarted [Seq:5]
[*][GET] 7:7:12 export: HP_Client_Stop [Seq:11]
[*][CALL] 7:7:12 export: HP_Client_Stop [Seq:11]
[*][CALL] 7:7:12 export: HP_Client_HasStarted [Seq:5]
[*][CALL] 7:7:12 export: HP_Client_Start [Seq:6]
[*][CALL] 7:7:12 export: HP_Client_Send [Seq:10]
[*][CALL] 7:7:12 export: HP_Client_Send [Seq:10]
[*][CALL] 7:7:12 export: HP_Client_Send [Seq:10]
```
