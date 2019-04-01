#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
# include<unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
# include <sys/types.h>
# include <signal.h>
#include<pthread.h>

void error(char *msg)
{
	perror(msg);
	exit(0);
}


void *reading(void *sock)
{
    char buf[1024];
    int fd = *(int *)sock;
    char prompt[] = "\nplease enter input\n";
    for(;;){
    	memset(&buf[0], 0, sizeof(buf));
   		write(STDOUT_FILENO, prompt, strlen(prompt));
    int nread = read(fd, buf, 1024);
    write(STDOUT_FILENO, buf, nread);
    }
    return NULL;
}


void *writing(void *sock) //writing to socket
{
    char buf[1024];
    int fd = *(int *)sock;

    for(;;){
   		memset(&buf[0], 0, sizeof(buf));
   		int count = read(STDIN_FILENO, buf, 100);
   		int n = write(fd,buf,count);
       if (n < 0)
            error("ERROR writing to socket");
    }

    return NULL;
}

void main()
{
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char buffer[256];
	char *token;

	while(1){
		write(STDOUT_FILENO,"Please connect:",strlen("Please connect:"));
		int count = read(STDIN_FILENO, buffer, 256);
		token = strtok(buffer, " \n");
		if (strcmp(token, "connect") == 0) {
			break;
		}
		else{
			write(STDOUT_FILENO,"connect first", strlen("connect first"));
		}

	}

	token = strtok(NULL, " \n");
	server = gethostbyname(token);
	if (server == NULL) {
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}
	token = strtok(NULL, " \n");
    	portno = atoi(token);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("ERROR opening socket");
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr,
	(char *)&serv_addr.sin_addr.s_addr,
	 server->h_length);
	serv_addr.sin_port = htons(portno);
	if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
		error("ERROR connecting");

	   pthread_t inputhread, outputhread;
	   int iret1, iret2;

	   pthread_create(&outputhread, NULL, writing, (void *)&sockfd);
	   pthread_create(&inputhread, NULL, reading, (void *)&sockfd);

	while(1){
                memset(&buffer[0], 0, sizeof(buffer));
		int n = read(sockfd,buffer,255);
		if (n < 0) error("ERROR reading from socket");
		write(STDOUT_FILENO, buffer, strlen(buffer));
   		write(STDOUT_FILENO,"\n", strlen("\n"));

	}


}

