# Cogmind 中文像素字体测试包 pixel.2

目标：Beta 17.1，build 260906a，Bazzite + Proton。当前译表覆盖全部已知上游清单和登记资源，原生动态文本仍在补充，未覆盖文本保留英文。规则数包含同一物品的大小写、数量、感叹号和信息窗变体，不能当作剧情完成率。

## 本版

- 内嵌 Fusion Pixel 12px 简体中文像素字库，中文占两格、ASCII 占一格；无需安装系统字体。
- 按整数倍放大并居中，无抗锯齿。小于字库原始尺寸时使用最近邻缩放。10×20 网格的 ASCII 保留已有原生字形适配。
- Luna 已完成本轮全部已知清单的说明、成就和对白；物品/机器人名由独立中文词典生成已有英文匹配变体，人工基础译文优先。
- 构建前检查匹配原文、占位符、标记、宽度、字库覆盖；运行真实 C 匹配与换行测试。译文仍需结合剧情和游玩审校。

## 文件与使用

`WININET.dll`、`zh_trans.tsv`、`zh_para.tsv` 和 `FONT-LICENSE.txt` 放在 `COGMIND.exe` 同级目录。`manifest.json` 记录版本和四文件 SHA-256。安装脚本在游戏正常退出后执行，备份旧文件；Cogmind 专用 `wininet=native,builtin` 覆盖设置使 Proton 加载本地 DLL。

```sh
python3 tools/install_remote.py --uninstall  # 已装旧版本时先回滚
python3 tools/install_remote.py             # 安装当前构建
```

禁止在游戏运行时替换 DLL。卸载使用同一脚本的 `--uninstall`，会检查文件是否被改动，恢复原文件和原有 Cogmind 专用覆盖值。原版 EXE 与 Steam 启动参数不改动。

加载日志仍名为 `kr_log.txt`，版本应为 `v102-zh-pixel.2`，字体日志包含 `Fusion Pixel 12px zh_Hans`。

- Caps Lock：切换中文覆盖层。
- Scroll Lock：中文字体显示测试。
- Num Lock：切换文本采集。
- F11：保存网格转储。

常用汉字占两格，四字节 Unicode 暂不支持。中文标点避头尾尚未实现，链接点击、全部窗口尺寸和长时间稳定性仍需继续游玩验证。

字体来源：[TakWolf/fusion-pixel-font](https://github.com/TakWolf/fusion-pixel-font)，2026.09.25 的 12px monospaced zh_hans BDF，OFL-1.1，许可证及第三方声明随包附带。

补丁基础：[magicianaos/KR-AOS-Cogmind](https://github.com/magicianaos/KR-AOS-Cogmind)，固定提交 `c105ef104137954887273c80e79fdb05841f681c`。未找到明确许可证；目前用于用户本地研究，公开发布前需厘清上游授权。游戏版权归 Grid Sage Games。
