# zerobase-master

## 介绍  
zerobase-master 是Windsor蓝牙板固件工程，它是手机与高压氧舱设备，桑拿设备,冷水浴缸设备，红外床设备的中间连接通道，手机app通过Windsor蓝牙板
可以对高压氧舱设备，桑拿设备，冷水浴缸设备，红外床设备进行控制和数据监视。 Windsor板使用的蓝牙主芯片为Nordic Nrf52840,芯片SDK版本是nRF5_SDK_17.0.2，
使用的开发工具是Keil MDK5,


## 支持的设备
- 高压氧舱
- 冷水浴缸
- 桑拿
- 红外床

## Developing prerequisites
-  官网：https://www.keil.com/
-  Keil MDK 下载链接：https://developer.arm.com/Tools%20and%20Software/Keil%20MDK
-  芯片pack包下载地址: http://www.keil.com/dd2/Pack/
-  Nordic 52840芯片pack包下载地址 https://developer.nordicsemi.com/nRF5_SDK/pieces/nRF_DeviceFamilyPack/NordicSemiconductor.nRF_DeviceFamilyPack.8.44.1.pack
-  nrfutil.exe 下载地址: https://www.nordicsemi.com/Products/Development-tools/nRF-Util/Download?lang=en#infotabs
-  nrfutil.exe 是 Nordic Semiconductor 提供的官方命令行工具，主要用于 nRF52、nRF53、nRF54 和 nRF91 系列芯片的 固件管理、设备编程


## Project File Structure
-   /app 
	-   config
	    -    arm5_no_packs  该目录下保存了keil 工程文件，使用keil 打开该工程目录下的工程文件OneBase_Nrf52840_s140.uvprojx 可以对固件工程进行编译
		-    arggcc         该目录下是使用gcc 编译器是的MakeFile文件，暂时没有使用
    -   src
	    -    projects
		     -    WAP-2-411 Oxford      高压氧舱设备相关的代码文件
				  -        app          包含了与空调以及传感器通讯协议的代码文件，手机app与高压氧舱设备相关部分的蓝牙通讯协议解析代码文件 
				  -        bsp          与高压氧舱相关的IO 控制代码,蓝牙板设备的通讯参数配置文件
 			 -    WAP-2-431 Sauna       桑拿设备相关的代码文件
				  -        app          桑拿控制的涂鸦通讯协议解析代码,手机app与桑拿设备相关的蓝牙通讯协议解析代码文件 
				  -        bsp          复位按键与运行指示灯控制代码,蓝牙板设备的通讯参数配置文件
			 -    WAP-2-411 Edinburgh   冷水浴缸设备相关的代码文件
				  -        app          该目录下包含了与冷水浴缸控制的涂鸦通讯协议解析代码,手机app与冷水浴缸设备相关的蓝牙通讯协议解析代码文件 
				  -        bsp          复位按键与运行指示灯控制代码,蓝牙板设备的通讯参数配置文件
 			 -    WAP-2-451 Bristol   红外床设备相关的代码文件
 				  -        app          红外床控制的涂鸦通讯协议解析代码,手机app与红外床设备相关的蓝牙通讯协议解析代码文件 
				  -        bsp          复位按键与运行指示灯控制代码,蓝牙板设备的通讯参数配置文件
		-	cmds          执行app设置命令的状态机代码，主动上传数据给app的命令代码 
		-   config                      蓝牙sdk 配置文件，控制器类型配置定义文件
		-   efsm                        fsm状态机实现代码
		-   session                     session 命令相关代码实现文件
		-   session_sharing             session 分享相关代码的实现文件
		-   tuya_cmd_management         涂鸦协议命令收发管理状态机代码实现文件
		-   user_app                    main 主程序，与手机app 蓝牙通讯相关的实现文件，包含广播处理，nrf52840 芯片falsh 读写，蓝牙uiid服务建立等应用相关功能
		-   user_ble                    实现蓝牙service ctcs  的代码文件
		-   user_bsp                    实现蓝牙芯片uart 通讯功能
		-   user_drivers                实现了看门狗功能
-   /bin                      执行脚本命令generate 生成固件烧录文件和升级文件 
-   /bootloader               Nordic官方提供的bootloader文件，实现蓝牙DFU升级功能
	-   project	
		-    arm5_no_packs  该目录下保存了bootloader keil 工程文件，使用keil 打开该工程目录下的工程文件secure_bootloader_ble_s140_pca10056.uvprojx 可以对固件工程进行编译
		-    arggcc         该目录下是使用gcc 编译器是的MakeFile文件，

	-   src   bootloader 主程序
-   /doc      与固件相关的一些文档资料
-   /external
	-   NordicSdk_17_0_2    Nordic SDK 相关文件
	
## 固件工程编译
在keil IDE中，使用菜单功能Project/Open Project ,打开 /app/project/arm5_no_packs 目录下的工程文件，在IDE 工具条上的Select Target 下拉选择框中可以选择要编译的目标工程（高压氧舱,
桑拿，冷水浴缸，红外床),选择工具栏上的Build就可以对工程进行编译

## bootloader编译
在keil IDE中，使用菜单功能Project/Open Project ,打开 /bootloader/project/arm5_no_packs 目录下的工程文件，选择工具栏上的Build就可以对工程进行编译	

## 烧录文件与升级文件的生成
将 /bootloader/project/arm5_no_packs/_build 目录下的bootloader.hex 拷贝到/bin 目录下(只需拷贝一次，除非重新修改编译了bootloader)，
双击generate命令就可以生成烧录文件(all.hex)和升级文件(test.zip)
注意: 运行generate命令，需要提前安装nrfutil.exe,前面已经给出了nrfutil.exe的下载地址
	
		
 



