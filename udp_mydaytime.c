#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>

#define MAXLINE 127

int main(int argc, char *argv[]) {
	int s, nbyte;
	struct sockaddr_in servaddr;
	char buf[MAXLINE + 1];

	if (argc != 2) {
		printf("Usage: %s ip_address\n", argv[0]);
		exit(0);
	}

	// 소켓 생성
	

	return 0;
}
