# AudioSink插件设计

## 1 音频的几种类型

### 1.1 Android系统的音频输出（第一优先级）

- AudioSink输出
- 硬件接口层：ALSA-lib/Tinyalsa
- AudioSettingManager管理
- 解码输出（包括multi-pcm）和源码输出（杜比/DTS）

### 1.2 Windows系统的音频输出（第二优先级）

- WASAPI接口

### 1.3 Linux系统的音频输出（第三优先级）

- 硬件接口：ALSA-lib
- 音频重采样
- AudioSettingManager管理

## 2 通用数据结构和架构

Layer-Plugin:

- [RTNode]           插件通用API,
- [RTNodeAudioSink]  继承并实现RTNode的通用API，内部根据系统类型实例化具体的AudioSink

Layer-Media:

- [RTAudioSink]           封装AudioSink的通用API,注意只能使用gcc的头文件
- [linux/RTAlsaSink]      继承并实现AudioSink的通用API，内部使用alsa音频接口
- [windows/RTWASAPISink]  继承并实现AudioSink的通用API，内部使用WASAPI音频接口
- [RTAudioSetting]        封装AudioSetting的通用API

## 3 开发计划

- 优先开发Android解码输出插件和源码输出插件;

- Windows 输出插件；

- Linux 输出插件，有需求后续开发；

- 音频插件测试用例；

## 4 音频关键概念

## 5 音频播放器简易流程图

- 当前解码和源码输出调用方式流程如下：

RTNodeAudioDecoder                RTNodeAudioSink
- AudioSetting
- renderDecodedAudio()    ->    AudioTrack/ALSA/WASAPI
- renderBitsteamAudio()   ->          HDMI

RTAudioDecoder 包含ffmpeg和bolby ddp(bolbyID4.4)两套实现。
AudioSetting 管理音频输出类型，分别走解码输出和源码输出通路；

## 6 音频调试经验（待完善）

参考资料（待完善）
https://www.cnblogs.com/Tty725/p/7368197.html
RockKit-2-项目需求分析.docx

