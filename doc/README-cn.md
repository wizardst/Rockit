# Rockit 文档 [[英文版]](./README.md)

## 1 版本管理

- [Git和Gerrit版本控制最佳实践](./cn/v11-git-gerrit-version.md)

## 2 设计实践

- [内存管理设计文档](./cn/v21-design-mem-manager.md)
- [通用容器设计(HashTable等)文档](./cn/v22-design-container.md)
- [消息和事件设计文档](./cn/v23-design-event-queue.md)
- [轻量级插件管理设计文档](./cn/v24-plug-in-manager.md)

## 3 代码风格指南

- [Style guides for Google-originated open-source projects](https://github.com/google/styleguide)

```
# 使用cpplint检查代码风格(!强烈推荐)
$ cd Rockit
$ git clone https://github.com/google/styleguide
$ ./styleguide/cpplint/cpplint.py ./src/rt_base/*

# 使用astyle检查代码风格(!不推荐)
$ cd Rockit/tools
$ chmod 777 astyle
$ ./tools/mpp_astyle.sh
```

## 4 项目示例

- ToDo
- ToDo

## 5 参考资料

- ToDo
- ToDo