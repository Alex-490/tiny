这次我们按真正的 C++ 后端面试手撕题 来写 LRU Cache，不直接把完整答案扔给你。

LRU 是非常值得掌握的经典组件，因为它同时考察：
- 哈希表：快速查找节点。
- 双向链表：快速调整节点位置。
- 类的设计：如何组织数据和操作。
- C++：指针、迭代器、unordered_map、list。
1. 先理解 LRU 要解决什么问题
LRU = Least Recently Used，最近最少使用淘汰策略。

假设缓存容量为 3：
当前缓存（左边最新，右边最旧）
C
最近使用
B

A
最久未使用

访问 A 后，A 移到最左边；如果插入 D，则淘汰最右边的 B。

我们需要支持两个操作：
1. get(key)：查找 key，找到后返回 value，并将它变成最近使用。
2. put(key, value)：插入或更新 key，并将它变成最近使用；容量超出时淘汰最久未使用的节点。
要求：两个操作平均时间复杂度都是 O(1)。
2. 数据结构怎么选？
这里是整道题最重要的设计。
需求	数据结构	原因
根据 key 找节点	unordered_map	平均 O(1)
将节点移动到头部	双向链表	O(1)
删除最久未使用节点	双向链表尾部	O(1)

我们可以直接使用 STL：
list<pair<int, int>> cache;
unordered_map<int, list<pair<int, int>>::iterator> mp;


约定：
- cache.begin()：最近使用。
- cache.back()：最久未使用。
- mp[key]：对应节点的迭代器。
注意：list 的迭代器在其他节点插入、删除时不会失效（被删除的节点除外）。
3. 先自己设计操作
我们暂时不写完整代码。先把三个核心操作拆出来：
- get(key)
- put(key, value)
- moveToFront(key) 或者直接使用迭代器移动节点。
其中 list::splice 可以把一个节点直接移动到链表头部，不需要删除再重新创建。