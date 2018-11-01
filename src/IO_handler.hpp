//
// Created by szc on 2018/10/16.
//

#ifndef TANK_IO_HANDLER_HPP
#define TANK_IO_HANDLER_HPP

#include <iostream>
#include "def.hpp"
#include "Field.hpp"
#include "jsoncpp/json.h"

using namespace std;

Color recover_from_input(Field_map &field_map, vector<pair<Field_map, Action> > history[2]) {
    Json::Reader reader;
    Json::Value input;
    string inputString;
    do {
        getline(cin, inputString);
    } while (inputString.empty());
#ifndef _BOTZONE_ONLINE
    // 猜测是单行还是多行
    char lastChar = inputString[inputString.size() - 1];
    if (lastChar != '}' && lastChar != ']') {
        // 第一行不以}或]结尾，猜测是多行
        string newString;
        do {
            getline(cin, newString);
            inputString += newString;
        } while (newString != "}" && newString != "]");
    }
#endif
    reader.parse(inputString, input);

    Json::Value requests = input["requests"], responses = input["responses"];

    // 处理砖块
    Json::Value bricks = requests[0]["field"];
    int has_brick[3];
    for (int i = 0; i != 3; ++i) {
        has_brick[i] = bricks[i].asInt();
    }
    FOR_THE_FIELD(i, j, {
        int index = i + 9 * j;
        if ((has_brick[index / 27] >> (index % 27)) & 1) {
            field_map.set_brick(i, j);
        }
    })

    // 己方颜色
    Color color = Color(requests[0]["mySide"].asInt());

    // 处理坦克动作
    size_t n = responses.size();
    for (int i = 0; i < n; i++) {
        Json::Value response = responses[i], request = requests[i + 1];
        Action action[2];
        action[color] = Action{Move(response[0].asInt()), Move(response[1].asInt())};
        action[op_color(color)] = Action{Move(request[0].asInt()), Move(request[1].asInt())};
        field_map.push_history(action[0], action[1], history);
        field_map.update(action[0], action[1]);
    }

    //打印一波
    return color;
}

void encode_output(Action action, string debug = "") {
#ifdef _BOTZONE_ONLINE
    Json::FastWriter writer;
#else
    Json::StyledWriter writer;
#endif
    Json::Value output(Json::objectValue), response(Json::arrayValue);
    for (int i = 0; i != 2; ++i) {
        response[i] = action.move[i];
    }
    output["response"] = response;
    if (!debug.empty()) {
        output["debug"] = debug;
    }
    cout << writer.write(output) << endl;
}

#endif //TANK_IO_HANDLER_HPP
