# 媒体原语层: ffmpeg封装

ffmpeg-adapter封装了ffmpeg原生接口的复杂性，对外提供了简洁的接口; ffmpeg-adapter隔离了ffmpeg原生接口的调用，方便ffmpeg的版本升级。模块调用关系： RTNode/RTDemuxer --> FFNodeDexer --> ffmpeg-adapter --> ffmpeg-native

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