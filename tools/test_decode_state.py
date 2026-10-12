#!/usr/bin/env python3
"""Host-test the bounded, visibility-fed decode phase state."""
from __future__ import annotations

import hashlib
import json
import shutil
import subprocess
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
HELPER = ROOT / 'runtime/zh_decode_state.h'
REPORT = ROOT / 'analysis/decode-state-tests.json'

TEST_C = r'''
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "zh_decode_state.h"

static void clear_all(void){memset(zh_decode_entries,0,sizeof zh_decode_entries);zh_decode_clock=0;}
static int active_count(void){int n=0;unsigned i;for(i=0;i<ZH_DECODE_STATE_CAP;++i)n+=zh_decode_entries[i].active!=0;return n;}
int main(void)
{
    unsigned t;int i;
    clear_all();
    if(active_count()!=0)return 1; /* idle: caller does not ask for invisible text */
    if(ZhDecodeGlyphReady(1,4,5,10,1,"目标文字",1000,10,0,4))return 2;
    if(active_count()!=1)return 3;
    /* A visible source is the caller's only gate; no palette/uniform input is required. */
    if(ZhDecodeGlyphReady(1,4,5,10,1,"目标文字",1001,11,0,4))return 4;
    if(ZhDecodeGlyphReady(1,4,5,10,1,"目标文字",1001,11,3,4))return 5;
    t=zh_decode_entries[0].duration_ms;
    if(t<180||t>360)return 6;
    if(!ZhDecodeGlyphReady(1,4,5,10,1,"目标文字",1000+t,12,3,4))return 7;
    if(!ZhDecodeGlyphReady(1,4,5,10,1,"目标文字",1000+t,13,3,4))return 8; /* settled stays done */
    /* A skipped frame models one or more invisible frames; re-entry restarts. */
    if(ZhDecodeGlyphReady(1,4,5,10,1,"目标文字",5000,30,3,4))return 9;
    if(ZhDecodeGlyphReady(1,4,5,10,1,"目标文字",5001,31,3,4))return 10;
    /* Same rectangle with different target text is a distinct decode. */
    if(ZhDecodeGlyphReady(1,4,5,10,1,"新文本",5002,32,0,3))return 11;
    if(ZhDecodeGlyphReady(1,4,5,10,1,"目标文字",5003,33,3,4))return 12; /* A→B→A restarts A */
    if(active_count()!=1)return 24;
    /* Different family/rectangle does not inherit a completed phase. */
    if(ZhDecodeGlyphReady(2,4,5,10,1,"目标文字",9000,50,0,4))return 13;
    /* Millisecond counter wrap must use modulo-2^32 elapsed arithmetic. */
    clear_all();
    if(ZhDecodeGlyphReady(7,1,1,12,1,"回绕测试",0xfffffff0u,100,0,4))return 14;
    t=zh_decode_entries[0].duration_ms;
    if(!ZhDecodeGlyphReady(7,1,1,12,1,"回绕测试",(unsigned)(0xfffffff0u+t),101,3,4))return 15;
    /* Bounded capacity: insert unique keys, then ensure the oldest can be evicted. */
    clear_all();
    for(i=0;i<ZH_DECODE_STATE_CAP;++i)
        (void)ZhDecodeGlyphReady(9,i,0,1,1,"x",100u+(unsigned)i,200u+(unsigned)i,0,1);
    if(active_count()!=ZH_DECODE_STATE_CAP)return 17;
    (void)ZhDecodeGlyphReady(9,1000,0,1,1,"x",1000,300,0,1);
    if(active_count()!=ZH_DECODE_STATE_CAP)return 19;
    for(i=0;i<ZH_DECODE_STATE_CAP;++i)
        if(zh_decode_entries[i].active&&zh_decode_entries[i].x==0)return 20;
    /* Frame counter wrap is sequential and must not restart the phase. */
    clear_all();
    if(ZhDecodeGlyphReady(3,2,2,8,1,"帧回绕",200,0xfffffffeu,0,4))return 21;
    if(ZhDecodeGlyphReady(3,2,2,8,1,"帧回绕",201,0xffffffffu,0,4))return 22;
    if(ZhDecodeGlyphReady(3,2,2,8,1,"帧回绕",202,0u,0,4))return 23;
    /* Paragraph targets must retain their full identity beyond 256 bytes. */
    { char text[ZH_DECODE_TEXT_MAX+1];
      memset(text,'a',sizeof text);text[ZH_DECODE_TEXT_MAX-1]=0;clear_all();
      if(ZhDecodeGlyphReady(10,52,28,45,8,text,100,1,10,20))return 25;
      if(!ZhDecodeGlyphReady(10,52,28,45,8,text,500,2,19,20))return 26;
      text[ZH_DECODE_TEXT_MAX-1]='b';text[ZH_DECODE_TEXT_MAX]=0;
      if(ZhDecodeGlyphReady(10,52,28,45,8,text,501,3,19,20))return 27;
    }
    puts("PASS: idle/visible, bounded decode, stable completion, gap and text reset, family isolation, capacity eviction, and tick/frame wrap");
    return 0;
}
'''


def main() -> None:
    compiler = shutil.which('clang') or shutil.which('cc')
    if not compiler:
        raise RuntimeError('clang or cc is required')
    with tempfile.TemporaryDirectory(prefix='zh-decode-state-') as td:
        folder = Path(td)
        source = folder / 'test.c'
        binary = folder / 'test'
        source.write_text(TEST_C, encoding='utf-8')
        build = subprocess.run([compiler, '-std=c11', '-O1', '-Wall', '-Wextra',
                                '-I', str(ROOT / 'runtime'), str(source), '-o', str(binary)],
                               text=True, capture_output=True)
        if build.returncode:
            raise RuntimeError('compile failed:\n' + build.stdout + build.stderr)
        run = subprocess.run([str(binary)], text=True, capture_output=True)
        if run.returncode:
            raise RuntimeError(f'test failed ({run.returncode}):\n' + run.stdout + run.stderr)
        print(run.stdout, end='')
    digest = hashlib.sha256(HELPER.read_bytes()).hexdigest()
    report = {
        'status': 'passed',
        'helper_sha256': digest,
        'scope': [
            'Idle state and caller-gated visible samples.',
            'Per-glyph progress over a deterministic 180–360 ms bounded duration.',
            'No color/uniformity dependency; stable completion remains decoded.',
            'Nonconsecutive-frame re-entry and changed target text reset progress.',
            'Same-rectangle A→B→A invalidation, independent family/rectangle keying, and 512-entry oldest eviction; full target identity up to 1023 bytes, oversize rejected.',
            'Modulo-2^32 GetTickCount arithmetic and sequential frame-counter wrap.'
        ],
        'limits': [
            'Offline C helper fixture; no production integration or game acceptance was performed.',
            'Caller must suppress all drawing when its native source is not visible and must provide a monotonically advancing frame counter.'
        ],
        'result': run.stdout.strip(),
        'test_command': 'python3 tools/test_decode_state.py'
    }
    REPORT.parent.mkdir(parents=True, exist_ok=True)
    REPORT.write_text(json.dumps(report, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
    print(f'Report: {REPORT}')


if __name__ == '__main__':
    main()
