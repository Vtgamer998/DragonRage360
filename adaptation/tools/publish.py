#!/usr/bin/env python3
"""Publish the prepared sources to DragonRage360 with normal Git authentication, preserving history.
Run locally with configured git user.name/user.email and GitHub write access. Never embeds credentials.
Does not upload XEX, ISO, private fixtures, saves, screenshots or the supplied Linux binary.
"""
import argparse,hashlib,json,shutil,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
REMOTE='https://github.com/Vtgamer998/DragonRage360.git'
def git(folder,*args,capture=False):
 cmd=['git','-C',str(folder),*args]
 if capture:return subprocess.check_output(cmd,text=True).strip()
 subprocess.run(cmd,check=True)
def files():
 for p in sorted(ROOT.rglob('*')):
  if not p.is_file():continue
  rel=p.relative_to(ROOT)
  if any(x in rel.parts for x in ['.git','__pycache__','build','upstream']) or rel.parts[0]=='xenia':continue
  if rel.parts[0]=='validation-adaptation' and str(rel)!='validation-adaptation/result.json':continue
  if p.suffix.lower() in ['.xex','.png','.iso','.raw','.elf','.o','.so','.pyc','.zip','.bin']:continue
  if rel.parts[0]=='validation-d43' and (len(rel.parts)<3 or rel.parts[1] not in ['final','native','xenia','toolchain','resource-audit'] or p.suffix not in ['.json','.log','.txt'] or 'prepared' in rel.parts):continue
  if p.name.startswith('private-'):continue
  if str(rel) in ['static.json','engine-diagnostic.json']:continue
  try:p.read_text()
  except UnicodeError:continue
  yield p,rel
def main():
 parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('checkout',type=Path);parser.add_argument('--push',action='store_true',help='explicitly push the reviewed commit to main');a=parser.parse_args();dst=a.checkout.resolve()
 if dst==ROOT or ROOT in dst.parents or dst in ROOT.parents:raise SystemExit('Use a separate checkout outside the source directory')
 if not dst.exists():subprocess.run(['git','clone',REMOTE,str(dst)],check=True)
 if git(dst,'remote','get-url','origin',capture=True)!=REMOTE:raise SystemExit('Origin must be '+REMOTE)
 if git(dst,'status','--porcelain',capture=True):raise SystemExit('Checkout must be clean; local changes preserved')
 if git(dst,'branch','--show-current',capture=True)!='main':raise SystemExit('Checkout main before publishing')
 git(dst,'pull','--ff-only','origin','main')
 for setting in ['user.name','user.email']:
  if not git(dst,'config','--get',setting,capture=True):raise SystemExit('Configure git '+setting+' before publishing')
 for source,relative in files():
  target=dst/relative;target.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(source,target)
 git(dst,'add','.')
 for relative,pin in json.loads((ROOT/'adaptation/UPSTREAM-PINS.json').read_text()).items():
  git(dst,'update-index','--add','--cacheinfo','160000,'+pin+','+relative)
 git(dst,'diff','--cached','--stat')
 git(dst,'commit','-m','D43 graphics source checkpoint: bounded model shader path and live command cursor splitting')
 print('Prepared commit',git(dst,'rev-parse','HEAD',capture=True))
 if a.push:git(dst,'push','origin','HEAD:main')
 else:print('Review the commit, then git push origin HEAD:main; or rerun from a clean checkout with --push')
if __name__=='__main__':main()
