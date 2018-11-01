//
// Created by szc on 2018/10/17.
//

#ifndef TANK_FIELD_INFO_H
#define TANK_FIELD_INFO_H

#include "def.hpp"
#include <map>
#include <vector>

using namespace std;

class Field_map;

/*
 * 0 1 2 3 4 5 6 7 8 pos.x
 * 1 A B
 * 2 A B
 * 3 A B
 * 4 A B
 * 5 A B
 * 6 A B
 * 7 A B
 * 8 A B
 * pos.y
 */

// 各个坦克到周围距离信息的缓存, 位于MCT上每个节点，最好能够增量维护
class Field_info {
    int clean_map[9]; // 缓存的地图,坦克当作虚无,基地当作不能摧毁

    unsigned distance_map[4][9][9];
    bool fire_map[4][9][9] = {0};
    unsigned base_row_barrier[2][9];

public:
    Field_info(const Field_map &field_map);

    Field_info(const Field_info &field_info);

    void update(Action &action1, Action &action2, const Field_map &field_map);

    void calc_fire_map(int i, const Field_map &field_map);

    void calc_base_row_barrier();

    void calc_distance(int i, Position position, bool loaded = true);

    // 以下均保证坦克合法

    // 允许在其它位置射击击穿砖块
    unsigned dist_to_shoot_base(int tank, const Field_map &field_map, bool enemy=true) const;

    bool block_route(int tank1, int tank2, const Field_map &field_map) const;

    // <斩杀领先的步数, 最短的斩杀步数>
    pair<int, unsigned> dist_to_shoot_avoid(int tank1, int tank2, const Field_map &field_map) const;

    // 保证己方双坦都在 <完成骚操作操作需要的步数, 第一波威胁对面需要的步数>
    pair<unsigned, unsigned> dist_to_shoot_after(int tank, const Field_map &field_map) const;

    // 视野宽敞 + 火力覆盖 (不能往家方向开火来防止那啥)
    unsigned area_fire(int tank, const Field_map &field_map) const;

    // 附近移动宽敞(应当避开敌方火线，和所有坦克坐标)
    unsigned area_move(int tank, const Field_map &field_map) const;

    void print(const Field_map &field_map) const;
};


#endif //TANK_FIELD_INFO_H
