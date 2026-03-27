#include <my_header.h>

/*
1.采用mkdir 的方案，批量创建1000个文件夹。
最外层的路径，由命令行参数给出。
里层的名字分别为 test1 test2 test3 .... test1000

在test1中，分别也创建 test10 test11 test12... test20
在test2中，创建 test20 test21 test22 ... test30
依次类推。 感受一下，为什么需要用程序来做这些事情。
*/
int main(int argc, char *argv[]){
    char *root_path = NULL;
    if(argc == 1){
        root_path = ".";
    }else{
        ARGS_CHECK(argc, 2);
        root_path = argv[1];
    }
    chdir(root_path);

    for(int i = 1; i <= 1000; i++){
        char dir_path[1024];
        sprintf(dir_path, "./test%d", i);
        mkdir(dir_path, 0775);
        chdir(dir_path);
        for(int j = 0; j <= 10; j++){
            char dir_path1[1024];
            sprintf(dir_path1, "./test%d", i*10 + j);
            mkdir(dir_path1, 0775);
        }
        chdir("..");
     }

    return 0;
}
