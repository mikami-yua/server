#include"serv_dec.h"

/*
解析注册消息
*/
int dec_reg_msg(char* buf, int n, REG_MSG** r) {
	if (n != sizeof(REG_MSG)) {
		printf("serv:reg msg len check failed\n");
		return -1;
	}
	printf("serv:recv reg msg,len=%d",n);
	*r=(REG_MSG*)(buf + sizeof(MSG_HDR));//注册消息得到了
	return 0;
}

int dec_login_msg(char* buf, int n, LOGIN_MSG** l) {
	if (n != sizeof(LOGIN_MSG)) {
		printf("serv:login msg len check failed\n");
		return -1;
	}
	printf("serv:recv login msg,len=%d", n);
	*l = (LOGIN_MSG*)(buf + sizeof(LOGIN_MSG));//登录消息得到了

	//需要得到login id
	(*l)->lg_id = ntohl((*l)->lg_id);
	printf("serv:recv login msg,id=%d", (*l)->lg_id);

	return 0;

}

int dec_chat_msg(char* buf, int n, CHAT_MSG** c, int* rid) {
	if (n <= sizeof(CHAT_MSG) || n>MAX_CHAT_MSG) {
		printf("serv:chat msg len check failed\n");
		return -1;
	}
	printf("serv:recv login msg,len=%d", n);
	*c = (CHAT_MSG*)(buf + sizeof(CHAT_MSG));//聊天消息得到了
	*rid = ntohl((*c)->ch_rid);
	return 0;

}
int dec_logout_msg(char* buf, int n, LOGOUT_MSG** l) {
	if (n != sizeof(LOGOUT_MSG)) {
		printf("serv:logout msg len check failed\n");
		return -1;
	}
	printf("serv:recv logout msg,len=%d", n);
	*l = (LOGOUT_MSG*)(buf + sizeof(LOGOUT_MSG));//登录消息得到了
	//需要得到login id
	(*l)->lg_id = ntohl((*l)->lg_id);
	printf("serv:recv logout msg,id=%d", (*l)->lg_id);
	return 0;
}

/*
解析好友管理的
*/
int dec_fmgt_msg(char* buf, int n, FRND_OP** f, int** fid, int* cnt) {
	//1.判断长度
	if (n != sizeof(FRND_OP) && (n-sizeof(FRND_OP))%sizeof(int)!=0) {
		printf("serv:fmgt msg len check failed\n");
		return -1;
	}
	printf("serv:recv fmgt msg,len=%d", n);
	//2.获得fop的指针
	*f = (FRND_OP*)(buf + sizeof(FRND_OP));
	//f id 换为本机id
	(*f)->f_id = ntohl((*f)->f_id);
	//计算多少个好友需要操作
	*cnt = (n - sizeof(FRND_OP)) / sizeof(int);

	int* p = (*f)->f_fid;//f_fid数组的首地址
	*fid = p;//拿到哪些需要操作的好友数组

	for (int i = 0; i < *cnt; i++) {
		*p = ntohl(*p);
		p++;
	}
	return 0;

}

//客户端消息的预处理
//int dec_client_msg(char* buf, int n);