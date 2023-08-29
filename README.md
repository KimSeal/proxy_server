# proxy_server

**Proxy_server_1 : Cache의 역할을 대신할 URL HIT/MISS 구현**
(2022.03.07 - 2022.04.13)

ver 1.1 : URL 입력 및 SHA-1 module 기반 Hashing 진행 및 저장.

ver 1.2 : HIT/MISS 시스템 구현 (시간, 날짜, 프로그램 실행시간, Request 횟수 표기 추가)

ver 1.3 : 사용자 요청 처리 프로세스 단위 처리. 4가지 명령어 생성 (connect / quit / bye / URL)

---

**Proxy_server_2 : Firefox와 연동되는 Proxy Server 개발**
(2022.04.13 - 2022.05.25)

ver 2.1 : Main/Sub Server Process, Client Process 구현

ver 2.2 : Web browser(Firefox)와의 연동

ver 2.3 : HTTP response에 대해 signal을 이용한 대처 추가

ver 2.4 : HIT된 URL에 대해 저장된 파일을 불러오는 기능 구현 (속도 향상)

---

**Proxy_server_3 : Logfile에 대한 critical section 제어**
(2022.05.25 - 2022.06.08)

ver 3.1 : Process 단위 log file에 대한 critical section 제어

ver 3.2 : Thread 단위 log file에 대한 critical section으로 수정
