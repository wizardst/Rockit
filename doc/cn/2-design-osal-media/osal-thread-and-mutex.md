# OSAL原语层: 线程和锁设计

pthread是一个跨平台的多线程库，类Linux系统优选pthread作为线程和锁的实现。windows系统由于编译器工具不同可能需要使用其他类型的线程和锁的实现。目前我们android/windows均使用gcc工具链，故优选pthread作为线程和锁的实现。为了保证跨平台性，我们封装了私有的线程和锁抽象类。根据需要支持多平台多实现。

Linux线程在核内是以轻量级进程的形式存在的，拥有独立的进程表项，而所有的创建、同步、删除等操作都在核外pthread库中进行。pthread库使用一个管理线程（__pthread_manager()，每个进程独立且唯一）来管理线程的创建和终止，为线程分配线程ID，发送线程相关的信号（比如Cancel），而主线程（pthread_create()）的调用者则通过管道将请求信息传给管理线程。  

## 1 互斥锁原语类

互斥锁操作主要包括加lock()、解锁unlock()和测试加锁trylock()三种基本操作。对于普通锁类型，解锁者可以是同进程内任何线程；对于适应锁类型，解锁者可以是同进程内任何线程；对于检错锁类型，必须由加锁者解锁才有效，否则返回EPERM。互斥锁原语类中包含一个自动锁原语，自动锁原语(构造函数)自动加锁和(析构函数)释放锁，使用非常方便。

``` c++
涉及文件: src/rt_base/include/rt_mutex.h
class RtMutex {
 public:
    RtMutex();
    ~RtMutex();

    void lock();
    void unlock();
    int  trylock();
    class RtAutolock {
     public:
        explicit inline RtAutolock(RtMutex rtMutex) { ... }
        explicit inline RtAutolock(RtMutex* rtMutex)  { ... }
        inline ~RtAutolock() { ... }
     private:
        RtMutex& mLock;
    };
}
```

使用方法见测试用例: src/tests/rt_base/test_base_mutex_thread.cpp

## 2 条件原语类

条件原语类实施条件变量。条件变量是利用线程间共享的全局变量进行同步的一种机制，主要包括两个动作：一个线程等待"条件变量的条件成立"而挂起；另一个线程使"条件成立"（给出条件成立信号）。为了防止竞争，条件变量的使用总是和一个互斥锁结合在一起。条件变量可以使用静态或动态方式创建。条件原语类的API如下:

``` c++
涉及文件: src/rt_base/include/rt_mutex.h
class RtCondition {
 public:
    RtCondition();
    ~RtCondition();

    INT32 wait(RtMutex* rtMutex);
    INT32 timedwait(RtMutex* rtMutex, UINT64 timeout_us);

    INT32 signal();
    INT32 broadcast();
}
```

使用方法见测试用例: src/tests/rt_base/test_base_mutex_thread.cpp

## 3 线程原语类

线程原语类封装了多线程的实现。相对进程而言，线程是一个更加接近于执行体的概念，它可以与同进程中的其他线程共享数据，但拥有自己的栈空间，拥有独立的执行序列。在串行程序基础上引入线程和进程是为了提高程序的并发度，从而提高程序运行效率和响应时间。与进程相比，线程执行开销小，但不利于资源的管理和保护。使用线程类的一般步骤:

- 一般传入RTThreadProc和this指针构造线程实例；
- 使用start() 和 join()函数 管理线程的生命周期；
- 一定要设置线程名，无名线程会对调试带来困扰；
- RtThread::get_tid()获取线程号，对调试有很多用处。

``` c++
涉及文件: src/rt_base/include/rt_thread.h
class RtThread  {
 public:
    typedef void* (*RTThreadProc)(void*);

    explicit RtThread(RTThreadProc entryPoint, void* data = NULL);
    ~RtThread();

    RT_BOOL start();

    void setName(const char* name);
    const char* getName();

    void join();
    static INT32 get_tid();
}
```

使用方法见测试用例: src/tests/rt_base/test_base_mutex_thread.cpp

## 多线程编程的最佳实践

- 如何优雅的终止运行线程？ 建议使用: 退出时调用pthead_cancel(tid)；执行体中调用pthread_testcancel(void)检测是否取消，如果取消，退出执行体。
- 如何优雅的清理线程资源？ 建议使用: 不论是可预见的线程终止还是异常终止，都会存在资源释放的问题，在不考虑因运行出错而退出的前提下，如何保证线程终止时能顺利的释放掉自己所占用的资源，特别是锁资源，就是一个必须考虑解决的问题。  
- 线程如何同步？ 建议使用: RtCondition
- 线程如何访问同享资源？ 建议使用: RtMutex和RtAutolock

## 参考资料

- [Posix线程编程指南(1)](https://www.ibm.com/developerworks/cn/linux/thread/posix_threadapi/part1/index.html)
- [Posix线程编程指南(2)](https://www.ibm.com/developerworks/cn/linux/thread/posix_threadapi/part2/index.html)
- [Posix线程编程指南(3)](https://www.ibm.com/developerworks/cn/linux/thread/posix_threadapi/part3/index.html)
- [Posix线程编程指南(4)](https://www.ibm.com/developerworks/cn/linux/thread/posix_threadapi/part4/index.html)
- [Posix线程编程指南(5)](https://www.ibm.com/developerworks/cn/linux/thread/posix_threadapi/part5/index.html)