#!/usr/bin/env python3
"""Read-only paired child/root palette samples for the isolated label probe."""
import json, subprocess, hashlib, argparse, re
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
REMOTE=r'''
import os,re,struct,time,json,hashlib
from pathlib import Path
ps=[]
for p in Path('/proc').iterdir():
 if not p.name.isdigit():continue
 try:a=(p/'cmdline').read_bytes().split(b'\0')
 except OSError:continue
 if any(re.fullmatch(rb'[ZS]:.*COGMIND\.exe',v,re.I) for v in a):
  if not any(b'Cogmind-ZH-sandbox' in v for v in a):raise SystemExit('Main game is running')
  ps.append(p)
if len(ps)!=1:raise SystemExit('Expected unique sandbox process')
base=Path(sandbox).resolve();main=Path(main).resolve()
if (Path.home()/'Games/Cogmind-ZH-sandbox').resolve() not in base.parents:raise SystemExit('Outside sandbox allowlist')
def userhash():return {str(p.relative_to(main/'user')):hashlib.sha256(p.read_bytes()).hexdigest() for p in (main/'user').rglob('*') if p.is_file()}
before=userhash();dll=hashlib.sha256((base/'WININET.dll').read_bytes()).hexdigest()
if dll!=expected_dll:raise SystemExit('Wrong probe DLL')
f=os.open(str(ps[0]/'mem'),os.O_RDONLY);start=time.monotonic();samples=[];latest={};errors=[]
pat=re.compile(r'\[zh-map-label-source\].*?self=([0-9A-F]+) root=([0-9A-F]+).*?local=(\d+),(\d+).*?text=\|(.*?)\|')
def header(a):return struct.unpack('<11I',os.pread(f,44,a))
def cell(b):return {'glyph':struct.unpack_from('<I',b,4)[0],'fg_bg':b[12:18].hex()}
def actions(a):
 manager=struct.unpack('<I',os.pread(f,4,a+0x64))[0]
 if not manager:return {'manager':0,'actions':[]}
 begin,end=struct.unpack('<2I',os.pread(f,8,manager+0x14))
 if end<begin or (end-begin)%4 or (end-begin)//4>256:raise ValueError('Invalid bounded action vector')
 pointers=struct.unpack('<'+str((end-begin)//4)+'I',os.pread(f,end-begin,begin)) if end>begin else []
 return {'manager':hex(manager),'begin':hex(begin),'end':hex(end),'actions':[{'pointer':hex(p),'words':list(struct.unpack('<8I',os.pread(f,32,p)))} for p in pointers]}
print('sampler_ready',flush=True)
try:
 while time.monotonic()-start<seconds:
  log=(base/'kr_log.txt').read_text(errors='replace')
  for m in pat.finditer(log[-150000:]):
   a,root,x,y,text=m.groups();latest[int(a,16)]=(int(root,16),int(x),int(y),text)
  t=round(time.monotonic()-start,4);entry=[]
  for a,(root,x,y,text) in list(latest.items())[:64]:
   try:
    h=header(a);rh=header(root)
    if h[2]>96 or h[3]!=1 or rh[2:4]!=(160,45) or rh[1]!=0 or rh[9:11]!=(0,0):continue
    n=len(text);px,py=h[9:11];rx,ry=px+x,py+y
    if x+n>h[2] or rx+n>160 or ry>=45:continue
    cb=os.pread(f,n*20,h[4]+x*h[3]*20+y*20)
    rb=[os.pread(f,20,rh[4]+((rx+i)*45+ry)*20) for i in range(n)]
    child=[cell(cb[i*20:(i+1)*20]) for i in range(n)];rootcells=[cell(v) for v in rb]
    item={'self':hex(a),'source':text,'local':[x,y],'project':[px,py],'root_at':[rx,ry],'child':child,'root':rootcells}
    if capture_actions:item['native_actions']=actions(a)
    entry.append(item)
   except (OSError,struct.error,ValueError) as exc:errors.append(str(exc));continue
  samples.append({'seconds':t,'entries':entry});time.sleep(.02)
finally:os.close(f)
after=userhash()
print(json.dumps({'status':'captured_read_only','pid':int(ps[0].name),'dll_sha256':dll,'duration':time.monotonic()-start,'samples':samples,'errors':sorted(set(errors)),'main_user_unchanged':before==after,'limits':'Sequential pread cells paired within a sample; not an atomic frame snapshot. Source log provides candidate pointers; live headers and exact glyphs must be evaluated before attribution.'}))
'''
parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--seconds',type=int,default=26);parser.add_argument('--name',default='map-label-palette-long');parser.add_argument('--actions',action='store_true');args=parser.parse_args()
if not 1<=args.seconds<=30 or not re.fullmatch(r'[A-Za-z0-9_-]+',args.name):parser.error('Use seconds1..30 and a simple capture name')
s=json.loads((ROOT/'analysis/remote-sandbox.json').read_text());probe=json.loads((ROOT/'analysis/map-label-probe-build.json').read_text())
prefix='\n'.join(k+'='+repr(v) for k,v in {'sandbox':s['sandbox_path'],'main':s['main_game_path'],'expected_dll':probe['dll_sha256'],'seconds':args.seconds,'capture_actions':args.actions}.items())
proc=subprocess.Popen(['ssh','-o','BatchMode=yes','bazzite','python3','-u','-'],stdin=subprocess.PIPE,stdout=subprocess.PIPE,stderr=subprocess.PIPE,text=True)
proc.stdin.write(prefix+'\n'+REMOTE);proc.stdin.close()
ready=proc.stdout.readline().strip()
if ready!='sampler_ready':raise SystemExit(ready+' '+proc.stderr.read())
print(ready,flush=True)
payload=proc.stdout.read();error=proc.stderr.read();code=proc.wait(timeout=30)
if code:raise SystemExit(error or payload)
data=json.loads(payload.splitlines()[-1]);path=ROOT/'output'/(args.name+'.json');path.write_text(json.dumps(data)+'\n')
report={'status':data['status'],'pid':data['pid'],'dll_sha256':data['dll_sha256'],'samples':len(data['samples']),'main_user_unchanged':data['main_user_unchanged'],'capture':str(path.relative_to(ROOT)),'sha256':hashlib.sha256(path.read_bytes()).hexdigest(),'limits':data['limits']}
(ROOT/'analysis'/(args.name+'-capture.json')).write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
