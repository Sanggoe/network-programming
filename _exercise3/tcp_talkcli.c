#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>

char *EXIT_STRING = "exit";
int recv_and_print(int sd);
int input_and_send(int sd);

#define MAXLINE 511

int main(int argc, char *argv[]) {
	struct sockaddr_in servaddr;
	int s;
	pid_t pid;

	if(argc != 3) {
		printf("Usage: %s server_ip port_number\n", argv[0]);
		exit(0);
	}
	
	// create socket
	if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket fail");
		exit(1);
	}

	// initialize servaddr
	bzero((char *)&servaddr, sizeof(servaddr));

	// setting servaddr
	servaddr.sin_family = AF_INET;
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
	servaddr.sin_port = htons(atoi(argv[2]));
	
	// connect() call
	if (connect(s, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("connect fail");
		exit(0);
	}

	// parent process
	if (pid = fork() > 0) {
		input_and_send(s);
	} 
	// child process
	else if (pid == 0) {
		recv_and_print(s);
	}
	close(s);

	return 0;
}

int recv_and_print(int sd) {
	char buf[MAXLINE+1];
	int nbyte;

	while(1) {
		if((nbyte = read(sd, buf, MAXLINE)) < 0) {
			perror("read fail");
			close(sd);
			exit(0);
		}
		buf[nbyte] = 0;

		// exit string
		if (strstr(buf, EXIT_STRING) != NULL) {
			puts("Good bye. ");
			kill(getppid(), SIGINT);
			exit(0);
		}
		printf("%s", buf); // print
	}
}

int input_and_send(int sd) {
	char buf[MAXLINE+1];
	int nbyte;

	while(fgets(buf, sizeof(buf), stdin) != NULL) {
		nbyte = strlen(buf);
		write(sd, buf, strlen(buf));

		// exit string
		if (strstr(buf, EXIT_STRING) != NULL) {
			puts("Good bye.	");
			close(sd);
			exit(0);
		}
	}
	return 0;
}
