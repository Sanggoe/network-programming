# Chapter 3. 유닉스 프로세스

### 3.1 프로세스의 이해

* 이 장에서는 정해진 시나리오 없이 자유롭게 메시지 주고받고 싶은 것이 목적이다.

* **talk 프로그램** : server와 client가 1대1로 자유자재로 메시지를 보내는 것. 
  * 일반적으로 read, write 함수 등을 쓰면 block 상태가 된다.
  * 그럼 자유로운 전송이 어렵다.
  * 따라서 하나의 클라이언트 프로그램만으로는 부족할 것이다.
  * 그래서 fork() 라는 함수를 사용할 것.
* **chat 프로그램** (4장) : server를 통해서 client끼리 자유롭게 메시지를 보내는 것.

<br/>

#### 3.1.1 프로세스의 정의

* 프로세스
  * 실행 가능한 프로그램이 실행중인 상태
    * 메인 메모리에 로드되고 운영체제에 의해 CPU의 서비스를 받을 수 있는 상태
    * 프로그램 : 프로그래밍 언어로 작성된 소스코드로, 일종의 파일. HDD 등 보조 메모리에 저장된 상태
  * 프로세스는 프로세서(CPU)가 수행하는 단위 이다.
* 프로세스의 모드
  * 사용자 모드 : 사용자 권한으로 명령 실행
  * 커널 모드 : 커널의 권한으로 실행. 시스템에서 HDD를 읽기 위한 read() 함수 등
    * 사용자 모드에서 I/O 명령 수행시, 커널모드로 바뀌어 수행이 되고, 완료하면 다시 사용자로 바뀐다.

<br/>

#### 프로세스의 상태

```c
int main() {					// 사용자 모드에서 처음 실행 (CPU의 서비스를 받는 실행상태)
	char buf[512];				
	int n = read(0, buf, 512);	// 처리를 위해 커널모드로 전환 (입력을 기다릴동안 블록상태)
	n++;						// 리턴되면 사용자모드로 전환
	exit(0);					// 종료를 위해 커널모드로 전환 (좀비 상태를 거쳐 종료)
}
```

* 프로세스가 종료되었다는 의미는, 프로세스에 관한 정보가 메모리에서 모두 사라진 상태를 의미한다.
* **ps 명령으로 프로세스 상태, pid 등의 정보를 알 수 있다.**

<br/>

* **실행**(running) 상태
  * 프로세스가 CPU의 서비스를 받을 수 있는 상태
* **블록**(waiting) 상태
  * I/O 처리나 어떤 조건을 기다리는 등 프로세스에서 스스로 멈추는 상태
* **중단**(stopped) 상태
  * 프로세스 실행이 **인터럽트** 등 외부의 요청 등에 의해 잠시 멈추는 상태
* **좀비**(zombie) 상태
  * 프로세스 실행은 끝났으나, 부모 프로세스가 종료를 명령하기 전까지 대기하는 상태

<br/>

#### 3.1.2 프로세스의 메모리 배치

#### 메모리 영역

* 프로세스 실행을 위해 일정한 (메인)메모리를 배정받아 사용하는데, 이 영역을 **프로세스 이미지**라고 한다.

* 원칙적으로 특정 프로세스가 사용하는 메모리영역(이미지)에는 다른 프로세스가 접근할 수 없다.

<br/>

* 프로세스 이미지 내용을 알아보기 위한 간단한 C 프로그램 예제

```c
#include <stdio.h>
#include <stdlib.h>
extern char **environ;		// extern 변수
int init_global_var = 3;	// 초기화된 global 변수 (초기값 3)
int unint_global_var;		// 초기화되지 않은 global 변수

int main(int argc, char **argv) {
    int	auto_var;			// 자동 변수
    static int static_var;	// static 변수
    register int reg_var;	// register 변수
    char *auto_ptr;			// 자동 변수
    auto_ptr = malloc(10);	// 메모리를 10byte 할당
    return 0;
}
```

<br/>

* 프로세스의 메모리 영역과 저장되는 변수 종류 예

![image-20201102164545138](C:/workspace/c/network-programming/docs/image-20201102164545138.png)

<br/>

* segmentation fault : 내가 접근할 수 없는 영역을 접근하려고 할 때 일어나는 에러..!!

<br/>

#### 스택과 힙

* 스택 (Stack)
  * 현재 호출되어 실행중인 함수의 코드와 환경 정보를 저장
  * 함수 내부에서 자동으로 생성되고 임시로 사용되는 자동 변수(지역변수)도 스택 영역에 할당
* 힙 (Heap)
  * 스택의 경우에는, 함수가 종료되면 사용하던 메모리도 함께 사라진다.
  * 함수가 리턴되어도 사라지지 않도록 한 영역이 힙이다.
  * malloc() 함수, static 등을 사용해 프로그램이 종료될 때까지 존재하는 영역은 힙에 저장된다.

<br/>

#### 3.1.3 프로세스의 생성과 종료

<br/>

#### fork()

* 새로운 프로세스를 만들기 위해 사용
* fork()를 호출한 프로세스의 이미지를 복사하여 새로운 프로세스를 생성
* 부모/자식 프로세스
  * 부모 프로세스 : fork()를 호출한 프로세스
    * fork()의 리턴값 : 자식프로세스의 PID
  * 자식 프로세스 : fork()에 의해 새로 생성된 프로세스
    * fork()의 리턴값 : 0
* 프로세스의 공유
  * 부모와 자식 프로세스는 변수를 서로 공유하지 않음
  * 개설한 파일이나 소켓은 프로세스 이미지 외부에 존재하므로 공유

<br/>

#### fork() 사용 예

* fork() 를 이용해 부모와 자식프로세스가 각각 다른 일을 수행하는 것을 확인하는 예제 코드

```c
//--------------------------------------------------------
// 파일명 : fork_test.c
// 기  능 : fork() 시스템 콜 사용 예
// 컴파일 : cc -o fork_test fork_test.c
// 사용법 : fork_test
//---------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
int global_var = 0;	// 전역 변수 선언
 
int main(void) {
	pid_t pid;
	int local_var = 0;	// 지역 변수 선언

	if((pid = fork()) < 0) {
		printf("fork error\n");
		exit(0);
	} else if (pid == 0) { // 자식 프로세스
		global_var++;
		local_var++;
		printf("CHILD - my pid is %d and parent's pid is %d\n", getpid(), getppid());
	} else {	// 부모 프로세스 pid > 0
		sleep(2);		// 2초 쉰다
		global_var +=5;	// 변수는 공유하지 않는다고 했다!
		local_var +=5;
		printf("PARENT - my pid is %d and child's pid is %d\n", getpid(), pid);
	}

	printf("\t global var : %d\n", global_var);
	printf("\t local var : %d\n", local_var);

	return 0;
}
```

* 결과

```
./fork_test
CHILD - my pid is 6731 and parent's pid is 6730
         global var : 1
         local var : 1
PARENT - my pid is 6730 and child's pid is 6731
         global var : 5
         local var : 5
```

* 자식 프로세스는 부모 프로세스의 이미지를 그대로 복사해서 가져간다.
* 다만 부모와 자식 프로세스는 변수를 공유하지 않음을 알 수 있다.
* 또한, 프로세스 id를 각자 다르게 가지는 것을 확인할 수 있다.

<br/>

#### getpid(), getppid()

```c
pid_t getpid() 	// 내 프로세스의 pid
pid_t getppid() // 부모 프로세스의 pid
```

* 자신의 pid, 부모의 pid를 반환하는 함수

<br/>

#### 프로세스의 종료

* 종료 조건
  * main() 함수에서 return 되는 경우
  * exit() 함수 호출할 경우
  * 프로세스 종료 signal을 받는 경우
* 종료 값
  * main()의 return 또는 exit() 호출시 인자
  * 종룟....
  * ㅇㅇ... 
* 즉, 자식 프로세스는 부모 프로세스가 생성과 종료를 담당한다.

### 

<br/>

### 3.2 토크 프로그램

#### 3.2.1 토크서버 프로그램

* 서버
  * listen()
  * accept() & fork()
  * 부모 - 입력 및 send(), 자식 - recv() 및 출력
* 클라이언트
  * connect()
  * fork()
  * send(), recv()

<br/>

#### 토크 서버측 프로그램

* tcp_talkserv.c 서버측 코드 예제

```c
//---------------------------------------------
// 파일명 : tcp_talkserv.c
// 기  능 : 토크 클라이언트와 1:1 통신을 한다
// 컴파일 : cc -o tcp_talkserv tcp_talkserv.c
// 사용법 : tcp_talkserv 3008
//---------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>

char *EXIT_STRING = "exit";	// 종료문자
int recv_and_print(int sd);	// 상대로부터 메시지 수신 후 화면 출력
int input_and_send(int sd);	// 키보드 입력 받고 상대에게 메시지 전달

#define MAXLINE 511

int main(int argc, char *argv[]) {
	struct sockaddr_in cliaddr, servaddr;
	int listen_sock, accp_sock,	// 소켓 번호
		addrlen = sizeof(cliaddr);
	pid_t pid;

	if(argc != 2) {
		printf("Usage: %s port_number\n", argv[0]);
		exit(0);
	}

	// 소켓 생성
	if((listen_sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket fail");
		exit(1);
	}
	// 서버의 소켓주소 구조체 초기화
	bzero((char*)&servaddr, sizeof(servaddr));

	// servaddr 셋팅
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(atoi(argv[1]));

	// bind() 호출
	if (bind(listen_sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("bind fail");
	}	
	
	puts("서버가 클라이언트를 기다리고 있습니다.");
	// 소켓 번호를 수동적 소켓으로 listen()
	listen(listen_sock, 5);


	// 클라이언트 연결요청 수락
	if ((accp_sock = accept(listen_sock, (struct sockaddr *)&cliaddr, &addrlen)) < 0) {
		perror("accept fail");
		exit(0);
	}	
	
	puts("클라이언트가 연결되었습니다.");
	
	if ( (pid = fork()) > 0) { // 부모
		input_and_send(accp_sock);	// 키보드 입력받고 상대에게 메시지 전달
	} else if (pid == 0) { // 자식
		recv_and_print(accp_sock);
	}
	close(listen_sock);
	close(accp_sock);
	return 0;
}

// 키보드 입력받고 상대에게 메시지 전달
int input_and_send(int sd) {
	char buf[MAXLINE+1];
	int nbyte;
	while(fgets(buf, sizeof(buf), stdin) != NULL) {
		nbyte = strlen(buf);
		write(sd, buf, nbyte);
		// 종료 문자열 처리
		if (strstr(buf, EXIT_STRING) != NULL) {
			puts("Good bye.");
			close(sd);
			exit(0);
		}
	}
	return 0;
}

// 상대로부터 메시지 수신 후 화면 출력
int recv_and_print(int sd) {
	char buf[MAXLINE+1];
	int nbyte;
	while(1) {
		if ((nbyte = read(sd, buf, MAXLINE)) < 0) {
			perror("read fail");
			close(sd);
			exit(0);
		}
		buf[nbyte] = 0;

		// 종료문자열 수신시 종료
		if (strstr(buf, EXIT_STRING) != NULL) {
			puts("Good bye, server! Client out.");
			kill(getppid(), SIGINT);
                        exit(0);			
		}
		printf("%s", buf); // 화면 출력
	}
	return 0;
}
```

* 서버는 클라이언트로부터 connect call 이 오면, 그 시점부터 해당 클라이언트의 정보를 가지고 있는다.
* 따라서 연결 설정이 된 이후부터는 server나 client 어느 쪽에서든지 먼저 메시지를 보낼 수 있다.

<br/>

#### 코드 일부 수정

* 입력 값을 비교하여 **exit** 을 입력하면 프로그램이 종료되도록 하였는데, 약간의 문제가 있어 수정하였다.
* 해당 코드의 경우 **부모 프로세스에서는 입력**을, **자식 프로세스에서는 수신**을 담당한다.
* 이 때, **입력한** 문자열이 exit이면 프로그램을 종료한다. 즉, **부모 프로세스**를 종료하는 것이다.
* 마찬가지로 **수신한** 문자열이 exit이면 프로그램을 종료한다. 하지만 이 때는 **자식 프로세스**가 종료된다.
* 자식 프로세스를 종료한다고 해도 **부모 프로세스는 존재**하기 때문에 계속해서 프로그램이 수행된다.
* 따라서 수신한 문자열이 exit일 경우 **부모 프로세스의 PID**를 이용해 **kill 함수로 프로세스를 종료**해준다.

<br/>

#### 토크 클라이언트측 프로그램

* tcp talkcli.c 클라이언트측 코드 예제

```c
//---------------------------------------------
// 파일명 : tcp_talkcli.c
// 기  능 : 토크 서버와 1:1 통신을 한다
// 컴파일 : cc -o tcp_talkcli tcp_talkcli.c
// 사용법 : tcp_talkcli 127.0.0.1 3008
//---------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>

#define MAXLINE 511

char *EXIT_STRING = "exit";	// 종료문자
int input_and_send(int sd);	// 키보드 입력 받고 상대에게 메시지 전달
int recv_and_print(int sd);	// 상대로부터 메시지 수신 후 화면 출력

int main(int argc, char *argv[]) {
	pid_t pid;
	static int s;
	static struct sockaddr_in servaddr;

	// 명령문 입력 인자 처리
	if(argc != 3) {
		printf("Usage: %s server_ip port_number\n", argv[0]);
		exit(0);
	}

	// 소켓 생성
	if((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Client: Can't open stream socket. \n");
		exit(1);
	}
	// 서버의 소켓주소 구조체 초기화
	bzero((char*)&servaddr, sizeof(servaddr));

	// servaddr 셋팅
	servaddr.sin_family = AF_INET; // 주소 체계
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
    //servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	servaddr.sin_port = htons(atoi(argv[2]));

	// 서버에 연결 요청
	if (connect(s, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		printf("Client: Can't connect to server. \n");
		exit(0);
	}	

	if ( (pid = fork()) > 0) { // 부모 프로세스
		input_and_send(s);
	} else if (pid == 0) { // 자식 프로세스
		recv_and_print(s);
	}
	close(s);
	return 0;
}

// 키보드 입력받고 상대에게 메시지 전달
int input_and_send(int sd) {
	char buf[MAXLINE+1];
	int nbyte;
	while(fgets(buf, sizeof(buf), stdin) != NULL) {
		nbyte = strlen(buf);
		write(sd, buf, nbyte);

		// 종료 문자열 처리
		if (strstr(buf, EXIT_STRING) != NULL) {
			puts("Good bye.");
			close(sd);
			exit(0);
		}
	}
	return 0;
}

// 상대로부터 메시지 수신 후 화면 출력
int recv_and_print(int sd) {
	char buf[MAXLINE+1];
	int nbyte;
	while(1) {
		if ((nbyte = read(sd, buf, MAXLINE)) < 0) {
			perror("read fail");
			close(sd);
			exit(0);
		}
		buf[nbyte] = 0;

		// 종료문자열 수신시 종료
		if (strstr(buf, EXIT_STRING) != NULL) {
			puts("Good bye, client! Server out.");
			kill(getppid(), SIGINT);
			exit(0);
		}
		printf("%s", buf); // 화면 출력
	}
	return 0;
}
```

* 위 서버측 프로그램의 내용을 설명한 부분과 같다. 마찬가지로 코드를 좀 수정하였다.
* 이제 서버나 클라이언트 어느 쪽에서든지 exit을 하면 양쪽 모두 프로그램이 종료된다.

<br/>

* 부모 프로세스가 종료 되면, 자식 프로세스가 종료가 된다.
* 하지만 자식 프로세스만 종료된다고 부모가 종료되지 않는다.
* 따라서.. 자식은 끝났지만 부모 프로세스가 아직 안끝났으면 좀비 프로세스 상태가 되는 것이다.

<br/>

<br/>