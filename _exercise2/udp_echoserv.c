#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>

#define MAXLINE 512

int main(int argc, char *argv[]) {
	struct sockaddr_in servaddr, cliaddr;
	int s, nbyte, addrlen = sizeof(cliaddr);
	char buf[MAXLINE + 1];

	if (argc != 2) {
		printf("Usage : %s port_number\n", argv[0]);
		exit(0);
	}

	// 소켓 생성
	if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket fail");
		exit(1);
	}

	// 소켓 주소 구조체 초기화
	bzero(&servaddr, sizeof(servaddr));
	bzero(&cliaddr, sizeof(cliaddr));

	// 서버 소켓 주소 셋팅
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(atoi(argv[1]));

	// 소켓 주소와 소켓 번호를 bind()
	if (bind(s, (struct sockaddr *)&servaddr, addrlen) < 0) {
		perror("bind fail");
		exit(1);
	}
	
	// iterative udp echo service 시작
	while(1) {
		puts("server waiting..");
		if ((nbyte = recvfrom(s, buf, MAXLINE, 0, (struct sockaddr *)&cliaddr, &addrlen)) < 0) {
			perror("recvfrom fail");
			exit(1);
		}
		buf[nbyte] = 0;
		printf("%d byte recv: %s", nbyte, buf);
		if (sendto(s, buf, nbyte, 0, (struct sockaddr *)&cliaddr, addrlen) < 0) {
			perror("sendto fail");
			exit(1);
		}
		puts("sendto complete\n");
	}
	close(s);

	return 0;
}
