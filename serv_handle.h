#pragma once
#ifndef  SERV_HANDLE_H
#define SERV_HANDLE_H
#include"im_serv.h"

//把处理的消息发送回去
int serv_snd_msg(int *user_id,int fd,char *buf,int n);
int handle_reg_msg(int* user_id, int fd,REG_MSG *r);
int handle_login_msg(int* user_id, int fd, LOGIN_MSG* l);
int handle_chat_msg(int* user_id, int fd, char *buf,int n,CHAT_MSG *c,int rid);
int handle_logout_msg(int* user_id, int fd,LOGOUT_MSG *l);
int handle_frndmgt_msg(int* user_id, int fd, unsigned char stype,FRND_OP *f,int *fid,int cnt);

int handle_client_msg(int* uid, int fd, char* buf, int n);



#endif // ! SERV_HANDLE_H
