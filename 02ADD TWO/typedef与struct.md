# typedef 与 struct 的区别

> 记录两种结构体声明写法的区别，以及 typedef 到底算不算"语法糖"。

---

## 一、两种写法对比

### 写法一：普通 struct

```c
struct ListNode {
    int val;
    struct ListNode *next;
};

// 使用时必须带 struct 关键字
struct ListNode node;
struct ListNode *p = (struct ListNode *)malloc(sizeof(struct ListNode));
```

### 写法二：typedef struct

```c
typedef struct {
    int value;
    int originalIndex;
} NumberWithIndex;

// 使用时不用写 struct
NumberWithIndex arr[100];
NumberWithIndex *p = (NumberWithIndex *)malloc(sizeof(NumberWithIndex));
```

也可以给已经定义好的 struct 起别名：

```c
struct ListNode { ... };   // 先定义
typedef struct ListNode ListNode;  // 再起别名
// 之后就能直接写 ListNode node;
```

---

## 二、核心区别一览

| 对比项 | `struct` | `typedef struct` |
|--------|----------|------------------|
| 声明变量时 | 必须写 `struct` 关键字 | 直接写类型名 |
| 本质 | 定义一个**类型** | 给类型**起一个别名** |
| 内存 | 不占任何内存 | 不占任何内存（别名不是变量） |
| 可读性 | 更啰嗦 | 更简洁，接近其他语言 |
| 自引用（链表） | 必须用 `struct 名字` | 结构体内部仍需 `struct` 或起名 |

---

## 三、自引用节点（链表）的坑

链表节点里要放一个"指向自己类型的指针"，此时必须能提到结构体自己的名字：

```c
// 写法一：结构体有名，自引用没问题
struct ListNode {
    int val;
    struct ListNode *next;   // 直接引用自己
};

// 写法二：匿名结构体 + typedef，自引用会出问题
typedef struct {
    int val;
    NumberWithIndex *next;   // ❌ 此时 NumberWithIndex 还没定义完！
} NumberWithIndex;
```

所以链表节点常用这种写法（**给结构体起名 + typedef 起别名**）：

```c
typedef struct ListNode {
    int val;
    struct ListNode *next;   // 自引用用 struct 名字
} ListNode;
```

---

## 四、这算语法糖吗？

**算，但只是"打字层面的语法糖"，不是"功能层面的"。**

### 语法糖的定义
> 一种不影响语言功能的写法，只是为了**让代码更好写、更好读**。

`typedef struct` 完全符合这个定义：

- ❌ 没有增加任何新功能（不能做普通 struct 做不到的事）
- ✅ 只是把 `struct 类型名` 缩写成 `类型名`，少打字、更好读

### 但注意：typedef 本身不是语法糖

`typedef` 这个关键字有自己独特的能力，不是简单的替换：

```c
typedef int (*FuncPtr)(int, int);   // 给"函数指针"起别名
typedef unsigned long size_t;       // 给已有类型起别名
typedef struct { ... } Point;       // 给匿名结构体起别名
```

这些都不能用"直接替换"实现，所以 `typedef` **本身**是一个独立特性；只是**用 typedef 给 struct 起别名**这一行为，可以看作语法糖。

### 一句话总结

> `typedef struct { ... } 别名;` ≈ `struct 类型名 { ... };` + `typedef struct 类型名 别名;`
> 功能完全等价，纯粹是写起来更方便 —— 这就是语法糖。

---

## 五、什么时候用哪种？

| 场景 | 推荐 |
|------|------|
| LeetCode 刷题（节点类结构） | `typedef struct ListNode { ... } ListNode;` 最常见 |
| 数据打包（数值+下标） | `typedef struct { ... } 名字;` 很清爽 |
| 追求兼容 C 老代码风格 | 用普通 `struct` |
| 结构体内部要自引用 | 结构体必须**起名**，否则无法自引用 |

---

## 六、回到本题（两数相加）

LeetCode 已经帮你定义好了，直接用即可：

```c
/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     struct ListNode *next;
 * };
 */
```

这是"写法一"（普通 struct），所以代码里创建新节点要写全：

```c
struct ListNode *node = (struct ListNode *)malloc(sizeof(struct ListNode));
node->val = 7;
node->next = NULL;
```
