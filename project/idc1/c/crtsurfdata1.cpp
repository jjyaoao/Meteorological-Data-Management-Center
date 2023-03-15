/*
*	project name: crtsurfdata1.cpp 用于生成全国气象站点观测的分钟数据
*	author: jjyaoao
*/

#include "_public.h"

CLogFile logFile(10);//指定日志文件大小为10兆

int main(int argc, char *argv[]){
	// inifile:qixiangcanshuwenjian outpath:shengchengdeceshishuju logfile:rizhi
	if(argc != 4){
		printf("Using:./crtsurfdata1 inifile outpath logfile\n");
		printf("Example:/project/idc1/bin/crtsurfdata1 /project/idc1/ini/stcode.ini /tmp/surfdata /log/idc/crtsurfdata1.log\n\n");

		printf("inifile 全国气象站点参数文件名。\n");
		printf("outpath 全国气象战点数据文件存放的目录。\n");
		printf("logfile 本程序运行的日志文件名。\n\n");
		
		return -1;
	}
	if (logFile.Open(argv[3]) == false) {//打开日志文件失败，程序 退出，没必要继续
		printf("logfile.Open(%s) failed.\n", argv[3]); 
		return -1;
	}	
	
	logFile.Write("crtsurfdata1 开始运行. \n");
	//这里插入业务代码
	for(int i = 0; i < 100000000; i++){
	logFile.Write("这是第%010d 条日志记录. \n", i);
	}
	logFile.Write("crtsurfdata1 运行结束。 \n");

		return 0;

}
