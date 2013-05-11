#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "parse.h"

extern char** environ;

void do_job(job* job_list){
	int index;
	job* jb;
	process* pr;
	int status_p;
	int pid_p;
	if((pid_p = fork()) == 0){
		//子プロセス
		for(index = 0, jb = job_list; jb != NULL; jb = jb->next, ++index){
			int pid;
			int status;
			int fd;
			int index = 0;	//プロセスカウント用
			int i;		//forループ用
			int pid_list[50];
			int old_fd[2] = {0, 0};
			int new_fd[2] = {0, 0};
			for(pr = jb->process_list; pr != NULL; pr = pr->next){
				old_fd[0] = new_fd[0];
				old_fd[1] = new_fd[1];
				if(pipe(new_fd) == -1)
					exit(EXIT_FAILURE);
				if((pid = fork()) == 0){
					//子プロセス
					if(pr->output_redirection != NULL){
						if(pr->output_option == TRUNC){
							fd = open(pr->output_redirection,O_CREAT|O_WRONLY|O_TRUNC,0666);
						}else if(pr->output_option == APPEND){
							fd = open(pr->output_redirection,O_CREAT|O_APPEND|O_WRONLY, 0666);
						}else{
							exit(EXIT_FAILURE);
						}
						if(fd == -1){
							exit(EXIT_FAILURE);
						}
						dup2(fd, 1);
					}
					if(pr->input_redirection != NULL){
						fd = open(pr->input_redirection,O_RDONLY,0666);
						if(fd == -1){
							exit(EXIT_FAILURE);
						}
						dup2(fd,0);
					}

					if(index ==0){
						close(new_fd[0]);
						if(pr->next != NULL)
							dup2(new_fd[1], 1);
					}else if(pr->next == NULL){
						close(old_fd[1]);
						dup2(old_fd[0],0);
						close(new_fd[0]);
						close(new_fd[1]);
					}else{
						close(old_fd[1]);
						dup2(old_fd[0], 0);
						close(new_fd[0]);
						dup2(new_fd[1] ,1);
					}
					execve(pr->program_name, pr->argument_list,environ);
					exit(EXIT_FAILURE);
				
				}else if(pid == -1){
					exit(EXIT_FAILURE);
				}else{
					//親プロセス
					pid_list[index] = pid;
					index ++;
					close(new_fd[1]);
					if(pr->next == NULL){
						close(new_fd[0]);
					}else{
						dup2(new_fd[0], 0);
					}
					continue;
				}
			}
		
			for(i = 0; i < index; i++){
				waitpid(pid_list[i], &status, WUNTRACED);
			}
		}
		exit(0);
	}else if (pid_p == -1){
			exit(EXIT_FAILURE);
	}else{
		//親プロセス
		waitpid(pid_p, &status_p, WUNTRACED);
	}
}

