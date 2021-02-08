#pragma once
#ifndef SERV_USER_H
#define SERV_USER_H

#include"im_serv.h"

#define USER_REG (0x1<<0)
#define USER_LOGIN (0x1<<1)

typedef struct im_user
{
	char u_name[MAXNAME_LEN];
	char u_pass[MAX_USERPASSWD_LEN];
	int u_id;
	int u_stat;
	int u_sckfd;

	struct list_head u_frndhd;
	int u_frndcnt;
}IM_USER;

typedef struct user_frnd {
	int f_id;
	struct list_head h_nodel;
}USER_FRND;

extern IM_USER all_user[MAX_USER_MUN];
extern int reg_user_num;//extern有这个东西但是没有分配空间

int init_all_user_struct(int argc,char **argv);

void user_fill_frndst(FRND_STAT *fs,int count ,IM_USER *u);

int user_friend_add(IM_USER *u,int uid);

#endif // !SERV_USER_H
