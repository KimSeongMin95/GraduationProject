# [프로젝트 개요]
- 홍익대학교 세종캠퍼스 게임소프트웨어 전공 4학년 김성민의 졸업 프로젝트 저장소입니다. <br>
- 언리얼 엔진 4와 IOCP 모델을 사용하여 8개월 동안 슈팅-RTS 장르의 3D PC 온라인게임을 졸업작품으로 개발했습니다. <br>
- 프로그램: Unreal Engine (4.23), Microsoft Visual Studio IDE (2017-19) <br>
- 지도: 강신진 교수님 <br>
- 기간: 2019-09-18 ~ 2020-05-25 <br>
- E-mail: ksm950310@naver.com <br>
- 제목: Spaceship Pioneers <br>
- 플랫폼: PC <br>
- 장르: 슈팅, RTS <br>
- 시점: 3D 쿼터뷰 <br>
- 스토리 <br>
우주 개척시대. 인류는 다른 행성으로 활동 영역을 넓혀 가고 있습니다. 어느 날, 개척 중인 행성에서 적 몬스터에게 공격을 받고 지원 요청 신호를 보냅니다. 신호를 받은 다른 행성의 개척자 소수가 지원하러 갑니다. 플레이어는 개척자를 조종하여 적 몬스터를 처치하고 최후의 기지를 방어하는 동시에 빼앗긴 구역들을 탈환하며 영역을 확장해야 합니다. 마지막으로 보스를 처치하거나 모든 구역을 점령하면 승리합니다. <br>
- 핵심목표 <br>
모든 건물이 파괴되지 않도록 방어하며 적의 보스를 처치하거나 모든 구역을 점령해야 합니다. <br>
<br>

# [유튜브 영상]
- 전시 영상: https://www.youtube.com/watch?v=r2KwCwumyJs&feature=youtu.be <br>
- 심사 영상: https://www.youtube.com/watch?v=WXYt6SJVHv8 <br>
<br>

# [실행 파일]
- 실행 파일 다운로드: https://drive.google.com/file/d/1ZmONn6bQx_pGNw7KuLs-IZtj2V5AYcv6/view?usp=sharing <br>
<br>

# [튜토리얼 플레이] <br>
1. 실행 파일.zip 압축 해제 <br>
2. Game.exe 실행 <br>
3. 튜토리얼 클릭 <br>
<br>

# [온라인 싱글 플레이] <br>
1. MainServer(Release).exe 실행 <br>
2. 나타난 DOS창에서 127.0.0.1 입력 후 8000 입력 <br>
3. Game.exe 실행 <br>
4. 온라인 클릭 <br>
5. ID 입력 <br>
6. 접속 <br>
7. 게임방 생성 <br>
8. 스테이지 설정 <br>
9. 시작 <br>
10. 네트워크 액세스 허용 <br>
- MainServer 오류 발생 시: Visual Studio 2019용 Microsoft Visual C++ 재배포 가능 패키지를 다운 받아 설치 <br>
다운로드 주소: https://support.microsoft.com/ko-kr/help/2977003/the-latest-supported-visual-c-downloads <br>
설치 파일: vc_redist.x64.exe 또는 vc_redist.x86.exe <br>
<br>

# [온라인 멀티 플레이] <br>
- 개발자 MainServer IP & Port: 58.233.177.231 8000 (제 컴퓨터의 IP가 변경되면 수정하여 다시 표기하겠습니다.) <br>
1. 모두 Game.exe 실행 <br>
2. 온라인 클릭 <br>
3. 각자 ID 입력 <br>
4. 127.0.0.1 대신 개발자 MainServer IP를 입력. <br>
5. 접속 <br>
6. 방장(공유기로 인해 내부(사설)/외부(공인) IP로 나눠진 경우, 그 사람은 방장 불가)이 게임방 생성 <br>
7. 방장이 방을 만들면 나머지는 새로고침하고 생성된 게임방에 참가 <br>
8. 방장이 스테이지 설정 <br>
9. 시작 <br>
10. 네트워크 액세스 허용 <br>
- IP 확인 사이트: https://www.findip.kr/ <br>
<br>

# [온라인 스테이지 종류] <br>
- 온라인 스테이지 1: 보스전 <br>
- 온라인 스테이지 2: 점령전 <br>
<br>

# [조작법]
- 메뉴 창 토글: F10키 / F10 UI 버튼 클릭 <br>
- 점수판 보기: Shift 누르고 있기 <br>
- 줌인 & 줌아웃: 마우스 휠 드래그 <br>
- 관전 대상 변경: 좌우 화살표 클릭 / AD키 <br>
- 관전 대상 조종: Enter키 / 캐릭터 조종 [Enter] UI 버튼 클릭 <br>
- 자유 시점 토글: Space키 / 자유 시점 [Space] UI 버튼 클릭 <br>
- 이동: WASD키 / 마우스 지형 우클릭 <br>
- 무기 발사: 마우스 좌클릭 <br>
- 무장 토글: 마우스 휠클릭 <br>
- 무기 변경: QE키 (무기 레벨 제한 1-3-5-7-9-11) <br>
- 건설할 건물 선택: 우측 하단 건물 UI 버튼 클릭 <br>
- 건물 회전: 선택한 상태에서 QE키 <br>
- 건물 건설: 선택한 상태에서 마우스 좌클릭 <br>
- 건물 선택 취소: ESC키 <br>
- 무장 토글: 마우스 휠클릭 <br>
<br>
