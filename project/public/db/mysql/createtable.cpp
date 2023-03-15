/*
*   程序名：createtable.cpp 此程序演示开发框架操作MySQL数据库（创建表）
*   作者：jjyaoao
*/

#include "_mysql.h"     // 开发框架操作MySQL的头文件

int main(int argc, char *argv[]){
    connection conn;       // 数据库连接类

    // 登录数据库，返回值：0-成功；其它是失败，存放了MySQL的错误代码。
    // 失败代码在conn.m_cda.rc中，失败描述在conn.m_cda.message中。
    if(conn.connecttodb("127.0.0.1,root,123456,h2,3306","utf8") != 0){
          printf("connect database failed.\n%s\n", conn.m_cda.message);
          return -1;
    }

    sqlstatement stmt(&conn);   // 执行sql语句的对象
    // sqlstatement stmt;   stmt.connect(&conn);  和上面一行一样的意思
     stmt.prepare("create table girls(id      bigint(10),\
                   name    varchar(30),\
                   weight  decimal(8,2),\
                   btime   datetime,\
                   memo    longtext,\
                   pic     longblob,\
                   primary key (id))");
     if (stmt.execute()!=0){
    printf("stmt.execute() failed.\n%s\n%s\n",stmt.m_sql,stmt.m_cda.message); return -1;
  }
    printf("create table girls ok.\n");
    return 0;
}