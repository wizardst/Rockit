# 核心插件: mpp/vpuapi编解码器

- FFNodeMPP 定位于基于mpp的Codec媒体组件
- FFNodeMPP 继承插件基类RTNode，插件基类RTNode定义了一组插件接口；
- FFNodeMPP 继承媒体基类RTCodec，媒体基类RTCodec定义了一组解码器接口；
- FFNodeMPP 通过调用 vpu-api的接口实现具体功能；

## RTCodec的接口

``` c++
class RTCodec {

};
```

## vpuapi-Codec的接口

``` c++
struct VpuCodecContext {
    void* vpuApiObj;
    OMX_RK_VIDEO_CODINGTYPE videoCoding;
    RK_S32 (*init)(struct VpuCodecContext *ctx, RK_U8 *extraData, RK_U32 extra_size);
    RK_S32 (*decode)(struct VpuCodecContext *ctx, VideoPacket_t *pkt, DecoderOut_t *aDecOut);
    RK_S32 (*encode)(struct VpuCodecContext *ctx, EncInputStream_t *aEncInStrm, EncoderOut_t *aEncOut);
    RK_S32 (*flush)(struct VpuCodecContext *ctx);
    RK_S32 (*control)(struct VpuCodecContext *ctx, VPU_API_CMD cmdType, void* param);
    RK_S32 (*decode_sendstream)(struct VpuCodecContext *ctx, VideoPacket_t *pkt);
    RK_S32 (*decode_getframe)(struct VpuCodecContext *ctx, DecoderOut_t *aDecOut);

    RK_S32 (*encoder_sendframe)(struct VpuCodecContext *ctx,  EncInputStream_t *aEncInStrm);
    RK_S32 (*encoder_getstream)(struct VpuCodecContext *ctx, EncoderOut_t *aEncOut);
};

RK_S32 vpu_open_context(struct VpuCodecContext **ctx);
RK_S32 vpu_close_context(struct VpuCodecContext **ctx);
```

## 常用插件调用关系

- 调用关系： RTNode/RTDemuxer --> FFNodeDemuxer--> ffmpeg-adapter --> ffmpeg-native
- 调用关系： RTNode/RTMuxer   --> FFNodeMuxer  --> ffmpeg-adapter --> ffmpeg-native
- 调用关系： RTNode/RTCodec   --> FFNodeCodec  --> ffmpeg-adapter --> ffmpeg-native
- 调用关系： RTNode/RTCodec   --> FFNodeMPP    --> vpuapi         --> mpp-native
- 调用关系： RTNode/RTFilter  --> FFNodeFilter --> ffmpeg-adapter --> ffmpeg-native