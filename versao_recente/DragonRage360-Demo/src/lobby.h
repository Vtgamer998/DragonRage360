/* SPDX-License-Identifier: MIT */
#ifndef DR_LOBBY_H
#define DR_LOBBY_H
#include <stdint.h>
#include <netinet/in.h>
enum { LOBBY_OFF, LOBBY_HOST, LOBBY_SEARCH, LOBBY_JOIN, LOBBY_PAIRED };
typedef struct {
    int fd, state, host;
    uint32_t nonce, session, last_send, last_seen, started;
    struct sockaddr_in target, peer;
    char message[240];
} DrLobby;
void dr_lobby_init(DrLobby *l);
/* target NULL means LAN broadcast. port=7371 in production; override for tests. */
int dr_lobby_start(DrLobby *l,int host,const char *target,uint16_t port,uint32_t now,uint32_t nonce);
void dr_lobby_tick(DrLobby *l,uint32_t now);
void dr_lobby_stop(DrLobby *l);
#endif
