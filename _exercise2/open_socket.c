#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <netdb.h>

int main() {
	int fd1, fd2, sd1, sd2;

	fd1 = open("/etc/passwd", O_RDONLY, 0);
	printf("/etc/passwd's file descriptor = %d\n", fd1);

	sd1 = socket(PF_INET, SOCK_STREAM, 0);
	printf("stream socket descriptor = %d\n", sd1);

	sd2 = socket(PF_INET, SOCK_DGRAM, 0);
	printf("datagram socket descriptor = %d\n", sd2);

	fd2 = open("/etc/hosts", O_RDONLY, 0);
	printf("/etc/hosts's file descriptor = %d\n", fd2);

	close(fd2);
	close(sd2);
	close(sd1);
	close(fd1);

	return 0;
}
