#include "_ftp.h"

Cftp ftp;

int main(){
	if(ftp.login("192.168.211.130:21", "jjyaoao", "gh") == false){
		printf("ftp.login(192.168.211.130:21) failed.\n");
		return -1;
	}
	printf("ftp.login(191.168.211.130:21) ok.\n");

	if(ftp.mtime("/project/public/socket/demo01.cpp") == false){
		printf("ftp.mtime(/project/public/socket/demo01.cpp) failed.\n");
		return -1;
	}
	printf("ftp.mtime(/project/public/socket/demo01.cpp) ok, mtime = %d.\n", ftp.m_mtime);

        if(ftp.size("/project/public/socket/demo01.cpp") == false){ 
                printf("ftp.size(/project/public/socket/demo01.cpp) failed.\n"); 
                return -1; 
        } 
        printf("ftp.size(/project/public/socket/demo01.cpp) ok, size = %d.\n", ftp.m_size); 

	if(ftp.nlist("/project/public/socket", "/aaa/bbb.lst") == false){
		printf("ftp.nlist(/project/public/socket) failed.\n");
		return -1;
	}
	printf("ftp.nlist(/project/public/socket) ok.\n");

	ftp.logout();

	return 0;
}
