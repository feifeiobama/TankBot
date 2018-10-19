## Tank AI

feifeiobama@gmail.com

在非 mac 系统上编译时，请将 `src/jsoncpp.cpp` 和 `src/jsoncpp/` 下的文件替换为 `JsonCPP_versions/` 中相应的版本。

若在本地调试，注释掉 `#define _BOTZONE_ONLINE`，否则请保留注释。

| 用途                  | 指令              |
| --------------------- | ----------------- |
| 生成 botzone 上的版本 | make Tank_library |
| 本地调试              | make Tank         |
| 体验调参              | make Anneal       |

强烈建议设置为 Release 编译模式。