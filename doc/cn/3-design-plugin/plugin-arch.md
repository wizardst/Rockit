# 架构: 轻量级插件架构设计

## 1 插件架构概述

## 2 插件架构设计

## 3 插件开发最佳实践

- 架构和方案选型，架构和选型过程建议[base-media]&[plugin]写入文档
- 方案选型，同行评审，方案优化
- 领域相关数据结构通用数据结构设计和插件私有API设计，关键过程建议写入[base-media]&[plugin]文档
- 接口设计完成之后，gerrit审核
- 领域相关数据结构代码实现；单元测试；gerrit审核。关键原理和流程写入[base-media]文档。
- 插件私有API实现；插件实现；单元测试；gerrit审核。关键原理和流程写入[plugin]文档。
- 重要参考资料；调试手段；调试心得，建议写入文档。

## 参考文献

- [构建自己的C/C++插件开发框架](https://blog.csdn.net/pizi0475/article/details/5471637)
- [值得推荐的C/C++框架和库](https://blog.csdn.net/qq_21950929/article/details/78668870)
- [sumeetchhetri/ffead-cpp](https://github.com/sumeetchhetri/ffead-cpp)
- [orocos-toolchain/log4cpp](https://github.com/orocos-toolchain/log4cpp)
- [MT6797/frameworks](https://github.com/MT6797/frameworks)
- [VLC hacking guide](https://wiki.videolan.org/Hacker_Guide)