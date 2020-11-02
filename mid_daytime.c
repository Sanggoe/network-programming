//---------------------------------------------
// 파일명 : mid_daytime.c
// 기  능 : daytime 서비스를 요청하는 UDP(비연결형) 클라이언트
// 컴파일 : cc -o mid_daytime mid_daytime.c
// 사용법 : mid_daytime css.skuniv.ac.kr 9999
//---------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>

#define MAXLINE 127

int main(int argc, char *argv[]) {
	struct sockaddr_in servaddr;
	int s, nbyte, addrlen = sizeof(servaddr);
	struct in_addr in;
	struct hostent *hp;
	char buf[ MAXLINE + 1];

	if(argc != 3) {
		printf("Usage: %s domain_name port_num\n", argv[0]);
		exit(0);
	}

	// 소켓 생성
	if((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket fail");
		exit(1);
	}

	hp = gethostbyname(argv[1]);
	memcpy(&in, hp->h_addr_list[0], sizeof(in));

	// 서버 소켓주소 구조체 servaddr 초기화
	bzero((char*)&servaddr, sizeof(servaddr));
	// servaddr 주소 셋팅
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = in.s_addr;
	servaddr.sin_port = htons(atoi(argv[2]));

	// UDP 에코 서버로 메시지 송신(?)
	if (sendto(s, buf, strlen(buf), 0,
		(struct sockaddr *)&servaddr, addrlen) < 0) {
		perror("sendto fail");
		exit(0);
	}
	
	// 서버가 보내오는 daytime 데이터의 수신 및 화면 출력
	if((nbyte = recvfrom(s, buf, MAXLINE, 0, (struct sockaddr *)&servaddr, &addrlen)) < 0) {
		perror("recvfrom fail");
		exit(0);
	}
	buf[nbyte] = 0;
	printf("%s", buf);
	close(s);
	return 0;
}
