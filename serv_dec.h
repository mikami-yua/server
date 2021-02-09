#pragma once
#ifndef SERV_DEC_H
#define SERV_DEC_H
#include"im_serv.h"

/*
解析注册消息
*/
int dec_reg_msg(char *buf,int n,REG_MSG **r);
int dec_login_msg(char* buf, int n, LOGIN_MSG** l);
int dec_chat_msg(char* buf, int n, CHAT_MSG** c,int *rid);
int dec_logout_msg(char* buf, int n, LOGOUT_MSG** l);
int dec_fmgt_msg(char* buf, int n, FRND_OP** f,int **fid,int *cnt);

//客户端消息的预处理
//int dec_client_msg(char *buf,int n);


#endif // !SERV_DEC_H
