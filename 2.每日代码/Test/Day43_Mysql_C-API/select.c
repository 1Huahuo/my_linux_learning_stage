#include <my_header.h>

int main(int argc, char* argv[]){                                  
    MYSQL* conn;
    MYSQL_RES* res;
    MYSQL_ROW row;

    conn = mysql_init(NULL);
    if(!mysql_real_connect(conn, "localhost", "root", "123456", "task", 0, NULL, 0)){
        printf("%s\n", mysql_error(conn));
        return 1;
    }
    mysql_set_character_set(conn, "utf8mb4");

    if(mysql_query(conn, "select * from student")){ // 0-成功，非0-失败
        printf("%s\n", mysql_error(conn));
        return 1;
    }
    
    res = mysql_store_result(conn);
    while((row = mysql_fetch_row(res))){
        for(int i = 0; i < mysql_num_fields(res); i++)
            printf("%s\t\t", row[i]);
        printf("\n");
    }

    mysql_free_result(res);
    mysql_close(conn);
    return 0;
}
