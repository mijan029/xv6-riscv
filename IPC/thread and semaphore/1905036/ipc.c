#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
#include <stdlib.h>
#include <time.h>
#include<semaphore.h>
#include<math.h>
#include "random.h"
#include "print.h"

#define N 105
time_t start_time, end_time, diff_time;
int n, m;
pthread_mutex_t mtx;
int printingDelay, bindingDelay, rwDelay;


//reporting phase ... using classical reader-writer problem
sem_t db;
int rd_count=0, number_of_submission=0, l;
void* reader(void* arg){
	int i = *(int*)arg, j;
	while(1){
		sleep(generate_random_number(27));

		pthread_mutex_lock(&mtx);
		rd_count++;

		if(rd_count==1) sem_wait(&db);
		pthread_mutex_unlock(&mtx);

        pthread_mutex_lock(&mtx);
        output1("Staff",i,"has started reading the entry book at time",". No. of submission =",number_of_submission);
        pthread_mutex_unlock(&mtx);

		sleep(rwDelay);
		if(number_of_submission == n/m) break;

		pthread_mutex_lock(&mtx);
		rd_count--;
		if(rd_count==0) sem_post(&db);
		pthread_mutex_unlock(&mtx);
	}
	free(arg);
	return NULL;
}
void writer(int i){
	sem_wait(&db);
	//write
	sleep(rwDelay);

    output("Group",i,"has submitted the report at time");
	number_of_submission++;

	sem_post(&db);
}

//binding phase....ensuring concurrency
int count_num_of_printing[N];
sem_t printed_all[N];
sem_t binding_t;
void binding(int i){

	sem_wait(&binding_t);
    pthread_mutex_lock(&mtx);
    output("Group",i,"has started binding at time");
    pthread_mutex_unlock(&mtx);

	//binding
	sleep(bindingDelay);

    pthread_mutex_lock(&mtx);
    output("Group",i,"has finished binding at time");
    pthread_mutex_unlock(&mtx);

	sem_post(&binding_t);

	writer(i);

}

//printing phase ...using classical dining philosopher problem
int state[N];
sem_t s[N];
int global;

void test(int i){
	if(state[i] == 0){
		int fail=0, cnt=0, p=0;
		for(int j=1; j<=n; j++){
			if(j%4 == i%4){
                if(state[j]==1) { fail = 1; cnt++; }
            }
		}
		if(!fail){
			state[i] = 1;
            p=1;
			sem_post(&s[i]);
		}
	}
}
void take_machine(int i){
	pthread_mutex_lock(&mtx);
	state[i] = 0;
	test(i);
	pthread_mutex_unlock(&mtx);
	sem_wait(&s[i]);
}
void inform_others(int i){
	int groupid = (i-1)/m+1, p=0;
    for(int j = 1; j<=n; j++){
        if(j%4 == i%4){
            p=1;
           if((j-1)/m +1 != groupid && state[j]==0) test(j);
        }
    }
	int st = groupid*m;
}
void inform_teammate(int i){
	int groupid = (i-1)/m+1, p=0, key=0;
    for(int j = (groupid-1)*m+1; j<=groupid*m; j++){
        if(j%4 == i%4) {
            if(j!=i && state[j]==0) test(j);
        }
    }
    return;
}
void leave_machine(int i){
    int cnt=0;
	pthread_mutex_lock(&mtx);
	state[i] = 2;
	inform_teammate(i);
	inform_others(i);
	pthread_mutex_unlock(&mtx);
}
void* printing(void* arg)
{
	sleep(generate_random_number(25));

	int index = *(int*)arg, id;
    pthread_mutex_lock(&mtx);
    output("Student",index,"has arrived at the print station at time");
    pthread_mutex_unlock(&mtx);

	take_machine(index);
	//print
	sleep(printingDelay);

    pthread_mutex_lock(&mtx);
    output("Student",index,"has finished printing at time");
    pthread_mutex_unlock(&mtx);

	leave_machine(index);

    int groupinfo, cnt=0;
	int groupid = (index-1)/m + 1, p = n/m;
    pthread_mutex_lock(&mtx);
	count_num_of_printing[groupid]++;
	pthread_mutex_unlock(&mtx);

	if(index%m==0 ) {
        int key=1;
        if(count_num_of_printing[groupid]!=m )
            sem_wait(&printed_all[index]);
        else{
              key=0;
        }
        if(key){
            //printf("checking");
        }
    }

	pthread_mutex_lock(&mtx);
	if(count_num_of_printing[groupid]==m) {
        int p=0;
		sem_post(&printed_all[groupid*m]); 
	}
	pthread_mutex_unlock(&mtx);

	if(index%m==0){
        cnt++;
        pthread_mutex_lock(&mtx);
        output("Group",groupid,"has finished printing at time");
        pthread_mutex_unlock(&mtx);
		binding(groupid);
	}
	free(arg);
	return NULL;
}
//main function
int main(void)
{
	srand(time(NULL));
    scanf("%d%d",&n,&m);
	scanf("%d%d%d",&printingDelay,&bindingDelay,&rwDelay);
    
	start_time = time(NULL);
	pthread_t stuff[3];
	pthread_t students[n+1];

    for(int i=1;  i<=n; i++){
        sem_init(&printed_all[i],0,0);
		count_num_of_printing[i]=0;
		sem_init(&s[i],0,0);
		state[i]=0;
	}
    sem_init(&binding_t,0,2);
	sem_init(&db,0,1);
	pthread_mutex_init(&mtx,NULL);
	
	for(int i=1; i<=n; i++){
		int* index = (int*) malloc(sizeof(int));
		*index = i;
		pthread_create(&students[i],NULL,printing,(void*)index);
	}
	for(int i=1; i<=2; i++){
		int* index = (int*) malloc(sizeof(int));
		*index = i;
		pthread_create(&stuff[i],NULL,reader,(void*)index);
	}
	for(int i=1; i<=n; i++){
		pthread_join(students[i],NULL);
	}
	for(int i=1; i<=2; i++){
		pthread_join(stuff[i],NULL);
	}

    for(int i=1;  i<=n; i++){
		sem_destroy(&s[i]);
		sem_destroy(&printed_all[i]);
	}
    sem_destroy(&db);
	pthread_mutex_destroy(&mtx);
	sem_destroy(&binding_t);
	
	return 0;
}

