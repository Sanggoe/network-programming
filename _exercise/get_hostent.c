#include <stdio.h>
#include <stdlib.h>
//#include <arpa/inet.h>
//#include <netinet/in.h>
#include <netdb.h>
#define SIZE 20

int main(int argc, char *argv[]) {
	struct hostent *hp;
	struct in_addr inaddr;
	int i;
	char buf[SIZE];

	if(argc != 2) {
		printf("Usage : %s hostname\n", argv[0]);
		exit(1);
	}

	hp = gethostbyname(argv[1]); // domain 주소를 매개변수로 주어 hostent 구조체 포인터 반환
	if(hp == NULL) {
		printf("gethostbyname fail\n");
		exit(1);
	}

	printf("호스트 이름		: %s\n", hp->h_name);
	printf("호스트 주소타입 번호	: %d\n", hp->h_addrtype);
	printf("호스트 주소의 길이	: %d\n", hp->h_length);
	printf("IP 주소(binary address) : 0x%X \n", hp->h_addr_list[0]);
	inet_ntop(AF_INET, hp->h_addr_list[0], buf, sizeof(buf));
	printf("IP 주소(dotted decimal) : %s\n", buf);
	for(i=0; hp->h_aliases[i]; i++) {
		printf("호스트 별명 : %s\n", hp->h_aliases[i]);	
	}

	return 0;
}
