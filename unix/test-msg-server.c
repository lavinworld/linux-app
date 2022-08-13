#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>

#define TEXT_SIZE  512

struct msgbuf
{
    long mtype ;
    int  status ;
    char time[20] ;
    char mtext[TEXT_SIZE] ;
}  ;

int main(int argc, char **argv)
{
    int msqid ;
    struct msqid_ds info ;
    struct msgbuf buf1 ;
    int flag ;
    int  recvlength ;
    int key ;
    int mtype ;
    int id;
 
    //argv is filepath and project id    
    if( argc != 3 )
        printf( " usage : ./a.out <filepath> <id>");
    id = atol( argv[2] );

    key = ftok(argv[1], id ) ;
    if ( key < 0 )
    {
        perror("ftok key error") ;
        return -1 ;
    }

    msqid = msgget( key, IPC_CREAT ) ;
    if ( msqid < 0 )
    {
        perror("get ipc_id error") ;

        return -1 ;
    }

    recvlength = sizeof(struct msgbuf) - sizeof(long) ;
    memset(&buf1, 0x00, sizeof(struct msgbuf)) ;
    /*
为0时，就是接收队列里的第一个消息，绝对是第一个消息；
>0时，接收队列中第一个类型为msgtype的消息，这里不一定是队列首个消息，可以读取队列里第2、3、....个消息，这个经过测试验证过了。
<0时，接收队列中第一个小于、等于|msgtype|的消息，不一定是第一个消息，如果队首消息大于|msgtype|将不会被取走。
结论：
操作系统提供的消息队列，确保同一个类型的消息是先进先出的，整个队列的消息可能不是先进先出的。
    */
    mtype = 0 ;
    flag = msgrcv( msqid, &buf1, recvlength ,mtype,0 ) ;

    if ( flag < 0 )
    {
        perror("recv message error\n") ;
        return -1 ;
    }

    printf("type=%ld,time=%s, message=%s\n", buf1.mtype, buf1.time,  buf1.mtext) ;

    system("ipcs -q") ;

   return 0 ;
}
 