# 核心API: 编解码器通用API
- RTNodeCodec定位为codec的核心实现API；
- 所有类型的codec需要基于该类接口实现拓展；

## 编解码器调用关系
```

                                                |------>FFNodeDecoder-----> |
                    |-----> ffmpegCodec ------> |                           | ----> FFAdapterCodec ----> ffmpeg API
                    |                           |------>FFNodeEncoder-----> |
                    |
                    |
RTNodeCodec---|     |
              |     |                           | -----> MpiDecoder ------> |
              |---> |--------> mpi -----------> |                           | ----> MpiAdapterCodec ---> mpi
              |     |                           | -----> MpiEncoder ------> |
RTNode--------|     |
                    |
                    |
                    |                           |---> MediaCodecEncoder---> |                           |-----> codec2.0
                    |-----> google codec api--> |                           | ----> MCAdapterCodec ---> |
                                                |---> MediaCodecDecoder---> |                           |-----> OMX
```

## RTNodeCodec的接口设计

``` c++
class RTNodeCodec {
    typedef enum {
        RT_PORT_INPUT,
        RT_PORT_OUTPUT,
        RT_PORT_BUTT,
    } RTPortType;

    enum {
        CB_INPUT_AVAILABLE              = 1,
        CB_OUTPUT_AVAILABLE             = 2,
        CB_ERROR                        = 3,
        CB_OUTPUT_FORMAT_CHANGED        = 4,
        CB_RESOURCE_RECLAIMED           = 5,
    };

    virtual RT_RET start() = 0;
    virtual RT_RET stop() = 0;
    virtual RT_RET reset() = 0;
    virtual RT_RET flush() = 0;

    virtual RT_RET configureCodec(RtMetaData *meta) = 0;

    virtual RT_RET dequeue(RTMediaBuffer **buffer, RTPortType type) = 0;
    virtual RT_RET enqueue(RTMediaBuffer *buffer, RTPortType type) = 0;

    virtual RT_RET getFormat(RtMetaData **meta, RTPortType type) = 0;
    virtual RT_RET setCallBack(RTMessage *callback) = 0;
};
```
```
RT_PORT_INPUT: 定义此时为输入端口。
RT_PORT_OUTPUT: 定义此时为输出端口。
```
```
消息上报的类型：
CB_INPUT_AVAILABLE: 输入端口有可用的buffer回调，产生此回调信息时，dequeue(&buffer, RT_PORT_INPUT)调用必定可以获得buffer。
CB_OUTPUT_AVAILABLE：输出端口有可用的buffer回调，产生此回调信息时，dequeue(&buffer, RT_PORT_OUTPUT)调用必定可以获得buffer。
CB_ERROR： 解码器出错产生的回调， 具体的错误信息由msg带回。
CB_OUTPUT_FORMAT_CHANGED： 在编解码器输出信息与当前编解码器条件不符时的上报信息， 产生此回调时， 需要根据信息的改变做出措施。
CB_RESOURCE_RECLAIMED： 对外资源需要回收时的上报。
```
```
RT_RET start()
编解码器开始工作。
返回值： 函数执行的正确性，正常返回RT_OK；

RT_RET stop()
编解码器停止工作。
返回值： 函数执行的正确性，正常返回RT_OK；

RT_RET reset()
重置编解码器。
返回值： 函数执行的正确性，正常返回RT_OK；

RT_RET flush()
刷新编解码器。
返回值： 函数执行的正确性，正常返回RT_OK；

RT_RET configureCodec(RtMetaData *meta)
动态配置编解码器的参数。
返回值： 函数执行的正确性，正常返回RT_OK；
参数1： meta: 需要被配置的参数合集；

RT_RET dequeue(RTMediaBuffer **buffer, RTPortType type)
获得编解码的buffer。
返回值： 函数执行的正确性，正常返回RT_OK；
参数1： buffer： 获得到的Buffer指针的地址；
参数2： type: 获得的buffer的端口，INPUT或者OUTPUT；

RT_RET enqueue(RTMediaBuffer **buffer, RTPortType type)
还回编解码的buffer。
返回值： 函数执行的正确性，正常返回RT_OK；
参数1： buffer： 获得到的Buffer指针的地址；
参数2： type: 获得的buffer的端口，INPUT或者OUTPUT；

RT_RET getFormat(RtMetaData **meta, RTPortType type)
获得端口信息。
返回值： 函数执行的正确性，正常返回RT_OK；
参数1： meta: 需要获得的信息指针的地址；
参数2： type: 端口类型；

RT_RET setCallBack(RTMessage *callback)
设置消息上报的监听。
返回值： 函数执行的正确性，正常返回RT_OK；
参数1： callback: 消息上报的监听函数；

```

