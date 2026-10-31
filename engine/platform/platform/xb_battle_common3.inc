/* SPDX-License-Identifier: AGPL-3.0-or-later
 * Texture subset of common 3. Unknown entries are never exposed as native.
 * Original scene indexes TABLE WORDS, so word 1 is pack entry 0. */
static uint32_t common3_word(const uint8_t *p){return p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
int XbBattleRuntime_AttachCommon3(XbBattleRuntime *r,const void *pack,size_t n,XbCommon3Info *info){
 if(info)memset(info,0,sizeof(*info));
 if(!r || active!=r || r->managers || r->common3 || !pack || n<12 || n>16*1024*1024)return -891;
 const uint8_t *p=pack;unsigned count=common3_word(p);
 if(count!=637)return -890;
 void **entries=calloc((size_t)count+1,sizeof(*entries));if(!entries)return -851;
 unsigned textures=0,images=0;int error=0;
 for(unsigned i=0;i<count;i++){
  size_t off,size;unsigned flags;
  error=dr_pack_memory_entry(pack,n,i,&off,&size,&flags);
  if(error==DR_NOT_FOUND){error=0;continue;}if(error)goto fail;
  if(flags){error=-890;goto fail;}
  const uint8_t *entry=p+off;
  /* TexFile has its 0x40-byte entry table at word offset 8. No
   * guessing schemas for emitters, animations, VIF or effect configs. */
  if(size<32 || common3_word(entry+4)!=8)continue;
  unsigned entries_count=common3_word(entry);
  if(!entries_count || entries_count>256)continue;
  error=XbNativeTexture_Prepare(entry,size,&entries[i+1]);
  if(error){if(error==-851 || error==-303)goto fail;
   /* Rejected data stays unavailable. Partial preparation is never scene readiness. */
   error=0;continue;
  }
  textures++;images+=entries_count;
 }
 if(!textures){error=-890;goto fail;}
 r->common3=entries;r->common3_count=count;r->common3_textures=textures;r->common3_images=images;
 if(info){*info=(XbCommon3Info){count,textures,images,count-textures};}
 return 0;
fail:
 for(unsigned i=1;i<=count;i++){XbNativeTexture_Free(entries[i]);}
 free(entries);return error;
}
int XbBattleRuntime_TryCommon3Entry(int index,void **out){
 if(!out || !XbBattleRuntime_HasResources() || !active->common3)return 0;
 *out=NULL;if(index>0 && (unsigned)index<=active->common3_count)*out=active->common3[index];return 1;
}
