# Rockit 文档 [[英文版]](./README.md)

## 1 版本管理

- [Git和Gerrit版本控制最佳实践](./cn/1-version-control/vc-git-gerrit-version.md)
- [Rockit项目C/C++代码风格规范](./cn/1-version-control/vc-code-style-standards.md)

## 2 设计实践

## 2.1 OSAL原语层设计

- [内存管理文档](./cn/2-design-osal-media/osal-memory-manager.md)
- [通用容器(HashTable等)文档](./cn/2-design-osal-media/osal-common-container.md)
- [线程和锁文档](./cn/2-design-osal-media/osal-thread-and-mutex.md)

## 2.2 Media原语层设计

### 基础多媒体数据结构

- [ffmpeg封装文档](./cn/2-design-osal-media/media-ffmpeg-api.md)
- [RTFrame文档](./cn/2-design-osal-media/media-rt-frame.md)
- [RTPacket文档](./cn/2-design-osal-media/media-rt-packet.md)
- [RTMetaData文档](./cn/2-design-osal-media/media-metadata.md)
- [消息队列文档](./cn/2-design-osal-media/media-event-queue.md)

### 扩展多媒体数据结构

- [HDMI管理文档](./cn/2-design-osal-media/media-hdmi-manager.md)
- [系统显示文档](./cn/2-design-osal-media/media-system-display.md)
- [GLES显示文档](./cn/2-design-osal-media/media-gles-display.md)

## 2.3 插件层设计

- [插件机制: 插件架构设计](./cn/3-design-plugin/plugin-arch.md)
- [插件机制: 插件管理和流水线](./cn/3-design-plugin/plugin-manager.md)
- [核心插件: ffmpeg解复用器](./cn/3-design-plugin/core-demuxer-ffmpeg.md)
- [核心插件: Rockit加密流解复用器](./cn/3-design-plugin/core-demuxer-rt-mp4.md)
- [核心插件: ffmpeg复用器](./cn/3-design-plugin/core-muxer-ffmpeg.md)
- [核心插件: ffmpeg编解码器](./cn/3-design-plugin/core-codec-ffmpeg.md)
- [核心插件: mpp编解码器](./cn/3-design-plugin/core-codec-vpuapi.md)
- [核心插件: Resample](./cn/3-design-plugin/core-resample.md)
- [Sink插件: 系统音频渲染](./cn/3-design-plugin/sink-audio-system.md)
- [Sink插件: ALSA音频渲染](./cn/3-design-plugin/sink-audio-alsa.md)
- [Sink插件: 系统视频显示](./cn/3-design-plugin/sink-video-system.md)
- [Sink插件: GLES视频显示](./cn/3-design-plugin/sink-video-gles.md)
- [Sink插件: 视频质量反馈](./cn/3-design-plugin/sink-qos-stat.md)

## 2.4 集成层设计

- [AndroidPlayer设计文档](./cn/4-design-player/android-player.md)
- [IPTVPlayer设计文档](./cn/4-design-player/iptv-player.md)

## 3 代码风格指南

``` shell
# 使用cpplint检查代码风格(!强烈推荐)
$ cd Rockit
$ ./tools/cpplint/check_style.sh

# Check Code Styles with astyle(!NOT Recommended)
$ cd Rockit/tools/AStyle
$ chmod 777 astyle
$ ./tools/rt_astyle.sh
```

## 4 项目示例

- ToDo
- ToDo

## 参考资料

- [Style guides for Google-originated open-source projects](https://github.com/google/styleguide)
