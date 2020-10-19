#include <stdio.h>
#include <stdlib.h>
//#include <netinet/in.h>
#include <netdb.h>

int main(int argc, char *argv[]) {
	struct hostent *myhost;
	struct in_addr inaddr;

	if (argc != 2) {
		printf("usage : %d ip_address \n", argv[0]);
	}

	inet_pton(AF_INET, argv[1], &inaddr);
	myhost = gethostbyaddr(&inaddr, sizeof(inaddr), AF_INET);

	if (myhost == NULL) {
		printf("Error at gethostbyaddr() \n");
		exit(0);
	}
	printf("Host name : %s\n", myhost->h_name);

	return 0;
}
