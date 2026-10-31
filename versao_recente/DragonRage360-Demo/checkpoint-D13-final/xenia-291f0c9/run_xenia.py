from pathlib import Path
import os,subprocess,time,json,sys
root=Path(__file__).resolve().parents[1];folder=root/'xenia-test'
env=dict(os.environ,DISPLAY='127.0.0.1:92',LD_LIBRARY_PATH=str(root/'deps/root/usr/lib/x86_64-linux-gnu'),VK_DRIVER_FILES=str(root/'deps/root/usr/share/vulkan/icd.d/lvp_icd.json'),XDG_RUNTIME_DIR=str(folder/'runtime'))
(folder/'runtime').mkdir(mode=0o700,exist_ok=True)
serverlog=(folder/'xvfb-local.log').open('w')
server=subprocess.Popen([str(root/'deps/root/usr/bin/Xvfb'),':92','-screen','0','1280x720x24','-nolisten','unix','-nolisten','local','-listen','tcp','-ac'],env=env,stdout=serverlog,stderr=subprocess.STDOUT)
try:
 time.sleep(0.5)
 run=subprocess.run([str(folder/'squashfs-root/AppRun'),*sys.argv[1:]],env=env,cwd=folder,capture_output=True,text=True,timeout=40)
 (folder/'run-stdout.txt').write_text(run.stdout+run.stderr)
 print(json.dumps({'returncode':run.returncode,'stdout':(run.stdout+run.stderr)[-2500:]}))
except subprocess.TimeoutExpired as e:
 (folder/'run-stdout.txt').write_bytes((e.stdout or b'')+(e.stderr or b''));print('TIMEOUT after 40 seconds')
finally:
 server.terminate();server.wait(timeout=5);serverlog.close()
