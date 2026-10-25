# SPDX-License-Identifier: AGPL-3.0-or-later
"""Pinned fixed-profile adaptations.
Explicit PC UI/session notifications are omitted. Owned fighter objects use
bounded native node/decoded-motion bridges; other objects keep original paths.
Body local points get a defined homogeneous W; attack point arguments satisfy
16-byte vector contracts. Every source anchor must match exactly.
"""
import re

def transform(source, text):
    if source=='src/battle/btl_input.c':
        anchor='        u32 bit = 1 << i;'
        if text.count(anchor)!=2:raise ValueError('input bit-counter source changed')
        return text.replace(anchor,'        u32 bit = 1u << i;'), []
    if source=='src/sys/save.c':
        anchor='#include "sys/save.h"'
        if text.count(anchor)!=1:raise ValueError('save include source changed')
        text=text.replace(anchor,anchor+'\n/* Character masks include bit 63: shift an unsigned 64-bit value. */\n#undef SAVE_CHARA_MASK\n#define SAVE_CHARA_MASK(id) (1ULL << ((id) % 64))')
        return text, []
    if source=='src/battle/btl_obj.c':
        anchor='    light->res = (u8 *)res->data[0] + ((((u32 *)res->data[0])[1] >> 2) << 2);\n    Res_RelocateOffsets(&light->res, light->res, light->res);'
        if text.count(anchor)!=1:raise ValueError('original light resource binding changed')
        text=text.replace(anchor,'    extern int XbBattleRuntime_BindLight(void **);\n    if (!XbBattleRuntime_BindLight(&light->res)) {\n'+anchor+'\n    }')
        return text, []
    if source=='src/battle/btl_char_mgr.c':
        anchor='            chr->objTbl[i] = FILE_TABLE(obj->file[i], 3);'
        if text.count(anchor)!=1:raise ValueError('original member table binding changed')
        text=text.replace(anchor,'            extern int XbFighter_MemberTable(void *,int,void **);\n            void *native_table=NULL;\n            if (XbFighter_MemberTable(obj,i,&native_table)) chr->objTbl[i]=native_table;\n            else '+anchor.strip())
        return text, []
    if source=='src/battle/stg_a_b.c':
        start='s32 BtlStage_GetStartPlace(s32 side, Vec4 *pos, Vec4 *rot, s32 alt) {\n'
        place='s32 BtlStage_GetPlace(Vec4 *pos, Vec4 *rot) {\n'
        for signature,args in ((start,'side,pos,rot,alt'),(place,'0,pos,rot,-1')):
            if text.count(signature)!=1:raise ValueError('stage placement source changed')
            guard=('    extern int XbStage_TryStartPlace(int,void *,void *,int,int *);\n'
                   '    int native_zone=-1;\n'
                   '    if (!pos || !rot) return -1;\n'
                   '    if (XbStage_TryStartPlace('+args+',&native_zone)) return native_zone;\n'
                   '    if (!gBtlStage || !BtlStage_IsReady() || !gBtlStage->data) {\n'
                   '        memset(pos,0,sizeof(*pos)); memset(rot,0,sizeof(*rot)); return -1;\n'
                   '    }\n')
            text=text.replace(signature,signature+guard)
        return text, []
    if source=='src/battle/bobj_a.c':
        node='BObjNode *BtlObj_GetNode(BObj *obj, s32 node) {\n'
        load='BobjAnim *BtlObjAnim_Load(BobjObj *obj, void *buf, s32 anim, s32 *size) {\n'
        if text.count(node)!=1 or text.count(load)!=1:raise ValueError('native fighter hook source changed')
        text=text.replace(node,node+'    extern int XbFighter_ResolveNode(void *, int, void **);\n    void *native_node;\n    if (XbFighter_ResolveNode(obj,node,&native_node)) return (BObjNode *)native_node;\n')
        text=text.replace(load,load+'    extern int XbFighter_LoadDecoded(void *,void *,int,int *,void **);\n    void *native_motion;\n    if (XbFighter_LoadDecoded(obj,buf,anim,size,&native_motion)) return (BobjAnim *)native_motion;\n')
        retarget='    bone = obj->mdl.nodes;\n    if (useOwn != 0'
        if text.count(retarget)!=1:raise ValueError('reference retarget source changed')
        text=text.replace(retarget,'    extern const float *XbFighter_CommonReference(void *,const float *);\n    const float *native_reference=XbFighter_CommonReference(obj,D_002F2420);\n    if (useTable && !native_reference) return;\n'+retarget)
        for suffix in ('id * 3','id * 3 + 1','id * 3 + 2'):
            anchor='keys[i].'+{'id * 3':'x','id * 3 + 1':'y','id * 3 + 2':'z'}[suffix]+' -= D_002F2420['+suffix+'];'
            if text.count(anchor)!=1:raise ValueError('reference table reader changed')
            text=text.replace(anchor,anchor.replace('D_002F2420[','native_reference['))
        bind='void BtlObj_BindTables(BObj *obj) {\n'
        if text.count(bind)!=1:raise ValueError('owned table binder source changed')
        text=text.replace(bind,bind+'    extern int XbFighter_BindOwnedTables(void *);\n    if (XbFighter_BindOwnedTables(obj)) return;\n')
        top='top = ((BObjModelBytes *)mdl->model)->flagsTop;'
        if text.count(top)!=2:raise ValueError('model flags byte readers changed')
        text=text.replace(top,'top = (s32)(mdl->model->flags >> 24);')
        point='        Vec3_Sub(&local, &part->offset, &part->bone->origin);'
        if text.count(point)!=1:raise ValueError('body local-point source changed')
        text=text.replace(point,'        local.w = 1.0f; /* homogeneous local point, not uninitialized stack data */\n'+point)
        marker='void BtlObjHit_BuildVolumes(BobjObj *obj) {'
        start=text.index(marker);end=text.index('/* Local rotation matrix',start)
        block=text[start:end]
        if block.count('    Vec4 center;')!=1:raise ValueError('attack center source changed')
        block=block.replace('    Vec4 center;','    Vec4 center __attribute__((aligned(16)));')
        block=block.replace('    Vec4 offset;','    Vec4 offset;\n    Vec4 boundsPoint __attribute__((aligned(16)));')
        for side in ('min','max'):
            call='                ColBox_AddPointKeepW(&work->atkBox, box.'+side+');'
            if block.count(call)!=2:raise ValueError('attack bound point source changed')
            setup='                boundsPoint.x=box.'+side+'[0]; boundsPoint.y=box.'+side+'[1]; boundsPoint.z=box.'+side+'[2]; boundsPoint.w=1.0f;\n'
            block=block.replace(call,setup+'                ColBox_AddPointKeepW(&work->atkBox, &boundsPoint);')
        text=text[:start]+block+text[end:]
        return text, []
    if source=='src/sys/gfxm_e_b.c':
        bind='void BtlObjMdl_BindFiles(ObjMdlObj *obj) {\n'
        if text.count(bind)!=1:raise ValueError('owned native files binder source changed')
        return text.replace(bind,bind+'    extern int XbFighter_BindOwnedFiles(void *);\n    if (XbFighter_BindOwnedFiles(obj)) return;\n'), []
    expected = {
        'src/sys/gfx.c': ('PortQuit_Draw', 1),
        'src/battle/battle_load.c': ('DrNet_BattleFinished', 1),
        'src/battle/btl_act_e.c': ('Port_ClashLog', 1),
        'src/battle/btl_act_f.c': ('Port_ClashLog', 2),
    }
    if source not in expected:
        return text, []
    name, count = expected[source]
    pattern = re.compile(r'(?m)^(\s*)'+name+r'\([^;\n]*\);')
    matches = list(pattern.finditer(text))
    if len(matches) != count:
        raise ValueError(f'{source}: expected {count} explicit {name} call sites, got {len(matches)}')
    return pattern.sub(lambda m: m[1]+'/* fixed offline build omits PC hook '+name+' */', text), [name]*count
