# Rockit的音频架构

## 1 音频架构

- 解复用插件RTNodeDemuxer将解析出原始音频数据包，并沿着数据管线将其发送到音频解码插件RTNodeAudioCodec。
- 音频解码插件RTNodeAudioCodec需要支持ffmpeg和bolby ddp两套音频解码的实现
- 音频解码插件RTNodeAudioCodec需要内部包含AudioSetting，支持音频参数设置，比如解码输出还是源码输出
- 音频解码插件RTNodeAudioCodec解码出音频包，并沿着数据管线将其发送到音频渲染插件RTNodeAudioSink。
- 音频渲染插件RTNodeAudioSink支持 linux/alsa linux/hdmi windows/wasapi等平台相关接口，支持HDMI源码输出。

## 2 音频架构实施建议

### 2.1 解复用插件RTNodeDemuxer

音频系统不用单独实施，使用基于ffpeg的FFNodeDemuxer即可。FFNodeDemuxer出来的数据包需要区分是音频还是视频包。

### 2.2 音频解码插件RTNodeAudioCodec

- 音频解码插件RTNodeAudioCodec建议支持ffmpeg和bolby ddp两套音频解码的实现。
- ffmpeg的音频软解实现目前已经支持。bolby ddp的音频软解需要开发
- 音频解码插件RTNodeAudioCodec需要内部包含AudioSetting，支持音频参数设置，比如解码输出还是源码输出

### 2.3 音频渲染插件RTNodeAudioSink

- 音频渲染插件RTNodeAudioSink支持linux/alsa linux/hdmi windows/wasapi等平台相关接口
- 音频渲染插件RTNodeAudioSink 解码输出和源码输出。

## 3 音频架构的数据结构

Layer-Plugin:

- [RTNode]           插件通用API,
- [RTNodeAudioCodec] 建议支持ffmpeg和bolby ddp两套音频解码的实现。
- [RTNodeAudioSink]  继承并实现RTNode的通用API，内部根据系统类型实例化具体的AudioSink

Layer-Media:

- [RTAudioSink]           封装AudioSink的通用API,注意只能使用gcc的头文件
- [linux/RTAlsaSink]      继承并实现AudioSink的通用API，内部使用alsa音频接口
- [windows/RTWASAPISink]  继承并实现AudioSink的通用API，内部使用WASAPI音频接口
- [RTAudioSetting]        封装AudioSetting的通用API

## 4 相关概念

### 4.1 Advanced Linux Sound Architecture(ALSA)

ALSA为Linux操作系统提供音频和MIDI功能,具有以下重要功能：

- 高效支持所有类型的音频接口，从消费者声卡到专业的多声道音频接口。
- 完全模块化的声音驱动程序。
- SMP和线程安全设计(请仔细阅读)。
- 用户空间库(alsa-lib)简化应用程序编程并提供更高级别的功能。
- 支持较旧的开放式声音系统OSS API，为大多数OSS程序提供二进制兼容性。

### 4.2 Windows Audio Session API(WASAPI)

WASAPI使客户端应用程序能够管理应用程序和音频端点设备之间的音频数据流。每个音频流都是音频会话的成员。通过会话抽象，WASAPI客户端可以将音频流识别为一组相关音频流的成员。系统可以将会话中的所有流作为单个单元进行管理。音频引擎是用户模式音频组件，应用程序通过该组件共享对音频端点设备的访问。音频引擎在端点缓冲区和端点设备之间传输音频数据。为了通过渲染端点设备播放音频流，应用程序定期将音频数据写入渲染端点缓冲区。音频引擎混合来自各种应用程序的流。为了记录来自捕获端点设备的音频流，应用程序定期从捕获端点缓冲区读取音频数据。

### 4.3 解码输出和源码输出。

解码输出包括: pcm, multi-pcm

源码输出包括: HMDI的杜比/DTS源码输出。源码输出需要源码包头，部分解复用器出来的音频包需要添加源码包头后，才能源码输出。

## 5 参考资料

- [Advanced Linux Sound Architecture(ALSA)](https://www.alsa-project.org/)
- [Windows Audio Session API(WASAPI)](https://docs.microsoft.com/en-us/windows/desktop/CoreAudio/wasapi)
