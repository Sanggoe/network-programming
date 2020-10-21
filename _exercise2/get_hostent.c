#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>

#define SIZE 20

int main(int argc, char *argv[]) {
	struct hostent *hp;
	struct in_addr in; // 여러개의 주소를 가지고 있을 때 임시변수
	int i;
	char buf[SIZE];

	if (argc != 2) {
		printf("Usage: %s hostname\n", argv[0]);
		exit(1);
	}
	
	// domain으로 host 정보 받아오기
	hp = gethostbyname(argv[1]);
	if (hp == NULL) {
		printf("gethostbyname fail\n");
		exit(0);
	}

	memcpy(&in, hp->h_addr_list[0], sizeof(in));
	inet_ntop(AF_INET, &in, buf, SIZE);

	printf("호스트 이름		: %s\n", hp->h_name);
	printf("호스트 주소타입 번호	: %d\n", hp->h_addrtype);
	printf("호스트 주소의 길이	: %d\n", hp->h_length);
	if (hp->h_aliases[0])	
		printf("호스트 별명		: %s\n", hp->h_aliases[0]);
	printf("IP 대표 주소(binary)	: 0x%X\n", in);
	printf("IP 대표 주소(dotted)	: %s\n", buf); 


	return 0;
}
