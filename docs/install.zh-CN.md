# 安装与回滚

适用版本：Cogmind Beta 17.1 / 260906a。已评估原版 `COGMIND.exe` 的 SHA-256：

```text
6d24c525c12de38f3fc21017b023f22ea6d3ec94e3b160d83ef0570327701666
```

1. 正常退出游戏。在 Steam 的游戏属性中选择「已安装文件 → 浏览」。
2. 建立一个备份文件夹，将现有 `manual.txt`，以及已有的 `WININET.dll`、`zh_trans.tsv`、`zh_para.tsv`、`FONT-LICENSE.txt` 复制进去；记下哪些文件原先不存在。
3. 解压预览包，将上述五个同名文件复制到 `COGMIND.exe` 同级目录，保留包中的 `manifest.json` 和字体许可证供核对。
4. Windows 可直接启动游戏。Steam Proton / Bazzite 需要让 Proton 加载本地 wininet DLL：在启动选项中使用 `WINEDLLOVERRIDES="wininet=n,b" %command%`。如果已有启动选项，请合并此环境变量并保留原参数；已有等效 wininet 覆盖设置可沿用。
5. 启动后查看 `kr_log.txt`，应出现版本 `v102-zh-pixel.34-log-scroll-preview`。

Caps Lock 可切换中文覆盖层。此预览包包含中文手册代理文件，手册需要与对应 DLL 一起安装和回滚。

回滚时先正常退出游戏，然后恢复备份文件；删除安装前不存在的补丁文件，并恢复原 Steam 启动选项。保留游戏原版 EXE 和存档。

目前主要实机测试为 160×45 网格、24×48 单元，3840×2160 窗口。其他字体尺寸、版本和分辨率需要继续验证。已知问题见项目首页和发布说明。
