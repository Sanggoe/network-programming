#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>

#define MAXLINE 127

int main(int argc, char *argv[]) {
	int s, nbyte;
	struct sockaddr_in servaddr;
	char buf[MAXLINE + 1];
 
	if (argc != 3) {
		printf("Usage: %s ip_address port_num\n", argv[0]);
		exit(0);
	}

	// 소켓 생성
	if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket fail");
		exit(1);
	}
	
	// 에코 서버 소켓주소 구조체 초기화
	bzero(&servaddr, sizeof(servaddr));

	// servaddr 주소 지정
	servaddr.sin_family = AF_INET;
	//inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	servaddr.sin_port = htons(atoi(argv[2]));

	// connect 요청
	if (connect(s, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("connect fail");
		exit(1);	
	}

	// 보낼 데이터 입력
	printf("입력 : ");
	if (fgets(buf, sizeof(buf), stdin) == NULL)
		exit(0);
	nbyte = strlen(buf);

	// 에코 서버로 메시지 송신
	if (write(s, buf, nbyte) < 0) {
		perror("write fail");
		exit(0);
	}
	
	// 수신한 에코 데이터 출력
	if ((nbyte = read(s, buf, MAXLINE)) < 0) {
		perror("read fail");
		exit(0);
	}
	buf[nbyte] = 0;
	printf("수신 : %s", buf);
	close(s);

	return 0;
}
