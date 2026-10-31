#!/usr/bin/env python3
"""Exercise the actual publisher against a temporary local bare Git repository."""
import tempfile,subprocess,importlib.util,sys,os,json
from pathlib import Path
spec=importlib.util.spec_from_file_location('publisher',Path(__file__).resolve().parents[1]/'tools/publish.py');m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)
def run(*args):return subprocess.check_output(list(args),stderr=subprocess.DEVNULL,text=True).strip()
with tempfile.TemporaryDirectory(prefix='dragon-publish-') as d:
 d=Path(d);bare=d/'remote.git';seed=d/'seed';src=d/'delivery';checkout=d/'checkout'
 run('git','init','--bare','--initial-branch=main',str(bare));run('git','init','--initial-branch=main',str(seed))
 for p in [seed]:
  run('git','-C',str(p),'config','user.name','Local Test');run('git','-C',str(p),'config','user.email','test@example.invalid')
 (seed/'preserved.txt').write_text('old history\n');run('git','-C',str(seed),'add','.');run('git','-C',str(seed),'commit','-m','initial');run('git','-C',str(seed),'push',str(bare),'main')
 old=run('git','--git-dir='+str(bare),'rev-parse','main');run('git','clone',str(bare),str(checkout))
 for k,v in [('user.name','Local Test'),('user.email','test@example.invalid')]:run('git','-C',str(checkout),'config',k,v)
 (src/'adaptation').mkdir(parents=True);(src/'adaptation/UPSTREAM-PINS.json').write_text(json.dumps({'upstream/Tenkaichi3Decomp':'7b43a75bd8b0aeabcce0a22c45e1ad66162ac9d7'}))
 (src/'README.md').write_text('prepared sources\n');(src/'default.xex').write_bytes(b'not for GitHub');(src/'private.iso').write_bytes(b'not for GitHub');(src/'private-fixture.bin').write_text('valid UTF8 must still be excluded')
 m.ROOT=src;m.REMOTE=str(bare);sys.argv=['publish.py',str(checkout)];m.main()
 assert run('git','--git-dir='+str(bare),'rev-parse','main')==old,'default mode must not push'
 assert run('git','-C',str(checkout),'rev-parse','HEAD^')==old,'history must be retained'
 assert (checkout/'preserved.txt').read_text()=='old history\n'
 names=run('git','-C',str(checkout),'ls-tree','-r','--name-only','HEAD').splitlines()
 assert 'README.md' in names and 'default.xex' not in names and 'private.iso' not in names and 'private-fixture.bin' not in names
 assert run('git','-C',str(checkout),'ls-tree','HEAD','upstream/Tenkaichi3Decomp').startswith('160000 commit 7b43')
 (checkout/'uncommitted.txt').write_text('do not overwrite')
 try:m.main()
 except SystemExit as e:assert 'clean' in str(e)
 else:raise AssertionError('dirty checkout accepted')
 print('PASS publisher: local bare remote, preserved parent/files, exact gitlink, binaries excluded, no push by default, dirty checkout refused')
