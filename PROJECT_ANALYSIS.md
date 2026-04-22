# JAsioServer 프로젝트 분석

## 1. 개요

JAsioServer는 **Boost.Asio** 기반의 학습용 C++ 채팅 서버 프로젝트이다. 네트워크 I/O와 게임 로직을 분리한 멀티스레드 구조로 설계되었으며, 패킷을 버퍼에 적재한 뒤 tick 단위(12 TPS)로 일괄 처리하는 전형적인 게임 서버 패턴을 따른다. 또한 tick 단위 패킷 기록/재생(Replay) 시스템을 포함해 결정적 재현 테스트가 가능하다.

- **플랫폼**: Windows (Visual Studio 2022, Toolset v143)
- **대상 아키텍처**: Win32 / x64 / ARM / ARM64
- **빌드 시스템**: Visual Studio Solution (`JAsioServer.sln`)
- **주요 외부 의존성**: Boost 1.71.0 (Asio), GoogleTest, .NET Framework(클라이언트 봇)

## 2. 솔루션 구성

`JAsioServer.sln`은 다음과 같은 8개 프로젝트로 구성된다.

| 프로젝트 | 타입 | 역할 |
|---------|------|-----|
| `JChatServer` | Console App (C++) | 서버 실행 엔트리 포인트 |
| `JServerLib` | Static Lib (C++) | 서버 로직 / 세션·캐릭터 관리 |
| `JNetLib` | Static Lib (C++) | TCP 네트워크, 패킷, Replay I/O |
| `JSharedLib` | Static Lib (C++) | 공용 유틸(Vector3, Tick, ThreadPool) |
| `JLogger` | Static Lib (C++) | 비동기 파일 로거 |
| `JServerLib_Test` | GoogleTest 실행 파일 | 서버 로직 유닛 테스트 |
| `JSharedLib_Test` | GoogleTest 실행 파일 | 공용 유틸 유닛 테스트 |
| `JClientBot` | WPF App (C#) | GUI 기반 테스트 클라이언트 |

### 모듈 의존 관계

```
JChatServer ──► JServerLib ──► JNetLib ──► JSharedLib
     │              │              │
     ├──────────────┴──────────────┴──► JSharedLib
     └──► JLogger

JServerLib_Test ──► JServerLib, JNetLib, JSharedLib, GoogleTest
JSharedLib_Test ──► JSharedLib, GoogleTest
JClientBot      ──► .NET Framework (독립)
```

- `JSharedLib`이 최하위 공용 모듈
- `JNetLib`은 네트워크 추상화 계층
- `JServerLib`이 실제 게임 로직
- `JLogger`는 독립 모듈

## 3. 모듈별 상세

### 3.1 JChatServer (엔트리 포인트)
- 파일: `JChatServer/JChatServer.cpp`
- 동작 흐름
  1. `JLogger` 초기화 (`JChatServer.log`)
  2. Boost.Asio `io_context` 생성
  3. `JSocketServer`(기본 포트 9001) 생성
  4. 두 개의 스레드 기동
     - **I/O Thread**: `io_context.run()` (비동기 소켓 이벤트 루프)
     - **Logic Thread**: 12 TPS로 동작하는 게임 루프
  5. `end` 입력 시 정상 종료
- 실행 인자
  - `(없음)` : 일반 서버
  - `-ReplayServer` : `packet_result.txt`를 읽어 재생
  - `-ReplaySave` : 패킷을 `packet_result.txt`에 기록

### 3.2 JServerLib (서버 로직)
- 주요 클래스
  - `JServer` : Commander 컬렉션 및 tick 기반 업데이트 관리
    - `PreUpdateCommanders()`, `ProcessPacket()`, `UpdateCommanders()`, `UpdateView()`, `BroadCastToCommander()`
  - `JSocketServer` : `JServer`를 상속하여 Boost.Asio TCP 리스너 추가
  - `JCommander` : 접속한 플레이어 세션 래퍼. `ISession`을 보유하며 패킷 라우팅(LOGIN/LOGOUT/CHAT/MOVE)과 시야(view) 목록 관리
  - `JCharacter` / `JPlayerChar` : 위치·이동 로직(`m_positionCur`, `m_positionDest`)을 가진 게임 엔티티
  - `JCommanderCommand`, `JLoginComCommand` : Command 패턴 기반의 상태 처리
  - `JReplayServer` : 실 네트워크 접속 대신 기록된 패킷을 재생
- 파일: `JServer.*`, `JCommander.*`, `JCommander_Packet.cpp`, `JCharacter.*`, `JPlayerChar.*`, `JCommanderCommand.*`, `JReplayServer.*`

### 3.3 JNetLib (네트워크 계층)
- 주요 클래스
  - `ISession` : 세션 추상 인터페이스 (`PostSend`, `ProcessPacket`, `IsDisconnected`, `GetSessionID`, `PopLastSendPacket`)
  - `JSession` : Boost.Asio 기반 TCP 세션 구현
    - 수신 버퍼 `RECV_BUF_SIZE = 1024`, 처리 버퍼 `RECV_PROCESS_BUF_SIZE = 4096`
    - 분할 패킷 재조립, 송신 큐, `std::mutex` 기반 동기화
  - `JReplaySession` : 메모리에 적재된 패킷을 재생하는 모의 세션
  - `JReplayReader` : 세션 ID/tick 별 패킷 맵 로딩
  - `JReplaySaveWorker`, `JReplaySaveWorkerMgr` : 별도 스레드에서 조건 변수 + 큐로 패킷을 직렬화해 파일에 기록
- 패킷 정의 (`JPacket.h`, `JSCPacket.h`, `#pragma pack(push,1)`)
  - `PACKET_HEADER { command(4B), size(4B) }` + 페이로드
  - `PACKET_CS_LOGIN`, `PACKET_SC_LOGIN_ACK`
  - `PACKET_CS_LOGOUT`
  - `PACKET_CS_CHAT`, `PACKET_SC_CHAT`
  - `PACKET_CS_MOVE`, `PACKET_SC_MOVE`
  - `PACKET_SC_VIEW` (최대 5명의 시야 내 플레이어)

### 3.4 JSharedLib (공용 유틸)
- `Vector3` : 3D 벡터, 연산자 오버로드, `GetMagnitude`, `Normalize`, `CalDist`
- `JTickClass` : tick 카운터, 경과 시간, `Update()`
- `ThreadPool` : 템플릿 기반 `EnqueueJob`, `std::future` 지원
- `JSharedConst` / `JSharedFunc` : `.jpk` 확장자 등 상수 및 헬퍼

### 3.5 JLogger (로깅)
- `JLoggerClass` : 싱글턴 로거 (`Init`, `Log`, `Error`)
- `JLoggerThread` : 백그라운드에서 비동기로 파일 기록
- 매크로 `JLogger`가 파일/라인을 자동 부착

### 3.6 JClientBot (테스트 클라이언트)
- C# WPF 애플리케이션
- `Network.cs`에서 서버 C++ 패킷 구조를 `Marshal`로 직렬화/역직렬화
- 입력한 수만큼 서버 접속을 생성하여 채팅·이동 송신 테스트 가능

### 3.7 테스트 프로젝트
- `JServerLib_Test`
  - `test.cpp`, `test_replay.cpp`
  - 헬퍼: `JServerForUnitTest`, `JTestSession` (패킷 캡처용 목 세션)
  - 커맨더 채팅 처리, 시야 갱신, Replay 로직 검증
- `JSharedLib_Test`
  - `Vector3` 연산 및 수학 유틸 검증

## 4. 런타임 아키텍처

```
[Client TCP] ──► JSession (async recv)
                    │  ▲
                    ▼  │ async send
              수신 버퍼(mutex)
                    │
                    ▼   (Logic Thread, 12 TPS)
           JServer::ProcessPacket
                    │
                    ▼
           JCommander::ProcessPacket
          (LOGIN / LOGOUT / CHAT / MOVE)
                    │
                    ▼
      상태 갱신 → UpdateCommanders / UpdateView
                    │
                    ▼
              BroadCastToCommander
                    │
                    ▼
           JSession::PostSend ──► [Client]
```

### 스레드 모델
- **I/O 스레드**: `io_context.run()` 상에서 비동기 read/write 콜백 처리
- **로직 스레드**: `JTickClass` 기반 12 TPS 루프
  - `PreUpdateCommanders → ProcessPacket → UpdateCommanders → UpdateView`
- **로거 스레드**, **Replay Save 스레드** 등 별도 백그라운드 스레드
- 동기화: `std::mutex`, `std::lock_guard`, `std::atomic_flag`, `condition_variable`

## 5. Replay 시스템

- 저장(`-ReplaySave`)
  - 모든 수신/송신 패킷을 `{tickCount, sessionID, buffer}` 형태로 큐에 적재
  - `JReplaySaveWorker`가 별도 스레드에서 `packet_result.txt`로 직렬화
- 재생(`-ReplayServer`)
  - `JReplayReader`가 파일을 메모리에 로드
  - `JReplayServer` + `JReplaySession`이 원래 tick에 맞춰 패킷을 주입
- 한계
  - DB 등 외부 요인이나 클래스 상태에 의존하는 기능이 추가되면 재현성 보장이 어려움
  - 현재 채팅 위주 기능에서는 안정적으로 동작

## 6. 빌드 / 실행

### 사전 요구 사항
- Visual Studio 2019 이상 (Toolset v143)
- Boost 1.71.0 (환경 변수 `BOOST_INCLUDE_DIR`, `BOOST_LIB_DIR`)
- GoogleTest (유닛 테스트 실행 시)
- .NET Framework (JClientBot)

### 빌드
```
JAsioServer.sln 을 Visual Studio로 열어 Build Solution
```
- 산출물: `../../Bin/` (실행 파일 및 `.lib`)
- 명명 규칙: `JChatServer_{Configuration}.exe`, `JNetLib_{Configuration}.lib` 등

### 실행 예시
```
JChatServer_Release.exe                    # 일반 서버
JChatServer_Release.exe -ReplaySave        # 패킷 기록 모드
JChatServer_Release.exe -ReplayServer      # 패킷 재생 모드
JServerLib_Test_Release.exe                # 서버 유닛 테스트
JSharedLib_Test_Release.exe                # 공용 유틸 테스트
JClientBot.exe                             # WPF 테스트 클라이언트
```

## 7. 디렉터리 구조

```
JAsioServer/
├── JAsioServer.sln
├── README.md
├── .gitignore / .gitattributes
├── .vscode/
├── JChatServer/        # 서버 엔트리 포인트
├── JServerLib/         # 서버 로직
├── JNetLib/            # 네트워크 & Replay
├── JSharedLib/         # 공용 유틸
├── JLogger/            # 로거
├── JServerLib_Test/    # 서버 로직 테스트
├── JSharedLib_Test/    # 공용 유틸 테스트
└── JClientBot/         # C# WPF 테스트 클라이언트
```

## 8. 설계 특징 및 패턴

- **계층 분리**: 네트워크(JNetLib) / 로직(JServerLib) / 공용(JSharedLib) / 로깅(JLogger)의 책임이 명확히 구분됨
- **인터페이스 추상화**: `ISession`을 통해 실제 TCP 세션(`JSession`)과 Replay 세션(`JReplaySession`), 테스트용 목 세션(`JTestSession`)을 교체 가능
- **Tick 기반 결정적 처리**: 네트워크 이벤트와 로직 처리를 분리해 race condition을 최소화
- **Command 패턴**: `JCommanderCommand` 계열로 커맨더 상태 처리 확장 가능
- **비동기 로깅/기록**: 로거와 Replay Save 모두 별도 스레드 + 조건 변수 기반 큐 사용
- **유닛 테스트 인프라**: GoogleTest + 목 세션으로 서버 로직 격리 테스트

## 9. 향후 개선 아이디어

- **크로스 플랫폼 빌드**: 현재 VS 솔루션 전용 → CMake 전환으로 Linux/macOS 지원
- **설정 외부화**: 포트(9001), TPS(12) 등 상수를 설정 파일로 분리
- **구조화 로깅**: 레벨·카테고리 기반 필터링, JSON 포맷 지원
- **Replay 강화**: 외부 상태(DB, 난수 시드 등) 스냅샷 포함
- **CI 파이프라인**: GitHub Actions 등으로 테스트 자동화
- **네트워크 계층 확장**: UDP / TLS / WebSocket 지원 고려
