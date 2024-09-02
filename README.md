# 게임서버 포트폴리오

## [코드 문서 링크](https://culrry.github.io/GameServerProject/)



## [포트폴리오 문서](https://drive.google.com/file/d/1CC8ziIt9qDEkPtzOKP_ZT1hy8fq5TvOv/view?usp=sharing)



## [시연영상](https://youtu.be/ZsGWLx0XW2A?si=rh9SRTyQAmXBev5w)



## 프로젝트 소개

### 0. [ServerLibray](https://culrry.github.io/GameServerProject/namespacenetlib.html)

#### 프로젝트 목표
- 네트워크 IO 작업을 라이브러리화 하여 서버 개발 생산성 증대
- 락 없는 고성능의 서버 라이브러리 개발

#### 사용 기술

- C++, IOCP

  


### 1. [LoginServer](https://culrry.github.io/GameServerProject/namespacelogin.html)
#### 프로젝트 목표
- 가상의 퍼블리셔 플랫폼(REST API)을 구현하여 실제 게임서비스의 유저 인증 시나리오 재현
- 토큰 기반의 인증 절차로 유저의 신뢰성 확보
#### 사용 기술
- C++, C#
- Redis
- ASP.Net
- MySQL

#### 사용 라이브러리 & 오픈소스
- Redis-cpp
- cpp-httplib
- MySQL X DevAPI (MySQL Connector C++)
- JSON for Modern C++

#### 제작 기능
- JWT를 이용한 토큰기반 인증
- Redis를 사용한 로그인서버 – 게임서버간의 인증



### 2. [ChatServer](https://culrry.github.io/GameServerProject/namespacechat.html)

#### 프로젝트 목표

- 채팅서버를 게임 로직서버와 분리하여, 서버의 부하 분산 및 안정성 확보
- DB읽기 작업을 비동기 처리하여, DB작업이 콘텐츠 로직에 영향을 주지않도록 설계

#### 사용 기술

- C++, C#
- Redis
- MySQL

#### 사용 라이브러리 & 오픈소스

- Redis-cpp
- MySQL X DevAPI (MySQL Connector C++)
- JSON for Modern C++

#### 제작기능

- 필드간 채팅
- 귓속말 기능
- DB 읽기 작업의 비동기처리



### 3. [GameServer](https://culrry.github.io/GameServerProject/namespacegame.html)

#### 프로젝트 목표

- 상호작용이 없는 데이터 단위(ex. 필드)단위로 쓰레드를 분산하여, 부하 분산 및 동기화가 필요 없는 콘텐츠 로직으로 코드의 생산성 증대와 복잡성 감소
- 콘텐츠 로직을 라이브러리화 하여, 섹터 연산, 플레이어 이동, 길찾기 등을 자동화
- 모든 로직을 서버에서 시뮬레이션 하여 신뢰성있는 서버 제작

#### 사용 기술

- C++
- Redis
- MySQL

#### 사용 라이브러리 & 오픈소스

- Redis-cpp
- MySQL X DevAPI (MySQL Connector C++)
- JSON for Modern C++
- Eigen (선형대수 라이브러리)

#### 제작기능

- 섹터 시스템
- 게임 콘텐츠 로직을 라이브러리화
- JPS를 이용한 길찾기의 비동기처리
- 게임 전투 로직 구현
- DB 쓰기 작업의 비동기처리



### 4. [GameClient](https://github.com/CULRRY/GameServerProject/tree/master/04_GameClient)

#### 프로젝트 목표

- 로그인, 채팅, 게임을 통합적으로 사용하는 유니티 클라이언트 및 네트워크 모듈 제작

#### 사용 기술
- C#
- Unity

#### 제작기능
- Unity 클라이언트에서 사용할 네트워크 모듈 제작

- 로그인, 채팅. 게임을 통합사용하는 클라이언트 구현

  

### 5. [PacketGenerator](https://github.com/CULRRY/GameServerProject/tree/master/05_PacketGenerator)

#### 프로젝트 목표

- 단순 반복작업인 패킷 직렬화/역직렬화와 핸들링코드를 자동화하여 개발 생산성 증가

#### 사용기술

- Python3

#### 사용 라이브러리

- jinja2

#### 제작기능

- IDL문법을 정의하여, 프로토콜을 정의하면 패킷 처리코드를 자동으로 생성해주는 기능

