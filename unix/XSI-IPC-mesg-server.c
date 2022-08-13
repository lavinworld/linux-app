#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
 
/*
    接收端：测试IPC之消息队列有关函数
 */
 
#define BUFSIZE 1024
 
struct my_msg{
    long my_type;       //msg type
    char my_text[BUFSIZE];
};
 
int main(int argc , char *argv[]){
    key_t key;
    int id;
    int msgque_fd , n;
	struct my_msg mymsg;
 
    //argv is filepath and project id    
    if( argc != 3 )
        printf( " usage : ./a.out <filepath> <id>");
    id = atol( argv[2] );
 
//    if( stat( argv[1] , &buf )  == -1)
//        printf( " stat error");
 
    /* begin to create key */
    //key = ftok( argv[1] , 0x3FE);
    key = ftok( argv[1] , id );
 
    /* get msg queue fd */
//    if( ( msgque_fd = msgget( key , IPC_PRIVATE) )
    if( ( msgque_fd = msgget( key , IPC_CREAT ) )
        == -1 )
        printf( " msgget" );
    else
        printf( "msgque_fd=%d.\n" , msgque_fd );
 
    //send data
    while(1){
 
 
        /* recv text */
        if( (n = msgrcv( msgque_fd , &mymsg , BUFSIZE , 0 
						,0) ) == -1)
			printf( " msgrcv");
		
		/* show recv text */
		printf( "recv msg: %s.\n" ,mymsg.my_text );
    }
    return 0;
}
 