
蛋尖对决

一个用 C++ 和 EasyX 写的图形化小游戏。玩法来源于小时候拿鸡蛋互相碰撞，比谁的鸡蛋更硬。


玩法来源于小时候拿鸡蛋互相碰撞，比谁的鸡蛋更硬。
灵感来源于我的童年，每天早饭，母亲都会拿装满热水的铁盆保温几个煮熟的鸡蛋，
我和哥哥从中挑选自己中意的鸡蛋，依赖观察+运气，寻找自认为最好的鸡蛋，随后分别寻找自认为鸡蛋攻击力/防御力最强的的部位，
称之为“尖角”/“钝角”。
然后回合制攻击对方鸡蛋钝角，谁的鸡蛋两端先全碎了，谁就输了。

游戏从最早的控制台文字版开始，后来逐步改成图形界面、实时操控、地图机制、人机 AI、头像贴图和结算统计。最终版支持双人对战和人机对战。

开发环境

C++  
Visual Studio  
EasyX 图形库  
Windows

运行方式

安装 EasyX 后，用 Visual Studio 打开项目，或者把 `final/main.cpp` 加入 C++ 项目中运行。

建议使用 x64 配置编译。

文件结构

```text
Egg-Duel
├─ final
│  └─ main.cpp
├─ versions
│  ├─ v0.7_console_text.cpp
│  ├─ v1.5_basic_easyx.cpp
│  ├─ v2.12_realtime_battle.cpp
│  ├─ v2.24_map_resource.cpp
│  ├─ v2.26_face_mode.cpp
│  ├─ v2.28_polish.cpp
│  └─ v2.29_final_ai.cpp
├─ assets
│  └─ egg_duel_icon.ico
├─ screenshots
│  ├─ menu.png
│  ├─ select.png
│  ├─ battle.png
│  └─ result.png
└─ README.md
````

基本操作

玩家 1：

```text
W A S D    移动
Q / E      微调方向
G          加速
F          发射蛋黄
```

玩家 2：

```text
方向键      移动
K / L      微调方向
右 Shift   加速
右 Ctrl    发射蛋黄
```

鼠标用于点击菜单、选择鸡蛋、放大检视和上传头像。

主要功能

选蛋

每局会随机生成候选鸡蛋。不同鸡蛋有不同外形、颜色、纹理、尖端属性、钝端属性和隐藏状态。玩家需要根据外观和线索判断哪颗更适合对战。

实时对战

玩家可以控制鸡蛋移动、转向和加速。碰撞结果和速度、角度、接触部位、攻击力、防御力、剩余血量有关。

鸡蛋受伤后会出现裂纹。

地图模式

游戏有三种模式。

基础模式：适合熟悉操作。
进阶模式：障碍更多，有尖刺和更强 AI。
噩梦模式：尖刺更大、出现更频繁，AI 压力更高。

道具

血包可以回血。
技能包可以获得一次蛋黄远程攻击。
蛋黄命中后会造成伤害和击退。

头像贴图

玩家可以上传本地头像，把头像贴到自己的鸡蛋上。

人机 AI

最终版对 AI 做了多次优化。AI 会躲避危险、争夺资源、使用蛋黄、反击和调整走位。后期主要修复了 AI 遇到墙壁、尖刺或玩家时容易原地抖动的问题。

版本迭代

| 版本    | 文件                                   | 说明                                   |
| ----- | ------------------------------------ | ------------------------------------ |
| v0.7  | `versions/v0.7_console_text.cpp`     | 控制台文字原型。实现选蛋、尖端/钝端对抗、人机对战和战斗日志。      |
| v1.5  | `versions/v1.5_basic_easyx.cpp`      | EasyX 图形化基础版。加入主菜单、选蛋界面、鸡蛋绘制、裂纹和结算页。 |
| v2.12 | `versions/v2.12_realtime_battle.cpp` | 实时操控版。加入移动、加速、自动朝向、碰撞伤害、体力条和完整度条。    |
| v2.24 | `versions/v2.24_map_resource.cpp`    | 地图资源版。加入三种模式、障碍、尖刺、血包、技能包和蛋黄攻击。      |
| v2.26 | `versions/v2.26_face_mode.cpp`       | 头像贴图版。支持上传本地头像并显示在鸡蛋上。               |
| v2.28 | `versions/v2.28_polish.cpp`          | 展示完善版。加入玩法说明、难度说明和本局摘要。              |
| v2.29 | `versions/v2.29_final_ai.cpp`        | 最终 AI 版。优化 AI 走位、抢资源、躲避、反击和防抖动逻辑。    |

截图

主菜单

<img width="2559" height="1599" alt="1" src="https://github.com/user-attachments/assets/047584c8-6679-46a0-9aac-7e32dc69c85b" />


选蛋界面

<img width="2559" height="1599" alt="2" src="https://github.com/user-attachments/assets/264d5959-ad0b-4d7f-bb7e-e4dbc3650dd4" />


对战界面

<img width="2559" height="1599" alt="3" src="https://github.com/user-attachments/assets/1e8a7708-e3af-4a64-a3d7-d37a65baa07e" />


结算界面

<img width="2559" height="1599" alt="4" src="https://github.com/user-attachments/assets/bf3ac6d8-7cec-4107-80d5-6e22c61d1915" />


项目特点

1. 从控制台文字版一步步迭代到图形化实时对战版。
2. 玩法不只靠数值，还加入了移动、碰撞、道具、地图和 AI。
3. 保留多个关键版本，能看出开发过程。
4. 最终版有说明页、结算统计和头像贴图，比较适合展示。

AI 辅助说明

本项目开发过程中使用了 ChatGPT 辅助完成部分需求整理、代码生成、Bug 排查和 AI 行为设计。

本人主要负责确定玩法方向、提出功能需求、运行测试、反馈问题、筛选方案、整理最终版本和提交材料。

总结

《蛋尖对决》最开始只是一个文字对战原型，后来逐步加入图形界面、实时移动、地图机制、道具系统和人机 AI。通过这个项目，我练习了 C++ 图形化编程、事件循环、碰撞检测、状态管理、随机生成和简单 AI 决策。

