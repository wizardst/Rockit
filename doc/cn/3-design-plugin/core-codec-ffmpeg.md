# 核心插件: ffmpeg编解码器

- FFNodeCodec 定位于基于ffmpeg的Codec媒体组件
- FFNodeCodec 继承插件基类RTNode，插件基类RTNode定义了一组插件接口；
- FFNodeCodec 继承媒体基类RTCodec，媒体基类RTCodec定义了一组解码器接口；
- FFNodeCodec 通过调用 ffmpeg-adapter的接口实现具体功能；

## FFNodeCodec的接口

``` c++
class FFNodeCodec {

};
```

## ffmpeg-av-codec的接口

``` c++

```

## 常用插件调用关系

- 调用关系： RTNode/RTDemuxer --> FFNodeDemuxer--> ffmpeg-adapter --> ffmpeg-native
- 调用关系： RTNode/RTMuxer   --> FFNodeMuxer  --> ffmpeg-adapter --> ffmpeg-native
- 调用关系： RTNode/RTCodec   --> FFNodeCodec  --> ffmpeg-adapter --> ffmpeg-native
- 调用关系： RTNode/RTCodec   --> FFNodeMPP    --> vpuapi         --> mpp-native
- 调用关系： RTNode/RTFilter  --> FFNodeFilter --> ffmpeg-adapter --> ffmpeg-native