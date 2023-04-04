#include <stdio.h>
#include <stdlib.h>
#include "sqlite3.h"
#define DATABASE "student.db"
#define N 128
int flag = 1;
int do_insert(sqlite3 *db)
{
    int id;
    char name[32];
    int score;
    char sql[128] = {};
    char *errmsg;
    printf("input id : ");
    scanf("%d", &id);
    getchar();
    printf("input name : ");
    scanf("%s", name);
    getchar();
    printf("input score : ");
    scanf("%d", &score);
    getchar();
    sprintf(sql, "insert into stu values(%d , '%s', %d)", id, name, score);
    if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
    {
        printf("%s\n", errmsg);
    }
    else
    {
        printf("insert done\n");
    }
    return 0;
}

int callback(void *arg, int ncolumn, char **f_value, char **f_name)
{
    int i = 0;
    if (flag == 1)
    {
        for (i = 0; i < ncolumn; i++)
        {
            printf("%-11s ", f_name[i]);
        }
        putchar(10);
        flag = 0;
    }
    for (i = 0; i < ncolumn; i++)
    {
        printf("%-11s ", f_value[i]);
    }
    putchar(10);
    return 0;
}
int do_select(sqlite3 *db)
{
    char sql[128] = {};
    char *errmsg;
    sprintf(sql, "select * from stu");
    if (sqlite3_exec(db, sql, callback, NULL, &errmsg) != SQLITE_OK)
    {
        printf("%s\n", errmsg);
    }
    else
    {
        printf("select done\n");
    }
    return 0;
}
int do_select_get_table(sqlite3 *db)
{
    char sql[N] = {};
    char *errmsg;
    char **resultp;
    int ncow;
    int ncolumn;
    int i = 0;
    int j = 0;
    sprintf(sql, "select * from stu");
    if (sqlite3_get_table(db, sql, &resultp, &ncow, &ncolumn, &errmsg) != SQLITE_OK)
    {
        printf("%s\n", errmsg);
    }
    else
    {
        printf("select dpne.\n");
    }
    for (j = 0; j < ncolumn; j++)

    {
        printf("%-8s ", resultp[j]);
    }
    putchar(10);
    int index = ncolumn;
    for (i = 0; i < ncow; i++)
    {
        for (j = 0; j < ncolumn; j++)
        {
            printf("%-8s ", resultp[index++]);
        }
        putchar(10);
    }
    sqlite3_free_table(resultp);
    return 0;
}
int do_delete(sqlite3 *db)
{
    int id;
    char sql[128] = {};
    char *errmsg;
    printf("input id : ");
    scanf("%d", &id);
    getchar();
    sprintf(sql, "delete from stu where id = %d", id);
    if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
    {
        printf("%s\n", errmsg);
    }
    else
    {
        printf("delete done\n");
    }
    return 0;
}
int do_update(sqlite3 *db)
{
    char sql[128] = {};
    char *errmsg;
    int id;
    int score;
    printf("input update id : ");
    scanf("%d", &id);
    getchar();
    printf("input score : ");
    scanf("%d", &score);
    getchar();
    sprintf(sql, "update stu set score = %d where id = %d", score, id);
    if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
    {
        printf("%s\n", errmsg);
    }
    else
    {

        printf("update done\n");
    }
    return 0;
}
int main(int argc, const char *argv[])
{
    sqlite3 *db;
    char *errmsg;
    int cmd;
    if (sqlite3_open(DATABASE, &db) != SQLITE_OK)
    {
        printf("%s\n", sqlite3_errmsg(db));
    }
    else
    {
        printf("sqlite is open done\n");
    }
    if (sqlite3_exec(db, "create table stu (id int, name char, score int);",
                     NULL, NULL, &errmsg) != SQLITE_OK)
    {
        printf("%s\n", errmsg);
    }
    else
    {
        printf("create table success\n");
    }
    while (1)
    {
        printf("********************************************************\n");
        printf("***1:插入数据 2:查询数据 3:删除数据 4:更改数据 5:退出***\n");
        printf("********************************************************\n");
        printf("input >>> ");
        scanf("%d", &cmd);
        getchar();
        switch (cmd)
        {
        case 1:
            do_insert(db);
            break;
        case 2:
            // flag = 1;
            // do_select(db);
            do_select_get_table(db);
            break;
        case 3:
            do_delete(db);
            break;
        case 4:
            do_update(db);
            break;
        case 5:
            sqlite3_close(db);
            exit(0);
        default:
            printf("Error cmd\n");
            break;
        }
    }
    return 0;
}