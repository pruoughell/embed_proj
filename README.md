1.系统概述

2.硬件部分esp32-cam

2.1 aiThinker esp32-cam引脚图

![](media/1d2f7a1d519c69c0a9777bac468bd23a.png)

2.2 esp32-cam上传程序

需要使用usb转串口模块 ch340。

U0R，U0T分别连接串口的TXD，RXD。上传程序时将GPIO0接地。

![](media/f7a7cea4a8e101731b829591940f9a43.png)

2.3esp32-cam运行程序

GPIO0连接摄像头XCLK，下载完成时悬空IO0。

![](media/758a1e7370dd96dd8a6ab87468b78e07.png)

3.程序设计

3.1环境配置arduino ide

3.1.1在arduino官网下载并安装arduino ide。

<https://www.arduino.cc/en/main/software>

3.1.2配置开发板

文件-\>首选项，在附加开发板管理器中添加网址：

<https://dl.espressif.com/dl/package_esp32_index.json>

添加完成后，工具-\>开发板-\>开发板管理器，安装esp32。

安装完后选择对应开发板AI Thinker ESP32-CAM。

3.2发送邮件

3.2.1 esp32 WiFi模块

3.2.1.1 AP与STA简介

AP模式指无线接入点，创建一个无线网络的模式。

ESP32在STA模式下工作就可以连接路由器发出的WiFi。

3.2.1.2 STA模式相关函数

3.2.1.2.1 WiFi.begin(const char\* ssid, const char\* passphrase)

函数用于启动esp32的STA无线终端模式。

参数ssid用于设置需要连接的wifi名称，passphrase用于设置需要连接的wifi的密码。

3.2.1.2.2 WiFi.localIP()

返回当前无线终端的IP地址

3.2.1.2.3 WiFi.status()

返回ESP32连接状态。

返回值如下：

0：WL_IDLE_STATUS正在WiFi工作模式间切换；

1：WL_NO_SSID_AVAIL无法访问设置的SSID网络；

2：WL_SCAN_COMPLETED扫描完成；

3：WL_CONNECTED连接成功；

4：WL_CONNECT_FAILED连接失败；

5：WL_CONNECTION_LOST丢失连接；

6：WL_DISCONNECTED断开连接；

255：WL_NO_SHIELD不用在意（兼容WiFi Shield而设计）

3.2.1.3 esp32以STA模式连接WiFi

Serial.print("Connecting to AP");

WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

while (WiFi.status() != WL_CONNECTED)

{

Serial.print(".");

delay(200);

}

Serial.println("");

Serial.println("WiFi connected.");

Serial.println("IP address: ");

Serial.println(WiFi.localIP());

Serial.println();

3.2.2使用163的SMTP服务发邮件

3.2.2.1POP3与IMAP区别

POP3协议允许电子邮件客户端下载服务器上的邮件，但是在客户端的操作（如移动邮件、标记已读等）不会反馈到服务器上。而IMAP提供webmail与电子邮件客户端之间的双向通信，客户端的操作都都会反馈到服务器上，对邮件进行的操作，服务器上的邮件也会做相应的动作。

3.2.2.2 SMTP介绍

Simple Mail Transfer Protocol，简单邮件传输协议，是一组用于从源地址到目的地址传输邮件的规范，控制邮件的中转方式。SMTP服务器就是遵循SMTP协议的发送邮件服务器。

3.2.2.3 设置163邮箱

登录163邮箱，开启IMAP/SMTP服务，保存授权码。

3.2.2.4配置

邮件服务器为smtp.163.com，端口号为465 (SSL加密端口，非SSL端口为25）。

密码为授权码。

//网易的SMTP服务器

\#define SMTP_HOST "smtp.163.com"

//端口号

\#define SMTP_PORT 465

//发送的邮箱

\#define AUTHOR_EMAIL "\<puoughell@163.com\>"

//授权码

\#define AUTHOR_PASSWORD "\<KUBMWLOFJOGGAQSO\>"

//接收的邮箱

\#define RECIPIENT_EMAIL "\<2064617582@qq.com\>"

3.2.2.5 调用的函数

3.2.2.5.1 MailClient.sendMail(&smtp,&message)

通过SMTP服务器发送邮件，邮件的内容在类SMTP_Messge的实例message中。SMTP会话为类SMTPSession的实例smtp。

3.2.2.5.2 message.addInlineImage(att)

向信息中加入image图像，att为SMTP_Attachment类的实例。

3.2.3发送邮件流程图

![](media/413cd87d90666c837482e94e9aa1b90f.png)

3.3获取图像

3.3.1摄像机初始化

3.3.2获取图像

3.3.3图片格式转化

3.4人脸检测

3.5舵机控制

3.6程序流程图

4.实现效果

4.1人脸检测与邮件发送

4.2人脸定位与舵机控制

5.开发历程

5.1vscode+esp-idf环境配置

5.2openCV移植到esp32TTGO

5.2TTGO实现人脸检测

5.3tensflow lite实现人脸检测

## 
