#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>

int main(int argc, char *argv[]) {
	struct hostent *myhost;
	struct in_addr in;

	if (argc != 2) {
		printf("Usage : %s ip_address \n", argv[0]);
		exit(0);
	}
	
	inet_pton(AF_INET, argv[1], &in);
	myhost = gethostbyaddr(&in, sizeof(in), AF_INET);

	if (myhost == NULL) {
		printf("Error at gethostbyaddr()\n");
		exit(0);
	}

	printf("호스트 이름: %s\n", myhost->h_name);

	return 0;
}	
