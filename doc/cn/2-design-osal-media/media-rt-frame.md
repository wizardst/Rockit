# 媒体原语层: RTFrame

所有媒体插件的输入和输入均为RTMediaBuffer。典型的，插件{解码器}的输出，插件{编码器}的输入和插件{VideoSink}的输入都是RTMediaBuffer；他们的共同点是这些插件的RTMediaBuffer均可以和RTFrame相互转换。他们的内部数据传递其实就是RTFrame。插件数据交互最佳实践。

- 插件之间数据传递使用RTMediaBuffer，将内部私有数据(如:RTFrame)转换为RTMediaBuffer, 转换时请使用封装好的{转换函数}以减少代码量和出错概率。
- 插件内部，将RTMediaBuffer转换为内部私有数据(如:RTFrame),数据彻底具象化，减少数据理解的隔阂。

涉及文件: src/rt_media/include/RTCodecData.h
转换函数: RT_RET rt_utils_frame_to_meta(RTFrame* rt_frame, RtMetaData* meta);
转换函数: RT_RET rt_utils_meta_to_frame(RtMetaData* meta, RTFrame* rt_frame);
释放函数: RT_RET rt_utils_frame_free(RTFrame* rt_frame);

特别说明一下RTFrame的释放函数, 由于RTFrame在具体实施中，RTFrame并不清楚如何释放它承载的内存，所以需要外挂的RTFrame的释放函数，以保证RTFrame的正确释放。

``` c++
typedef INT32 (*RT_RAW_FREE)(void*);
typedef struct _RTFrame {
    INT64    mPts;                /* with unit of us*/
    INT64    mDts;               /* with unit of us*/
    uint8_t *mData;
    INT32    mSize;
    UINT32   mWidth;
    UINT32   mHeight;
    UINT32   mFormat;
    UINT32   mQuality;
    UINT32   mKeyFrame;
    enum RTImageType  mImgType;
    enum RTFieldOrder mFieldOrder;
    RT_RAW_FREE mFuncFree;
} RTFrame;

/*utils for rt_frame*/
RT_RET rt_utils_frame_free(RTFrame* rt_frame);
RT_RET rt_utils_frame_to_meta(RTFrame* rt_frame, RtMetaData* meta);
RT_RET rt_utils_meta_to_frame(RtMetaData* meta, RTFrame* rt_frame);
```

## 相关原语类

- RTPacket <-trans-> RTMediaBuffer
- RTFrame  <-trans-> RTMediaBuffer