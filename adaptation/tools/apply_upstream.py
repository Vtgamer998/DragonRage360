#!/usr/bin/env python3
"""Install the feature bridge into the pinned upstream settings UI; preserve its game/netcode."""
import argparse,hashlib,json,subprocess,shutil
from pathlib import Path
PIN='7b43a75bd8b0aeabcce0a22c45e1ad66162ac9d7'
def transform(s):
    marker='#include "dragonrage/dr_features.c"'
    if marker in s:return s
    changes={
        '#include "net_update.h"':'#include "net_update.h"\n'+marker,
        'Port_LobbyCreate(useCode ? 1 : 0, useCode ? 0 : atoi(port), battle, timeLimit, speed, roll * 2, delay);':'DrFeature_OpenLobby(useCode ? 1 : 0, useCode ? 0 : atoi(port), battle, timeLimit, speed, roll * 2, delay);',
        'Port_LobbyJoin(address, atoi(port));':'DrFeature_JoinLobby(address, atoi(port));',
        '            gPortUnlockAll = 1;':'            (void)DrFeature_UnlockAll(1);',
    }
    for old,new in changes.items():
        if s.count(old)!=1:raise ValueError('upstream integration anchor changed: '+old)
        s=s.replace(old,new)
    return s
def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('upstream',type=Path);a=p.parse_args();up=a.upstream.resolve()
    head=subprocess.check_output(['git','-C',str(up),'rev-parse','HEAD'],text=True).strip()
    if head!=PIN:raise SystemExit('Refusing unreviewed upstream; expected '+PIN)
    ui=up/'port/src/gs/ui.cpp';before=ui.read_text();after=transform(before)
    dest=ui.parent/'dragonrage';dest.mkdir(exist_ok=True)
    for f in (Path(__file__).resolve().parents[1]/'features').glob('dr_features.*'):shutil.copy2(f,dest/f.name)
    ui.write_text(after)
    print(json.dumps({'upstream':PIN,'settings_ui_patched':before!=after,'online':'original upstream net_lobby.c/net.c',
                      'cheat':'original gPortUnlockAll game-thread queue','Xbox_online':False,'XEX_rebuilt':False}))
if __name__=='__main__':main()
