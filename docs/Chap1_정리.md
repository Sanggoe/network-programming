# 네트워크 프로그래밍

<br/>

### 이 수업에서 우리는

* 클라이언트-서버 프로그래밍 방식을 사용할 것임
* UNIX 소켓 프로그래밍을 할 것임
* TCP/IP 상에서 진행할 것임으로, 개념을 복습하고 오기

<br/>

### Unix Commands

* pwd : print working directory
* ls : print list
  * -l : print by long form
  * -t : print order by edited time
  * -a : print all files and lower directories
  * -s : print size of block
* cd : change directory
* mkdir : make directory
* rmdir : remove directory
* rm : remove
  * -f : ignore permission
  * -i : print responed message every time
* cat : concatenation
  * -u : not do buffering
  * \> : make file
* cp : copy
* mv : move
* pr : print
* lp : line print
* prt1 filename : print
* chmod [Option] filename : change mode permission

<br/>

<br/>

* passwd : 계정 비밀번호 변경
* man : manual의 약자
* script [filename] : commend 창 내역 저장

<br/>

### vi 에디터

* vi : **VI**sual display editor
* [문서 참조 ](./viGuide.pdf)

<br/>

<br/>

# Chapter 01

<br/>

## 1.1 컴퓨터 통신 프로토콜

<br/>

### 1. 통신 프로토콜의 특징

* 효율성
  * 인터넷은 Best effort service. 처음에는 효율적으로 전달하는 것을 중심으로 했지만 광케이블이 생기면서 채널이 생기고, 이제는 퀄리티를 보장하는 방식으로 통신 프로토콜이 변화하게 된다. QoS. Quality of Service
* 안정성
  * 을 강조하여..

<br/>

### 2. OSI 7계층 구조

* 복습해옵시다~

<br/>

### 3. TCP/IP 개요

* RARP, ARP
* ICMP : IP 가 패킷을 제대로 전송할 수 있도록 해주는 프로



* BGP
* DHCP 
* 메시지 사이즈가 작거나 delay 에 민감한.. 

<br/>





상태 정의...



동작 정의...



네트워크 프로그래밍의 계층별 분류와 특징

CORBA : middle ware! OS와 어플리케이션 사이에 존재하는. 통신에 특화된.. 프로그램.

<br/>

<br/>

## 1.2 네트워크 프로그램 구현 모델

### 클라이언트 - 서버 모델

<br/>

### 2-tier 클라이언트 - 서버 모델

* 단점!
  * 클라이언트의 증가로 인한 서버 병목현상 발생
* fat 클라이언트!
  * 위 단점을 해결하기 위한 방법.
  * 서버의 기능 일부를 클라이언트에 구현한 것
    * 예를 들어 입력값이 6자리가 맞나? 를 클라이언트에서 미리 검사한 후 서버에 전송하는 것
    * 폰에 있는 어플들이 fat클라이언트라고 볼 수 있는것. 서버 접속을 위한...
    * 대신 업데이트 해줘야함

<br/>

### 3-tier 클라이언트 서버 모델

* 2-tier 모델 문제점 개선한 구조!

<br/>

### n-tier 클라이언트 서버 모델

* 3-tier 모델을 확장한 구조!

<br/>

### P2P 모델

* 서버, 클라이언트 역할이 미리 정해지지 않음
* 동작이 단순하지만, 효율적이진 못하다~

#### 하이브리드형 P2P

* 순수 P2P 모델 개선하여 인덱스 서버가 존재

<br/>

### 분산 객체 모델

#### 미들웨어!!

* CORBA, Java RMI, .NET, SOAP등이 미들웨어 환경을 제공 

<br/>

### 서버 구현 기술

### 연결형과 비연결형 서버

#### 연결형

* 안정적
* 서버부담 클 수 있음

#### 비연결형

* 효율적
* 서버 부담 적음

<br/>

### Stateful과 Stateless 서버

* Stateful 서버
  * 클라이언트 정보를 전부 다 유지관리하면서 하는 방식
  * 서버 부담.. 하나하나 어찌 관리하누. 보통은 이거보다 stateless 많이 씀

<br/>

### Iterative와 Concurrent 서버

* Iterative 서버
  * 클라이언트 요청을 순서대로 처리
  * 요청이 짧은 시간에 처리 가능한 경우 적합
  * 구현이 간단!
* Concurrent 서버
  * 클라이언트 요청을 동시에 처리
  * 다중처리 기능 필요
    * 여러 작업을 동시에 처리하는 기능

<br/>

## 1.3 네트워크 서비스의 성능

### 성능 척도!

* 처리용량(capacity)
* 안정성(stability)
* 처리율(throughput)
* 지연(delay)
* 효율(efficiency)

각각 세부 항목들 다 읽어보자~ pdf 파일~

<br/>

### 최적의 패킷 크기

시스템 자원...

<br/>

### 플랫폼의 선택

에붸붸베ㅔ...



<br/>

## 1.4 유닉스 프로그래밍 환경

### 네트워크 환경

control message를 우선순위로 한다! 따라서 이게 계속오면 서버가 다운되는 것.

icmp를 통해서... 그냥 한번 쭉 읽어봐~

<br/>

### 컴파일 환경

<br/>

<br/>

# Chapter 2

 소켓 프로그래밍

소켓은 응용 프로그램에서 TCP/IP 서비스를 필요로 할 때 불러서 쓰는 통신 창구!

포트번호 : 응용프로그램을 찾아가는 창구! 통신 어플리케이션에 따라서 부여된다.

소켓번호 : 실행중인 프로세스를 찾아가는 정보!

같은 통신 어플리케이션이면, 포트번호는 같다.

Well-known 포트
