# mysql头文件存放的目录。
MYSQLINCL = -I/usr/local/mysql/include

# mysql库文件存放的目录。
MYSQLLIB = -L/usr/local/mysql/lib

# 需要链接的mysql库。   缺省是动态库
MYSQLLIBS = -lmysqlclient
CFLAGS=-g -Wno-write-strings

all:createtable inserttable updatetable selecttable filetoblob

createtable:createtable.cpp _mysql.h _mysql.cpp
		g++ $(CFLAGS) -o createtable createtable.cpp $(MYSQLINCL) $(MYSQLLIB) $(MYSQLLIBS) _mysql.cpp -lm -lc `mysql_config --cflags --libs` 

inserttable:inserttable.cpp _mysql.h _mysql.cpp
		g++ $(CFLAGS) -o inserttable inserttable.cpp $(MYSQLINCL) $(MYSQLLIB) $(MYSQLLIBS) _mysql.cpp -lm -lc `mysql_config --cflags --libs` 

updatetable:updatetable.cpp _mysql.h _mysql.cpp
		g++ $(CFLAGS) -o updatetable updatetable.cpp $(MYSQLINCL) $(MYSQLLIB) $(MYSQLLIBS) _mysql.cpp -lm -lc `mysql_config --cflags --libs` 

selecttable:selecttable.cpp _mysql.h _mysql.cpp
		g++ $(CFLAGS) -o selecttable selecttable.cpp $(MYSQLINCL) $(MYSQLLIB) $(MYSQLLIBS) _mysql.cpp -lm -lc `mysql_config --cflags --libs` 

filetoblob:filetoblob.cpp _mysql.h _mysql.cpp
	g++ $(CFLAGS) -o filetoblob filetoblob.cpp $(MYSQLINCL) $(MYSQLLIB) $(MYSQLLIBS) _mysql.cpp -lm -lc `mysql_config --cflags --libs`

clean:
		rm -rf createtable inserttable updatetable selecttable filetoblob