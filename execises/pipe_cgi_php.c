#include<sys/types.h>
#include<unistd.h>
#include<stdio.h>


int main(void){

int n,fd[2],fds[2];
pid_t pid;
char line[10];
if(pipe(fd)<0){
fprintf(stderr,"pipe error\n");
exit(1);
}

if(pipe(fds)<0){
fprintf(stderr,"pipe error\n");
exit(1);
}

if((pid=fork())<0){
	fprintf(stderr,"fork error\n");
	exit(1);

	}
else if(pid==0){
//Child
	printf("child__\n");
	dup2(fd[1],1);
	execlp("php","php","index.php","a=2",NULL)
}
else{
//Master  
close(fd[1]);
if(waitpid(pid,NULL,0)<0){
fprintf(stderr,"waitpid error\n");
	exit(1);
}
printf("father__\n");
n=read(fd[0],line,100);
line[n]='\0';
printf("Msg: %s \n %d \n",line,n);
close(fd[0]);

}


exit(0);
}
