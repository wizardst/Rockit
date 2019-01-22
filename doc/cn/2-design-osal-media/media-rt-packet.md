# 媒体原语层: RTPacket

所有媒体插件的输入和输入均为RTMediaBuffer。典型的，插件{解复用器}的输出，插件{编码器}的输出和插件{解码器}的输入都是RTMediaBuffer；他们的共同点是这些插件的RTMediaBuffer均可以和RTPacket相互转换。他们的内部数据传递其实就是RTPacket。

插件RTPacket数据交互最佳实践:

- 插件之间数据传递使用RTMediaBuffer，将内部私有数据(如:RTPacket)转换为RTMediaBuffer, 转换时请使用封装好的{转换函数}以减少代码量和出错概率。
- 插件内部，将RTMediaBuffer转换为内部私有数据(如:RTPacket), 数据彻底具象化，减少数据理解的隔阂。

涉及文件: src/rt_media/include/RTCodecData.h
转换函数: RT_RET rt_utils_pkt_to_meta(RTPacket* rt_pkt, RtMetaData* meta);
转换函数: RT_RET rt_utils_meta_to_pkt(RtMetaData* meta, RTPacket* rt_pkt);
释放函数: RT_RET rt_utils_packet_free(RTPacket* rt_pkt);

特别说明一下RTPacket的释放函数, 由于RTPacket在具体实施中，RTPacket并不清楚如何释放它承载的内存，所以需要外挂的RTPacket的释放函数，以保证RTPacket的正确释放。

``` c++
typedef INT32 (*RT_RAW_FREE)(void*);

typedef struct _RTPacket {
    INT64    mPts;
    INT64    mDts;
    INT64    mPos;
    INT32    mSize;
    INT32    mTrackIndex;
    INT32    mFlags;
    uint8_t *mData;
    void*    mRawPkt;
    RT_RAW_FREE mFuncFree;
} RTPacket;

/*utils for rt_packet*/
RT_RET rt_utils_packet_free(RTPacket* rt_pkt);
RT_RET rt_utils_pkt_to_meta(RTPacket* rt_pkt, RtMetaData* meta);
RT_RET rt_utils_meta_to_pkt(RtMetaData* meta, RTPacket* rt_pkt);
```

## 相关原语类

- RTPacket <-trans-> RTMediaBuffer
- RTFrame  <-trans-> RTMediaBuffer