#include"im_serv.h"

/*
表示所有用户的结构体数组
索引与id相对应

1001-----alluser【1】

*/
IM_USER all_user[MAX_USER_MUN];

int init_all_user_struct(int argc, char** argv) {
	IM_USER* u = all_user;
	memset(all_user, 0, sizeof(IM_USER) * MAX_USER_MUN);//初始化所有用户的结构体
	for (int i = 0; i < MAX_USER_MUN; i++) {
		u->u_id = CONFIG_LOW_ID + i+1;
		u->u_sckfd = -1;
		INIT_LIST_HEAD(&u->u_frndhd);
		u++;
	}
	return 0;
}