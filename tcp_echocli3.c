//---------------------------------------------
// 파일명 : tcp_echocli.c
// 기  능 : echo 서비스를 요청하는 TCP(연결형) 클라이언트
// 컴파일 : cc -o tcp_echocli tcp_echocli.c
// 사용법 : tcpi_echocli 203.252.65.3
//---------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#define MAXLINE 127

int main(int argc, char *argv[]) {
	int s, nbyte;
	struct sockaddr_in servaddr;
	char buf[ MAXLINE + 1];

	struct hostent *hp;
	struct in_addr in;
	int i;
	char buf_ip[20];

	
	if(argc != 3) {
		printf("Usage: %s ip_address portnumber\n", argv[0]);
		exit(0);
	}


	hp = gethostbyname(argv[1]);

	if(hp==NULL) {
		printf("gethostbyname fail\n");
		exit(0);
	}

	for( i=0; hp->h_addr_list[i]; i++) {
		memcpy(&in.s_addr, hp->h_addr_list[i], sizeof(in.s_addr));
		inet_ntop(AF_INET, &in, buf_ip, sizeof(buf_ip)); // &in이나 &in.s_addr이나 상관없다.
		printf("IP 주소(%d 번째) : %s ", i+1, buf_ip); // 구조체 멤버가 하나 밖에 없기 때문
	}


	// 소켓 생성
	if((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket fail");
		exit(1);
	}
	// 에코 서버의 소켓주소 구조체 작성
	bzero((char*)&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
	servaddr.sin_port = htons(atoi(argv[2]));

	// 연결 요청
	if (connect(s, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("connect fail");
		exit(1);
	}

	printf("입력 : ");
	if (fgets(buf, sizeof(buf), stdin) == NULL)
		exit(0);

	nbyte = strlen(buf);
	// 에코 서버로 메시지 송신
	if (write(s, buf, nbyte) < 0) {
		printf("write error\n");
		exit(0);
	}

	// 수신된 에코 데이터 화면출력
	printf("수신 : ");
	if((nbyte = read(s, buf, MAXLINE)) < 0) {
		perror("read fail");
		exit(0);
	}
	buf[nbyte] = 0;
	printf("%s", buf);
	close(s);
	return 0;
}
