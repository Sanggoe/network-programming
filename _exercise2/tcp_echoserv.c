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
	char buf[MAXLINE + 1];

	if (argc != 2) {
		printf("Usage: %s port\n", argv[0]);
		exit(0);
	}
 
	// 소켓 생성
	if ((listen_sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket fail");
		exit(1);
	}

	// 소켓 주소 구조체 초기화
	bzero(&servaddr, sizeof(servaddr));
	bzero(&cliaddr, sizeof(cliaddr)); /////////////////

	// 소켓 주소 구조체 셋팅
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(atoi(argv[1]));

	// 소켓 번호와 소켓 주소를 bind()
	if (bind(listen_sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("bind fail");
	}

	// 소켓 번호를 수동적 소켓으로 listen()
	listen(listen_sock, 5);

	// iterative 에코 서버 수행
	while(1) {
		puts("server waiting..");
		// 클라이언트의 연결 요청 accept()
		if ((accp_sock = accept(listen_sock, (struct sockaddr *)&cliaddr, &addrlen)) < 0) {
			perror("accept fail");
			exit(0);
		}
		
		// 클라이언트가 보낸 정보 수신 read
		if ((nbyte = read(accp_sock, buf, MAXLINE)) < 0) { /////////////////////////////
			perror("read fail");
			exit(0);
		}
		buf[nbyte] = 0;

		// 클라이언트에게 다시 송신 write
		if (write(accp_sock, buf, nbyte) < 0) {
			perror("write fail");
			exit(0);
		}
		// 통신할 소켓 close
		close(accp_sock);
	}
	// 연결 요청을 받을 소켓 close
	close(listen_sock);

	return 0;
}	
