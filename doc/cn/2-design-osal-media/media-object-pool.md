# 媒体原语层: 对象池技术

媒体应用或中间件涉及巨量的内存分配和读写。巨量的实例化和释放资源，对内存性能、内存碎片有显著影响。对象池技术就是针对这种内存使用场景的典型解决方案。

## 对象池使用的检查点

- 是否包含大量创建和销毁的对象的场景？
- 是否包含多线程访问对象池的场景？
- 使用何种数据类型来对这些对象进行储存？
- 是否使用泛型实现通用对象池？不适用泛型如何实现通用对象池？
- 需要保证对象能够重用，否则对象池形同虚设。
- 对象池是固定大小还是按需增长需要综合具体使用场景一起考虑。

## 依赖注入的对象池

我们不希望使用复杂的C++语法实现通用对象池，我们希望使用简单的语法实现通用对象池。通用对象池最大的依赖是对象的创建和销毁，我们使用依赖注入技术，让调用者定义对象池中的对象创建和销毁，通用性就完全有保证了。

我们设计的RTObjectPool具有以下特性:

- 使用FIFO队列管理对象
- 使用依赖注入技术，让调用者定义对象创建和销毁
- 使用锁保证获取和放入对象是线程安全的。

``` c++
typedef RTObject* (*AllocListener)();

class RtMutex;
class RTObjectPool : public RTObject {
 public:
    RTObjectPool(AllocListener listener, UINT32 maxNum);
    virtual ~RTObjectPool();
    RTObject* borrowObj();
    void      returnObj(RTObject* obj);
    UINT32    getNumIdle();
    UINT32    getNumUsed();
 private:
    UINT32    mMaxNum;
    UINT32    mObjNum;
    RT_Deque* mUsedDeque;
    RT_Deque* mIdleDeque;
    AllocListener mAllocObj;
    RtMutex*  mLock;
};
```