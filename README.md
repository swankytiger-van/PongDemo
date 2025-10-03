# PongDemo – 跨平台 2D 小游戏框架（面试作品）

&gt; 用 **C++20 + SFML + CMake + vcpkg + GitHub Actions** 构建的轻量级 2D 游戏框架；  
&gt; 目前仅渲染绿色圆球，后续迭代 **双人乒乓 → 网络对战 → AI bot → Tracy 性能剖析**。

---

## 1. 面试卖点（HR/技术面都能讲）
| 维度 | 已实现/可讲 |
|----|-------------|
| 语言标准 | C++20 强枚举、结构化绑定、std::optional 事件 |
| 资源管理 | RAII 封装 sf::Texture/sf::Sound，智能指针管理生命周期 |
| 构建系统 | CMake + vcpkg 跨平台依赖管理，Windows/Linux/macOS 三端 CI 自动发布绿色可执行包 |
| 工程素养 | -Wall -Wextra -Werror + Address/UB Sanitizer + 单元测试（预留 doctest 接口） |
| 网络预留 | 事件队列已抽象，后续直接塞 asio 协程 |
| 性能工具 | Tracy 标记点已留好，编译开关 `-DENABLE_TRACY=ON` 即可可视化帧率 |
| 热重载 | 关卡/配置 JSON 外置，运行时改文件立即生效（已留接口） |

---

## 2. 当前进度（Checkpoint）
- [x] 环境搭建：SFML + CMake + vcpkg + GitHub Actions ✔  
- [x] 最小可运行 Demo：窗口 + 事件循环 + 绿色圆球 ✔  
- [x] 三平台 CI 自动构建 & Release 可执行包 ✔  
- [ ] 核心玩法：双人乒乓（球拍、球、碰撞、得分、重置）  
- [ ] 本地 AI 算法：minimax + alpha-beta 可调难度  
- [ ] 网络联机：asio C++20 协程 + 客户端预测 + 服务器权威  
- [ ] Tracy 性能剖析：帧时间、GPU 占用、内存波浪图  
- [ ] 音效 + 粒子 + ImGui 调试面板  
- [ ] 关卡编辑器（Qt/QML 或 ImGui）  

---

## 3. 技术栈
| 组件 | 选型 | 备注 |
|----|------|------|
| 语言 | C++20 | MSVC / GCC / Clang 均支持 |
| 图形 | SFML 2.6 | 2D 硬件加速，OpenGL 后端 |
| 包管理 | vcpkg | 一键拉取 SFML、asios、Tracy、doctest |
| 构建 | CMake ≥ 3.20 | 支持 Presets / Ninja / VS |
| CI/CD | GitHub Actions | 并行编译 Win/Linux/macOS，自动上传 Release |
| 网络 | asio standalone | 预留头文件路径，未实际编译 |
| 测试 | doctest（单头） | `src/test/` 已留文件夹 |
| 性能 | Tracy | 开关变量 `ENABLE_TRACY` |

---

## 4. 一键跑起来
### 4.1 下载即玩
1. 进入 [Release](https://github.com/你的昵称/PongDemo/releases) 页面  
2. 下载对应平台 zip → 解压 → 双击 `main.exe`（零依赖）

### 4.2 自己编译
```bash
# ① 克隆
git clone https://github.com/你的昵称/PongDemo.git
cd PongDemo

# ② 安装依赖（仅第一次）
vcpkg install sfml:x64-windows       # Win
# macOS: vcpkg install sfml:x64-osx
# Linux: vcpkg install sfml:x64-linux

# ③ 编译
cmake --preset=default               # 或 mkdir build && cd build && cmake .. -DCMAKE_TOOLCHAIN_FILE=[vcpkg]/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Debug

# ④ 运行
build\bin\Debug\main.exe             # Win
# ./build/bin/Debug/main             # mac/linux

---

## 5.目录规范
PongDemo/
├─ src/                   # 源码
│  ├─ main.cpp           # 入口
│  ├─ Game.*             # 游戏状态机（待写）
│  ├─ Paddle.*           # 球拍（待写）
│  ├─ Ball.*             # 球（待写）
│  ├─ utils/             # 工具（数学常量、资源加载器）
│  └─ test/              # doctest 单元测试
├─ assets/                # 图片、音效、字体、关卡 json
├─ cmake/                 # 模块脚本（FindTracy.cmake 等）
├─ doc/                   # 截图、gif、架构图
├─ .github/workflows/     # CI 定义
├─ CMakeLists.txt
├─ vcpkg.json             # 依赖清单（CI 用）
├─ README.md              # 本文件
└─ LICENSE                # MIT（后续加）

---

## 6.后续开发路线图
| 迭代                   | 目标             | 关键技术点                              | 面试可聊                    |
| -------------------- | -------------- | ---------------------------------- | ----------------------- |
| 1. PongCore          | 球拍、球、碰撞、得分、重置  | AABB 碰撞、帧率无关速度、状态机                 | 碰撞优化、固定时间步              |
| 2. AI                | 可调节难度电脑        | Minimax + Alpha-Beta + 评估函数        | 算法复杂度、剪枝率               |
| 3. Online            | 局域网/公网 1v1     | asio C++20 协程、客户端预测、回滚             | lock-free 队列、UDP vs TCP |
| 4. Tracy             | 性能可视化          | Tracy 标记、GPU 计时、内存波浪               | 热点函数、优化前后对比             |
| 5. Editor            | 关卡/皮肤 JSON 热重载 | std::filesystem + std::chrono 文件监听 | 热重载实现原理                 |
| 6. Particles & ImGui | 粒子效果、调试面板      | 手写粒子系统、ImGui-SFML 后端               | GPU/CPU 粒子对比            |

---

## 7.常见坑速查（实时更新）
| 现象         | 解决                                                                   |
| ---------- | -------------------------------------------------------------------- |
| 找不到 SFML   | 确认 `vcpkg integrate install` 已执行，且 CMake 时传递 toolchain 文件            |
| 运行缺 dll    | 把 `vcpkg\installed\x64-windows\bin` 加入 PATH，或拷贝 dll 到 exe 同级         |
| CI 编译失败    | 检查 `vcpkg.json` 里 triplet 与 runner 系统是否一致（win-x64/linux-x64/osx-x64） |
| Tracy 无法连接 | 先运行 `Tracy.exe`  profiler，再开 `-DENABLE_TRACY=ON` 编译游戏                |
| 别再用你那个死鬼校园网了 |                                                                 |

---
## 8.许可

---