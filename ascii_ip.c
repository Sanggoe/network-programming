//-------------------------------------------------------------
// 파일명: ascii_ip.c
// 기  능: ASCII (dotted decimal)로 포현된 주소를 4 바이트 IP 주소로 변환
// 컴파일: cc -o ascii_ip ascii_ip.c
// 사용법: ascii_ip 192.203.144.11
//-------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

int main(int argc, char *argv[]) {
	struct in_addr inaddr;		// 32비트 IP주소 구조체
	char buf[20];

	if(argc < 2) {
		printf("사용법 : %s IP 주소(dotted decimal) \n", argv[0]);
		exit(0);
	}
	printf("* 입력한 dotted decimal IP 주소: %s\n", argv[1]);

	inet_pton(AF_INET, argv[1], &inaddr.s_addr);
	printf(" inet_pton(%s) = 0x%X\n", argv[1], inaddr.s_addr);
	inet_ntop(AF_INET, &inaddr.s_addr, buf, sizeof(buf));
	printf(" inet_ntop(0x%X) = %s\n", inaddr.s_addr, buf);

	return 0;
}
