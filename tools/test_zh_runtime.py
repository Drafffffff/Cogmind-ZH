#!/usr/bin/env python3
"""Exercise real upstream C matching and wrapping against every Chinese rule."""
import json
from pathlib import Path
import re
import shutil
import subprocess

from validate_translations import ROOT, UPSTREAM, rows, validate


def c_string(s):
    return '"' + ''.join(f'\\{b:03o}' for b in s.encode('utf-8')) + '"'


def expand(src, dst):
    tokens = re.findall(r'\{(n|\*|[1-9])\}', src)
    values = [str(42+i) if t == 'n' else f'Grunt{i}' for i, t in enumerate(tokens)]
    pos = iter(values)
    sample = re.sub(r'\{(n|\*|[1-9])\}', lambda _: next(pos), src)
    pos = iter(values)
    expected = re.sub(r'\{(n|\*|[1-9])\}',
                      lambda m: values[int(m[1])-1] if m[1].isdigit() else next(pos), dst)
    return sample, expected


def main():
    report = validate()
    build = ROOT/'build/zh-tests'
    build.mkdir(parents=True, exist_ok=True)
    entries = []
    for table in ['trans', 'para']:
        for src, dst in rows(ROOT/f'build/tables/zh_{table}.tsv').items():
            sample, expected = expand(src, dst)
            entries.append('{'+', '.join(c_string(x) for x in [src, dst, sample, expected])+'}')
    code = '#include <stdio.h>\n#include "cmkr_core.h"\n'
    code += 'struct Entry {const char *src,*dst,*sample,*expected;};\n'
    code += 'static struct Entry entries[] = {\n'+',\n'.join(entries)+'\n};\n'
    code += r'''
int main(void) {
    size_t i; int count=0;
    for (i=0;i<sizeof(entries)/sizeof(entries[0]);i++) {
        Rule rule; Caps caps; int offsets[MAX_TOK]; char out[8192];
        struct Entry *e=&entries[i];
        if (!cm_rulebuild(&rule,e->src,e->dst)) {printf("build failure %zu\n",i);return 1;}
        if (!cm_match(&rule,e->sample,(int)strlen(e->sample),&caps,offsets)) {
            printf("match failure %zu: %s\n",i,e->sample);return 1;
        }
        cm_buildfree(&rule,e->sample,&caps,out,sizeof out);
        if(strcmp(out,e->expected)) {printf("capture failure %zu: %s\n",i,out);return 1;}
        count++;
    }
    if(cm_cells("核心耐久 100/100")!=16 || cm_wide(0x6838)!=1 || cm_wide(0x2500)!=0) {
        puts("Chinese cell width failure");return 1;
    }
    {
        const char *original="中文换行测试核心耐久物质能量";
        int widths[]={2,4,6,10,16}; int w;
        for(w=0;w<5;w++) {
            const char *p=original; char line[128], joined[512]=""; int guard=0;
            while(*p) {
                const char *next=cm_wrap(p,widths[w],line,sizeof line);
                if(next<=p || cm_cells(line)>widths[w] || ++guard>100) {
                    puts("Chinese wrapping progress/width failure");return 1;
                }
                strcat(joined,line); p=next;
            }
            if(strcmp(joined,original)) {puts("Chinese wrapping lost text");return 1;}
        }
    }
    printf("PASS: %d translation matches/captures; CJK widths; 5 Chinese wrapping widths\n",count);
    return 0;
}
'''
    source = build/'test_zh.c'
    source.write_text(code, encoding='utf-8')
    compiler = shutil.which('clang') or shutil.which('cc')
    if not compiler:
        raise SystemExit('C compiler not found')
    binary = build/'test_zh'
    subprocess.run([compiler, '-O1', '-I', str(UPSTREAM), str(source), '-o', str(binary)], check=True)
    result = subprocess.run([str(binary)], check=True, capture_output=True, text=True)
    report['runtime_core_test'] = result.stdout.strip()
    (ROOT/'analysis/zh-runtime-tests.json').write_text(json.dumps(report, ensure_ascii=False, indent=2)+'\n')
    print(result.stdout, end='')


if __name__ == '__main__':
    main()
