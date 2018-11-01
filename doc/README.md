# Rockit Documents

## 1 Version Management

- [1] [Best Practice for Git and Gerrit Version](./V11-git-gerrit-version.md)

## 2 Design Practice

- [1] [Design of Memory Manager](./V21-design-mem-manager.md)
- [2] [Design of Queue/Stack/HashTable/Array](./V22-design-container.md)
- [3] [Design of Message/Event Queue](./V23-design-event-queue.md)
- [4] [Design of Plug-in Manager](./V24-plug-in-manager.md)

## Code Style Guide

- [1] [Style guides for Google-originated open-source projects](https://github.com/google/styleguide)

```
# 使用cpplint检查代码风格
$ cd Rockit
$ git clone https://github.com/google/styleguide
$ ./styleguide/cpplint/cpplint.py ./src/rt_base/*

# 使用astyle检查代码风格
$ cd Rockit/tools
$ chmod 777 astyle
$ ./tools/mpp_astyle.sh
```

## 3 Samples

## 4 References
