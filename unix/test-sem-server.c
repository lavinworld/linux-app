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
#include <sys/sem.h>

/*
PV原子操作（很重要的）

   PV原子操作的具体定义如下：（好好理解，很重要的啊）

   ●  P操作：如果有可用的资源（信号量值>0），则此操作所在的进程占用一个资源（此时信号量值减1,进入临界区代码）;如果没有可用的资源（信号量值=0）,则此操作所在的进程被阻塞直到系统将资源分配给该进程（进入等待队列，一直等到资源轮到该进程）。

   ●  V操作：如果在该信号量的等待队列中有进程在等待资源，则唤醒一个阻塞进程;如果没有进程等待它，则释放一个资源（即信号量值加1）。
*/

union semun {
	int 			 val;	 /* Value for SETVAL */
	struct semid_ds *buf;	 /* Buffer for IPC_STAT, IPC_SET */
	unsigned short	*array;  /* Array for GETALL, SETALL */
	struct seminfo	*__buf;  /* Buffer for IPC_INFO
								(Linux-specific) */
};

/*
sem_op  正数
释放相应的资源数，将sem_op的值加到信号量的值上
sem_op  0
进程阻塞直到信号量的相应值为0，当信号量已经为0，函数立即返回。如果信号量的值不为0，则依据sem_flg的IPC_NOWAIT位决定函数动作。
sem_flg指定IPC_NOWAIT，则semop函数出错返回EAGAIN。sem_flg没有指定IPC_NOWAIT，则将该信号量的semncnt值加1，然后进程挂起直到下述情况发生。
信号量值为0，将信号量的semzcnt的值减1，函数semop成功返回；此信号量被删除（只有超级用户或创建用户进程拥有此权限），函数smeop出错返回EIDRM；
进程捕捉到信号，并从信号处理函数返回，在此情况将此信号量的semncnt值减1，函数semop出错返回EINTR
sem_op  负数
请求sem_op的绝对值的资源。如果相应的资源数可以满足请求，则将该信号量的值减去sem_op的绝对值，函数成功返回。当相应的资源数不能满足请求时，
这个操作与sem_flg有关。sem_flg指定IPC_NOWAIT，则semop函数出错返回EAGAIN。sem_flg没有指定IPC_NOWAIT，则将该信号量的semncnt值加1，
然后进程挂起直到下述情况发生：当相应的资源数可以满足请求，该信号的值减去sem_op的绝对值。成功返回；此信号量被删除（只有超级用户或创建用户进程拥有此权限），
函数smeop出错返回EIDRM：进程捕捉到信号，并从信号处理函数返回，在此情况将此信号量的semncnt值减1，函数semop出错返回EINTR
*/
void pKey(int semid)
{
	struct sembuf sops;
	
	sops.sem_num = 0;		  
	sops.sem_op  = -1; 	  
	sops.sem_flg = SEM_UNDO;
	semop(semid,&sops,1);
}
void vKey(int semid)
{
	struct sembuf sops;
	
	sops.sem_num = 0;		  
	sops.sem_op  = 1; 	  
	sops.sem_flg = SEM_UNDO;
	semop(semid,&sops,1);
}

/*
第一个参数是信号量集IPC标识符。
第二个参数是操作信号在信号集中的编号，第一个信号的编号是0。

第三个参数cmd

·IPC_STAT读取一个信号量集的数据结构semid_ds，并将其存储在semun中的buf参数中。
·IPC_SET设置信号量集的数据结构semid_ds中的元素ipc_perm，其值取自semun中的buf参数。
·IPC_RMID将信号量集从内存中删除。
·GETALL用于读取信号量集中的所有信号量的值。
·GETNCNT返回正在等待资源的进程数目。
·GETPID返回最后一个执行semop操作的进程的PID。
·GETVAL返回信号量集中的一个单个的信号量的值。
·GETZCNT返回正在等待完全空闲的资源的进程数目。
·SETALL设置信号量集中的所有的信号量的值。
·SETVAL设置信号量集中的一个单独的信号量的值。
*/
int main(int argc,char**argv)
{	
	key_t key;
	int pid;
	int semId;

	key = ftok(".",1);
	semId = semget(key,1,IPC_CREAT|0666);
	union semun initsem;
	initsem.val = 0;
	semctl(semId,0,SETVAL,initsem);
    
	pid = fork();
	if(pid>0){
		sleep(3);
		// pKey(semId);
		printf("this is in father\n");
		vKey(semId);
	}else if(pid == 0){
        pKey(semId);
		printf("this is in chird\n");
		vKey(semId);
		semctl(semId,0,IPC_RMID);
	}else{
		perror("fork error:");
		return -1;
	}
	

    system("ipcs -s") ;

   return 0 ;
}
 