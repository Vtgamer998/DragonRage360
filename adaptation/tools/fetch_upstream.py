#!/usr/bin/env python3
"""Fetch exact original sources from a clone OR a source ZIP. No game assets."""
import json,subprocess
from pathlib import Path
root=Path(__file__).resolve().parents[2]
pins=json.loads((root/'adaptation/UPSTREAM-PINS.json').read_text())
for relative,pin in pins.items():
 path=root/relative
 if not path.exists():
  path.parent.mkdir(parents=True,exist_ok=True)
  subprocess.run(['git','clone','--no-checkout','https://github.com/z3xox/Tenkaichi3Decomp.git',str(path)],check=True)
 dirty=subprocess.check_output(['git','-C',str(path),'status','--porcelain'],text=True)
 if dirty:raise SystemExit('Preserving modified checkout; inspect it first: '+str(path))
 subprocess.run(['git','-C',str(path),'cat-file','-e',pin+'^{commit}'],check=True)
 subprocess.run(['git','-C',str(path),'checkout','--detach',pin],check=True)
 print(relative,pin)
