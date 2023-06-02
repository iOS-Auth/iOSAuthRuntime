# iOS-Authorize-Runtime

[![996.icu](https://img.shields.io/badge/link-996.icu-red.svg)](https://996.icu)

*用于给指定设备进行帐号授权的运行时库*


# 说明
*此工程是[iOS-Auth](https://github.com/iOS-Auth)的基础运行时支持库*

*[iOSAuthSingle](https://github.com/iOS-Auth/iOSAuthSingle)用于通过套接字授权单个设备(主要用于调试)*

*[iOSAuthService](https://github.com/iOS-Auth/iOSAuthService)用于通过套接字监听授权数据并创建管道及[iOSAuthHandle](https://github.com/iOS-Auth/iOSAuthHandle)进行授权(主要用于防止运行时错误及内存泄漏)*

*[iOSAuthHandle](https://github.com/iOS-Auth/iOSAuthHandle)用于通过管道连接[iOSAuthService](https://github.com/iOS-Auth/iOSAuthService)进行单个设备授权*


# 模块
*Library:          扫描当前设备的iTunes库并加载*
*Handle:           根据iTunes版本设置指定的偏移地址并调用*
*Network:          通过套接字或管道与客户端或服务器通信*
*Convert:          十六进制转换*
*Process:          进程执行函数封装*
*Exec:             执行入口*


# 通信流程
*客户端: 第一次数据，格式：前4字节udid长度，再4字节FairPlayCertificate长度，再加GrappaSupportInfo长度为0x14，再加FairPlayArg长度0x10，再加udid，再加FairPlayCertificate数据*

*服务端: 第一次数据，长度是固定的0x54 (84)字节(Grappa)*

*客户端: 第二次数据，前0x53个字节为Grappa数据，再跟0x15字节为afsync.rq.sig，再跟afsync.rq*

*服务端: 第二次数据，格式：前面15个字节为/AirFair/sync/afsync.rs.sig，后面的字节为/AirFair/sync/afsync.rs*


# 运行流程
*加载iTunes.dll/iTunesCore.dll以及AirTrafficHost.dll*

*根据iTunes.dll/iTunesCore.dll版本找到指定的偏移地址（欢迎补充其它版本的偏移地址）*

*调用 KBSyncMachineKey 获取本机的同步ID*

*调用 KBSyncLibraryKey 获取库的同步ID*

*调用 KBSyncTokenCreate 获取同步Token，这里的 SCInfoDir 下必须要求有用iTunes.exe授权过此电脑产生的 SC Info.sidb 和 SC Info.sidd 文件*

*接收第一个客户端的包，并拆分 UDID、FairPlayCertificate、FairPlayDeviceType 以及 KeyTypeSupportVersion。具体请看 auth_exec_recv_package_1*

*通过得到的数据调用 AirFairSyncSessionCreate 获得同步Session*

*使用 UDID 调用 AirFairSyncGrappaCreate 获得 Grappa 数据及ID*

*将 Grappa 发送至客户端。具体请看 auth_exec_send_package*

*接收第二个客户端的包，并拆分 Grappa、/AirFair/sync/afsync.rq 以及 /AirFair/sync/afsync.rq.sig。具体请看 auth_exec_recv_package_2*

*调用 AirFairSyncSetRequest 设置 RQ*

*调用 AirFairSyncAccountAuthorize 给此请求授权，DSID由使用者自己抓包获取，AppleId会绑定到一个DSID，DSID需要和 SC Info 相匹配*

*使用设备传回的 Grappa 调用 AirFairSyncGrappaUpdate 去更新随机密钥*

*调用 AirFairSyncGetResponse 获取答复*

*通过前面获取的 GrappaID 调用 AirFairSyncCalcSig 去计算 SIG签名*

*将生成的数据发回客户端设备*

