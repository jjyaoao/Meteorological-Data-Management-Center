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

    // 定义用于超女信息的结构，与表中字段对应
    struct st_girls{
        long    id;         // 超女编号
        char    name[31];   // 超女姓名
        double  weight;     // 超女体重
        char    btime[20];  // 报名时间
    }   stgirls;

    sqlstatement stmt(&conn);  // 操作SQL语句的对象

    // 准备插入表的SQL语句   注意，时间哪里使用一个百分号会段错误，得用转译%,因为%d之类的，代表这需要一个整数... str_to_date(:4,'%%Y-%%m-%%d %%H:%%i:%%s'))");
    stmt.prepare("\
        insert into girls(id,name,weight,btime) values(:1,:2,:3,str_to_date(:4,'%%Y-%%m-%%d %%H:%%i:%%s'))");
    
       //printf("%s", stmt.m_cda.message); 
        stmt.bindin(1, &stgirls.id);
        stmt.bindin(2, stgirls.name, 30);
        stmt.bindin(3, &stgirls.weight);
        stmt.bindin(4, stgirls.btime, 19);

    // 模拟超女数据，向表中插入5条测试数据
    for(int i = 0; i < 5; i++){
        memset(&stgirls, 0, sizeof(struct st_girls));   // 结构体变量初始化
        
        // 为结构体变量的成员赋值
        stgirls.id = i + 1;                             // 超女编号
        sprintf(stgirls.name, "西施%05dgirl", i + 1);   // 超女姓名
        stgirls.weight = 45.25 + i;                     // 超女体重
        sprintf(stgirls.btime, "2022-05-07 14:47:%02d", i);

        if(stmt.execute() != 0){
            printf("stmt.execute() failed.\n%s\n%s\n", stmt.m_sql, stmt.m_cda.message);
            return -1;
        }
        printf("成功的插入了1条记录");
    }

    printf("insert table girls ok.\n");
    conn.commit();//提交数据库事务，不提交会回滚事务
    return 0;
}