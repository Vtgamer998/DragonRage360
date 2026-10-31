/* SPDX-License-Identifier: MIT
 * Independent experimental lobby protocol. Does not carry battle inputs.
 * All wire integers are little endian; no native structs are transmitted.
 */
#include "lobby.h"
#include <arpa/inet.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
enum { DISCOVER=1,OFFER,JOIN,ACCEPT,HEARTBEAT,LEAVE,BUSY, PACKET=32 };
static uint32_t rd(const unsigned char *p) { return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24; }
static void wr(unsigned char *p,uint32_t v) { for(int i=0;i<4;i++) p[i]=(unsigned char)(v>>(i*8)); }
static int same(const struct sockaddr_in *a,const struct sockaddr_in *b)
{ return a->sin_addr.s_addr==b->sin_addr.s_addr && a->sin_port==b->sin_port; }
static void send_packet(DrLobby *l,const struct sockaddr_in *to,unsigned type,uint32_t nonce,uint32_t session)
{
    unsigned char p[PACKET]={0}; memcpy(p,"DRL1",4);wr(p+4,1);wr(p+8,type);wr(p+12,nonce);wr(p+16,session);
    (void)sendto(l->fd,p,sizeof(p),0,(const struct sockaddr *)to,sizeof(*to));
}
void dr_lobby_init(DrLobby *l) { memset(l,0,sizeof(*l));l->fd=-1;strcpy(l->message,"Rede desligada."); }
void dr_lobby_stop(DrLobby *l)
{
    if(l->fd>=0) {
        if(l->session && l->peer.sin_port) send_packet(l,&l->peer,LEAVE,l->nonce,l->session);
        close(l->fd);
    }
    dr_lobby_init(l);
}
int dr_lobby_start(DrLobby *l,int host,const char *target,uint16_t port,uint32_t now,uint32_t nonce)
{
    dr_lobby_stop(l);l->host=host;l->nonce=nonce?nonce:1;l->session=host?l->nonce:0;
    l->target.sin_family=AF_INET;l->target.sin_port=htons(port);
    if(inet_pton(AF_INET,target?target:"255.255.255.255",&l->target.sin_addr)!=1) goto fail;
    l->fd=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);if(l->fd<0) goto fail;
    int yes=1;
    if(setsockopt(l->fd,SOL_SOCKET,SO_BROADCAST,&yes,sizeof(yes))) goto fail;
    struct sockaddr_in local={.sin_family=AF_INET,.sin_port=host?htons(port):0};
    if(bind(l->fd,(const struct sockaddr *)&local,sizeof(local))) goto fail;
    l->state=host?LOBBY_HOST:LOBBY_SEARCH;l->started=now;l->last_seen=now;l->last_send=now-1000;
    snprintf(l->message,sizeof(l->message),host?"Sala aberta UDP %u. Aguardando cliente...":"Buscando host UDP %u...",port);return 1;
fail:
    dr_lobby_stop(l);strcpy(l->message,"Falha ao abrir rede. Confira IP e porta ocupada.");return 0;
}
static void paired(DrLobby *l)
{
    char ip[16]="?";inet_ntop(AF_INET,&l->peer.sin_addr,ip,sizeof(ip));
    l->state=LOBBY_PAIRED;
    snprintf(l->message,sizeof(l->message),"Lobby conectado: %s %s:%u\nMotor de luta ainda indisponível.",l->host?"cliente":"host",ip,(unsigned)ntohs(l->peer.sin_port));
}
void dr_lobby_tick(DrLobby *l,uint32_t now)
{
    if(l->fd<0) return;
    /* Fixed budget prevents an incoming flood from monopolizing the frame. */
    for(int budget=0;budget<16;budget++) {
        struct pollfd ready={.fd=l->fd,.events=POLLIN};
        if(poll(&ready,1,0)<=0 || !(ready.revents&POLLIN)) break;
        unsigned char p[PACKET+1];struct sockaddr_in from={0};socklen_t size=sizeof(from);
        ssize_t n=recvfrom(l->fd,p,sizeof(p),0,(struct sockaddr *)&from,&size);
        if(n!=PACKET || from.sin_family!=AF_INET || memcmp(p,"DRL1",4) || rd(p+4)!=1 || rd(p+20) || rd(p+24) || rd(p+28)) continue;
        unsigned type=rd(p+8);uint32_t nonce=rd(p+12),session=rd(p+16);
        if(!nonce) continue;
        if(l->host) {
            if(type==DISCOVER && !session && l->state==LOBBY_HOST && !l->peer.sin_port)
                send_packet(l,&from,OFFER,nonce,l->session);
            else if(type==JOIN && session==l->session) {
                if(l->peer.sin_port && (!same(&from,&l->peer) || nonce!=l->nonce)) send_packet(l,&from,BUSY,nonce,session);
                else {
                    l->peer=from;l->nonce=nonce;l->last_seen=now;
                    send_packet(l,&from,ACCEPT,nonce,session);
                }
            } else if(same(&from,&l->peer) && session==l->session && nonce==l->nonce) {
                if(type==HEARTBEAT) {l->last_seen=now;paired(l);}
                if(type==LEAVE) {l->peer.sin_port=0;l->state=LOBBY_HOST;strcpy(l->message,"Cliente saiu. Aguardando novo cliente...");}
            }
        } else {
            if(type==OFFER && l->state==LOBBY_SEARCH && nonce==l->nonce && session &&
               (l->target.sin_addr.s_addr==htonl(INADDR_BROADCAST) || same(&from,&l->target))) {
                l->peer=from;l->session=session;l->state=LOBBY_JOIN;l->last_send=now-1000;l->last_seen=now;
                strcpy(l->message,"Host encontrado. Conectando...");
            } else if(same(&from,&l->peer) && nonce==l->nonce && session==l->session) {
                if(type==ACCEPT && (l->state==LOBBY_JOIN || l->state==LOBBY_PAIRED)) {l->last_seen=now;paired(l);l->last_send=now-1000;}
                if(type==HEARTBEAT && l->state==LOBBY_PAIRED) l->last_seen=now;
                if(type==BUSY || type==LEAVE) {dr_lobby_stop(l);strcpy(l->message,"Host ocupado ou sala encerrada. Busque novamente.");return;}
            }
        }
    }
    if((l->state==LOBBY_PAIRED || l->state==LOBBY_JOIN || (l->host && l->peer.sin_port)) && (uint32_t)(now-l->last_seen)>12000) {
        if(l->host) {l->peer.sin_port=0;l->state=LOBBY_HOST;strcpy(l->message,"Cliente desconectado. Aguardando outro...");}
        else {dr_lobby_stop(l);strcpy(l->message,"Conexão perdida. Busque novamente.");return;}
    }
    if(l->state==LOBBY_SEARCH && (uint32_t)(now-l->started)>30000) {
        dr_lobby_stop(l);strcpy(l->message,"Nenhum host em 30 segundos. Confira a rede.");return;
    }
    if((uint32_t)(now-l->last_send)<500) return;
    l->last_send=now;
    if(l->state==LOBBY_SEARCH) send_packet(l,&l->target,DISCOVER,l->nonce,0);
    if(l->state==LOBBY_JOIN) send_packet(l,&l->peer,JOIN,l->nonce,l->session);
    if(l->state==LOBBY_PAIRED) send_packet(l,&l->peer,HEARTBEAT,l->nonce,l->session);
}
