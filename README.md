## Tank AI

##### feifeiobama@gmail.com

#### 运行方法

在非 mac 系统上若无法编译，请将 `src/jsoncpp.cpp` 和 `src/jsoncpp/` 下的文件替换为其它版本。

| 用途                  | 指令              |
| --------------------- | ----------------- |
| 生成 botzone 上的版本 | make Tank_xxx_lib |
| 本地调试              | make Tank_xxx     |
| 体验 minimax 对战调参  | make Anneal       |

上述的 xxx 包括 minimax, mcts 和 mix（以解决 mcts 在斩杀或被斩杀时的异常表现）

强烈建议设置为 Release 编译模式。

#### 估价函数

* 基本估价 (debug 情况下一次估值需要 5e-5 s)

    val = sum(argv[0] * tank - argv[1] * dist + argv[2] * min_ahead + argv[3] * area_fire + argv[4] * area_move) - argv[5] * first_threat

* 斩杀判定

    考虑若干种对位关系，每一方选择：

    (1) 某坦克进攻/防守
    
    (2) 两坦克进攻敌方一名坦克，放空另一只坦克

​	若一方全胜 val += argv[6] - argv[7] * 斩杀步数

    最后sigmoid(blue - red)

#### 总结

* mcts 其实两层都搜不完，和 minimax 表现差异不大。
* 有两个特判一定要加入代码：坦克没法动，被对方射死；坦克被两个坦克夹击，必死。
