/*
#include <pthread.h>
#include <stdio.h>
#include <windows.h>

#pragma comment(lib,"pthreadVC2.lib")

void* thread(void* val)
{
	int value = *((int*)val);
	printf("value: %d \n", value);
	while (value-- > 0)
	{
		printf(" %d \n", value);
	}
	return NULL;
}

int main()
{
	pthread_t tid;
	int value = 100;
	pthread_create(&tid, 0, thread, &value);

	system("pause");
	return 0;
}
*/
