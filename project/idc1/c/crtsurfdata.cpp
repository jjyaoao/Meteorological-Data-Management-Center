/*
*	project name: crtsurfdata.cpp 用于生成全国气象站点观测的分钟数据
*	author: jjyaoao
*/

#include "_public.h"
CPActive PActive;	//进程心跳

// 全国气象站点参数结构体
struct st_stcode{
	char provname[31]; //省
	char obtid[11];	//站号
	char obtname[31]; //站名
	double lat; //维度
	double lon; //经度
	double height; //海拔高度
};
// 存放全国气象站点参数的容器。 
vector<struct st_stcode> vstcode;

// 把站点参数文件加载到vstcode容器中。
bool LoadSTCode(const char *inifile);

// 全国气象站点分钟观测数据结构
struct st_surfdata
{
  char obtid[11];      // 站点代码。
  char ddatetime[21];  // 数据时间：格式yyyymmddhh24miss
  int  t;              // 气温：单位，0.1摄氏度。
  int  p;              // 气压：0.1百帕。
  int  u;              // 相对湿度，0-100之间的值。
  int  wd;             // 风向，0-360之间的值。
  int  wf;             // 风速：单位0.1m/s
  int  r;              // 降雨量：0.1mm。
  int  vis;            // 能见度：0.1米。
};

// 存放全国气象站点分钟观测数据的容器
vector<struct st_surfdata> vsurfdata;

//观测数据的时间
char strddatetime[21];
// 模拟生成全国气象站点分钟观测数据，存放在vsurfdata容器中 
void CrtsurfData();

CFile File;//各种文件操作，封装为CFile

// 把容器vsurfdata中的全国气象站点分钟观测数据写入文件
bool CrtSurFile(const char *outpath, const char *datafmt);

//CLogFile logfile(10);// 指定日志文件大小为10兆
CLogFile logfile;	// 日志类

void EXIT(int sig);	// 程序退出和信号2、15的处理函数

int main(int argc, char *argv[]){
	if((argc != 5) && (argc != 6)){//若传入argc为6，则为指定历史时间,不指定即为当前时间
		printf("Using:./crtsurfdata inifile outpath logfile datafmt [datetime]\n");//[]意思即为你可以填，你也可以不填
		printf("Example:/project/idc1/bin/crtsurfdata /project/idc1/ini/stcode.ini /tmp/idc/surfdata /log/idc/crtsurfdata.log xml,json,csv\n\n");
		printf("Example:/project/idc1/bin/crtsurfdata /project/idc1/ini/stcode.ini /tmp/idc/surfdata /log/idc/crtsurfdata.log xml,json,csv 20220416123000\n\n");	
		printf("Example:/project/tools1/bin/procctl 60 /project/idc1/bin/crtsurfdata /project/idc1/ini/stcode.ini /tmp/idc/surfdata /log/idc/crtsurfdata.log xml,json,csv\n\n");


		printf("inifile 全国气象站点参数文件名。\n");
		printf("outpath 全国气象战点数据文件存放的目录。\n");
		printf("logfile 本程序运行的日志文件名。\n");
		printf("datafmt 生成数据文件的格式，支持xml、json和csv三种格式，中间用逗号分隔。\n");
		printf("datetime 这是一个可选参数，表示生成指定时间的数据和文件\n\n\n");
		
		return -1;
	}
	
	// 关闭全部的信号和输入输出
	// 设置信号，在shell状态下可用"kill + 进程号" 正常终止进程
	// 但别用"kill -9 + 进程号"强制终止
	CloseIOAndSignal(true);
	signal(SIGINT, EXIT); signal(SIGTERM, EXIT);//可用数字也可以名称，为了标准这里使用名称（2, 15）

	if (logfile.Open(argv[3], "a+", false) == false) {//打开日志文件失败，程序 退出，没必要继续
		printf("logfile.Open(%s) failed.\n", argv[3]); 
		return -1;
	}	
	
	logfile.Write("crtsurfdata 开始运行. \n");

	PActive.AddPInfo(20, "crtsurfdata");//20秒写入一次，因为太短了，程序，所以心跳的时间就不用更新了

	// 把站点参数文件加载到vstcode容器中
	if (LoadSTCode(argv[1]) == false) return -1;

  	// 获取当前时间，当作观测时间。
  	memset(strddatetime,0,sizeof(strddatetime));
	if(argc == 5)
  		LocalTime(strddatetime,"yyyymmddhh24miss");
  	else
		STRCPY(strddatetime, sizeof(strddatetime), argv[5]);

	// 模拟生成全国气象站点分钟观测数据，存放在vsurfdata容器中 
	CrtsurfData();
	if(strstr(argv[4], "xml")!=0) CrtSurFile(argv[2], "xml");	
	if(strstr(argv[4], "json")!=0) CrtSurFile(argv[2], "json");
	if(strstr(argv[4], "csv")!=0) CrtSurFile(argv[2], "csv");

	logfile.Write("crtsurfdata 运行结束。 \n");

		return 0;
}

// 把站点参数文件加载到vstcode容器中。
bool LoadSTCode(const char *inifile){
	//打开站点参数文件。
	if(File.Open(inifile, "r") == false){
		logfile.Write("File.Open(%s) failed.\n", inifile);
		return false;
	}
	char strBuffer[301];
	CCmdStr CmdStr;
	struct st_stcode stcode;
	while (true){
		//从站点参数文件中读取一行，如果读取完，跳出循环
		//通常情况我们需要初始化字符串，不然可能会有bug，memset strBuffer已经再Fgets里面做了
		if(File.Fgets(strBuffer, 300, true) == false) break;
		// 把读到的一行拆分,第三个参数为选择是否删除多余空格
		CmdStr.SplitToCmd(strBuffer, ",", true);
		//扔掉无效行
		if(CmdStr.CmdCount() != 6) continue; 
		// 把站点参数的每个数据项保存到站点参数结构体中
		CmdStr.GetValue(0, stcode.provname, 30); // 省
		CmdStr.GetValue(1, stcode.obtid, 10);	 // 站号
		CmdStr.GetValue(2, stcode.obtname, 30);  // 站名
		CmdStr.GetValue(3, &stcode.lat);	 // 纬度
		CmdStr.GetValue(4, &stcode.lon);	 // 经度	
		CmdStr.GetValue(5, &stcode.height);	 // 海拔高度
		// 把站点参数结构体放入站点参数容器
		vstcode.push_back(stcode);
	}
	/*
	for (int i = 0; i < vstcode.size(); i++){
		logfile.Write("provnmae=%s, obtid=%s, obtname=%s\n",\
				vstcode[i].provname, vstcode[i].obtid, vstcode[i].obtname);
	}        测试程序
	*/
	// 关闭文件,在析构函数里面已经自动关闭了（makefile）
	return true;
}

// 模拟生成全国气象站点分钟观测数据，存放在vsurfdata容器中 
void CrtsurfData(){
 // 播随机数种子。
  srand(time(0));


  struct st_surfdata stsurfdata;

  // 遍历气象站点参数的vstcode容器。
  for (int i=0;i<vstcode.size();i++)
  {
    memset(&stsurfdata,0,sizeof(struct st_surfdata));

    // 用随机数填充分钟观测数据的结构体。i
    // 随机函数，例如下面第一个rand()%351,就是取0-350的随机函数
    strncpy(stsurfdata.obtid,vstcode[i].obtid,10); // 站点代码。
    strncpy(stsurfdata.ddatetime,strddatetime,14);  // 数据时间：格式yyyymmddhh24miss
    stsurfdata.t=rand()%351;       // 气温：单位，0.1摄氏度
    stsurfdata.p=rand()%265+10000; // 气压：0.1百帕
    stsurfdata.u=rand()%100+1;     // 相对湿度，0-100之间的值。
    stsurfdata.wd=rand()%360;      // 风向，0-360之间的值。
    stsurfdata.wf=rand()%150;      // 风速：单位0.1m/s
    stsurfdata.r=rand()%16;        // 降雨量：0.1mm
    stsurfdata.vis=rand()%5001+100000;  // 能见度：0.1米

    // 把观测数据的结构体放入vsurfdata容器。
    vsurfdata.push_back(stsurfdata);
  }
}

// 把容器vsurfdata中的全国气象站点分钟观测数据写入文件。
bool CrtSurFile(const char *outpath, const char *datafmt){
	CFile File;
	// 拼接生成数据的文件名，例如：/tmp/idc/surfdata/SURF_ZH_20210629092200_2254.csv
	char strFileName[301];
	// 在文件名中加入进程编号，这是为了保证临时文件名不重复(getpid())，这里不加也可以
	sprintf(strFileName, "%s/SURF_ZH_%s_%d.%s", outpath, strddatetime, getpid(), datafmt);
	// 打开文件。
	if(File.OpenForRename(strFileName, "w") == false){//一般是没有磁盘空间或者权限不足
		logfile.Write("File.OpenForRename(%s) failed.\n", strFileName);
		return false;
	}
	// 写入第一行标题
	if(strcmp(datafmt, "csv") == 0) File.Fprintf("站点代, 数据时间, 气温, 气压, 相对湿度, 风向, 风速, 降雨量, 能见度\n");
	// 遍历存放观测数据的vsurfdata容器。
	for(int i = 0; i < vsurfdata.size(); i++){
		// 写入一条记录
		if(strcmp(datafmt, "csv") == 0){
			File.Fprintf("%s, %s, %.1f, %.1f, %d, %d, %.1f, %.1f, %.1f\n",\
				       	vsurfdata[i].obtid, vsurfdata[i].ddatetime, vsurfdata[i].t / 10.0, vsurfdata[i].p / 10.0, \
					vsurfdata[i].u, vsurfdata[i].wd, vsurfdata[i].wf / 10.0, vsurfdata[i].r / 10.0, vsurfdata[i].vis / 10.0);
		}
		
	    if (strcmp(datafmt, "xml") == 0)
	      File.Fprintf("<obtid>%s</obtid><ddatetime>%s</ddatetime><t>%.1f</t><p>%.1f</p>"\
		           "<u>%d</u><wd>%d</wd><wf>%.1f</wf><r>%.1f</r><vis>%.1f</vis><endl/>\n",\
		 vsurfdata[i].obtid,vsurfdata[i].ddatetime,vsurfdata[i].t/10.0,vsurfdata[i].p/10.0,\
		 vsurfdata[i].u,vsurfdata[i].wd,vsurfdata[i].wf/10.0,vsurfdata[i].r/10.0,vsurfdata[i].vis/10.0);
		 
		if (strcmp(datafmt, "json") == 0)
		    {
		      File.Fprintf("{\"obtid\":\"%s\",\"ddatetime\":\"%s\",\"t\":\"%.1f\",\"p\":\"%.1f\","\
				   "\"u\":\"%d\",\"wd\":\"%d\",\"wf\":\"%.1f\",\"r\":\"%.1f\",\"vis\":\"%.1f\"}",\
			 vsurfdata[i].obtid,vsurfdata[i].ddatetime,vsurfdata[i].t/10.0,vsurfdata[i].p/10.0,\
			 vsurfdata[i].u,vsurfdata[i].wd,vsurfdata[i].wf/10.0,vsurfdata[i].r/10.0,vsurfdata[i].vis/10.0);
		      if (i<vsurfdata.size()-1) File.Fprintf(",\n");
		      else   File.Fprintf("\n");
		    }
    
	  if (strcmp(datafmt,"xml")==0) File.Fprintf("</data>\n");
	  if (strcmp(datafmt,"json")==0) File.Fprintf("]}\n");
   }    
	//sleep(10); //单元测试
	// 关闭文件
	File.CloseAndRename();

	UTime(strFileName, strddatetime); // 修改文件的时间属性

	logfile.Write("生成数据文件%s成功，数据时间%s，记录数%d.\n", strFileName, strddatetime, vsurfdata.size());
	return true;
}


// 程序退出和信号2、15的处理函数
void EXIT(int sig){	
	logfile.Write("程序退出，sig = %d\n\n", sig);

	exit(0);	
}
