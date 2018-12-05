# Rockit Documents [[Chinese Version]](./README-cn.md)

## 1 Version Management

- [Best Practice for Git and Gerrit Version](./en/v11-git-gerrit-version.md)

## 2 Design Practice

## 2.1 Design of OSAL Primitive Layer

- [Memory Manager](./en/2-design-osal-media/osal-memory-manager.md)
- [Common Container(HashTable etc.)](./en/2-design-osal-media/osal-common-container.md)
- [RTThread and RTMutex](./en/2-design-osal-media/osal-thread-and-mutex.md)

## 2.2 Design of Media Primitive Layer

### Base media

- [FFMPEG Wrapper](./en/2-design-osal-media/media-ffmpeg-api.md)
- [RTFrame](./en/2-design-osal-media/media-rt-frame.md)
- [RTPacket](./en/2-design-osal-media/media-rt-packet.md)
- [Message Queue](./en/2-design-osal-media/media-event-queue.md)

### Extensive media

- [HDMI Manager](./en/2-design-osal-media/media-hdmi-manager.md)
- [System Display](./en/2-design-osal-media/media-system-display.md)
- [GLES Display](./en/2-design-osal-media/media-gles-display.md)

## 2.3 Design of Plugin Layer

- [Arch: Plugin Architecture](./en/3-design-plugin/plugin-arch.md)
- [Arch: Plugin manager and PIPLINE](./en/3-design-plugin/plugin-manager.md)
- [CorePlugin: ffmpeg demuxer](./en/3-design-plugin/core-demuxer-ffmpeg.md)
- [CorePlugin: Rockit demuxer for encrypted](./en/3-design-plugin/core-demuxer-rt-mp4.md)
- [CorePlugin: ffmpeg muxer](./en/3-design-plugin/core-muxer-ffmpeg.md)
- [CorePlugin: ffmpeg codec](./en/3-design-plugin/core-codec-ffmpeg.md)
- [CorePlugin: mpp codec](./en/3-design-plugin/core-codec-vpuapi.md)
- [CorePlugin: Resample](./en/3-design-plugin/core-resample.md)
- [SinkPlgin: system audio render](./en/3-design-plugin/sink-audio-system.md)
- [SinkPlgin: ALSA audio render](./en/3-design-plugin/sink-audio-alsa.md)
- [SinkPlgin: system video render](./en/3-design-plugin/sink-video-system.md)
- [SinkPlgin: GLES video render](./en/3-design-plugin/sink-video-gles.md)
- [SinkPlgin: video quality feekback](./en/3-design-plugin/sink-qos-stat.md)

## 2.4 Design of Integration Layer

- [Android Player](./en/4-design-player/android-player.md)
- [IPTV Player](./en/4-design-player/iptv-player.md)

## 3 Code Style Guide

- [Style guides for Google-originated open-source projects](https://github.com/google/styleguide)

``` shell
# Check Code Styles with cpplint(!Recommended)
$ cd Rockit
$ ./tools/cpplint/check_style.sh

# Check Code Styles with astyle(!NOT Recommended)
$ cd Rockit/tools/AStyle
$ chmod 777 astyle
$ ./tools/rt_astyle.sh
```

## 4 Samples

- ToDo
- ToDo

## References

- [Style guides for Google-originated open-source projects](https://github.com/google/styleguide)
