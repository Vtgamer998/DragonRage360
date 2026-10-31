#!/usr/bin/env python3
import argparse,subprocess,shutil,importlib.util,json
from pathlib import Path
p=argparse.ArgumentParser();p.add_argument('--upstream',required=True,type=Path);p.add_argument('--output',type=Path,default=Path('validation-adaptation'));a=p.parse_args()
r=Path(__file__).resolve().parents[1];out=a.output.resolve();out.mkdir(parents=True,exist_ok=True)
# Build against the actual pinned upstream public header, not a made-up lobby API.
header=a.upstream/'port/src/gs/net_lobby.h';ui=a.upstream/'port/src/gs/ui.cpp'
if not header.is_file() or not ui.is_file():raise SystemExit('upstream checkout with net_lobby.h and ui.cpp required')
shutil.copy2(header,out/'net_lobby.h');f=out/'dragonrage';f.mkdir(exist_ok=True)
for p in (r/'features').glob('*'):shutil.copy2(p,f/p.name)
logs=[]
for compiler in ['gcc','g++']:
 for xbox in [False,True]:
  exe=out/(compiler+('-xbox' if xbox else '-upstream'))
  cmd=[compiler,'-Wall','-Wextra','-Werror','-O1','-g','-fsanitize=address,undefined','-fno-pie','-no-pie','-I'+str(f),str(r/'tests/test_features.c'),str(f/'dr_features.c'),'-o',str(exe)]
  if xbox:cmd.insert(1,'-DDR_FEATURE_XBOX')
  subprocess.run(cmd,check=True);run=subprocess.run([str(exe)],check=True,text=True,capture_output=True);logs.append(run.stdout.strip())
spec=importlib.util.spec_from_file_location('adapt',r/'tools/apply_upstream.py');module=importlib.util.module_from_spec(spec);spec.loader.exec_module(module)
s=ui.read_text();patched=module.transform(s);assert patched!=s and module.transform(patched)==patched
assert 'DrFeature_OpenLobby(' in patched and 'DrFeature_JoinLobby(' in patched and '(void)DrFeature_UnlockAll(1);' in patched
try:module.transform(s.replace('#include "net_update.h"',''))
except ValueError:pass
else:raise AssertionError('changed upstream must be rejected')
report={'result':'PASS','checks':logs+['exact original UI patch, idempotence and changed-anchor refusal'],'scope':'C/C++ forwarding and UI source integration only; no two-machine match, GPU, full UI compilation or XEX test'}
(out/'result.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
