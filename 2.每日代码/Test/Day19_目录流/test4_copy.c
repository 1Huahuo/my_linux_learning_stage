#include <my_header.h>
#include <stdio.h>

void copy_file_by_path(char *src_path, char *dest_path){
    FILE *src_fp = fopen(src_path, "r");

    /* if(src_fp == NULL){ */
    /*     // 打开失败。 */ 
    /*     printf("fopen src failed.\n"); */
    /*     return; */
    /* } */
    ERROR_CHECK(src_fp, NULL, "fopen src failed");

    FILE *dest_fp = fopen(dest_path, "w");

    /* if(dest_fp == NULL){ */
    /*     printf("fopen failed.\n"); */
    /*     fclose(src_fp); */
    /*     return; */
    /* } */
    ERROR_CHECK(dest_fp, NULL, "fopen dest failed");

    int ch;

    while((ch = fgetc(src_fp)) != EOF){
        fputc(ch, dest_fp);
    }

    fclose(src_fp);
    fclose(dest_fp);

}

int main(int argc, char *argv[]){
    // 将一些东西抽取成宏定义，以保持代码简洁

    ARGS_CHECK(argc, 3);

    copy_file_by_path(argv[1], argv[2]);


    return 0;
}

