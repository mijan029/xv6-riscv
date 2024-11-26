//printing with time
#include<unistd.h>
#include <time.h>
#include<stdio.h>
extern time_t start_time, end_time, diff_time;
void output(const char* s1, int a, const char* s2){
    end_time = time(NULL);
    diff_time = (long int)difftime(end_time,start_time);
    printf("%s %d %s %ld\n",s1,a,s2,diff_time);
}
void output1(const char* s1, int a, const char* s2, const char* s3, int c){
    end_time = time(NULL);
    diff_time = (long int)difftime(end_time,start_time);
    printf("%s %d %s %ld %s %d\n",s1,a,s2,diff_time,s3,c);
}