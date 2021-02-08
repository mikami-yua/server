#include"serv_handle.h"

/*
服务器处理某个程序的子线程，调用recv函数接收消息
调用handle_client_msg()对消息进行初步解析
	判断长度是否合法
	从消息头中获取消息的类型和长度

根据不同的消息类型，调用不同的消息处理函数，进行解析处理

消息解析成功再调用对应的消息处理函数

处理完成后，再调用recv函数收取下一条消息进行处理

*/

/*
发送消息的函数
调用send
*/
int serv_snd_msg(int* user_id, int fd, char* buf, int n) {
	if (send(fd, buf, n, 0) != n) {
		printf("serv:send msg err,user exit...\n");
		if (*user_id != 0) {
			user_logout_exit(*user_id);
		}
		return -2;
	}
}

/*
注册消息处理
用户注册的流程
*/
int handle_reg_msg(int* user_id, int fd, REG_MSG* r) {
	//是否有空闲id
	char buf[MAX_MSG_SIZE],err[MAX_ERR_LEN];
	int uid = get_next_uid();
	int n,ret;
	IM_USER* u = &all_user[uid - CONFIG_LOW_ID];
	memset(buf, 0, MAX_MSG_SIZE);
	memset(err, 0, MAX_ERR_LEN);
	if (reg_user_num >= MAX_USER_MUN) {
		printf("serv:reg failed,maximum user reached!\n");
		//把错误的消息放入err中
		strncpy(err, "maximum user reached!", MAX_ERR_LEN - 1);
		enc_reg_resp(buf, &n, -1, err);
		return serv_snd_msg(user_id, fd, buf, n);
	}
	//回复消息正常
	enc_reg_resp(buf, &n, uid, NULL);
	ret = serv_snd_msg(user_id, fd, buf, n);
	if (ret > 0) {//发送成功
		//对用户信息更新
		memset(u->u_name, 0, MAXNAME_LEN - 1);
		strncpy(u->u_name, r->r_name, MAXNAME_LEN - 1);//姓名拷贝完成

		memset(u->u_pass, 0, MAX_USERPASSWD_LEN- 1);
		strncpy(u->u_pass, r->r_passwd, MAX_USERPASSWD_LEN - 1);//密码拷贝完成

		u->u_id = uid;
		u->u_stat = USER_REG;
		u->u_sckfd = fd;

		reg_user_num_inc();//注册用户数目增1
	}
	printf("serv:send reg resp,n=%d", n);
	return ret;
}

/*
登录处理
考虑id 是否重复登录 考虑密码
*/
int handle_login_msg(int* user_id, int fd, LOGIN_MSG* l) {
	int uid = l->lg_id;
	IM_USER* u = &all_user[uid - CONFIG_LOW_ID];//拿到结构体，结构体里有所有需要的信息
	char buf[MAX_MSG_SIZE], err[MAX_ERR_LEN];
	int n, ret;
	int cmp = strcmp(u->u_pass, l->lg_pass);
	printf("serv:handle login msg from client\n");
	memset(buf, 0, MAX_MSG_SIZE);
	if ((u->u_stat & USER_REG) &&//按位与
		!(u->u_stat & USER_LOGIN) &&
		cmp == 0
		) {
		enc_login_resp(buf, &n, LOGIN_OK,u->u_name,NULL);//没问题的情况
	}
	ret = serv_snd_msg(uid, fd, buf, n);
	if (ret > 0) {
		u->u_sckfd = fd;
		u->u_stat = USER_LOGIN;

		*user_id = uid;
	}
	printf("serv:send login resp(login ok),n=%d\n", n);
	return ret;
}

/*
用户聊天信息的处理
需要判断好友是否在线
服务器加上转发标志 最高位是1
*/
int handle_chat_msg(int* user_id, int fd, char* buf, int n, CHAT_MSG* c, int rid) {
	IM_USER* u = &all_user[rid - CONFIG_LOW_ID];
	int ret;
	char rbuf[MAX_MSG_SIZE];
	MSG_HDR* h;

	memset(rbuf, 0, MAX_MSG_SIZE);
	if ((u->u_stat & USER_LOGIN) &&
		u->u_sckfd < 0) {//用户描述符异常
		ret = -1;
		printf("serv:chat msg snd failed,friend not login\n");
		enc_chat_resp(rbuf, &n, SND_ERR, "friend not lgoin!");
	}
	else {
		h = (MSG_HDR*)buf;
		h->msg_type = htons(h->msg_type | SERVER_FORWARD_FLAG);//把消息最高位置位1
		ret = serv_snd_msg(user_id, u->u_frndcnt, buf, n);
		if (ret <= 0) {
			user_logout_exit(rid);
			enc_chat_resp(rbuf, &n, SND_ERR, "chat msg send err!");
		}
		else {
			enc_char_resp(rbuf, &n, SND_OK, NULL);
		}
	}
	printf("serv:forwaing chat msg,n=%d\n", n);
	return serv_snd_msg(user_id, fd, rbuf, n);
}

/*
登出处理函数
*/
int handle_logout_msg(int* user_id, int fd, LOGOUT_MSG* l) {
	int uid = l->lg_id;
	IM_USER* u = &all_user[uid - CONFIG_LOW_ID];
	
	u->u_stat &= ~USER_LOGIN;
	return 0;
}

/*
处理好友消息
*/
int handle_frndmgt_msg(int* user_id, int fd, unsigned char stype, FRND_OP* f, int* fid, int cnt) {
	int uid = f->f_id;
	IM_USER* u = &all_user[uid - CONFIG_LOW_ID];
	char buf[MAX_MSG_SIZE];
	int friend_count = u->u_frndcnt;
	int n;
	FRND_STAT* fs;
	FRND_STAT fnds;
	USER_FRND* uf;
	FRND_STAT user_ret[MAX_USER_SIZE];
	int ret;
	//判断客户发送的id
	if (cnt != 1 && cnt != 0) {
		printf("serv:frnd mgt msg,invalid cnt=%d\n", cnt);
		return -1;
	}
	/*
	#define F_LREG 0x01//好友状态
	#define F_ADD 0x02//增加
	#define F_DEL 0x03//删除
	#define F_ALET 0x04//展示所有好友
	#define F_STAT 0x05//状态
	*/
	if (stype == F_LREG) {
		printf("serv:user lreg msg \n");
		fs = malloc(friend_count * sizeof(FRND_STAT));
		if (fs == NULL) {
			printf("serv:memory allocation failed\n");
			return -1;
		}
		user_fill_frndst(fs,friend_count,u);
		enc_fmgt_resp();
	}
	else if (stype == F_ADD) {
		printf("serv:user add msg \n");
		ret = user_friend_add();
		if (ret < 0) {
			printf("serv:user already in friend list \n");
			enc_fmgt_resp();
		}
		else {
			memset(&fnds, 0, sizeof(fnds));
			u = &all_user[*fid - CONFIG_LOW_ID];
			strncpy(fnds.fr_name, u->u_name, MAXNAME_LEN - 1);
			fnds.fr_id = u->u_id;
			fnds.fr_stat = u->u_stat;
			u->u_frndcnt++;
			printf("serv: entry enc_fmgt_resp\n");
			enc_fmgt_resp(buf, &n, F_ADD, OP_ALL_OK, &fnds, 1);
		}
	}
	else if (stype == F_DEL) {
		printf("serv: user del msg\n");
		ret = user_friend_del(u, *fid);
		printf("serv: usr_friend_del return %d\n", ret);
		if (ret < 0)
		{
			printf("serv: user NOT exist in friend list.\n");
			enc_fmgt_resp(buf, &n, F_DEL, OP_ALL_FAIL, NULL, 0);
		}
		else
		{
			memset(&fnds, 0, sizeof(fnds));
			u = &all_user[*fid - CONFIG_LOW_ID];
			strncpy(fnds.fr_name, u->u_name, MAXNAME_LEN - 1);
			fnds.fr_id = u->u_id;
			fnds.fr_stat = u->u_stat;
			u->u_frndcnt--;
			printf("serv: entry enc_fmgt_resp[F_DEL] ALL_OK\n");
			enc_fmgt_resp(buf, &n, F_DEL, OP_ALL_OK, &fnds, 1);
		}
	}
	else if (stype == F_ALET) {
		printf("serv: user ALIST msg\n");
		get_online_users();
		enc_fngt_resp();
	}
	else if (stype == F_STAT) {
		printf("serv: user STAT msg\n");
		uf = usr_friend_find(u, *fid);//查找这个用户
		if (uf == NULL)
		{
			enc_fmgt_resp(buf, &n, F_STAT, OP_ALL_FAIL, NULL, 0);
		}
		else {
			memset(&fnds, 0, sizeof(fnds));
			u = &all_user[*fid - CONFIG_LOW_ID];
			strncpy(fnds.fr_name, u->u_name, MAXNAME_LEN - 1);
			fnds.fr_id = u->u_id;
			fnds.fr_stat = u->u_stat;
			
			enc_fmgt_resp(buf, &n, F_STAT, OP_ALL_OK, &fnds, 1);
		}
	}
	printf("serv: sending fmgt resp, n=%d\n", n);
	return serv_snd_msg(user_id, fd, buf, n);
}




/*
消息的初步解析
*/
int handle_client_msg(int* uid, int fd, char* buf, int n) {
	//消息的类型分类
	unsigned char type, stype;
	REG_MSG* r;
	LOGIN_MSG* li;
	CHAT_MSG* c;
	LOGOUT_MSG* lo;
	FRND_OP* f;

	//拿到msgheader
	MSG_HDR* h = (MSG_HDR*)buf;
	short m_type, m_len;
	m_type = ntohs(h->msg_type);
	m_len = ntohs(h->msg_len);//拿到了消息的类型和长度

	if (n != sizeof(MSG_HDR) + m_len) {
		printf("serv:msg len sanity check failed!\n");
		return -1;
	}
	//对消息类型进行计算
	/*
	//用户注册消息
	#define MSG_REG 0x01
	//用户登录消息
	#define MSG_LOGIN 0x02 0000 0010
	//好友管理消息
	#define MSG_FRNDMGT 0x03
	//用火聊天消息
	#define MSG_CHAT 0x04
	//退出登录
	#define MSG_LOGOUT 0x05

	MSG_LOGIN 0x02 0000 0010 0000 0000 (登录消息没有子类型)
	移动8位变为0000 0000 0000 0010
	计算类型时不考虑子类型，子需要向右移动8位
	-------------------------------------------------------
	好友管理子类型
	MSG_FRNDMGT 0x03
	#define F_LREG 0x01//好友状态
	#define F_ADD 0x02//增加 0000 0011 0000 0010
	#define F_DEL 0x03//删除
	#define F_ALET 0x04//展示所有好友
	#define F_STAT 0x05//状态
	F_ADD 0x02//增加 0000 0011 0000 0010
	位移计算获得大的消息类型
	0000 0000 0000 0011 MSG_FRNDMGT
	按位与
	0000 0011 0000 0010
	1111 1111 1111 1111
	*/
	type = (m_type >> 8)&0xff;//得到消息的大类型 0xff全是1不影响  unsinged char只有一位实际是0000 0011
	//消息的子类型只需要进行按位与
	stype = m_type & 0xff;//也是unsinged char 前半部分被舍去0000 0010仅剩消息子类型
	switch (type)
	{
	case MSG_REG:
		if (dec_reg_msg() >= 0) {
			return handle_reg_msg();
		}
		break;
	case MSG_LOGIN:
		if (dec_login_msg() >= 0) {
			return handle_login_msg();
		}
		break;
	case MSG_FRNDMGT:
		if (dec_frndmgt_msg() >= 0) {
			return handle_frndmgt_msg();
		}
		break;
	case MSG_CHAT:
		if (dec_chat_msg() >= 0) {
			return handle_chat_msg();
		}
		break;
	case MSG_LOGOUT:
		if (dec_logout_msg() >= 0) {
			return handle_logout_msg();
		}
		break;
	default:
		return -1;
		break;
	}
	return -1;
}