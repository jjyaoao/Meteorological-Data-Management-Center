#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

void func(int num){
	printf("jieshou%d\n", num);
}

int main(){
	for(int i = 1; i < 64; i++) 
		signal(i, func);
	while(1){
	printf("zhixing\n");
	sleep(1);
	}
}