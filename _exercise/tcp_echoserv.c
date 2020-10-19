#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>

#define MAXLINE 127

int main(int argc, char *argv[]) {
	struct sockaddr_in servaddr, cliaddr;
	int listen_sock, accp_sock,
	    addrlen = sizeof(cliaddr),
	    nbyte;
	char buf[MAXLINE+1];

	if (argc != 2) {
		printf("Usage: %s port\n", argv[0]);
		exit(0);
	}

	// socket 생성
	if ((listen_sock = socket(PF_INET, SOCK_STREAM, 0)) < 0 ) {
		perror("socket fail");
		exit(0);
	}

	// servaddr 0으로 초기화
	bzero(&servaddr, sizeof(servaddr));

	// servaddr 셋팅
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(atoi(argv[1]));

	// bind() 호출
	if (bind(listen_sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("bind fail");
		exit(0);
	}
	// socket 대기모드 셋팅.
	listen(listen_sock, 5);

	// iterative echo service start!
	while(1) {
		puts("서버가 요청을 기다림 ..");
		// connect() 기다림. block 상태
		accp_sock = accept(listen_sock, (struct sockaddr *)&cliaddr, &addrlen);
		if (accp_sock < 0) {
			perror("accept fail");
			exit(0);
		}
		nbyte = read(accp_sock, buf, MAXLINE);
		buf[nbyte] = 0;
		write(accp_sock, buf, nbyte);
		close(accp_sock);
	}
	close(listen_sock);

	return 0;
}
