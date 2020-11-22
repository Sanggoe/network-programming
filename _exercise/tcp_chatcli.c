// tcp chat clien

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>

#define MAXLINE 1000
#define NAME_LEN 20

char *EXIT_STRING = "exit";
int tcp_connect(int af, char* servip, unsigned short port);
void errquit(char *mesg) {
	perror(mesg);
	exit(1);
}

int main(int argc, char* argv[]) {
	char bufall[MAXLINE+NAME_LEN],
	     *bufmsg;
	int maxfdp1, 
	    s, 
	    namelen;
	fd_set read_fds;
	
	if (argc != 4) {
		printf("사용법 : %s server_ip port name\n", argv[0]);
		exit(0);
	}

	sprintf(bufall, "[%s] : ", argv[3]);
	namelen = strlen(bufall);
	bufmsg = bufall + namelen; // 메시지 시작부분 지정(초기화)

	s = tcp_connect(AF_INET, argv[1], atoi(argv[2]));
	if (s == -1) {
		errquit("tcp_connect fail");
	}
	
	puts("서버에 접속되었습니다.");
	maxfdp1 = s+1;
	FD_ZERO(&read_fds);

	while(1) {
		FD_SET(0, &read_fds);
		FD_SET(s, &read_fds); // 초기화
		// 여기까지 select 준비 완료
		
		if (select(maxfdp1, &read_fds, NULL, NULL, NULL) < 0) {
			errquit("select fail");
		}
			
		// s의 변화면 서버로부터 수신한 메시지 출력
		if (FD_ISSET(s, &read_fds)) {
			int nbyte;
			if ((nbyte = recv(s, bufmsg, MAXLINE, 0)) > 0) {
				bufmsg[nbyte] = 0;
				printf("%s\n", bufmsg);
			}
		}


		// 0의 변화면 키보드로부터 입력 받은 내용 송신
		if(FD_ISSET(0, &read_fds)) {
	        	if(fgets(bufmsg, MAXLINE, stdin)) {
				if(send(s, bufall, namelen + strlen(bufmsg), 0) < 0) {
					errquit("send fail");
					//puts("Error : Write error on socket.");
				}
			
				if(strstr(bufmsg, EXIT_STRING) != NULL) {
					puts("Good bye.");
					close(s);
					exit(0);
				}
			}
		}
	} // end of while
	return 0;
}

int tcp_connect(int af, char* servip, unsigned short port) {
	struct sockaddr_in servaddr;
	int s;

	// 소켓 생성
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		return -1;
	}
	
	// 서버주소 구조체 초기화
	bzero((char*)&servaddr, sizeof(servaddr));
	servaddr.sin_family = af;
	inet_pton(AF_INET, servip, &servaddr.sin_addr);
	//servaddr.sin_addr.s_addr = inet_addr(servip);
	servaddr.sin_port = htons(port);

	// connect 콜
	if (connect(s, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
		return -1;
	}
	return s;
}
