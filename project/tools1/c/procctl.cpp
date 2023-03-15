#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/wait.h> 
 
int main(int argc,char *argv[]) { 
  if (argc<3) {
    printf("Using:./procctl timetvl program argv ...\n");
    printf("Example:/project/tools1/bin/procctl 5 /usr/bin/tar zcvf /tmp/tmp.tgz /usr/include\n\n");

    printf("本程序是服务程序的调度程序，周期性启动服务程序或shell脚本。\n");
    printf("timetvl 运行周期，单位：秒。被调度的程序运行结束后，在timetvl秒后会被procctl重新启动。\n");
    printf("program 被调度的程序名，必须使用全路径。\n");
    printf("argvs   被调度的程序的参数。\n");
    printf("注意，本程序不会被kill杀死，但可以用kill -9强行杀死。\n\n\n");
 
    return -1;//注意，这里不返回，到时候会继续往下执行
  }

// 关闭信号和IO，本程序不希望被打扰。
  for(int i = 0; i < 64; i++){
	  signal(i, SIG_IGN);
	  close(i);
  }

// 生成子进程，父进程退出，让程序运行在后台，由系统1号进程托管。
  if(fork() != 0) exit(0);

// 启用SIGCHLD信号，让父进程可以wait子进程退出的状态。
  signal(SIGCHLD, SIG_DFL);//DFL为默认执行信号
  char *pargv[argc];
  for(int i = 2; i < argc; i++)
	  pargv[i-2] = argv[i];

  pargv[argc-2] = NULL;//这两步是将argv[2]之后的命令存入pargv;

  while(true){
	  if(fork() == 0){//再度生成子进程？
		  execv(argv[2], pargv);//以第三个参数为路径 PS:仔细看main函数传参就懂得了
		  exit(0);//execv成功找到路径，就不执行，不然就退出进程了
	  }else{
		  int status;
		  wait(&status);//第二个参数就是睡眠时间
		  sleep(atoi(argv[1]));//睡眠，atoi为转化为整数
		}
  }
}

