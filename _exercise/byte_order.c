#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>

int main() {
	struct servent *servent;
	servent = getservbyname("echo", "udp"); // service_name, protocol
	if(servent == NULL) {
		printf("can't get service info.");
		exit(0);
	}

	printf("UDP 에코 포트번호 (네트워크 순서) : %d\n", servent->s_port);
	printf("UDP 에코 포트번호 (호스트 순서) : %d\n", ntohs(servent->s_port));

	return 0;
}
