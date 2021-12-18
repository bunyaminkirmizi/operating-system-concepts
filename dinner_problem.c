#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define PHILOSOPHERS 5
#define AMOUNT_OF_RICE 20
int rice = AMOUNT_OF_RICE;
sem_t *rice_amount;
sem_t *take_chopstick;
typedef struct
{
	int philospher_id;
	sem_t *left_chopstick;
	sem_t *right_chopstick;
} philosopher;

void take_chopsticks(philosopher *philosopher)
{
	sem_wait(philosopher->right_chopstick);
	sem_wait(philosopher->left_chopstick);
}

void leave_chopsticks(philosopher *philosopher)
{
	sem_post(philosopher->right_chopstick);
	sem_post(philosopher->left_chopstick);
}

int rice_countdown(){

	int temp = 0;
	sem_wait(rice_amount);
	if(rice != 0) temp = rice--;
	sem_post(rice_amount);
	return temp;
}

void* eat_rice(void *args)
{
	philosopher *p = args;
	while(1){
		sem_wait(take_chopstick);
		take_chopsticks(p);
		sem_post(take_chopstick);
		int r = rice_countdown() ;
		printf("philospher the %d started eating rice %d\n",p->philospher_id,r);
		fflush(stdout);
		if(r <= 0){
			printf("%d rice exiting\n",rice);
			fflush(stdout);
			leave_chopsticks(p);
			pthread_exit(0);
		}
		sleep(2);/* eat for a while */
		fflush(stdout);
		leave_chopsticks(p);
		printf("philospher the %d ate rice %d\n",p->philospher_id,r);
		printf("philospher the %d started thinking\n",p->philospher_id);
		fflush(stdout);
		sleep(4);/* think for awhile */
		printf("philospher the %d stop thinking\n",p->philospher_id);
		fflush(stdout);
	}
	
}
philosopher *philospher_create(int id, sem_t *chopfirst, sem_t *chopsecond)
{
	philosopher *new_p = (philosopher *)malloc(sizeof(philosopher));
	new_p->philospher_id = id;
	new_p->left_chopstick = chopfirst;
	new_p->right_chopstick = chopsecond;
	return new_p;
}

int main()
{
	philosopher philosophers[PHILOSOPHERS];
	sem_t chopsticks[PHILOSOPHERS];
	sem_t sem,sem2;
	rice_amount = &sem;
	take_chopstick = &sem2;
	sem_init(rice_amount, 0, 1);
	sem_init(take_chopstick, 0, 1);
	for (int i = 0; i < PHILOSOPHERS; i++){
		philosophers[i].philospher_id = i;
		philosophers[i].left_chopstick = chopsticks+i;
		if(i+1 == PHILOSOPHERS){
			philosophers[i].right_chopstick = chopsticks;
		}else{
			philosophers[i].right_chopstick = chopsticks+i+1;
		}
	}

	for (int i = 0; i < PHILOSOPHERS; i++)
	{
		sem_init(chopsticks + i, 0, 1);
	}

	pthread_t riceeating[PHILOSOPHERS];
	for (int i = 0; i < PHILOSOPHERS; i++)
	{
		pthread_create(&riceeating[i], NULL, eat_rice, &philosophers[i]);
	}

	for (int i = 0; i < PHILOSOPHERS; i++)
	{
		pthread_join(riceeating[i], NULL);
	}

	return 0;
}
