# <div align="center">csp · 类 QQ 局域网通信软件</div>

<div align="center">
  
**csparities** · Qt 练习项目 · C/S 双模式
  
</div>

---

## 📖 项目简介

这是一个**模仿 QQ** 设计的局域网通信软件，主要用于 **Qt 学习**。它集合了 **C/S 架构**，既可以作为客户端，也可以作为服务器，适合在局域网中使用。目前实现了基础的文本通讯和图片传输功能。

> 练习用的，之前的完整版本找不到了，只有这个半残版本 😅 主要使用了 **XML**、**TCP**、**多线程** 等技术，数据库采用 **SQLite**。

后续计划将**串口**和**USB**相关功能从另一个项目融合过来，实现一个全功能的应用。有一定学习价值，会持续完善。

---

## 🛠️ 技术栈

- **框架**：Qt (C++)
- **网络**：TCP 套接字
- **数据存储**：SQLite + XML
- **并发**：多线程
- **通信模式**：C/S 双模式（可作客户端或服务器）

---

## 📸 界面预览

<div align="center">

| 登录界面 | 服务器模式 |
|:--------:|:----------:|
| ![登录界面](https://github.com/Hlikias/csp/blob/main/demo/QQ_1772668957060.png?raw=true) | ![服务器模式](https://github.com/Hlikias/csp/blob/main/demo/QQ_1772668969040.png?raw=true) |

| 主界面 | 数据库管理 | 写入数据 |
|:------:|:----------:|:--------:|
| ![主界面](https://github.com/Hlikias/csp/blob/main/demo/QQ_1772668802299.png?raw=true) | ![数据库管理](https://github.com/Hlikias/csp/blob/main/demo/QQ20260309-015818.png?raw=true) | ![写入的一些数据](https://github.com/Hlikias/csp/blob/main/demo/QQ20260309-015911.png?raw=true) |

</div>

> ⚠️ 注：最后一张图片的链接中 `raw=tru` 已修正为 `raw=true`。

---

## 🚀 如何运行

1. 克隆仓库  
2. 使用 Qt Creator 打开项目文件（`.pro` 或 `CMakeLists.txt`）  
3. 配置 Qt 版本（要求 Qt 5 或 Qt 6）  
4. 编译运行，选择客户端或服务器模式  

---

## 📌 后续计划

- [ ] 融合串口与 USB 通信功能  
- [ ] 完善界面与用户体验  
- [ ] 增加文件传输与消息记录  

---

<div align="center">

**欢迎 Star ⭐ 或提供改进建议！**

</div>
