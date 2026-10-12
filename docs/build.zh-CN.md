# 构建预览版

依赖：Python 3、Git、Zig、C 编译器（clang 或 cc）。构建脚本只生成本地文件。

```sh
python3 tools/setup_build_inputs.py --game-dir /path/to/Cogmind
python3 tools/build_zh.py --animation-layout
python3 tools/test_zh_runtime.py
python3 tools/test_pixel_font.py
```

设置脚本克隆 AOS 上游，固定到 `c105ef104137954887273c80e79fdb05841f681c`；下载 Fusion Pixel `2026.09.25` 的 12px 单宽 BDF；把自己的 `manual.txt` 复制到 `original/bazzite/manual.txt`，并核对对应版本的哈希。

输出目录为 `output/Cogmind-ZH-manual-preview/`，压缩包为 `output/Cogmind-ZH-manual-preview.zip`。默认不带 `--animation-layout` 的构建属于旧稳定路径，不是当前 GitHub 预览发行。

`translations/` 保存人工基础表、Luna 翻译批次、术语和名称词典；合并结果由 `compile_translations.py` 生成。`runtime/` 保存中文绘制辅助代码；`build_zh.py` 根据严格锚点修改固定上游源码并编译 DLL。字体许可位于 `fonts/licenses/`。

维护者的部分回归测试引用本地实机网格采样，需要另外提供夹具；发行验证记录区分宿主测试与实机验收。
