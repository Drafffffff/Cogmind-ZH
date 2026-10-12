#!/usr/bin/env python3
"""Build a local Chinese test DLL without installing or changing the upstream."""
import hashlib
import json
from pathlib import Path
import re
import shutil
import subprocess
import zipfile
import argparse

from validate_translations import ROOT, UPSTREAM, validate
from build_pixel_font import build as build_pixel_font, check_coverage

PIN = 'c105ef104137954887273c80e79fdb05841f681c'


def replace_once(text, old, new):
    if text.count(old) != 1:
        raise ValueError(f'Upstream patch anchor changed: {old[:60]}')
    return text.replace(old, new)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--manual-layout', action='store_true',
                        help='Build a separate manual layout preview; never install it')
    parser.add_argument('--animation-layout', action='store_true',
                        help='Include the experimental intro provider in the separate manual preview')
    args = parser.parse_args()
    if args.animation_layout:
        args.manual_layout = True
    version = 'v102-zh-pixel.3-manual-preview' if args.manual_layout else 'v102-zh-pixel.2'
    if args.animation_layout:
        version = 'v102-zh-pixel.34-log-scroll-preview'
    report = validate()
    commit = subprocess.check_output(['git', '-C', str(UPSTREAM.parent),
                                      'rev-parse', 'HEAD'], text=True).strip()
    if commit != PIN:
        raise ValueError('Unexpected upstream commit; review adaptation before building')
    build = ROOT/('build/zh-runtime-manual-preview' if args.manual_layout else 'build/zh-runtime')
    output = ROOT/('output/Cogmind-ZH-manual-preview' if args.manual_layout else 'output/Cogmind-ZH-test')
    build.mkdir(parents=True, exist_ok=True)
    output.mkdir(parents=True, exist_ok=True)
    for name in ['cmkr_core.h', 'cmkr_font.h', 'cmkr_wininet.def']:
        shutil.copy2(UPSTREAM/name, build/name)
    glyphs = build_pixel_font(build)
    check_coverage(ROOT/'build/tables',glyphs)
    if args.manual_layout:
        from build_manual_layout import build as build_manual_layout
        build_manual_layout()
        manual = json.loads((ROOT/'build/zh-manual/manifest.json').read_text())
        required = {ord(c) for row in manual['records'] for c in row['translation']}
        missing = sorted(required - set(glyphs))
        if missing:
            raise ValueError('Missing manual pixel glyphs: '+repr(missing))
        report['manual_required_pixel_glyphs'] = len(required)
        report['manual_layout'] = {k: v for k, v in manual.items() if k != 'records'}
        for name in ['zh_manual.h', 'zh_manual_plan.h', 'zh_animation_probe.h',
                     'zh_clipped_literal.h', 'zh_tooltip_plan.h']:
            shutil.copy2(ROOT/'runtime'/name, build/name)
        shutil.copy2(ROOT/'build/zh-manual/zh_manual_data.h', build/'zh_manual_data.h')
        shutil.copy2(ROOT/'build/zh-manual/manual.txt', output/'manual.txt')
    source = (UPSTREAM/'cmkr.c').read_text(encoding='utf-8-sig')
    if args.animation_layout:
        from build_animation_layout import build as build_animation_layout
        report['animation_layout'] = build_animation_layout(build)
        from build_hud_layout import build as build_hud_layout
        report['hud_layout'] = build_hud_layout(build)
        for name in ['zh_hud_draw.h','zh_manual_draw.h','zh_short_reveal.h',
                     'zh_child_text_draw.h','zh_child_text_hook.h','zh_advanced_help_draw.h','zh_child_paragraph_draw.h','zh_raw_paragraph_draw.h','zh_robot_reveal_draw.h','zh_decode_state.h','zh_records_draw.h','zh_menu_op_draw.h','zh_log_layout.h','zh_literal_match.h','zh_capture_names.h','zh_border_gap.h','zh_log_paragraph.h']:
            shutil.copy2(ROOT/'runtime'/name, build/name)
        noise = 'ㄅㄆㄇㄈㄉㄊㄋㄌㄍㄎㄏㄐㄑㄒㄓㄔㄕㄖㄗㄘㄙㄚㄛㄜㄝㄞㄟㄠㄡㄢㄣㄤㄥㄦㄧㄨㄩ'
        if {ord(c) for c in noise} - set(glyphs):
            raise ValueError('Animation Zhuyin glyphs missing from embedded atlas')
        shutil.copy2(ROOT/'runtime/zh_animation_draw.h', build/'zh_animation_draw.h')
        shutil.copy2(ROOT/'runtime/zh_hud_probe.h', build/'zh_hud_probe.h')
        shutil.copy2(ROOT/'runtime/zh_hud_grid_probe.h', build/'zh_hud_grid_probe.h')
        source = '#define ZH_ANIMATION_LAYOUT 1\n' + source
        source = replace_once(source, 'static void OnFrame(SSurf *sf)',
                              '#include "zh_animation_draw.h"\n#include "zh_hud_grid_probe.h"\n#include "zh_hud_draw.h"\n#include "zh_manual_draw.h"\n#include "zh_short_reveal.h"\n#include "zh_menu_op_draw.h"\n#include "zh_child_text_draw.h"\n#include "zh_advanced_help_draw.h"\n#include "zh_child_paragraph_draw.h"\n#include "zh_raw_paragraph_draw.h"\n#include "zh_robot_reveal_draw.h"\n#include "zh_records_draw.h"\n\nstatic void OnFrame(SSurf *sf)')
        source = replace_once(source, '    }   /* if (g_on) 끝 */',
                              '    }   /* if (g_on) 끝 */\n    ObserveZhHudGrid();\n    DrawZhAnimations(sf);\n    DrawZhHudMessages(sf);\n    if(g_on) { DrawZhRawShortRows(sf); DrawZhManualRows(sf); DrawZhChildTexts(sf); DrawZhAdvancedHelpContexts(sf); DrawZhRawParagraphs(sf); DrawZhRobotReveal(sf); DrawZhRecordsTitles(sf); DrawZhOptionsLabels(sf); }')
        source = replace_once(source, 'static void PlanParagraphs(void)',
                              '#include "zh_log_layout.h"\n'
                              'static int ZhLogRunBlockCanReflow(const int *idx,int nb,int same_key)\n'
                              '{ int cells[8],i; if(!same_key||nb<2||nb>8)return 0;\n'
                              '  for(i=0;i<nb;++i){const Run*r;if(idx[i]<0||idx[i]>=g_nrun)return 0;\n'
                              '    r=&g_runs[idx[i]];if(r->f!=1||r->vs!=1)return 0;\n'
                              '    cells[i]=r->row*g_grid.w+r->x0;}\n'
                              '  return ZhLogBlockCanReflowCandidate(cells,nb,1,1,same_key); }\n'
                              'static void PlanParagraphs(void)')
        source = replace_once(source, '        char svk, svk0;',
                              '        char svk, svk0;\n        int zh_log_merged_candidate;')
        source = replace_once(source, '        ksig  = cm_sig(key,  klen);',
                              '        zh_log_merged_candidate=ZhLogRunBlockCanReflow(idx,nb,klen==klen0 && !memcmp(key,key0,(size_t)klen));\n        ksig  = cm_sig(key,  klen);')
        source = replace_once(source, 'if (!use0 && !merged) continue;',
                              'if (!use0 && !merged && !zh_log_merged_candidate) continue;')
    if args.animation_layout:
        source = replace_once(source,
            '            /* ★ v22 - 먼저 들어가는지만 재본다.',
            '            /* Exact matched log blocks own the same 48-cell text area\n               regardless of continuation indentation or adjacent panel. */\n            if(ZhLogRunBlockCanReflow(idx,nb,1)){ int zh_line;\n                for(zh_line=0;zh_line<nb;++zh_line){dx0[zh_line]=1;dw[zh_line]=48;}\n                zh_log_layout=1; }\n            /* ★ v22 - 먼저 들어가는지만 재본다.')
    if args.animation_layout:
        source = replace_once(source,
            '            Caps cp; int litOfs[MAX_TOK];\n            const char *p;',
            '            Caps cp; int litOfs[MAX_TOK]; int zh_log_layout=0;\n            const char *p;')
        if source.count('(use0 ? rr->len : dw[ln])')!=2:
            raise ValueError('Paragraph width anchors changed')
        source = source.replace('(use0 ? rr->len : dw[ln])',
                                '((use0&&!zh_log_layout) ? rr->len : dw[ln])')
        source = replace_once(source, 'int w  = use0 ? rr->len : dw[line];',
                              'int w  = (use0&&!zh_log_layout) ? rr->len : dw[line];')
        source = replace_once(source, 'int xx = use0 ? rr->x0  : dx0[line];',
                              'int xx = (use0&&!zh_log_layout) ? rr->x0 : dx0[line];')
    if args.animation_layout:
        source = replace_once(source,
            'static void __fastcall hk_blit(void *self, void *edx, void *dst, void *rect)',
            '#include "zh_child_text_hook.h"\nstatic void __fastcall hk_blit(void *self, void *edx, void *dst, void *rect)')
    if args.animation_layout:
        source = replace_once(source, '    char t[MAX_TEXT];\n} Op;',
                              '    char t[MAX_TEXT];\n    int zhReveal;\n} Op;')
        source = replace_once(source, '    o->big = (f > 1);',
                              '    o->big = (f > 1);\n    o->zhReveal = 0;')
        source = replace_once(source, '    o->big = 0;',
                              '    o->big = 0;\n    o->zhReveal = 0;')
        begin=source.index('static void PlanShort(void)')
        end=source.index('static void PlanParaSingle(void)',begin)
        short=source[begin:end]
        short=replace_once(short,'                Caps  cp;',
                           '                int zh_first_op=g_nop;\n                Caps  cp;')
        short=replace_once(short,'                r->used = 1;\n                break;',
                           '                if(R->ncap==0 && g_nop==zh_first_op+1 && r->f==1 && r->vs==1 &&\n                   g_ops[zh_first_op].col==r->x0 && g_ops[zh_first_op].cells==r->len)\n                    g_ops[zh_first_op].zhReveal=1;\n                r->used = 1;\n                break;')
        source=source[:begin]+short+source[end:]
        source=replace_once(source,
            '            DrawSpan(sf, o->col, o->row, o->cells, o->vs, o->big,\n                     o->bgSet ? bgc : 0, o->t, o->fr, o->fg, o->fb);',
            '            if(!DrawZhShortReveal(sf,o) && !DrawZhMenuOpReveal(sf,o))\n                DrawSpan(sf, o->col, o->row, o->cells, o->vs, o->big,\n                     o->bgSet ? bgc : 0, o->t, o->fr, o->fg, o->fb);')
    if args.animation_layout:
        source = replace_once(source, 'static int CapXlatN(const char *s, int n, int budget, char *out, int outsz)',
                              '#include "zh_capture_names.h"\nstatic int CapXlatN(const char *s, int n, int budget, char *out, int outsz)')
        cap_start=source.index('static int CapXlatN(')
        cap_end=source.index('static int CapXlat(const char *s, int n, char *out, int outsz)',cap_start)
        cap_part=source[cap_start:cap_end]
        pos=cap_part.rfind('    return 0;')
        if pos<0: raise ValueError('Capture fallback changed')
        cap_part=cap_part[:pos]+'    return ZhCaptureNameComposite(s,n,budget,out,outsz);'+cap_part[pos+len('    return 0;'):]
        source=source[:cap_start]+cap_part+source[cap_end:]
        source=replace_once(source, 'static void FillGaps(void)\n{',
                            '#include "zh_border_gap.h"\nstatic void FillGaps(void)\n{')
        source=replace_once(source,'IsBox(o->col - 1, row) && IsBox(nx, row)',
                            'ZhBorderGapAllowed(o->col - 1, row, nx)')
    if args.animation_layout:
        source=replace_once(source,'static void PlanParagraphs(void)',
                            '#include "zh_log_paragraph.h"\nstatic void PlanParagraphs(void)')
        source=replace_once(source,'        PlanParagraphs();',
                            '        PlanZhLogParagraphs();\n        PlanParagraphs();')
        reload_anchor='        BuildDict();\n        g_lastHash = 0;'
        if source.count(reload_anchor)!=2:raise ValueError('Reload invalidation anchors changed')
        source=source.replace(reload_anchor,'        BuildDict();\n        ZhLogInvalidateIndex();\n        g_lastHash = 0;')
    source = replace_once(source, 'static Glyph  *g_gtab[4096];', '#include "zh_pixel.h"\nstatic Glyph  *g_gtab[4096];')
    source = replace_once(source, '#define CMKR_VER  "v102"',
                          '#define CMKR_VER  "'+version+'"')
    if args.manual_layout:
        source = replace_once(source, 'static int PlanParaPartial(const int *idx, int nblk, const char *key, int klen)',
                              '#include "zh_clipped_literal.h"\n\nstatic int PlanParaPartial(const int *idx, int nblk, const char *key, int klen)')
        start = source.index('static int PlanParaPartial(')
        end = source.index('/* ★ v30', start)
        partial = source[start:end]
        partial = replace_once(partial, '    if (nblk < 1 || klen < 40) return 0;',
                               '    if (nblk < 1 || !ZhPartialLiteralAllowed(key, klen)) return 0;')
        partial = replace_once(partial, '    Rule *hit = 0;', '    Rule *hit = 0;\n    Rule hitStorage;')
        partial = replace_once(partial, '        Rule       *R = &g_pa[j];',
                               '        Rule candidate;\n        Rule *R = &candidate;\n        if (!ZhPartialLiteral(&g_pa[j], R)) continue;')
        partial = replace_once(partial, '        hit = R; hitHead = head;',
                               '        hitStorage = *R; hit = &hitStorage; hitHead = head;')
        partial = replace_once(partial, '    if (!hit) return 0;',
                               '    if (!hit) return 0;\n    ZhPartialLiteralTargetOverride(key, klen, hit);')
        source = source[:start] + partial + source[end:]
        source = replace_once(source, '        if (!r->used && r->len >= 40) {',
                              '        if (!r->used && ZhPartialLiteralAllowed(r->t, r->len)) {')
        source = replace_once(source, 'static void PlanShort(void)',
                              '#include "zh_tooltip_plan.h"\n\nstatic void PlanShort(void)')
        source = replace_once(source, '        PlanShort();',
                              '        PlanShort();\n        PlanZhNameTooltips();')
        source = replace_once(source, 'static DWORD WINAPI Init(LPVOID p)',
                              '#include "zh_animation_probe.h"\n\nstatic DWORD WINAPI Init(LPVOID p)')
        source = replace_once(source, '    if (!InstallBlitHook())',
                              '    InstallZhAnimationProbe();\n    if (!InstallBlitHook())')
    if args.animation_layout:
        source = replace_once(source, '#include "zh_animation_probe.h"',
                              '#include "zh_animation_probe.h"\n#include "zh_hud_probe.h"')
        source = replace_once(source, '    InstallZhAnimationProbe();',
                              '    InstallZhAnimationProbe();\n    InstallZhHudProbe();')

        source = replace_once(source, 'static int IsNoiseRun(const char *s, int n)',
                              '#include "zh_manual.h"\n\nstatic int IsNoiseRun(const char *s, int n)')
        source = replace_once(source, '    int i, dig = 0, al = 0;',
                              '    int i, dig = 0, al = 0;\n    if (ZhManualDecode(s, (size_t)n)) return 0;')
        source = replace_once(source, 'static void PlanParagraphs(void)',
                              '#include "zh_manual_plan.h"\n\nstatic void PlanParagraphs(void)')
        source = replace_once(source, '        PlanParagraphs();',
                              '        PlanZhManual();\n        PlanParagraphs();')
    source = replace_once(source, 'static const wchar_t *KO_FACE[] = {',
                          'static const wchar_t *KO_FACE[] = {\n'
                          '    L"Noto Sans CJK SC", L"Noto Sans SC",\n'
                          '    L"Source Han Sans SC", L"Microsoft YaHei", L"SimHei",')
    match = re.search(r'static const wchar_t KO_REF\[\] = \{.*?\};', source, re.S)
    if not match:
        raise ValueError('Font reference not found')
    reference = ', '.join(hex(ord(c)) for c in '汉字物质能量核心耐久警报装备')
    source = replace_once(source, match.group(),
                          f'static const wchar_t KO_REF[] = {{ {reference}, 0 }};')
    source = replace_once(source, 'lf.lfCharSet = HANGEUL_CHARSET;',
                          'lf.lfCharSet = GB2312_CHARSET;')
    source = replace_once(source, '    g_mdc = CreateCompatibleDC(NULL);',
                          '    L("[font] Fusion Pixel 12px zh_Hans: %u glyphs, cell=%dx%d", (unsigned)ZH_PIXEL_COUNT, cw, ch);\n'
                          '    return 1; /* Embedded bitmap atlas: no system fonts required. */\n'
                          '    g_mdc = CreateCompatibleDC(NULL);')
    source = replace_once(source, "    if (cp < 0x10000u) { w[0] = (wchar_t)cp; } else { w[0] = L'?'; }", r'''
    {
        const ZhPixel *pixel = ZhPixelFind(cp);
        if (!pixel) pixel = ZhPixelFind('?');
        if (!pixel) return NULL;
        g = (Glyph *)calloc(1, sizeof(Glyph));
        if (!g) return NULL;
        g->cp=cp; g->w=boxW; g->h=g_cellH;
        g->a=(unsigned char *)calloc(1,(size_t)boxW*g_cellH);
        if (!g->a) {free(g);return NULL;}
        ZhPixelRaster(pixel,boxW,g_cellH,g->a);
        g->next=g_gtab[slot];g_gtab[slot]=g;
        return g;
    }
    if (cp < 0x10000u) { w[0] = (wchar_t)cp; } else { w[0] = L'?'; }
''')
    source = source.replace('kr_trans.tsv', 'zh_trans.tsv').replace('kr_para.tsv', 'zh_para.tsv')
    source = replace_once(source,
        '한글 렌더 테스트 - 물질 207/300 받침 잘림 확인 뭉쫓짧',
        '中文显示测试 - 物质 207/300 核心耐久 标点，。！？')
    for old, new in {
        '[한글표기 ON]': '[中文 ON]', '[한글표기 OFF]': '[中文 OFF]',
        '[렌더테스트]': '[显示测试]', '[수집 ON]': '[采集 ON]',
        '[수집 OFF]': '[采集 OFF]', '[원시 덤프]': '[原始转储]',
        '[그리드 덤프]': '[网格转储]', '[링크: 배경]': '[链接: 背景]',
        '[링크: 글자색]': '[链接: 字色]', '[링크 앵커 ON]': '[链接锚点 ON]',
        '[링크 앵커 OFF]': '[链接锚点 OFF]',
    }.items():
        source = replace_once(source, old, new)
    if args.animation_layout:
        source = replace_once(source, '    InstallZhAnimationProbe();',
                              '    InstallZhAnimationProbe();\n    InstallZhChildTextHooks();')
    (build/'cmkr.c').write_text(source, encoding='utf-8')
    zig = shutil.which('zig')
    if not zig:
        raise SystemExit('zig compiler not found')
    subprocess.run([zig, 'cc', '-target', 'x86-windows-gnu', '-O2', '-shared',
                    '-o', str(output/'WININET.dll'), str(build/'cmkr.c'),
                    str(build/'cmkr_wininet.def'), '-lkernel32', '-luser32', '-lgdi32'],
                   check=True)
    for name in ['zh_trans.tsv', 'zh_para.tsv']:
        shutil.copy2(ROOT/'build/tables'/name, output/name)
    (output/'FONT-LICENSE.txt').write_text('\n\n'.join(str(p.relative_to(ROOT/'fonts/licenses'))+'\n'+p.read_text() for p in sorted((ROOT/'fonts/licenses').rglob('*.txt'))))
    manifest = {'upstream_commit': commit, 'version': version,
                'validation': report, 'files': {}}
    for name in ['WININET.dll', 'zh_trans.tsv', 'zh_para.tsv', 'FONT-LICENSE.txt']:
        manifest['files'][name] = hashlib.sha256((output/name).read_bytes()).hexdigest()
    if args.manual_layout:
        manifest['files']['manual.txt'] = hashlib.sha256((output/'manual.txt').read_bytes()).hexdigest()
    (output/'manifest.json').write_text(json.dumps(manifest, ensure_ascii=False, indent=2)+'\n')
    analysis = 'zh-manual-preview-build.json' if args.manual_layout else 'zh-build.json'
    (ROOT/'analysis'/analysis).write_text(json.dumps(manifest, ensure_ascii=False, indent=2)+'\n')
    instructions = 'install.zh-CN.md' if args.animation_layout else 'test-pack.zh-CN.md'
    shutil.copy2(ROOT/'docs'/instructions, output/'说明.md')
    shutil.copytree(ROOT/'fonts/licenses', output/'font-licenses', dirs_exist_ok=True)
    archive = ROOT/('output/Cogmind-ZH-manual-preview.zip' if args.manual_layout else 'output/Cogmind-ZH-pixel.2.zip')
    with zipfile.ZipFile(archive, 'w', zipfile.ZIP_DEFLATED) as z:
        for name in ['WININET.dll', 'zh_trans.tsv', 'zh_para.tsv', 'manifest.json', '说明.md', 'FONT-LICENSE.txt']:
            z.write(output/name, arcname=name)
        if args.manual_layout:
            z.write(output/'manual.txt', arcname='manual.txt')
        for path in sorted((output/'font-licenses').rglob('*.txt')):
            z.write(path, arcname=str(path.relative_to(output)))
    print(output)
    print(archive)


if __name__ == '__main__':
    main()
