#include <my_header.h>

void deepdir(char *pathname, int depth){
    DIR *dirp=opendir(pathname);
    ERROR_CHECK(dirp,NULL,"opendir failed.\n");
    struct dirent *dirent_p=NULL;
    while((dirent_p=readdir(dirp))!=NULL){
        if(strcmp(dirent_p->d_name, ".") == 0 || strcmp(dirent_p->d_name, "..") == 0){
            continue;
        }
        if(dirent_p->d_type==DT_DIR){
            for(int i=0;i<depth;i++){
                printf("\t");
            }
            printf("├──");
            printf("%s\n",dirent_p->d_name);
            char new_path[256];
            sprintf(new_path, "%s/%s", pathname, dirent_p->d_name);
            deepdir(new_path,depth + 1);
        }
        else{
            for(int i=0;i<depth;i++){
                printf("\t");
            }
            printf("├──");
            printf("%s\n",dirent_p->d_name);

        }

    }
    closedir(dirp);
}
/* Usage:  */
int main(int argc, char *argv[]){
    deepdir(".",0);
    return 0;
}
