#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */

#define MMAP_DATA_SIZE 1024

//进程1，写入数据:write by t08
int main(int argc, char **argv)
{

    int flag ;
    int  recvlength ;
    int key ;
    int mtype ;

    int fd = shm_open("/sz02-shm", O_CREAT|O_RDWR|O_EXCL, 0777);
    if(fd < 0) // 说明这个共享内存对象已经存在了
    {
        fd = shm_open("/sz02-shm", O_RDWR, 0777);
        printf("open ok\n");
        if(fd < 0)
        {
            printf("error open shm object\n");
            return 0;
        }
    }
    else
    {
        printf("create ok\n");
        // 说明共享内存对象是刚刚创建的
        ftruncate(fd, MMAP_DATA_SIZE); // 设置共享内存的尺寸
    }
    char* ptr = (char*)mmap(NULL, MMAP_DATA_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    strcpy(ptr, "write by t08");


    getchar();
    shm_unlink("/sz02-shm");
    munmap(ptr,MMAP_DATA_SIZE);

    // system("ipcs -m") ;

   return 0 ;
}
 