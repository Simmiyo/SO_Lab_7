#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>

sem_t smph;
pthread_mutex_t mtx;
unsigned int N;

int barrier_point()
{
	if(N>1)
	{
		pthread_mutex_lock(&mtx);
		N--;
		pthread_mutex_unlock(&mtx);
		if(sem_wait(&smph))
		{
			perror("sem wait");
			return errno;
		}
	}
	if(sem_post(&smph))
	{
		perror("sem post");
		return errno;
	}
}

void *tfun(void *v)
{
	int *tid=(int *)v;
	printf("%d reached the barrier.\n",*tid);
	barrier_point();
	printf("%d passed the barrier.\n",*tid);
	free(tid);
	return NULL;
}

int main(int argc,char *argv[])
{
	N=atoi(argv[1]);
	if(pthread_mutex_init(&mtx,NULL))
	{
		printf("mutex init");
		return errno;
	}
	if(sem_init(&smph,0,0))
	{
		perror("semaphore init");
		return errno;
	}
	pthread_t thr[atoi(argv[1])];
	for(int i=0;i<atoi(argv[1]);i++)
	{
		int *p=malloc(sizeof(int));
		*p=i;
		if(pthread_create(&thr[i],NULL,tfun,p))
		{
			perror("pthread_create");
			return errno;
		}
	}
	void *rezultat;
	for(int i=0;i<atoi(argv[1]);i++)
	{
		if(pthread_join(thr[i],(void **)&rezultat))
		{
			perror("pthread_join");
			return errno;
		}
	}
	pthread_mutex_destroy(&mtx);
	sem_destroy(&smph);
}
