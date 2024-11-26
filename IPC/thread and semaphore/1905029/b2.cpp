#include<bits/stdc++.h>
#include<semaphore.h>
using namespace std;

sem_t a, b;
int n;
void* t1(void * arg){
    int i=0;
    while(true){
        sem_wait(&a);
        cout<<"_";
        int val;
        sem_getvalue(&a,&val);
        if(val==0){
            i++; 
            for(int j=1; j<=i; j++) sem_post(&b);
            if(i==n) break;
        }
    }
    return NULL;
}
void* t2(void * arg){
    int i=2*n-1;
    while(true){
        sem_wait(&b);
        cout<<"+";
        int val;
        sem_getvalue(&b,&val);
        if(val==0){
            cout<<endl;
            i--;
            for(int j=1; j<=i; j++) sem_post(&a);
            if(i==n-1) break;
        }
    }
    return NULL;
}
int main(void)
{
    cin>>n;
    pthread_t one,two;
	sem_init(&a,0,2*n-1);
    sem_init(&b,0,0);
    pthread_create(&one,NULL,t1,NULL);
    pthread_create(&two,NULL,t2,NULL);
    pthread_join(one,NULL);
    pthread_join(two,NULL);
	sem_destroy(&a);
    sem_destroy(&b);
}