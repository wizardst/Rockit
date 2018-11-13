# Rockit Documents [[Chinese Version]](./README-cn.md)

## 1 Version Management

- [Best Practice for Git and Gerrit Version](./en/v11-git-gerrit-version.md)

## 2 Design Practice

- [Design of Memory Manager](./en/v21-design-mem-manager.md)
- [Design of Queue/Stack/HashTable/Array](./en/v22-design-container.md)
- [Design of Message/Event Queue](./en/v23-design-event-queue.md)
- [Design of Lightweight Plugin Mechanism](./en/v24-plug-in-manager.md)

## 3 Code Style Guide

- [Style guides for Google-originated open-source projects](https://github.com/google/styleguide)

```
# Check Code Styles with cpplint(!Recommended)
$ cd Rockit
$ git clone https://github.com/google/styleguide
$ ./styleguide/cpplint/cpplint.py ./src/rt_base/*

# Check Code Styles with astyle(!NOT Recommended)
$ cd Rockit/tools
$ chmod 777 astyle
$ ./tools/mpp_astyle.sh
```

## 4 Samples

- ToDo
- ToDo

## 4 References

- ToDo
- ToDo
