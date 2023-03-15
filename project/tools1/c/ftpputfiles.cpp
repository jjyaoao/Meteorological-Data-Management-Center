#include "_public.h"
#include "_ftp.h"

struct st_arg{		//注意都多了1，因为char数组最后要留给\0一个位置
  char host[31];           // 远程服务器的IP和端口。
  int  mode;               // 传输模式，1-被动模式，2-主动模式，缺省采用被动模式。
  char username[31];       // 远程服务器ftp的用户名。
  char password[31];       // 远程服务器ftp的密码。
  char remotepath[301];    // 远程服务器存放文件的目录。
  char localpath[301];     // 本地文件存放的目录。
  char matchname[101];     // 待上传文件匹配的规则。
  int  ptype;		   // 上传后客户端文件的处理方式：1-什么都不做；2-删除；3-备份
  char localpathbak[301];  // 上传后客户端文件的备份目录
  char okfilename[301];	   // 已上传成功文件名清单
  int  timeout;			   // 进程心跳的超时时间
  char pname[51];		   // 进程名，建议用"ftpputfiles_后缀"的方式
} starg;

struct st_fileinfo{
	char filename[301];	// 文件名
	char mtime[21];		// 文件时间
};

vector<struct st_fileinfo> vlistfile1;	// 已上传成功文件名的容器，从okfilename中加载
vector<struct st_fileinfo> vlistfile2;	// 上传前列出客户端文件名的容器，从nlist文件中加载
vector<struct st_fileinfo> vlistfile3;	// 本次不需要上传的文件的容器
vector<struct st_fileinfo> vlistfile4;	// 本次需要上传的文件的容器

  // 加载okfilename文件中的内容到容器vlistfile1中。
  bool      LoadOKFile();
  // 比较vlistfile2和vlistfile1，得到vlistfile3和vlistfile4
  bool      CompVector();
  // 把容器vlistfile3中的内容写入okfilename文件，覆盖之前的旧okfilename文件
  bool      WriteToOKFile();

  // 如果ptype == 1，把上传成功的文件记录追加到okfilename文件中。 
  bool AppendToOKFile(struct st_fileinfo *stfileinfo); 
 	

// 把localpath目录下的文件加载到vlistfile2容器中
bool LoadLocalFile();

CLogFile logfile;
Cftp ftp;

// 程序退出和信号2、15的处理函数
void EXIT(int sig);

void _help();

// 把xml解析到参数starg结构中
bool _xmltoarg(char *strxmlbuffer);

// 上传文件功能的主函数
bool _ftpputfiles();

CPActive PActive;	// 进程心跳

int main(int argc, char *argv[]){
	if(argc != 3){
		_help();//帮助文档
		return -1;
	}
	// 处理程序的退出信号
	// 关闭全部的信号和输入输出
      // 设置信号,在shell状态下可用 "kill + 进程号" 正常终止些进程。
      // 但请不要用 "kill -9 +进程号" 强行终止。
       CloseIOAndSignal(true); //一般来说开发阶段把这行注释掉，为了方便调试
	signal(SIGINT, EXIT);  signal(SIGTERM, EXIT);
	// 打开日志文件
	if(logfile.Open(argv[1], "a+") == false){
		printf("打开日志文件失败(%s).\n", argv[1]);
		return -1;
	}
	// 解析xml，得到程序运行的参数
	if(_xmltoarg(argv[2]) == false) return -1;

	PActive.AddPInfo(starg.timeout, starg.pname);	// 心跳信息写入共享内存中
	// 登录ftp服务器
	if(ftp.login(starg.host, starg.username, starg.password, starg.mode) == false){
		logfile.Write("ftp.login(%s, %s, %s) failed.\n", starg.host, starg.username, starg.password);
		return -1;
	}
	// logfile.Write("ftp.login ok.\n");//正式写程序可以不用写，是用来测试的

	_ftpputfiles();
	ftp.logout();
	return 0;
}

void EXIT(int sig){
	printf("程序退出， sig = %d\n\n", sig);
	exit(0);
}

void _help()
{
  printf("\n");
  printf("Using:/project/tools1/bin/ftpputfiles logfilename xmlbuffer\n\n");

  printf("Sample:/project/tools1/bin/procctl 30 /project/tools1/bin/ftpputfiles /log/idc/ftpputfiles_surfdata.log \"<host>127.0.0.1:21</host><mode>1</mode><username>jjyaoao</username><password>gh</password><localpath>/tmp/idc/surfdata</localpath><remotepath>/tmp/ftpputest</remotepath><matchname>SURF_ZH*.JSON</matchname><ptype>1</ptype><localpathbak>/tmp/idc/surfdatabak</localpathbak><okfilename>/idcdata/ftplist/ftpgetfiles_surfdata.xml</okfilename><timeout>80</timeout><pname>ftpgetfiles_surfdata</pname>\"\n\n\n");

  printf("本程序是通用的功能模块，用于把远程ftp服务器的文件上传到本地目录。\n");
  printf("logfilename是本程序运行的日志文件。\n");
  printf("xmlbuffer为文件上传的参数，如下：\n");
  printf("<host>127.0.0.1:21</host> 远程服务器的IP和端口。\n");
  printf("<mode>1</mode> 传输模式，1-被动模式，2-主动模式，缺省采用被动模式。\n");
  printf("<username>jjyaoao</username> 远程服务器ftp的用户名。\n");
  printf("<password>gh</password> 远程服务器ftp的密码。\n");
  printf("<remotepath>/tmp/ftpputest</remotepath> 远程服务器存放文件的目录。\n");
  printf("<localpath>/tmp/idc/surfdata</localpath> 本地文件存放的目录。\n");
  printf("<matchname>SURF_ZH*.JSON</matchname> 待下载文件匹配的规则。"\
         "不匹配的文件不会被下载，本字段尽可能设置精确，不建议用*匹配全部的文件。\n");
  printf("<ptype>1</ptype> 文件下载成功后，远程服务器文件的处理方式：1-什么也不做; 2-删除; 3-备份，如果为3，还要指定备份的目录.\n");
  printf("<remotepathbak>/tmp/idc/surfdatabak</remotepathbak> 文件下载成功后，服务器文件的备份目录，此参数只有当ptype=3时才有效.\n");
  printf("<okfilename>/idcdata/ftplist/ftpgetfiles_surfdata.xml</okfilename> 已下载成功文件名清单，此参数只有当ptype = 1时才有效。\n");
  printf("<checkmtime>true</checkmtime> 是否需要检查服务端文件的时间，true-需要，false-不需要，此参数只有当ptype = 1时才有效，缺省为false\n");
  printf("<timeout>80</timeout> 下载文件超时时间，单位：秒，视文件大小和网络带宽而定。\n");
  printf("<pname>ftpgetfiles_surfdata</pname> 进程名，尽可能采用易懂，与其他进程不同的名称，方便排查错误。\n\n\n"); 
}

// 把xml解析到参数starg结构中
bool _xmltoarg(char *strxmlbuffer){

	memset(&starg, 0, sizeof(struct st_arg));

		GetXMLBuffer(strxmlbuffer, "host", starg.host, 30);	// 远程服务器的ip和端口
		if(strlen(starg.host) == 0){
			logfile.Write("host is null.\n");
			return -1;
		}

		GetXMLBuffer(strxmlbuffer, "mode", &starg.mode);	// 传输模式，1-被动模式，2-主动模式，缺省采用被动模式
		if(starg.mode != 2) starg.mode = 1;		// 实现缺省

		GetXMLBuffer(strxmlbuffer,"username",starg.username,30);   // 远程服务器ftp的用户名。
 		 if (strlen(starg.username)==0)
 		 { logfile.Write("username is null.\n");  return false; }

  		GetXMLBuffer(strxmlbuffer,"password",starg.password,30);   // 远程服务器ftp的密码。
  		if (strlen(starg.password)==0)
  		{ logfile.Write("password is null.\n");  return false; }

  		GetXMLBuffer(strxmlbuffer,"remotepath",starg.remotepath,300);   // 远程服务器存放文件的目录。
		if (strlen(starg.remotepath)==0)
		{ logfile.Write("remotepath is null.\n");  return false; }

		GetXMLBuffer(strxmlbuffer,"localpath",starg.localpath,300);   // 本地文件存放的目录。
		if (strlen(starg.localpath)==0)
		{ logfile.Write("localpath is null.\n");  return false; }

 		 GetXMLBuffer(strxmlbuffer,"matchname",starg.matchname,100);   // 待上传文件匹配的规则。
 		 if (strlen(starg.matchname)==0)
 		 { logfile.Write("matchname is null.\n");  return false; }	
                   		 
		 
		 // 上传后客户端文件的处理方式：1-什么都不做；2-删除；3-备份
		 GetXMLBuffer(strxmlbuffer, "ptype", &starg.ptype);
		if((starg.ptype != 1) && (starg.ptype != 2) && (starg.ptype != 3)){
			logfile.Write("ptype is error.\n"); 
			return false;
		}

		 GetXMLBuffer(strxmlbuffer,"localpathbak",starg.localpathbak, 300);   // 上传前列出客户端文件名的文件
                 if ((starg.ptype == 3) && strlen(starg.localpathbak) == 0) {//多加了一个判断，只有ptype为3，才判断是否为空
                 	logfile.Write("localpathbak is null.\n");  
		 	return false; 
		 }

		 GetXMLBuffer(strxmlbuffer,"okfilename",starg.okfilename, 300);   // 已上传成功文件名清单
                 if ((starg.ptype == 1) && strlen(starg.okfilename) == 0) {//多加了一个判断，只有ptype为1，才判断是否为空 
                        logfile.Write("okfilename is null.\n");  // 居然是这里传参错了......... 
                        return false;  
                 }

		 GetXMLBuffer(strxmlbuffer, "timeout", &starg.timeout);   // 进程心跳的超时时间
                 if (starg.timeout == 0) 
                 { logfile.Write("timeout is null.\n");  return false; }  

		 GetXMLBuffer(strxmlbuffer,"pname",starg.pname, 50);   // 进程名
                 if (strlen(starg.pname) == 0) 
                 { logfile.Write("pname is null.\n");  return false; }  
		return true;
}

// 上传文件功能的主函数
bool _ftpputfiles(){

	// 把localpath目录下的文件加载到vlistfile2容器中
	if(LoadLocalFile() == false){
		logfile.Write("LoadLocalFile() failed.\n");
		return false;
	}

	if(starg.ptype == 1){
		// 加载okfilename文件中的内容到容器vlistfile1中。
		LoadOKFile();
		// 比较vlistfile2和vlistfile1，得到vlistfile3和vlistfile4
		CompVector();
		// 把容器vlistfile3中的内容写入okfilename文件，覆盖之前的旧okfilename文件
		WriteToOKFile();
		// 把vlistfile4中的内容复制到vlistfile2中
		vlistfile2.clear();   vlistfile2.swap(vlistfile4);
	}
	// 遍历容器vlistfile2
	PActive.UptATime();	// 更新进程的心跳
	char strremotefilename[301], strlocalfilename[301];
	for(int i = 0; i < vlistfile2.size(); i++){
		SNPRINTF(strremotefilename, sizeof(strremotefilename), 300, "%s/%s", starg.remotepath, vlistfile2[i].filename);
		SNPRINTF(strlocalfilename, sizeof(strlocalfilename), 300, "%s/%s", starg.localpath, vlistfile2[i].filename);
		logfile.Write("put %s ...", strlocalfilename);
	//	logfile.WriteEx("filename%s\n", vlistfile2[i].filename);
	//	logfile.WriteEx("localpath%s\n", starg.localpath);
	//	logfile.WriteEx("remotepath%s\n", starg.remotepath);
		// 调用ftp.put()方法上传文件到服务端, 第三个参数填true确保文件上传成功，对方不可抵赖
		if(ftp.put(strlocalfilename, strremotefilename, true) == false) {
			logfile.WriteEx("failed.\n");
			//logfile.Write("%s\n", strremotefilename);//有了这句才找到了问题在哪
			return false;
		}
		logfile.WriteEx("ok.\n");
		
		PActive.UptATime();	// 更新进程的心跳

		// 如果ptype == 1，把上传成功的文件记录追加到okfilename文件中。
		if(starg.ptype == 1) AppendToOKFile(&vlistfile2[i]);
		// 删除文件
		if(starg.ptype == 2){
			if(REMOVE(strlocalfilename) == false){
				logfile.Write("REMOVE(%s) failed.\n", strlocalfilename);
				return false;
			}
		}

		// 转存到备份目录
		if(starg.ptype == 3){
			char strlocalfilenamebak[301];
			SNPRINTF(strlocalfilenamebak, sizeof(strlocalfilenamebak), 300, "%s/%s", starg.localpathbak, vlistfile2[i].filename);
				if(RENAME(strremotefilename, strlocalfilenamebak) == false){
					logfile.Write("RENAME(%s, %s) failed.\n", strlocalfilename, strlocalfilenamebak);
					return false;
				}	
			}
		}
		

		return true;
	}

	// 把localpath目录下的文件加载到vlistfile2容器中
	bool LoadLocalFile(){
		vlistfile2.clear();
		CDir Dir;
		
		if(Dir.OpenDir(starg.localpath, starg.matchname) == false){
			logfile.Write("Dir.OpenDir(%s) 失败。\n", starg.localpath);
			return false; 
		}

	struct st_fileinfo stfileinfo;
	while(true){
		memset(&stfileinfo, 0, sizeof(stfileinfo));
	//logfile.Write("ftp.m_mtime(%s) 222222222222222222.\n", ftp.m_mtime);
		if(Dir.ReadDir() == false) break;

		strcpy(stfileinfo.filename, Dir.m_FileName);	// 文件名，不包括目录名
		strcpy(stfileinfo.mtime, Dir.m_ModifyTime);		// 文件时间
//		logfile.WriteEx("%s\n", Dir.m_FileName);   3 5 2 4 1 
		vlistfile2.push_back(stfileinfo);
	}

	return true;
}

  // 加载okfilename文件中的内容到容器vlistfile1中。
  bool LoadOKFile(){
	vlistfile1.clear();
	CFile File;
	// 注意：如果程序第一次上传，okfilename是不存在的，并不是错误，所以也要返回true
	if((File.Open(starg.okfilename, "r")) == false) return true; // 程序第一次运行，肯定返回失败呀	
	
	char strbuffer[501];

	struct st_fileinfo stfileinfo; 
        while(true){ 
                memset(&stfileinfo, 0, sizeof(stfileinfo)); 
                if(File.Fgets(strbuffer, 300, true) == false) break; 
				GetXMLBuffer(strbuffer, "filename", stfileinfo.filename);
				GetXMLBuffer(strbuffer, "mtime", stfileinfo.mtime);
                vlistfile1.push_back(stfileinfo); 
        }

	return true;
  }
  // 比较vlistfile2和vlistfile1，得到vlistfile3和vlistfile4
  bool      CompVector(){
	vlistfile3.clear(); vlistfile4.clear();

	int i, j;// 细节的在外面定义ij了

	// 遍历vlistfile2
	for( i = 0; i < vlistfile2.size(); i++){
		// 在vlistfile1中查找vlistfile2[i]的记录
		for( j = 0; j < vlistfile1.size(); j++){
			// 如果找到了，把记录放入vlistfile3
		       if((strcmp(vlistfile2[i].filename, vlistfile1[j].filename) == 0) &&
			     	(strcmp(vlistfile2[i].mtime, vlistfile2[j].mtime) == 0)){
		       		vlistfile3.push_back(vlistfile2[i]);
		 		break;
				}
    	}
		// 如果没有找到，把记录放入vlistfile4
		if(j == vlistfile1.size()) vlistfile4.push_back(vlistfile2[i]);	
	}	
	return true;
  }
  // 把容器vlistfile3中的内容写入okfilename文件，覆盖之前的旧okfilename文件
  bool      WriteToOKFile(){
	CFile File;
	if(File.Open(starg.okfilename, "w") == false){
		logfile.Write("File.Open(%s) failed.\n", starg.okfilename);
		return false;
	}
	for(int i = 0; i < vlistfile3.size(); i++)//可能3里面有好几个，同时拷贝过去, 
		File.Fprintf("<filename>%s</filename><mtime>%s</mtime>\n", vlistfile3[i].filename, vlistfile3[i].mtime);
	return true;
  }

	// 如果ptype == 1，把上传成功的文件记录追加到okfilename文件中。
  bool AppendToOKFile(struct st_fileinfo *stfileinfo){ // 这里有错啊=-=。。。。。。。。。。。。。。 
        CFile File; 
        if(File.Open(starg.okfilename, "a") == false){
                logfile.Write("File.Open(%s) failed.\n", starg.okfilename); 
                return false; 
        } 
		File.Fprintf("<filename>%s</filename><mtime>%s</mtime>\n", stfileinfo->filename, stfileinfo->mtime);
        return true; 
  } 
	

