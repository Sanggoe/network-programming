#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>

#define MAXLINE 511

int main(int argc, char *argv[]) {
	struct sockaddr_in servaddr;
	int s, nbyte, addrlen = sizeof(servaddr);
	char buf[MAXLINE + 1];

	if (argc != 3) {
		printf("Usage : %s ip_address port_number\n", argv[0]);
		exit(0);
	}

	// 소켓 생성
	if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket fail");
		exit(1);
	}
	
	// 서버 소켓 주소 초기화
	bzero(&servaddr, sizeof(servaddr));

	// servaddr 셋팅
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	servaddr.sin_port = htons(atoi(argv[2]));
	
	// 보낼 데이터 입력
	printf("입력 : ");
	if (fgets(buf, sizeof(buf), stdin) < 0)
		exit(0);
	nbyte = strlen(buf);

	// sendto()로 송신
	if (sendto(s, buf, nbyte, 0, (struct sockaddr *)&servaddr, addrlen) < 0) {
		perror("sendto fail");
		exit(1);
	}
	
	// recvfrom()으로 수신 및 출력
	if ((nbyte = recvfrom(s, buf, MAXLINE, 0, (struct sockaddr *)&servaddr, &addrlen)) < 0) {
		perror("recvfrom fail");
		exit(1);
	}
	buf[nbyte] = 0;
	printf("수신 : %s", buf);

	// 소켓 close
	close(s);
	return 0;
}
