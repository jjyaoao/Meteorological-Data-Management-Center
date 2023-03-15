/*
*	project name: crtsurfdata2.cpp 用于生成全国气象站点观测的分钟数据
*	author: jjyaoao
*/

#include "_public.h"

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

//CLogFile logfile(10);//指定日志文件大小为10兆
CLogFile logfile;
int main(int argc, char *argv[]){
	// inifile:qixiangcanshuwenjian outpath:shengchengdeceshishuju logfile:rizhi
	if(argc != 4){
		printf("Using:./crtsurfdata2 inifile outpath logfile\n");
		printf("Example:/project/idc1/bin/crtsurfdata2 /project/idc1/ini/stcode.ini /tmp/surfdata /log/idc/crtsurfdata2.log\n\n");

		printf("inifile 全国气象站点参数文件名。\n");
		printf("outpath 全国气象战点数据文件存放的目录。\n");
		printf("logfile 本程序运行的日志文件名。\n\n");
		
		return -1;
	}
	if (logfile.Open(argv[3]) == false) {//打开日志文件失败，程序 退出，没必要继续
		printf("logfile.Open(%s) failed.\n", argv[3]); 
		return -1;
	}	
	
	logfile.Write("crtsurfdata2 开始运行. \n");
	// 把站点参数文件加载到vstcode容器中
	if (LoadSTCode(argv[1]) == false) return -1;

	logfile.Write("crtsurfdata2 运行结束。 \n");

		return 0;
}

// 把站点参数文件加载到vstcode容器中。
bool LoadSTCode(const char *inifile){
	CFile File;
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

