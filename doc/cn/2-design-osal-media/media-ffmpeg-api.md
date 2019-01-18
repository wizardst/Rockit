# 媒体原语层: ffmpeg封装

ffmpeg-adapter封装了ffmpeg原生接口的复杂性，对外提供了简洁的接口; ffmpeg-adapter隔离了ffmpeg原生接口的调用，方便ffmpeg的版本升级。模块调用关系： RTNode/RTDemuxer --> FFNodeDexer --> ffmpeg-adapter --> ffmpeg-native。

## 1 ffmpeg封装的功能块

### 1.1 av_format模块

- 重点封装: ffmepg-av-demuxer
- 重点封装: ffmepg-av-muxer
- 源码文件: src/rt_media/FFMpeg/FFAdapterFormat.h

### 1.2 av_codec模块

- 重点封装: ffmepg-av-codecs
- 重点关注: H264/H265/VP8/VP9/rmvb 软解
- 源码文件: src/rt_media/FFMpeg/FFAdapterCodec.h

### 1.3 av_filter模块

- 重点封装: ffmepg-av-filters
- 重点关注: ffmpeg-resample
- 源码文件: src/rt_media/FFMpeg/FFAdapterFilter.h

### 1.4 ffmpeg封装

- 重点封装: ffmepg-av-utils
- 重点关注: 常用工具api
- 源码文件: src/rt_media/FFMpeg/FFAdapterUtils.h

## 2 ffmpeg和RTNode的关系

- FFNodeDexer继承插件基类RTNode，插件基类RTNode定义了一组功能接口；
- FFNodeDexer另外还需要继承媒体基类RTDemuxer，媒体基类RTDemuxer定义了一组功能接口；
- 基于ffmpeg的Demuxer媒体组件：FFNodeDexer通过调用 ffmpeg-adapter的接口实现具体功能；
- ffmpeg-adapter的接口直接调用原生的ffmpeg接口;
- ffmpeg-adapter封装了ffmpeg原生接口的复杂性，对外提供了简洁的接口;
- ffmpeg-adapter隔离了ffmpeg原生接口的调用，方便ffmpeg的版本升级;
- ffmpeg-adapter的基本原则: 保持简洁性和完全隔离ffmpeg.
- 调用关系： RTNode/RTDemuxer --> FFNodeDexer --> ffmpeg-adapter --> ffmpeg-native

## 3 基于ffmeng的插件建议的模块关系

- 调用关系： RTNode/RTDemuxer --> FFNodeDexer  --> ffmpeg-adapter --> ffmpeg-native
- 调用关系： RTNode/RTCodec   --> FFNodeCodec  --> ffmpeg-adapter --> ffmpeg-native
- 调用关系： RTNode/RTFilter  --> FFNodeFilter --> ffmpeg-adapter --> ffmpeg-native

## 4 ffmpeg的功能模块

- 容器/文件（Container/File）：常见媒体格式的复用和解复用器，诸如: MP4, MKV和HLS等。
- 媒体流(Stream)：表示在时间轴上的一段连续的数据，可以关联特定编码器的视频或音频数据，一般为压缩数据。
- 数据帧/数据包(Frame/Packet)：通常一个媒体流是由大量的数据帧组成的，对于压缩数据，帧对应着编解码器的最小处理单元，分属于不同媒体流的数据帧交错存储与容器之中。
- 编解码器：编解码器是以帧为单位实现压缩数据和原始数据之间的相互转换的。

前面介绍的术语，就是FFmpeg中抽象出来的概念。其中：

- AVFormatContext：就是对容器或者媒体文件层次的抽象。
- AVStream：在文件中（容器里面）包含了多路流（音频流、视频流、字幕流），AVStream 就是对流的抽象。
- AVCodecContext 与 AVCodec：在每一路流中都会描述这路流的编码格式，对编解码器格式以及编解码器的抽象就是AVCodecContext 与 AVCodec。
- AVPacket 与 AVFrame：对于编码器或者解码器的输入输出部分，也就是压缩数据以及原始数据的抽象就是AVPacket与AVFrame。
- AVFilter：除了编解码之外，对音视频的处理肯定是针对于原始数据的处理，也就是针对AVFrame的处理，使用的就是AVFilter。