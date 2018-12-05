# 版本控制: Rockit项目C/C++代码风格规范

## 1 头文件

通常，每一个.cpp 文件（C++的源文件）都有一个对应的.h 文件（头文件），也有一些例外，如单元测试代码和只包含 main()的.cpp 文件。
正确使用头文件可令代码在可读性、文件大小和性能上大为改观。
下面的规则将引导你规避使用头文件时的各种麻烦。

### 1.1 #define的保护

我们规定所有头文件都应该使用#define 防止头文件被多重包含。

为了保证唯一性，头文件的命名应基于其所在项目源代码树的全路径。

例如文件存放在include/rt_test_header.h 那么依照命名规范，代码应是：

```c
#ifndef INCLUDE_RT_TEST_HEADER_H_
#define INCLUDE_RT_TEST_HEADER_H_
......
#endif  // INCLUDE_RT_TEST_HEADER_H_
```

### 1.2 包含文件的名称及次序

将包含次序标准化可增强可读性、避免隐藏依赖，次序如下：C 库、C++库、其他库的.h、项目内的.h

### 1.3 #include头文件

- 针对自定义的头文件， 建议使用相对路径；例如：

```c
#include "../../rt_test_header.h"
```

- 如果使用工具包含了路径定义，可以不写明相对路径，但需要注意的是为了避开cppjint检查， 在include后加入" // NOLINT"。

## 2 作用域

- 尽量不要 使用public；
- 尽量不用全局函数和全局变量，考虑作用域和命名空间限制;
- 多线程中的全局变量（含静态成员变量）不要使用 class 类型（含 STL STL STL STL 容器），避免不明确行为导致的 bugs;
- 作用域的使用，除了考虑名称污染、可读性之外，主要是为降低耦合度，提高编译、执行效率;

## 3 类

- 不在构造函数中做太多逻辑相关的初始化；
- 规定编码者必须提供默认构造函数，防止编译器提供的默认构造函数不对变量进行初始化；
- 为避免隐式转换，需将单参数构造函数声明为 explicit；
- 仅在作为数据集合时使用 struct;
- 避免使用多重继承，使用时，除一个基类含有实现外，其他基类均为纯接口；
- 接口类类名以 Interface 为后缀，除提供带实现的虚析构函数、静态成员函数外，其他均为纯虚函数，不定义非静态数据成员，不提供构造函数，提供的话，声明为 protect ；
- 存取函数一般内联在头文件中；
- 声明次序：public->protected->private ；
- 函数体尽量短小、紧凑，功能单一。

## 4 其他C++特性

- 引用形参加上 const ，否则使用指针形参；
- 函数重载的使用要清晰、易读；
- 禁止使用变长数组；
- 使用 C++ 风格的类型转换，除单元测试外不要使用 dynamic_cast ；
- 能用前置自增/减不用后置自增/减；
- const 能用则用，提倡 const 在前；
- 使用确定大小的整型，除位组外不要使用无符号型；
- 格式化输出及结构对齐时，注意 32 位和 64 位的系统差异；
- 整数用 0，实数用 0.0 ，指针用 NULL ，字符（串）用'\0'；
- 用 sizeof(varname) 代替 sizeof(type) ；

## 5 命名约定

### 5.1 命名要清晰

命名尽量不要使用缩写，例如变量命名为ChangeLocalValue而不要写成ChgLocVal，除函数名可适当为动词外，其他命名尽量使用清晰易懂的名词；

### 5.2 基本命名规则

#### 5.2.1 文件命名

```c
rt_hash_table.cpp;
```

#### 5.2.2 变量命名

除了类成员变量使用驼峰命名法，其他的变量命名使用小写+下划线的写法。

- 普通变量： ```int32_t num_errors;```
- 结构体数据成员：

```c
    struct UrlTableProperties {
        string name;
        int num_entries;
    }
 ```

- 类数据成员：

```c
    int32_t mDataCount;
    int64_t mTimeStamps;
 ```

- 全局变量： ```int32_t g_array_list_count;```
- 命名空间： ```google_awesome_project;```

#### 5.2.3 类型命名

- 结构体： ```struct UrlTableProperties { ...;```
- 类： ```class UrlTableTester { ...;```
- 类型定义： ```typedef hash_map<UrlTableProperties *, string> PropertiesMap;```
- 枚举： ```enum UrlTableErrors { ...;```


#### 5.2.4 函数命名

class的函数命名遵循驼峰命名法，非class函数使用小写+下划线的写法。

- 类成员函数：   ```AddTableEntry();```
- 非类成员函数：```add_table_entry();```
- 存取函数：

```c
int GetNumEntries() const { return mNumEntries; }
void GetNumEntries(int num_entries) { mNumEntries = num_entries; }
```

#### 5.2.5 常量命名

```c
const int kDaysInAWeek = 7;
```

#### 5.2.6 枚举命名

```c
enum UrlTableErrors {
    OK = 0,
    ERROR_OUT_OF_MEMORY,
    ERROR_MALFORMED_INPUT,
};
```

#### 5.2.7 宏命名

```c
#define ROUND(x) ...
#define PI_ROUNDED 3.0
```

## 6 注释

### 6.1 注释风格

注释要求清晰的描述代码段，单行字符不要超过120个字符，其他并未做严格的要求， 简单的注释如下：

- 单句的注释：

```c
// insert header, when list is RT_NULL
```

- 较长的注释：

```c
/*
 * Simple doubly linked list implementation.
 *
 * Some of the internal functions ("__xxx") are useful when
 * manipulating whole lists rather than single entries, as
 * sometimes we already know the next/prev entries and we can
 * generate better code by using them directly rather than
 * using the generic single-entry routines.
 */
```

### 6.2 版权注释

按照我们项目的要求，示例如下：

```c
/*
 * Copyright 2018 Rockchip Electronics Co. LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * author: rimon.xu@rock-chips.com
 *   date: 20180903
 */
```

### 6.3 注释格式要求

如果是//注释，需要与代码保持2个空格以上的间距。

如果是/* */注释，需要置于需要注释代码的上方。

### 6.4 TODO注释

对于未实现，需要重写的代码， 需要引入TODO加以注释。

TODO后需要加上(你的邮箱)，例如：

```c
// TODO(rimon.xu@rock-chips.com): Use a "*" here for concatenation operator
```

## 7 格式

这里的格式要求大部分在cpplint中可以被检测出来， 如果在cpplint中被检测的格式警告可以在这里被查阅到。

### 7.1 行长度

在Rockit项目中，规定了行长度不能超过120个字符，如果超过，请换行处理。

### 7.2 使用空格缩进

在Rockit项目中，请缩进4个空格，而不要使用tab进行缩进。

### 7.3 函数声明与定义

返回类型、函数名、参数最好都在同一行，例如：

```c
void rt_mem_service::remove_node(void* ptr, UINT32 *size) {
```

如果超过120个字符长度，请使用

```c
struct RtHashTable *rt_hash_table_init(UINT32 num_buckets,
                                       rt_hash_func hash,
                                       rt_hash_comp_func compare) {
    ....
}
或者
struct RtHashTable *rt_hash_table_init(
        UINT32 num_buckets,  // 八个空格缩进
        rt_hash_func hash,
        rt_hash_comp_func compare) {
    ....
}
```

建议统一使用第二种方式。如果不太明白具体细节， 请看下列这段话：

- 返回值总是和函数名在同一行；
- 左圆括号 "(" 总是和函数名在同一行；
- 函数名和左圆括号间没有空格；
- 圆括号与参数间没有空格；
- 左大括号 "{" 总在最后一个参数同一行的末尾处；
- 右大括号 "}" 总是单独位于函数最后一行；
- 右圆括号 ")" 和左大括号间总是有一个空格；
- 函数声明和实现处的所有形参名称必须保持一致；
- 所有形参应尽可能对齐；
- 缺省缩进为 4 个空格；
- 独立封装的参数保持 8 个空格的缩进;

### 7.4 函数调用

采用以下几种方式都是被允许的：

- 正常情况下，使用下面的方式：

```c
bool retval = DoSomething(argument1, argument2, argument3);
```

- 长度太长的话，可以将第二个参数保持跟第一个参数对齐，例如：

```c
bool retval = DoSomething(averyveryveryverylongargument1,
                          argument2, argument3);
```

- 如果参数较多的情况下，使用下面的方式：

```c
bool retval = DoSomething(argument1,
                          argument2,
                          argument3,
                          argument4);
```

- 如果函数名实在太长，使用下面的方式：

```c
DoSomethingThatRequiresALongFunctionName(
        very_long_argument1, // 8个空格缩进
        argument2,
        argument3,
        argument4);
```

### 7.5 条件语句

我们提倡使用时加入{}, 使用下面的方式：

```c
if (condition) {
    ...
} else {
    ...
}
```

### 7.6 循环或条件开关语句

- 条件开关语句，请遵循下列的格式：

```c
switch (var) {
    case 0: { // 4 space indent
        ... // 8 space indent
    } break;
    case 1: {
        ...
    } break;
    default: {
        assert(false);
    }
}
```

- 循环语句， 请遵循下列格式：

```c
while (condition) {
    ...
}

for (int i = 0; i < kSomeNumber; ++i) {
    ...
}
```

- 尽量不要使用空循环，实在要用的话，请使用：

```c
while (condition) continue;

for (int i = 0; i < kSomeNumber; ++i) {}
```

### 7.7 指针或引用

请使用下列格式：

```c
x = *p;
p = &x;
x = r.y;
x = r->y;
```

### 7.8 布尔表达式

如果条件过多而超过了120个字符长度，使用下面的方式：

```c
if ((count1 > count2) &&
    (count3 == count4) ||  //保证条件2和条件1字符对齐
    (count5 & count6)) {
        ...
}
```

建议如上使用()来隔开条件，增强可读性。

### 7.9 预处理指令

预处理命令不要缩进， 如下：

```c
if (lopsided_score) {
#if DISASTER_PENDING // Correct -- Starts at beginning of line
    DropEverything();
#endif
    BackToNormal();
}
```

### 7.10 类格式

一个常见的标准类格式，如下：

```c
class MyClass : public OtherClass {
 public: // 1个空格缩进
    MyClass(); // 4个空格缩进
    explicit MyClass(int var);
    ~MyClass() {}
    void SomeFunction();
    void SomeFunctionThatDoesNothing() {
    }
    void SetSomeVar(int var) { mSomeVar = var; }
    int GetSomeVar() const { return mSomeVar; }

 private:
    bool SomeInternalFunction();
    int mSomeVar;
    int mSomeOtherVar;
    DISALLOW_COPY_AND_ASSIGN(MyClass);
};
```

需要注意以下几点：

- 所以基类名应在 80 列限制下尽量与子类名放在同一行；
- 关键词 public:、protected:、private:要缩进 1 个空格;
- 除第一个关键词（一般是 public）外，其他关键词前空一行;
- public 放在最前面，然后是 protected 和 private;

### 7.11 留白

- 代码的末尾不要出现留白，空行也不要出现留白；
- 垂直留白尽量少用；

## 8 其他cpplint代码规范要求

- 强制类型转换，使用C++的规范要求，例如：

```c
在类型转换时，需要使用reinterpret_cast来进行。
    FF_PARSER_CONTEXT* parser_ctx = (FF_PARSER_CONTEXT*)ctx;
需要修改为：
    FF_PARSER_CONTEXT* parser_ctx = reinterpret_cast<FF_PARSER_CONTEXT*>(ctx);
在const变量赋值到none-const变量时， 需要使用const_cast：
    &rtMutex
需要修改为：
    const_cast<RtMutex *>(&rtMutex)
```

- 优先使用cpplint建议的库函数。某些库函数存在的一些缺陷，cpplint将会把这些有缺陷的库函数给出建议的替换函数。

## 9 参考文献

- [google c++ code style guide](https://github.com/google/styleguide)
- [NetBSD's C style guidelines](https://users.ece.cmu.edu/~eno/coding/CCodingStandard.html)
- [NetBSD's C++ style guidelines](https://users.ece.cmu.edu/~eno/coding/CppCodingStandard.html)
