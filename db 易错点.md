# 易错points

- [x] unique(允许null),primary(不允许null)

- [x] delete from 和 drop
  - alter table drop
  - delete from R where

- [x] 关系代数的两个角色
  - Query language
  - Constraint language(2 ways)

- [x] 多值依赖
  - 遵守的规则
    - Trivial MVD
    - Transitive rule
    - FD promotion
    - Complementation rule
    - More trivial MVD's
  - 不遵守的规则
    - 分解/结合规则中的分解部分
- [x] 子类结构向关系的转换（E/R方式转换、面向对象方法、空值方法）

- [x] 分解算法

  |                      |    BCNF     |  3NF   |    4NF     |
  | -------------------- | :---------: | :----: | :--------: |
  | 分解关系时需要的FD数 | 1个BCNF违例 | 所有FD | 1个4NF违例 |

- [x] 基于元组的check约束&断言约束（均用到check）

  ![Screen Shot 2021-06-27 at 9.55.41 PM](/Users/zhao/Desktop/Screen Shot 2021-06-27 at 9.55.41 PM.png)

- [x] 选择**键**为索引的原因
  1. 键上的索引通常会被频繁使用
  2. 键值唯一，即与给定键值匹配的元组最多只有一个，从而取得该元组最多只有一个磁盘页需要被读进主存

- [x] 索引**没有建立在键**上也依然有效的两种情形
  1. 没有多少元组在该索引上具有给定值，该索引几乎可以被看作是一个键
  2. 元组在该键值上是聚集的，从而即使要检索的元组很多，也不必检索与元组数量相当的磁盘页

