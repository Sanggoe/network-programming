//----------------------------
// tcp_talkcli.c
// tcp_talkcli 127.0.0.1 9008
//----------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
//#include <signal.h>

#define MAXLINE 511

char *EXIT_STRING = "exit";
int input_and_send(int s);
int recv_and_print(int s);

int main(int argc, char *argv[]) {
	pid_t pid;
	int s;
	struct sockaddr_in servaddr;

	if (argc != 3) {
		printf("usage : %s server_ip, port_number\n", argv[0]);
		exit(0);
	}

	// 소켓 생성
	if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket fail");
		exit(1);
	}
	
	// 서버 소켓 주소 구조체 초기화
	bzero(&servaddr, sizeof(servaddr));

	// servaddr 셋팅
	servaddr.sin_family = AF_INET;
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
	servaddr.sin_port = htons(atoi(argv[2]));

	// connect()
	if (connect(s, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("connect fail");
		exit(0);
	}

	// fork()
	if ((pid = fork()) > 0) {	// parent
		input_and_send(s);
	} else if (pid == 0) {		// child
		recv_and_print(s);
	}

	close(s);
	return 0;
}

int input_and_send(int s) {
	char buf[MAXLINE + 1];
	int nbyte;

	while(fgets(buf, MAXLINE, stdin) != NULL) {
		nbyte = strlen(buf);
		write(s, buf, nbyte);

		if (strstr(buf, EXIT_STRING) != NULL) {
			puts("Good bye.");
			close(s);
			exit(0);
		}
	}
	return 0;
}

int recv_and_print(int s) {
	char buf[MAXLINE + 1];
	int nbyte;

	while (1) {
		if ((nbyte = read(s, buf, MAXLINE)) < 0) {
			perror("read fail");
			close(s);
			exit(0);
		}
		buf[nbyte] = 0;

		if (strstr(buf, EXIT_STRING) != NULL) {
			puts("Server quit. type exit.");
			// kill(getpid(), SIGINT);
			exit(0);
		}
		printf("%s", buf);
	}
	return 0;
}
