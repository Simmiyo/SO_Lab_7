#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <error.h>
#include <errno.h>

pthread_mutex_t mtx1;
pthread_mutex_t mtx2;
#define MAX_RESOURCES 7
int available_resources=MAX_RESOURCES;

int decrease_count(int n)
{
	pthread_mutex_lock(&mtx1);
	if(available_resources>n)
	{
		available_resources-=n;
		printf("Got %d resources, %d resources remain \n",n,available_resources);
		pthread_mutex_unlock(&mtx1);
		return 0;
	}
	else
	{
		pthread_mutex_unlock(&mtx1);
		return -1;
	}
}

int increase_count(int n)
{
	pthread_mutex_lock(&mtx2);
	available_resources+=n;
	printf("Released %d resources, %d total resoruces \n",n,available_resources);
	pthread_mutex_unlock(&mtx2);
	return 0;
}

void *res(void *nr)
{
	int *n=(int *)nr;
	if(decrease_count(*n)>-1)
		increase_count(*n);
	return n;
}

int main(int argc,char *argv[])
{
	if(pthread_mutex_init(&mtx1,NULL))
	{
		perror("mutex init");
		return errno;
	}
	if(pthread_mutex_init(&mtx2,NULL))
	{
		perror("mutex init");
		return errno;
	}

	pthread_t thr[argc-1];
	int *rezultat=malloc(sizeof(int));
	for(int i=0;i<argc-1;i++)
	{
		int *nr_resurse=malloc(sizeof(int));
		*nr_resurse=atoi(argv[i+1]);
		if(pthread_create(&thr[i],NULL,res,nr_resurse))
		{
			perror("pthread_create");
			return errno;
		}
	}
	for(int i=0;i<argc-1;i++)
	{
		if(pthread_join(thr[i],(void **)&rezultat))
		{
			perror("join");
			return errno;
		}
	}
	pthread_mutex_destroy(&mtx1);
	pthread_mutex_destroy(&mtx2);
	return 0;
}

