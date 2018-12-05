# Sink插件: GLES视频渲染

## 1 显示的几种类型

### 1.1 Android系统的显示(High Priority)

- GLES显示
- Surface显示
- HDMI状态管理和控制(帧率同步/vsync等)

### 1.2 Window系统的显示(High Priority)

- GLES显示
- 硬件同步信号

### 1.3 Linux系统的显示(Low Priority)

- GLES显示
- FrameBuffer显示
- HDMI状态管理和控制(帧率同步/vsync等)- 硬件同步信号

## 2 通用数据结构和架构

Layer-Plugin:

- [RTNode](../src/rt_node/rt_node.h)
- [RTNodeDisplayGLES](../src/rt_node/core/rt_display_gles.h)
- [RTNodeDisplaySystem](../src/rt_node/core/rt_display_system.h)

Layer-Media:

- [HMDIManager](../src/rt_media/device/HDMIManager.h)
- [GLESDisplay](../src/rt_media/device/HDMIManager.h)
- [SurfaceDisplay](../src/rt_media/device/HDMIManager.h)

## 3 开发计划

- 优先开发基础模块(HMDIManager/GLESDisplay/SurfaceDisplay)；
- 优先开发Android和Window的显示插件；
- Linux显示插件，有需求后续开发；
- 基础模块测试用例和显示插件测试用例。

## 4 显示关键概念

### 4.1 同步信号(vsync)

### 4.2 HDMI内核节点

## 5 显示关键流程

## 6 显示调试经验

## 参考资料
