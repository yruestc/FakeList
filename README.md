# FakeList
高性能超大数组(字符串)处理(70%)

## 背景:<br>
   c++的stl中只提供了简单string类,该类对插入,删除和修改的复杂度大概是O(N+n),N为已有的字符长度,n为需要插入,删除或修改的字符串长度,即string为了保持其结构,在插入,删除和修改的时候会对已有的字符串进行复制或移动.当然在一些情况下复杂度为O(n);所以实现一个string_builder类来减少对已有字符串的复制或移动是有必要的,这里将之写为模板以扩大其适用范围为所有多插入,删除和修改操作的大数组.当然主要作用是锻炼和学习;

## 概述:<br>
   1.采用变长度链表,使插入复杂度为O(M+n),M为节点数,与插入(修改)次数有关,且提供format函数以降低M;删除的复杂度为O(K+n),K为所涉及到的节点数;修改可以拆解为删除后插入,当然在一些情况下会得到优化;<br>
  
   2.采用引用计数,使尽最大可能减少复制,提供的元素通常只会被复制两次,即"输入"/"输出"的时候,这里的"输入"指将元素提供给类,"输出"指将元素以整齐的格式提取出来;当然复制的次数可以减少(使用右值引用)和增加(使用format函数降低M);

## 比较:<br>
   1.众所周知,数组的插入,删除和修改能力是非常低的,普通链表则非常高,但其随机访问较慢.FakeList结合两者,或者说是改进的链表,不仅依旧保持一般链表的插入,删除和修改的能力,甚至说更快,而且其随机访问速度也是非常快的;<br>
  
## 缺点:<br>
   1.极端情况下,如每次插入1个元素,FakeList就会退化为一般链表,且比一般链表更耗内存;不过我已经想到一个极好的方法解决这个问题;<br>
   
   2.元素暂时只能为POD类型;
