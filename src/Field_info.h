//
// Created by szc on 2018/10/17.
//

#ifndef TANK_FIELD_INFO_H
#define TANK_FIELD_INFO_H

#include "def.hpp"

class Field_map;

// 各个坦克到周围距离信息的缓存, 位于MCT上每个节点，最好能够增量维护
class Field_info {
    unsigned distance_map[4][9][9];
    bool fire_map[4][9][9] = {0};
    int clean_map[9]; // 缓存的地图,坦克当作虚无,基地当作不能摧毁
public:
    Field_info(const Field_map &field_map);

    Field_info(const Field_info &field_info);

    void update(Action action1, Action action2, const Field_map &field_map);

    void calc_fire_map(const Field_map &field_map);

    void refresh_distance(int i, Position position, bool loaded = true);

    // 以下均保证坦克合法

    // 允许在其它位置射击击穿砖块
    int dist_to_shoot_base(int tank, const Field_map &field_map) const;

    // 因为坦克机动性高，不允许隔着射击（容易被窄口挡住）(tank1 fire tank2)
    int dist_to_fire(int tank1, int tank2, const Field_map &field_map) const;

    // 追击坦克 or 己方协助
    int dist_to_tank(int tank1, int tank2, const Field_map &field_map) const;

    // 视野宽敞 + 火力覆盖 (不能往家方向开火来防止那啥)
    int area_fire(int tank, const Field_map &field_map) const;

    // 附近移动宽敞(应当避开敌方火线，和所有坦克坐标)
    int area_move(int tank, const Field_map &field_map) const;

    // 局部卡路线
    bool block_route(int tank1, int tank2, const Field_map &field_map) const;

    void print(const Field_map &field_map) const;
};


#endif //TANK_FIELD_INFO_H
