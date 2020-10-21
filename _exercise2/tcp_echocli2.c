#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>

#define MAXLINE 127

int main(int argc, char *argv[]) {
	int s, nbyte;
	struct sockaddr_in servaddr;
	struct in_addr in;
	struct hostent *hp;
	char buf[MAXLINE + 1];

	if (argc != 3) {
		printf("Usage : %s domain_name port_num\n", argv[0]);
		exit(0);
	}

	// 소켓 생성
	if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket fail");
		exit(0);
	}

	// 도메인을 바이너리로 변환
	hp = gethostbyname(argv[1]);
	memcpy(&in, hp->h_addr_list[0], sizeof(in));
	
	// 서버 소켓주소 초기화
	bzero(&servaddr, sizeof(servaddr));

	// servaddr 셋팅
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = in.s_addr;
	servaddr.sin_port = htons(atoi(argv[2]));

	// connect() 연결
	if (connect(s, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("connect fail");
		exit(1);
	}

	// 보낼 데이터 입력
	printf("입력 : ");
	if (fgets(buf, sizeof(buf), stdin) == NULL) {
		exit(0);
	}
	nbyte = strlen(buf);

	// 데이터 송신 write()
	if (write(s, buf, nbyte) < 0) {
		perror("write fail");
		exit(0);
	}

	// 데이터 수신 read() 및 출력
	if ((nbyte = read(s, buf, MAXLINE)) < 0) {
		perror("read fail");
		exit(0);
	}
	buf[nbyte] = 0;
	printf("수신 : %s", buf);
	
	// 소켓 close
	close(s);
	
	return 0;
}
