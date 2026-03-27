#include <inttypes.h>
#include <my_header.h>
#include <stdio.h>

void test(){
    // 刚开始程序启动的事后，我们的工作路径，就是shell所在位置
    // 使用chdir 会切换工作路径。 切换工作路径，会影响起点。
    FILE *fp1 = fopen("1.txt", "r");
    chdir("..");
    FILE *fp2 = fopen("1.txt", "r");
}

int main(int argc, char *argv[]){

    // chdir 切换工作路径
    // 每一个程序都有自己的工作路径。 工作路径，一般是继承的。 
    // 工作路径，就是相对路径的起点。 

    FILE *fp = fopen("1.txt", "r");
    ERROR_CHECK(fp, NULL, "fopen first failed");
    int ret = chdir("/");
    printf("ret = %d\n", ret);

    FILE *fp2 = fopen("1.txt", "r");
    ERROR_CHECK(fp2, NULL, "fopen second failed");

    return 0;
}

