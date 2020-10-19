// client

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>

#define MAXLINE 511

int main(int argc, char *argv[]) {
	struct sockaddr_in servaddr;
	int s, nbyte, addrlen = sizeof(servaddr);
	char buf[MAXLINE + 1];
	int test;
	
	if(argc != 3) {
		printf("Usage: %s ip_address port_number\n", argv[0]);
		exit(0);
	}

	// socket 생성
	if((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket fail");
		exit(1);
	}
	
	// echo server's socket address structure 작성
	bzero(&servaddr, addrlen);
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	//inet_pton(AF_INET, argv[1], &servaddr.sin_addr);   // same!
	servaddr.sin_port = htons(atoi(argv[2]));

	// keyborad input
	printf("입력 : ");
	if (fgets(buf, sizeof(buf), stdin) == NULL) { 
		printf("fgets fail");
		exit(0);
	}
	
	// send message to echo server
	if ((test=sendto(s, buf, strlen(buf), 0,
		(struct sockaddr *)&servaddr, addrlen)) < 0) {
		perror("sendto fail");
		exit(0);
	}

	// receive message from echo server
	if ((nbyte = recvfrom(s, buf, MAXLINE, 0,
		(struct sockaddr *)&servaddr, &addrlen)) < 0) {
		perror("recvfrom fail");
		exit(0);
	}

	buf[nbyte] = 0;
	printf("%s\n", buf);
	close(s);
	return 0;
}
