# 核心插件：ffmpeg 复用器

- FFNodeMuxer 定位于基于ffmpeg的Demuxer媒体组件
- FFNodeMuxer 继承插件基类RTNode，插件基类RTNode定义了一组插件接口；
- FFNodeMuxer 继承媒体基类RTMuxer，媒体基类RTMuxer定义了一组解码器接口；
- FFNodeMuxer 通过调用 ffmpeg-adapter的接口实现具体功能；

## RTCodec的接口

``` c++
class RTMuxer {

};
```

## ffmpeg-av-muxer的接口

``` c++

```

## 常用插件调用关系

- 调用关系： RTNode/RTDemuxer --> FFNodeDemuxer--> ffmpeg-adapter --> ffmpeg-native
- 调用关系： RTNode/RTMuxer   --> FFNodeMuxer  --> ffmpeg-adapter --> ffmpeg-native
- 调用关系： RTNode/RTCodec   --> FFNodeCodec  --> ffmpeg-adapter --> ffmpeg-native
- 调用关系： RTNode/RTCodec   --> FFNodeMPP    --> vpuapi         --> mpp-native
- 调用关系： RTNode/RTFilter  --> FFNodeFilter --> ffmpeg-adapter --> ffmpeg-native