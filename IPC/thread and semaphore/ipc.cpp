#include<bits/stdc++.h>
#include<semaphore.h>
using namespace std;

#define ld long double
#define N 100
int n, m;
int printing_delay, binding_delay, rw_delay;
time_t start_t, end_t, diff;
pthread_mutex_t mtx;

//random number generators using poisson distribution
ld rand_0_to_1() {
    return (ld) rand() / RAND_MAX;
}
int poisson_random(ld lambda) {
    ld L = exp(-lambda);
    ld p = 1.0;
    int k = 0;

    do {
        k++;
        p *= rand_0_to_1();
    } while (p > L);

    return k - 1;
}

int generate(ld mean) {
    //srand(time(NULL));
    int sample = poisson_random(mean) % 20;
	return sample+1;
}
//reporting phase ... using classical reader-writer problem
int reader_count=0, num_of_submission=0;
sem_t db;
void* reader(void* arg){
	int i = *(int*)arg;
	while(1){
		sleep(generate(30));
		pthread_mutex_lock(&mtx);
		reader_count++;

		end_t = time(NULL);
	    diff = difftime(end_t,start_t);
		cout<<"Staff "<<i<<" has started reading the entry book at time "<<diff<<". No. of submission = "<<num_of_submission<<endl;

		if(reader_count==1) sem_wait(&db);
		pthread_mutex_unlock(&mtx);

		sleep(rw_delay);
		if(num_of_submission == n/m) break;

		pthread_mutex_lock(&mtx);
		reader_count--;
		if(reader_count==0) sem_post(&db);
		pthread_mutex_unlock(&mtx);
	}
	free(arg);
	return NULL;
}
void writer(int i){
	sem_wait(&db);
	//write
	sleep(rw_delay);
	num_of_submission++;

	end_t = time(NULL);
	diff = difftime(end_t,start_t);
	cout<<"Group "<<i<<" has submitted the report at time "<<diff<<"\n";

	sem_post(&db);
}

//binding phase....ensuring concurrency
sem_t binding_t;
sem_t printed_all[N];
int count_printing[N];
void binding(int i){

	sem_wait(&binding_t);

	pthread_mutex_lock(&mtx);
	end_t = time(NULL);
	diff = difftime(end_t,start_t);
	cout<<"Group "<<i<<" has started binding at time "<<diff<<"\n";
	pthread_mutex_unlock(&mtx);

	//binding
	sleep(binding_delay);

	pthread_mutex_lock(&mtx);
	end_t = time(NULL);
	diff = difftime(end_t,start_t);
	cout<<"Group "<<i<<" has finished binding at time "<<diff<<"\n";
	pthread_mutex_unlock(&mtx);

	sem_post(&binding_t);

	writer(i);

}

//printing phase ...using classical dining philosopher problem
#define print_korte_chai 0
#define printingg 1
#define printed 2
#define teammates 0
#define others 1
sem_t s[N];
int state[N];

void test(int i){
	if(state[i] == print_korte_chai){
		int fail=0;
		for(int j=1; j<=n; j++){
			if(j%4 == i%4 && state[j]==printingg) fail = 1;
		}
		if(!fail){
			state[i] = printingg;
			sem_post(&s[i]);
		}
	}
}
void send_message(int i){
	test(i);
}
void take_printing_machine(int i){
	pthread_mutex_lock(&mtx);
	state[i] = print_korte_chai;
	test(i);
	pthread_mutex_unlock(&mtx);
	sem_wait(&s[i]);
}
void inform(int condition, int i){
	int groupid = (i-1)/m+1;
	if(condition == teammates){
		for(int j = (groupid-1)*m+1; j<=groupid*m; j++){
			if(j%4 == i%4 && j!=i && state[j]==print_korte_chai) send_message(j);
		}
	}else{
		for(int j = 1; j<=n; j++){
			if(j%4 == i%4 && (j-1)/m +1 != groupid && state[j]==print_korte_chai) send_message(j);
		}
	}
}
void leave_printing_machine(int i){
	pthread_mutex_lock(&mtx);
	state[i] = printed;
	inform(teammates, i);
	inform(others, i);
	pthread_mutex_unlock(&mtx);
}
void* printing(void* arg)
{
	int index = *(int*)arg;
	sleep(generate(30));
	pthread_mutex_lock(&mtx);
	end_t = time(NULL);
	diff = difftime(end_t,start_t);
	cout<<"Student "<<index<<" has arrived at the print station at time "<<diff<<"\n";
	pthread_mutex_unlock(&mtx);

	take_printing_machine(index);
	//print
	sleep(printing_delay);

	pthread_mutex_lock(&mtx);
	end_t = time(NULL);
	diff = difftime(end_t,start_t);
	cout<<"Student "<<index<<" has finished printing at time "<<diff<<"\n";
	pthread_mutex_unlock(&mtx);

	leave_printing_machine(index);

	int groupid = (index-1)/m + 1;
    pthread_mutex_lock(&mtx);
	count_printing[groupid]++;
	pthread_mutex_unlock(&mtx);

	if(index%m==0 && count_printing[groupid]!=m )sem_wait(&printed_all[index]);

	pthread_mutex_lock(&mtx);
	if(count_printing[groupid]==m) { 
		sem_post(&printed_all[groupid*m]); 
	}
	pthread_mutex_unlock(&mtx);

	if(index%m==0){
		pthread_mutex_lock(&mtx);
		end_t = time(NULL);
		diff = difftime(end_t,start_t);
		cout<<"Group "<<groupid<<" has finished printing at time "<<diff<<"\n";
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
    cin>>n>>m;
	cin>>printing_delay>>binding_delay>>rw_delay;
	start_t = time(NULL);
	pthread_t students[n+1];
	pthread_t stuff[3];

	pthread_mutex_init(&mtx,NULL);
	sem_init(&binding_t,0,2);
	sem_init(&db,0,1);
	for(int i=1;  i<=n; i++){
		sem_init(&s[i],0,0);
		state[i]=0;
		sem_init(&printed_all[i],0,0);
		count_printing[i]=0;
	}

	for(int i=1; i<=n; i++){
		int* index = new int;
		*index = i;
		pthread_create(&students[i],NULL,printing,(void*)index);
	}
	for(int i=1; i<=2; i++){
		int* index = new int;
		*index = i;
		pthread_create(&stuff[i],NULL,reader,(void*)index);
	}
	for(int i=1; i<=n; i++){
		pthread_join(students[i],NULL);
	}
	for(int i=1; i<=2; i++){
		pthread_join(stuff[i],NULL);
	}
	pthread_mutex_destroy(&mtx);
	sem_destroy(&db);
	sem_destroy(&binding_t);
	
	for(int i=1;  i<=n; i++){
		sem_destroy(&s[i]);
		sem_destroy(&printed_all[i]);
	}

}

