# JAsioServer
 - server for study
 
# 개요
 - boost asio 를 이용한 기본적인 채팅 서버
 - 패킷을 받아 버퍼에 저장하고, tick 마다 logic thread 에서 버퍼에 저장된 패킷을 읽어 실행한다.
 
# JClientBot
 - 테스트를 위한 클라이언트 대용
 - 기본적으로 입력된 숫자만큼 서버에 접속을 시도하며, 채팅을 보낼 수 있다.

# replay
- tickCount 마다 패킷을 기록하고, 기록한 패킷을 같은 tickCount에 재생해준다.
- 아직은 채팅기능밖에 없어서 크게 문제없이 돌아가는 중.
- 기능 추가 시 외부요인(DB) 나 클래스 상태에 따라 결과가 달라질 수 있기 때문에 잘 작동할지 의문이다.
- 일단 확실치 않기 때문에 기존의 코드에 영향을 주지 않는 범위에서 on/off할 수 있도록 함

# server param
- 없음 : 일반 서버 실행
- -ReplayServer : packet_result.txt 를 읽어 재생하는 서버를 실행시킨다.
- -ReplaySave : packet_result.txt 파일에 패킷을 기록한다
