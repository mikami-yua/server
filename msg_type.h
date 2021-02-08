#pragma once

#define SERVER_FORWARD_FLAG (0x1<<15)//是否有服务器转发消息

/*
实现一个通用的消息结构
*/
typedef struct msg_header {
	unsigned short msg_type;//消息类型
	unsigned short msg_len;//消息长度
	char msg_data[0];//可变长度的消息内容，虚拟的消息内容指针，定义为0个成员的数组，其实不占空间
}MSG_HDR;

/*
编号为1		注册消息	消息字类型0		用户注册向服务器发起注册
编号为2		登录认证消息	消息字类型0		用户登录认证
编号为3		好友管理消息	好友状态查询	查询好友是否在线
							增加好友
							删除好友
							列出所有用户
编号为4		聊天消息	消息字类型0		用户向其他用户发送聊天信息
编号为5		退出消息	消息字类型0		用户向服务器发起退出
*/

/*
所有的消息类型
*/
//用户注册消息
#define MSG_REG 0x01
//用户登录消息
#define MSG_LOGIN 0x02
//好友管理消息
#define MSG_FRNDMGT 0x03
//用火聊天消息
#define MSG_CHAT 0x04
//退出登录
#define MSG_LOGOUT 0x05

/*
好友管理信息子类型
*/
#define F_LREG 0x01//好友状态
#define F_ADD 0x02//增加
#define F_DEL 0x03//删除
#define F_ALET 0x04//展示所有好友
#define F_STAT 0x05//状态

//最大用户名长度
#define MAXNAME_LEN 16
//最大密码长度
#define MAX_USERPASSWD_LEN 16

/*
注册消息与回复消息
*/
typedef struct erg_msgdata {
	char r_name[MAXNAME_LEN];//昵称
	char r_passwd[MAX_USERPASSWD_LEN];
}REG_MSG;

#define MAX_ERR_LEN 80
typedef struct reg_msg_resp {
	//将uid返回给客户端 -1出错
	int re_id;
	//出错原因
	char re_reason[0];
}REG_RESP;

/*
登录消息与回复消息
*/
typedef struct login_msg {
	int lg_id;
	char lg_pass[MAX_USERPASSWD_LEN];
}LOGIN_MSG;
#define LOGIN_OK 1
#define LOGIN_ERR 2
typedef struct login_resp {
	//登录状态 1ok 2error
	int lg_stat;
	char lg_name[MAXNAME_LEN];
	char lg_reason[0];//出错原因
}LOGIN_RESP;

/*
好友管理
*/
//好友操作结构体
typedef struct frnd_op {
	int f_id;//用户本身
	int f_fid[0];//需要操作的用户id数组
}FRND_OP;

#define USER_ONLINE 1//好友的在线状态
#define USER_OFFLINE 2

//好友状态的定义
typedef struct frnd_stat {
	char fr_name[MAXNAME_LEN];
	int fr_id;
	int fr_stat;
}FRND_STAT;

#define OP_ALL_OK 1//好友管理全部成
#define OP_PART_OD 2//好友管理部分成功
#define OP_ALL_FAIL 3//好友管理全部失败

typedef struct frnd_op_resp {
	short fre_stat;//好友的在线状态
	short fre_num;//对好友操作成功的数目
	FRND_STAT fre_ok_frnd[0];//好友的状态
}FRND_RESP;

/*
聊天信息
*/
typedef struct chat_msg {
	int ch_sid;//自身id
	int ch_rid;//接收id
	char ch_msg[0];//需要发送的文本信息
}CHAT_MSG;

#define SND_OK 1//消息发送成功
#define SND_ERR 2

typedef struct chat_resp {
	int c_stat;
	char c_reason[0];
}CHAT_RESP;

/*
登出
*/
typedef struct logout_msg {
	int lg_id;
	char lg_pass[MAX_USERPASSWD_LEN];
}LOGOUT_MSG;

/*
线程锁
pthread_mutex_t *lock
int num
pthread_mutex_t *lock(lock)
*/


