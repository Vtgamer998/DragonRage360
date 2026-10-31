#!/usr/bin/env python3
"""Verify the imported game source and syntax-check its original menus (host only)."""
import argparse,hashlib,importlib.util,json,re,subprocess,concurrent.futures
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
def blob(b):return hashlib.sha1(b'blob '+str(len(b)).encode()+b'\0'+b).hexdigest()
def verify(root):
 manifest=json.loads((root/'IMPORT-MANIFEST.json').read_text())
 for row in manifest['files']:
  p=root/row['path']
  if not p.is_file() or blob(p.read_bytes())!=row['git_blob_sha']:raise ValueError('source changed/missing: '+row['path'])
 return manifest

def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',type=Path,default=ROOT/'validation-original');p.add_argument('--jobs',type=int,default=4);a=p.parse_args();out=a.output.resolve();out.mkdir(parents=True,exist_ok=True)
 if not 1<=a.jobs<=8:raise SystemExit('jobs must be 1..8')
 r=ROOT/'vendor/tenkaichi-original';manifest=verify(r)
 spec=importlib.util.spec_from_file_location('portsrc_original',r/'port/tools/portsrc.py');m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)
 sources=m.sources();missing=[];enabled=0
 for f in sources:
  _,yes,no=m.enable_attempts(f.read_text());enabled+=len(yes)
  if no:missing.append({'path':str(f.relative_to(r)),'functions':no})
 if missing:raise ValueError('original C bodies missing: '+str(missing))
 main_source=(r/'src/main.c').read_text();flow=re.search(r'void Game_Main\(void\)\s*\{(.*)',main_source,re.S)[1]
 for name in ['Overlay_Load(0);','Progress_Main(0);','Battle_Main(0);']:assert name in flow
 assert flow.index('Overlay_Load(0);')<flow.index('Progress_Main(0);')<flow.index('Battle_Main(0);')
 battle=(r/'src/battle/battle.c').read_text();assert re.search(r'Battle_Init\(\);\s*Battle_Loop\(\);\s*Battle_Term\(\);',battle)
 selected=[f for f in sources if '/menu/' in f.as_posix()]+[r/'src/main.c',r/'src/battle/battle.c']
 def check(f):
  rel=f.relative_to(r);text,yes,no=m.enable_attempts(f.read_text());gen=out/'prepared'/rel;gen.parent.mkdir(parents=True,exist_ok=True);gen.write_text(text)
  cmd=['gcc','-std=gnu89','-DPORT','-fno-builtin','-fsyntax-only','-I'+str(r/'include'),'-I'+str(r/'port/include'),'-I'+str(f.parent),str(gen)]
  run=subprocess.run(cmd,text=True,capture_output=True);log=out/'logs'/(str(rel).replace('/','_')+'.log');log.parent.mkdir(exist_ok=True);log.write_text(run.stdout+run.stderr)
  return {'path':str(rel),'exit_code':run.returncode,'warnings':run.stderr.count('warning:'),'log':str(log.relative_to(out))}
 with concurrent.futures.ThreadPoolExecutor(max_workers=a.jobs) as pool:checks=list(pool.map(check,selected))
 failures=[c for c in checks if c['exit_code']]
 report={'source_integrity':'PASS','commit':manifest['commit'],'verified_files':len(manifest['files']),'game_C_units':len(sources),'menu_C_units':len(selected)-2,'C_attempts_enabled':enabled,'missing_original_C_bodies':0,'original_flow':['Game_Main','Overlay_Load','Progress_Main','Battle_Main','Battle_Init','Battle_Loop','Battle_Term'],'host_syntax_result':'PASS' if not failures else 'PARTIAL','host_syntax_units':checks,'host_syntax_failed':len(failures),'scope':'Host syntax and source integrity only. No PowerPC ABI validation, native linking, UI rendering, game execution or FPS claim.','XEX_rebuilt':False,'Xbox_menus_running':False,'last_native_binary':'D42'}
 (out/'report.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='host_syntax_units'},indent=2))
if __name__=='__main__':main()
