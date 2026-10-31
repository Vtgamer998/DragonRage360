#!/usr/bin/env python3
# SPDX-License-Identifier: AGPL-3.0-or-later
"""Package the strictly linked Battle_Main with the native diagnostic entry.

This does NOT call Battle_Main or Game_Main. Data and battle boot are missing.
The new diagnostic logs the linked function address and refuses any claim of
gameplay readiness. Never overwrites the ordinary build/default.xex.
"""
from pathlib import Path
import argparse, json, re, subprocess, hashlib
from build_overlay_catalog import read_object

p=argparse.ArgumentParser(description=__doc__)
p.add_argument('build',type=Path);p.add_argument('xsdk',type=Path);p.add_argument('output',type=Path)
p.add_argument('--xenia-display',action='store_true',help='explicit keep-kernel-LUT/viewport policy; no gameplay authorization')
a=p.parse_args();build=a.build.resolve();sdk=a.xsdk.resolve();out=a.output.resolve();out.mkdir(parents=True,exist_ok=True)
project=Path(__file__).resolve().parents[2]
r=json.loads((build/'battle-integration.json').read_text())
assert r['link']=='PASS' and r['all_sources_compiled'] and not r['unresolved_count'] and not r['ABI_conflict_count']
assert r['native_data_sections_audited'] and r['fixed_battle_policy']
assert not r['data_values_loaded'] and not r['native_boot_integrated'] and not r['battle_executed']
args=json.loads((build/'link-arguments.json').read_text());cc=Path(args[0])
main=out/'main-engine-diagnostic.o'
cmd=[str(cc),'-mcpu=cell','-fno-pic','-std=gnu17','-O2','-g','-ffunction-sections','-fdata-sections',
     '-Wall','-Wextra','-Werror','-I'+str(sdk/'include'),'-I'+str(project/'src'),
     '-I'+str(project/'engine/graphics'),'-I'+str(project/'engine/capture/port/src/gs'),
     '-I'+str(project/'engine/platform/platform'),'-DDR_XBOX360','-DDR_ENGINE_LINKED',
     '-c',str(project/'src/main.c'),'-o',str(main)]
if r.get('typed_catalog_linked'):cmd.insert(1,'-DDR_TYPED_CATALOG')
if a.xenia_display:cmd.insert(1,'-DDR_XENIA_DISPLAY')
subprocess.run(cmd,check=True)
elf=out/'default.elf';xex=out/'default.xex'
cmd=[str(main) if x==str(project/'build/main.o') else
     x.replace(str(build/'battle-link'),str(out/'default')) for x in args]
roots=re.search(r'^NATIVE_API_ROOTS := (.+)$',(project/'Makefile').read_text(),re.M)
assert roots
# Preparing the original menu catalog retains Progress_Main and the full
# menu services. This diagnostic exposes no menu boot, and does not root it.
cmd[1:1]=[x for x in roots[1].split() if x!='-Wl,--undefined=XbOverlay_PrepareBuild']
run=subprocess.run(cmd,capture_output=True,text=True)
(out/'link.txt').write_text(run.stdout+run.stderr);run.check_returncode()
(out/'link-arguments.json').write_text(json.dumps(cmd,indent=2)+'\n')
b,h,ss,names,syms=read_object(elf)
defined={s[0]:s for s in syms if s[0] and s[4]}
assert 'Battle_Main' in defined and 'Game_Main' not in defined
assert not [s[0] for s in syms if s[0] and not s[4]]
for name in ['func_002967C0','sceSifCallRpc','XbSfx_Mix','ADXT_Create','_ctype_','gPortDataStripped']:
    assert name in defined,name
subprocess.run(['python3',str(sdk/'tools/elf2xex.py'),'--title-id','0x44523630',
                '--title-name','DragonRage360 0.52 D18 Diagnostic' if r.get('typed_catalog_linked') else 'DragonRage360 0.34 L1 Diagnostic',str(elf),str(xex)],check=True)
report={'kind':'native diagnostic with linked combat code; NOT a playable game',
        'version':'0.52-D18' if r.get('typed_catalog_linked') else '0.34-L1','Battle_Main_address':hex(defined['Battle_Main'][1]),
        'Battle_Main_linked':True,'Game_Main_linked':False,'battle_called':False,
        'strong_or_weak_undefined':0,'native_game_data_loaded':False,'typed_catalog_linked':bool(r.get('typed_catalog_linked')),'partial_preview_available':bool(r.get('typed_catalog_linked')), 
        'boot_combat_available':False,'hardware_executed':False,'xenia_display_policy':bool(a.xenia_display),
        'sha256':hashlib.sha256(xex.read_bytes()).hexdigest(),'bytes':xex.stat().st_size}
(out/'engine-diagnostic.json').write_text(json.dumps(report,indent=2)+'\n')
print(json.dumps(report,indent=2))
