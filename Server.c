# include<stdio.h>
# include<unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include<time.h>
# include<sys/types.h>
# include<sys/wait.h>
# include<signal.h>
#include<pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>



void error(char *msg) {
	perror(msg);
	exit(1);
}
struct ProcessTable {
	char pid[100];
	char name[100];
	char status[100];
	char start[50];
	char endTime[50];
	float elapsedTime;
	int rawtime;
};
struct ProcessTable list[100];

struct serverstuff {
	char IP[100];
	int port;
	int fd[2];
	int fd2[2];
	char pid[10];
	int childprocs;
	int socketfd;
};
struct serverstuff serverlist[100];

int counter;
int child;
int fd[2];
int fd2[2];

static void sigint_handler(int signo) {
	//pause();
	if (signo == SIGCHLD) {
		//waiting
		while(1){
		int status;
		int pid = waitpid(-1, &status, WNOHANG);

		if(pid <= 0){
			break;
		}
	if (pid > 0) {
			char print[100];
			sprintf(print, "%d", pid);
			time_t rawtime;
			struct tm *info;
			char end[80];
			time(&rawtime);
			info = localtime(&rawtime);
			strftime(end, 80, "%X", info);
			write(STDOUT_FILENO, print, strlen(print));
			for (int k = 0; k < counter; k++) {
				if (strcmp(list[k].pid, print) == 0) {
					strcpy(list[k].status, "Inactive");
					strcpy(list[k].endTime, end);
					list[k].elapsedTime = difftime(rawtime, list[k].rawtime);
					break;
				}
			}
		}
	}
	}
}

void *lister(void *ptr) { //child client thread

	while (1) {
		char buffer[1024];
		memset(buffer, 0, sizeof(buffer));

		for (int x = 0; x <= child; x++) {
			if (strlen(serverlist[x].IP) != 0) {

				char what[10];
				sprintf(what, "%d", x);
				write(STDOUT_FILENO, what, 1); //structure counter no update
				char fromclient[1024];
				read(serverlist[x-1].fd[0], buffer, 1024);
				for (int k = 0; k < serverlist[x-1].childprocs+1; k++) {
					sprintf(fromclient,
							"pid: %s name: %s start time: %s, end time: %s, status: %s\n",
							list[k].pid, list[k].name, list[k].start,
							list[k].endTime, list[k].status);
					write(serverlist[x - 1].fd2[1], fromclient, strlen(fromclient));
				//	write(list2[x-1].socketfd, "what", 4); //HERE??
				}
			}
				if(strcmp(buffer, "exit")==0){
				for (int k = 0; k < serverlist[x-1].childprocs+1; k++) {
					int i;
					sscanf(list[k].pid ,"%d", &i);
					kill(i, SIGTERM);
					write(serverlist[x-1].fd2[1], "killed processes", strlen("killed processes"));
					write(STDOUT_FILENO, "killing", 7);
				}
			exit(0);
				}
				char temp[4];
				char message[15];
				memcpy(temp, buffer, 4);
				memcpy(message, &buffer[5] , strlen(buffer));
				if(strcmp(temp, "send")==0){
					write(serverlist[x-1].socketfd, message, strlen(message));
				}
  			break;

		}
	}
}

void *serverThread(void *ptr) {
	while (1) {
		write(STDOUT_FILENO, "please enter input", strlen("please enter input"));
		char buffer[1024];
		memset(buffer, 0, sizeof(buffer));
		int count = read(STDIN_FILENO, buffer, 100);
		char temp[4];
		memcpy(temp, buffer, 4);
		if (strcmp(temp, "send") == 0) {
			for (int m = 0; m < 100; m++) {
				if (strlen(serverlist[m].IP) != 0) {
			int f = write(serverlist[m].fd[1], buffer, strlen(buffer));
				}
			}
		}
		if (strcmp(buffer, "list connections\n") == 0) {
			for (int c = 0; c < 100; c++) {
				if (strlen(serverlist[c].IP) != 0) {
					write(STDOUT_FILENO, serverlist[c].IP, strlen(serverlist[c].IP));
					//write(STDOUT_FILENO, "\n", 1);
				}
			}
		}
		if (strcmp(buffer, "exit\n") == 0) {
			write(STDOUT_FILENO, "exiting.. . .\n", 13);
			for (int x = 0; x < 100; x++) {
				if (strlen(serverlist[x].IP) == 0) {

					write(serverlist[x-1].fd[1], "exit", strlen("exit"));
					read(serverlist[x-1].fd2[0], buffer, 1024);
				}
			//	write(STDOUT_FILENO, "exited", 7);
				exit(0);
				//	write(STDOUT_FILENO, "\n", 1);
			}
		}

		if (strcmp(buffer, "list\n") == 0) {
			for (int x = 0; x <=child; x++) {
				if (strlen(serverlist[x].IP) != 0) {
					//close(list2[x].fd[0]);
					//close(list2[x].fd2[1]);
					int f = write(serverlist[x].fd[1], buffer, strlen(buffer));
					if (f == -1) {
						write(STDOUT_FILENO, "can't write list to child pipe", 1);
					}
					memset(&buffer[0], 0, sizeof(buffer));
					int g = read(serverlist[x].fd2[0], buffer, 1024);
					if (g == -1) {
						write(STDOUT_FILENO, "read error", 10);
					}
					int z = write(STDOUT_FILENO, buffer, g);
				} else {
					break;}
		}
	}
}
}

int main(int argc, char *argv[]) {
	child = 0;

	pipe(fd);
	pipe(fd2);
	pthread_t thread1;
	int iret1 = pthread_create(&thread1, NULL, serverThread, NULL);

	memset(serverlist, 0, sizeof(serverlist));

	int sockfd, newsockfd, portno, clilen;
	char buffer[256];
	char temp[256];
	struct sockaddr_in serv_addr, cli_addr;
	int n;
	if (argc < 2) {
		fprintf(stderr, "ERROR, no port provided\n");
		exit(1);
	}
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("ERROR opening socket");
	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		error("ERROR on binding");

	listen(sockfd, 5);
	clilen = sizeof(cli_addr);
	while (1) {
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
				serverlist[child].fd[0] = fd[0];  //explicitly set child with main pipe fds 
				serverlist[child].fd[1] = fd[1];
				serverlist[child].fd2[0] = fd2[0];
				serverlist[child].fd2[1] = fd2[1];
				strcpy(serverlist[child].IP, inet_ntoa(cli_addr.sin_addr));
				serverlist[child].port = (int) ntohs(cli_addr.sin_port);
				serverlist[child].socketfd = newsockfd;
				child++; //structure counter

		if (newsockfd < 0)
			error("ERROR on accept");

		int cpid = fork();
		if (cpid > 0) {
			char pids[10];
		sprintf(pids, "%d", cpid);
		strcpy(serverlist[child-1].pid, pids);
		}

		if (cpid == 0) {
			counter = 0;
			pthread_t thread2;
			int iret2 = pthread_create(&thread2, NULL, lister, NULL);
			//signal handler
			if (signal(SIGCHLD, sigint_handler) == SIG_ERR) {
				write(STDOUT_FILENO, "signal error", 12);
			}
			for (;;) {

				memset(buffer, 0, sizeof(buffer));
				int socread = read(newsockfd, buffer, 256);
				if(socread == -1){
					write(STDOUT_FILENO, "error in (infinite loop)", 38);
				}

				if (socread < 0) {
					error("ERROR reading from socket or disconnected");
				}
				strcpy(temp, buffer);
				//write(STDOUT_FILENO, buffer, strlen(buffer));
				char *token;
				token = strtok(buffer, " ,.\n");

				//add
				if (strcmp(token, "add") == 0) {
					int result = 0;
					int out = 0;
					token = strtok(NULL, " ,.\n");
					while (token != NULL) {
					out = (int) atoi(token);
						result = result + out;
						token = strtok(NULL, " ,.\n");
					}

					char printer[100];
					sprintf(printer, "result of add: %d", result);

					n = write(newsockfd, printer, strlen(printer));
					if (n < 0)
						error("ERROR writing to socket");
				}

				else if (strcmp(token, "sub") == 0) {  //subtract

					int result = 0;
					int out = 0;

					token = strtok(NULL, " ,.\n");
					out = (int) atoi(token);
					result = out;
					token = strtok(NULL, " ,.\n");

					while (token != NULL) {
						out = (int) atoi(token);
						result = result - out;
						token = strtok(NULL, " ,.\n");
					}
					char printer[100];
					sprintf(printer, "result of sub: %d", result);
					int z = write(newsockfd, printer, strlen(printer));
					if (z == -1) {
						error("error writing to socket");
					}

				}  //subtract

				else if (strcmp(token, "mul") == 0) {
					int result = 1;
					int out = 0;
					token = strtok(NULL, " ,.\n");
					while (token != NULL) {
						out = (int) atoi(token);
						result = result * out;
						token = strtok(NULL, " ,.\n");
					}

					char print[100];
					sprintf(print, "result of sub: %d", result);
					int z = write(newsockfd, print, strlen(print));
					if (z == -1) {
						error("Error writing to socket");
					}
				} //multiply

				else if (strcmp(token, "div") == 0) {
					float result = 1.00;
					float out = 0;
					token = strtok(NULL, " ,.\n");
					while (token != NULL) {
						out = (float) atof(token);
						result = out / result;
						token = strtok(NULL, " ,.\n");	}

					char print[100];
					sprintf(print, "result of sub: %d", result);
					int z = write(newsockfd, print, strlen(print));
					if (z == -1) {
						error("error writing to socket");
					}
				} //divide

				else if (strcmp(token, "list") == 0) {
					char dump[1024];
					for (int i = 0; i < counter; i++) {

						if (strcmp(list[i].status, "Active") == 0) {
							//	write(STDOUT_FILENO, "list", strlen("list"));
							sprintf(dump,
									"pid: %s name: %s start time: %s, end time: %s, status: %s\n",
									list[i].pid, list[i].name,
									list[i].start, list[i].endTime,
									list[i].status);
							//strcat(printer, dump);
							write(newsockfd, dump, strlen(dump));
						}
					}
					//		write(newsockfd, printer, strlen(printer));
				} //list

				else if (strcmp(token, "listall") == 0) {
					char printer[1000];
					memset(printer, 0, sizeof(printer));

					for (int i = 0; i < counter; i++) {
						time_t rawtime;
						struct tm * timeinfo;
						time(&rawtime);
						timeinfo = localtime(&rawtime);
						list[i].elapsedTime = difftime(rawtime,
								list[i].rawtime);
						sprintf(printer,
								"pid: %s name: %s start time: %s, end time: %s, elapsed time: %f, status: %s\n",
								list[i].pid, list[i].name, list[i].start,
								list[i].endTime, list[i].elapsedTime, list[i].status);
						write(newsockfd, printer, strlen(printer));
					}
				} //listall

				else if (strcmp(token, "kill") == 0) {

					token = strtok(NULL, " ,.\n");
					//write(STDOUT_FILENO, token, strlen(token));
					for (int k = 0; k < counter; k++) {
						if (strcmp(list[k].pid, token) == 0
								&& strcmp(list[k].status, "Active") == 0) {
							int pid = (int) atoi(token);
							kill(pid, SIGTERM);

							break;
						}
						if (strcmp(list[k].name, token) == 0
								&& strcmp(list[k].status, "Active") == 0) {
							char ptr[10];
							strcpy(ptr, list[k].pid);
							int pid = (int) atoi(ptr);
							kill(pid, SIGTERM);
							break;
						}

						if (strcmp(token, "all") == 0
								&& strcmp(list[k].status, "Active") == 0) {
							char ptr[10];

							time_t rawtime;
							struct tm *info;
							char end[80];
							time(&rawtime);
							info = localtime(&rawtime);
							strftime(end, 80, "%X", info);

							strcpy(ptr, list[k].pid);
							strcpy(list[k].status, "Inactive");
							strcpy(list[k].endTime, end);
							int pid = (int)atoi(ptr);
							kill(pid, SIGTERM);
						}
					}
					int z = write(newsockfd, "killed", strlen("killed"));
				} //kill

				else if (strcmp(token, "run") == 0) {
					//int fd[2];
					int fd2run[2];
					//pipe(fd);
					pipe(fd2run);
					fcntl(fd2run[1], F_SETFD, FD_CLOEXEC);
					int exec = fork();
					if (exec > 0) {
						char buff[100];
						memset(buff, 0, sizeof(buff));
						close(fd2run[1]);
						//close(fd[1]);
						int c = read(fd2run[0], buff, sizeof(buff));
						if (c == 0) {
							char print[100];
							sprintf(print, "%d", exec);

							time_t rawtime;
							struct tm * info;
							time(&rawtime);
							info = localtime(&rawtime);
							list[counter].rawtime = rawtime;
							char start[100];
							strftime(start, 80, "%X", info);

							strcpy(list[counter].pid, print);
							strcpy(list[counter].status, "Active");
							token = strtok(temp, "  .\n");
							token = strtok(NULL, " .\n");
							strcpy(list[counter].name, token);
							strcpy(list[counter].start, start);
							strcpy(list[counter].endTime, "running");
							serverlist[child].childprocs = counter;
							counter++;
							n = write(newsockfd, "running.. .", 11);
						} else {
							c = read(fd2run[0], buff, sizeof(buff));
						}

						if (n < 0)
							error("ERROR writing to socket");
						//sleep(1);
					}
					if (exec == 0) {
						char *address[50];
						int j = 0;
						while (token = strtok(NULL, " ,.\n")) {
							address[j] = token;
							j++;}
						int x = execvp(address[0], address);
						if(x ==-1){
							write(STDOUT_FILENO, "exec error", 10);
						}
						int z = write(fd2run[1], "k", 1);
					}
				}
				else if (strcmp(token, "disconnect") == 0) {
					write(newsockfd, "disconnected by client",
							strlen("disconnected by client"));
					close(newsockfd);
				}
			}
		} else {
			close(newsockfd);
		}
	}
	return 0;
}
