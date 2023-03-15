#include "_public.h"

CSEM sem; // 用于给共享内存加锁的信号量
struct st_pid{
	int pid;	// 进程编号
	char name[51];	// 进程名称
};

int main(int argc, char* argv[]){
	if(argc < 2){
		printf("Using:./book1 procname\n");
		return 0;
	}
	
	// 共享内存的标志
	int shmid;

	//获取或者创建共享内存，键值为0x5005
	if((shmid = shmget(0x5005, sizeof(struct st_pid), 0640|IPC_CREAT)) == -1){
		printf("shmget(0x5005) failed\n");
		return -1;
	}

	//如果信号量已存在，获取信号量；如果信号量不存在，则创建它并初始化为value
	if(sem.init(0x5005) == false){ 
                printf("sem.init(0x5005) failed\n"); 
                return -1; 
        }

	//用于指向共享内存的结构体变量
	struct st_pid* stpid = 0;

	//把共享内存连接到当前进程的地址空间
	//并且处理了未找到情况，需要强转成同一类型才能比对
	if((stpid = (struct st_pid *)shmat(shmid,0,0)) == (void *)-1){
		printf("shmat failed\n");
		return -1;
	}
	
	printf("aaa time = %d, val = %d\n", time(0), sem.value());
	sem.P();	//加锁
        printf("bbb time = %d, val = %d\n", time(0), sem.value()); 	
	printf("pid=%d,name=%s\n", stpid -> pid, stpid -> name);
	stpid -> pid = getpid();	//进程编号
	sleep(10);
	strcpy(stpid -> name, argv[1]);	//进程名称
        printf("pid=%d,name=%s\n", stpid -> pid, stpid -> name); 
        printf("ccc time = %d, val = %d\n", time(0), sem.value()); 	
	sem.V();	//解锁
	printf("ddd time = %d, val = %d\n", time(0), sem.value());


	//把共享内存从当前进程中分离
	shmdt(stpid);
}
