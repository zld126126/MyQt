#include <stdio.h>
#include "sqlite3.h"

//查询的回调函数 ------- 每一条结果调用一次
int sql_callback(void *arg,int col,char **str,char **name)
{
	// 消除未使用参数警告
	(void)arg;
	
	int i;
	for(i=0;i<col;i++){
		printf("%s:%s ",name[i],str[i]);//打印本字段
	}
	printf("\n");
	
	return SQLITE_OK;
}

// 检查表是否存在的回调函数
static int check_table_callback(void *data, int argc, char **argv, char **azColName)
{
	// 消除未使用参数警告
	(void)argc;
	(void)azColName;
	
	// 将查询结果转换为整数并存储到传入的数据指针中
	*((int*)data) = (argv[0] && atoi(argv[0]) > 0) ? 1 : 0;
	return 0;
}

int main()
{
	//声明数据库的句柄
	sqlite3 *pDb = NULL;
	char *errMsg = NULL;
	
	//1.打开数据库 - 使用项目中已存在的数据库文件
	int res = sqlite3_open("./sqlite3demo.db",&pDb);
	if(res!=SQLITE_OK){
		printf("打开数据库失败!\n");
		return -1;
	}
	printf("数据库打开成功!\n");
	
	//2.检查表格是否存在
	const char *checkTableSql = "SELECT count(*) FROM sqlite_master WHERE type='table' AND name='stutbl';";
	int tableExists = 0;
	res = sqlite3_exec(pDb, checkTableSql, check_table_callback, &tableExists, &errMsg);
	
	if(res != SQLITE_OK) {
		printf("检查表格失败: %s\n", errMsg);
		sqlite3_free(errMsg);
		sqlite3_close(pDb);
		return -1;
	}
	
	//3.如果表格不存在，则创建表格
	if(!tableExists) {
		printf("表格stutbl不存在，正在创建...\n");
		const char *createTableSql = "create table stutbl(id int unique,name ntext,age int);";
		res = sqlite3_exec(pDb, createTableSql, NULL, NULL, &errMsg);
		if(res != SQLITE_OK) {
			printf("创建表格失败: %s\n", errMsg);
			sqlite3_free(errMsg);
			sqlite3_close(pDb);
			return -1;
		}
		printf("表格创建成功!\n");
		
		//4.插入一些示例数据
		const char *insertDataSql = "insert into stutbl values(1,'张三',20),(2,'李四',22),(3,'王五',21);";
		res = sqlite3_exec(pDb, insertDataSql, NULL, NULL, &errMsg);
		if(res != SQLITE_OK) {
			printf("插入数据失败: %s\n", errMsg);
			sqlite3_free(errMsg);
		}
	} else {
		printf("表格stutbl已存在\n");
	}
	
	//5.执行查询
	printf("执行查询操作...\n");
	const char *querySql = "select * from stutbl;";
	res = sqlite3_exec(pDb, querySql, sql_callback, NULL, &errMsg);
	if(res != SQLITE_OK) {
		printf("查询失败: %s\n", errMsg);
		sqlite3_free(errMsg);
	}
	
	//6.关闭数据库
	sqlite3_close(pDb);
	
	return 0;
}
