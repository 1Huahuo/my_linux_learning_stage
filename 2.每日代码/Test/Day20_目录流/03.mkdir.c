#include <my_header.h>

int main(int argc, char *argv[]){
    // mkdir 创建目录。 
    mode_t mode = 0767;
    int ret = mkdir("test111", mode);

    printf("ret = %d\n", ret);
    ERROR_CHECK(ret, -1, "mkdir failed");


    // 权限可写可不写。
    mode_t mode1 = 0777;
    if(argc == 3){
        sscanf(argv[2], "%o", &mode);
    }

    int ret1 = mkdir(argv[1], mode);
    ERROR_CHECK(ret , -1, "mkdir failed");

    return 0;
}

