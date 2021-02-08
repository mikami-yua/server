#include"im_serv.h"


/*
网络的初始化
初始化 创建 绑定 监听
*/
int serv_sock_init() {
	WSADATA wsa_data;
	int ret, listen_fd;
	struct sockaddr_in saddr;
	ret = WSAStartup((MAKEWORD(2, 2)), &wsa_data);
	if (ret != 0) {
		printf("serv:windows socket load falied with error code %d\n", ret);
		return -1;
	}
	listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_fd < 0) {
		perror("socket return error!\n");
		return -1;
	}
	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = hton(INADDR_ANY);
	saddr.sin_port = htons(SERV_PORT);

	ret = bind(listen_fd, (struct sockaddr*)&saddr, sizeof(saddr));
	if (ret < 0) {
		perror("socket bind error\n");
		return -1;
	}
	listen(listen_fd, 10);



	return listen_fd;
}

void serv_sock_clean(int fd) {
	closesocket(fd);
	WSACleanup();
}

/*
子线程
*/
void* client_thread(void *targs) {
	THREAD_ARGS* a = (THREAD_ARGS*)targs;
	int fd = a->t_sckfd;//当前用户的描述符
	int uid = a->t_id;
	int n;
	char buf[MAX_MSG_SIZE];
	while (1)
	{
		//判断能否收到消息
		n = recv(fd, buf, MAX_MSG_SIZE,0);
		if (n <= 0) {
			printf("serv:recv error,exit this thread\n");
			break;//退出进程
		}
		//处理消息
		if (handle_client_msg(&uid,fd,buf,n) == -2) {
			printf("serv:handle msg return -2,thread exiting...\n");
			break;
		}
	}
	user_logout_exit(uid);//用户退出，线程结束
	free(targs);
	return NULL;
}


/*
服务器处理程序的框架
@fd:监听的socket描述符
*/
void serv_main_loop(int fd) {
	int cfd;//client fd
	int len;
	char line[80];
	struct sockaddr_in caddr;
	THREAD_ARGS* targs;

	while (1)
	{
		len = sizeof(caddr);
		printf("serv:waiting for client's connection......\n");
		cfd = accept(fd, (struct sockaddr*)&caddr, &len);
		printf("serv:connection from %s,port %d\n",
			inet_ntop(AF_INET, &caddr.sin_addr, line, sizeof(line)), ntohs(caddr.sin_port));//inet_ntop 转换为点分十进制的地址
		
		//使用多线程处理
		targs = (THREAD_ARGS*)malloc(sizeof(*targs));
		if (targs == NULL) {
			printf("serv:memory malloc for thread args failed\n");
			closesocket(cfd);
			continue;
		}
		memset(targs, 0, sizeof(*targs));
		targs->t_sckfd = cfd;
		//创建子线程
		pthread_create(&targs->t_id, NULL, &client_thread, (void*)targs);


	}


}

/*
初始化参数
@argv：参数1 程序自身的地址 /参数2 包含用户文件名 /参数3 包含用户好友的文件名
*/
int init_args(int argc,char **argv) {
	if (argc > 3) {
		fprintf(stderr, "%s <user file> <frined file>\n", argv[0]);
		return -1;
	}
	init_all_user_struct(argc, argv);

	return 0;
}

int serv_init(int argc, char** argv) {
	if (init_args(argc, argv) < 0) {
		return -1;
	}

	return serv_sock_init();
}



int main(int argc,char **argv) {

	int listen_fd;
	listen_fd = serv_init(argc,argv);
	if (listen_fd < 0) {
		fprintf(stderr, "serv:socket init failed\n");
		return -1;
	}
	serv_main_loop(listen_fd);

	serv_sock_clean(listen_fd);

	return 0;
}