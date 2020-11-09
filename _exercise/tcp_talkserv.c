//--------------------
// tcp_talkserv2.c
// tcp_talkserv2 9008
//--------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <signal.h>
#include <netdb.h>

char *EXIT_STRING = "exit";	// 종료문자
int recv_and_print(int sd);	// 상대에게 메시지 수신, 출력
int input_and_send(int sd);	// 키보드로 입력받아 상대에게 전달

#define MAXLINE 511

struct sockaddr_in cliaddr, servaddr;

int main(int argc, char *argv[]) {
	int s, addrlen = sizeof(cliaddr);
	pid_t pid;

	if (argc != 2) {
		printf("usage : talkserv port_number\n", argv[0]);
		exit(1);
	}
	
	// 소켓 생성
	if((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket fail");
		exit(0);
	}

	// 소켓 주소 구조체 초기화
	bzero((struct sockaddr *)&servaddr, addrlen);
	bzero((struct sockaddr *)&cliaddr, addrlen);
	
	// servaddr setting
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(atoi(argv[1]));
	
	// bind() 호출
	if (bind(s, (struct sockaddr *)&servaddr, addrlen) < 0) {
		perror("bind fail");
		exit(0);
	}
	// ready
	

	puts("server wait");	
		
	nbyte = recvfrom(s, buf, MAXLINE, 0, (struct sockaddr*)&cliaddr, &addrlen);
	if(nbyte<0) {
		perror("recvfrom fail");
		exit(1);	
	}
	buf[nbyte] = 0;
	printf("%d byte recv: %s\n", nbyte, buf);

	if (sendto(s, buf, nbyte, 0, (struct sockaddr*)&cliaddr, addrlen) < 0) {
		perror("sendto fail");
		exit(1);
	}
	puts("sendto complete");	
	
	// fork() 호출
	if ((pid = fork()) > 0) {	// parent
		input_and_send(s);
	 else if (pid == 0) {		// child
		recv_and_print(s);
	}
	close(s);
		
	return 0;
}


int recv_and_print(int sd) {	// 상대에게 메시지 수신, 출력 - 자식 프로세스 호출
	char buf[MAXLINE + 1];
	int nbyte;
	
	while(1) {
		nbyte = recvfrom(sd, buf, MAXLINE, 0, (struct sockaddr*)&cliaddr, &addrlen);
		if (nbyte < 0) {
			perror("recvfrom fail");
			close(sd);
			exit(0);
		}
		buf[nbyte] = 0;

		if (strstr(buf, EXIT_STRING) != NULL) {
			puts("client quit. type exit.");
			close(sd);
			exit(0);
		}
		printf("%s", buf);
	}
}


int input_and_send(int sd) {	// 키보드로 입력받아 상대에게 전달- 부모 프로세스 호출
	char buf[MAXLINE + 1];
	int nbyte;

	while(fgets(buf, sizeof(buf), stdin) != NULL) {
		nbyte = strlen(buf);
		if (sendto(sd, buf, nbyte, 0, (struct sockaddr*)&cliaddr, addrlen) < 0) {
			perror("sendto fail");
			exit(1);
		}
		if (strstr(buf, EXIT_STRING) != NULL) {
			puts("Good bye.");
			break;
		}
	}
	close(sd);
	return 0;
}
