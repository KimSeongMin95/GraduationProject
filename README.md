# [프로젝트 개요]
- 홍익대학교 세종캠퍼스 게임소프트웨어 전공 4학년 김성민의 졸업 프로젝트 저장소입니다. <br>
- 언리얼 엔진 4와 IOCP API를 사용하여 8개월 동안 슈팅-RTS 장르의 3D PC 온라인게임을 졸업작품으로 개발했습니다. <br>
- 프로그램: Unreal Engine (4.23), Microsoft Visual Studio IDE (2017-19) <br>
- 지도: 강신진 교수님 <br>
- 기간: 2019-09-18 ~ 2020-05-25 <br>
- E-mail: ksm950310@naver.com <br>
- 타이틀: Spaceship Pioneers <br>
- 플랫폼: PC <br>
- 장르: 슈팅, RTS <br>
- 타겟 플레이어: 전 연령 <br>
- 시점: 3D 쿼터뷰 <br>
- 스토리 <br>
우주 개척시대. 인류는 다른 행성으로 활동 영역을 넓혀 가고 있습니다. 어느 날, 개척 중인 행성에서 적 몬스터에게 공격을 받고 지원 요청 신호를 보냅니다. 신호를 받은 다른 행성의 개척자 소수가 지원하러 갑니다. 플레이어는 개척자를 조종하여 적 몬스터를 처치하고 최후의 기지를 방어하는 동시에 빼앗긴 구역들을 탈환하며 영역을 확장해야 합니다. 마지막으로 보스를 처치하거나 모든 구역을 점령하면 승리합니다. <br>
- 핵심목표 <br>
모든 건물이 파괴되지 않도록 방어하며 적의 보스를 처치하거나 모든 구역을 점령해야 합니다. <br>
- [먼저, 이 게임은 영리목적 아닌 교육 및 학습을 위하여 개발된 졸업작품임을 밝힙니다.] <br>
- [First of all, I would like to announce that this game is a graduation work developed for education and learning, not for profit.] <br>
<br>

# [유튜브 영상]
- 포트폴리오 영상: https://www.youtube.com/watch?v=MhvNvdHbUnQ <br>
<br>

# [기술적 특징]
1. 클라이언트 <br>
- 인공지능 <br>
플레이어가 조종할 수 있는 아군 캐릭터인 개척자와 적인 몬스터의 인공지능을 [대기, 추적, 공격, 죽음]의 상태를 가진 간단한 FSM으로 구현하였습니다. <br>

- 애니메이션 <br>
개척자와 몬스터의 UAnimInstance 클래스를 부모로 하는 애니메이션 블루프린트를 정의하고 각 상태에 해당하는 애니메이션 시퀀스를 적절히 블렌딩하여 애니메이션이 재생되도록 구현하였습니다. <br>
개척자를 생성하는 우주선은 한 가지의 애니메이션만 사용하므로 C++ 코드로 UAnimSequence를 USkeleton에 설정하였고 애니메이션이 재생되도록 간단히 구현하였습니다. <br>

- 관전 <br>
플레이어는 개척자를 관전하는 상태로 게임을 시작합니다. <br>
AController의 SetViewTargetWithBlend 함수로 카메라를 부드럽게 전환하여 플레이어가 관전하는 대상을 변경할 수 있도록 구현하였습니다. <br>

- 조종 <br>
생성된 개척자와 몬스터는 각 AAIController에 빙의되어 조종되도록 하였습니다. <br>
플레이어가 관전 중이던 AI개척자를 조종하면 AAIController가 UnPossess를 한 뒤에 APlayerController가 Possess하도록 하였습니다. <br>

- 충돌 및 겹침 <br>
서로 통과하지 말아야 하는 오브젝트들은 CollisionResponseToChannel을 Block으로 설정하여 충돌하도록 하였습니다. <br>
개척자와 몬스터는 USphereComponent로 탐지 범위와 공격 범위를 가지면서 Overlap Event를 활성화하여 겹친 오브젝트를 파악할 수 있도록 하였습니다. <br>
따라서 탐지 범위 안에 들어온 대상을 추적하며 공격 범위 안에 들어온 대상을 공격하도록 구현하였습니다. <br>
모든 오브젝트의 CollisionObjectType을 다르게 설정하고 필요한 CollisionResponseToChannel만 설정하여 나머지는 Ignore 함으로써 효율적으로 감지하도록 구현하였습니다. <br>

- LineTrace <br>
개척자와 몬스터는 UWorld의 LineTraceMultiByObjectType 함수로 탐지 범위 안에 들어온 대상 중에 가까우면서도 경로에 장애물이 없는 대상을 우선으로 추적하도록 구현하였습니다. <br>
그리고 AController의 GetHitResultUnderCursor 함수로 마우스가 위치한 지형에 커서가 표시되도록 하였습니다. <br>

- 이동 <br>
월드에 NavMeshBoundsVolume을 배치하고 개척자와 몬스터에 UCharacterMovementComponent를 사용하여 UAIBlueprintHelperLibrary의 SimpleMoveToLocation 함수로 원하는 지점으로 이동하도록 구현하였습니다. <br>
개척자가 무기를 사용할 때 발사되는 탄환에 UProjectileMovementComponent를 사용하여 자연스럽게 발사되도록 구현하였습니다. <br>

- UI <br>
언리얼 엔진의 위젯 블루프린트를 사용하고 각 위젯의 이벤트를 C++ 코드의 함수에 바인딩하여 플레이어와 상호작용하도록 구현하였습니다. <br>

- 무기 <br>
개척자는 개인화기인 무기를 사용하여 적을 공격할 수 있으며, 6종의 다양한 무기가 존재합니다. <br>
종류: 권총, 소총, 산탄총, 저격 소총, 유탄 발사기, 미사일 발사기 <br>

- 공유자원 및 건물 <br>
공유자원으로 건물의 주재료인 무기물과 유기물 그리고 에너지가 존재합니다. <br>
에너지 자원이 0이 되면 에너지를 소모하는 건물들이 비활성화됩니다. <br>
플레이어가 9종의 다양한 건물들을 건설하여 전략적으로 플레이할 수 있습니다. <br> <br>
종류: 방호벽, 계단, 관문, 무기물 광산, 유기물 농장, 핵융합 발전기, 기본 포탑, 관통 포탑, 폭발 포탑 <br>
모든 건물이 파괴되면 플레이어들이 패배합니다. <br>

- 몬스터 <br>
특정 구역에서 근접 공격을 하는 9종의 다양한 몬스터가 생성되어 개척자들의 최후의 기지를 공격합니다. <br>
보스를 처치하면 플레이어들이 승리합니다. <br>

- 다중 스테이지 <br>
튜토리얼: 진행하면서 스토리와 시스템에 대한 설명을 보며 조작을 익힐 수 있습니다. <br>
온라인 스테이지 1: 보스전으로써 마지막 구역의 보스를 처치하면 승리합니다. <br>
온라인 스테이지 2: 점령전으로써 모든 구역을 점령하면 승리합니다. <br>
<br>

2. 서버 <br>
- IOCP API <br>
당시 언리얼 엔진의 데디케이티드 서버에 대한 지식이 없었고 직접 서버를 구현해보고 싶었기에 IOCP API를 사용하여 1개의 Accept 스레드와 다수의 IO 스레드로 TCP 서버를 구현하였습니다. <br>
C++ STL의 deque 컨테이너로 수신한 데이터를 관리하고 패킷 크기에 맞게 분할하여 데이터를 처리하도록 구현하였습니다. <br>
클라이언트의 정상/비정상 접속 종료에 대한 처리를 하였습니다. <br>

- 패킷 설계 <br>
패킷을 [크기, 헤더, 데이터, 끝] 순서를 가진 가변 길이로 설계하였고 클래스로 정의하였습니다. <br>
패킷 클래스에서 연산자 <<와 >>를 오버로딩하여 stringstream으로 간편하게 데이터를 읽고 쓰도록 구현하였습니다. <br>

- 부하 분산 <br>
서버의 부하를 분산하기 위해 메인 서버와 게임 서버로 분할하여 구현하였습니다. <br>
메인 서버는 데디케이티드 서버로써, 모든 플레이어 및 게임방의 정보를 관리하므로 따로 구동합니다. <br>
게임 서버는 리슨 서버로써, 게임 내의 정보를 동기화하므로 방장이 게임을 시작하면 호스트가 되고 언리얼 엔진 내에서 구동하도록 하였습니다. <br>
<br>

3. 온라인 <br>
- 로그인 <br>
온라인 Widget에서 ID, IP, Port를 입력하여 로그인 버튼을 클릭하면 메인 클라이언트를 구동하여 메인 서버에 접속을 시도합니다. <br>
서버 접속에 성공하면, 게임방 리스트 Widget으로 전환됩니다. <br>

- 게임방 리스트 <br>
모든 게임방 목록을 볼 수 있으며 플레이어는 게임방을 생성하여 방장이 될 수 있고 게임방에 참가하여 참가자가 될 수 있습니다. <br>

- 대기실 <br>
게임방의 정보와 대기실에 참가한 다른 플레이어의 ID를 확인할 수 있습니다. <br>
방장은 플레이할 온라인 스테이지를 1 또는 2로 선택할 수 있습니다. <br>
방장이 게임을 시작하면 방장은 호스트가 되어 게임 서버를 구동하고 다른 참가자들은 게임 클라이언트를 구동하여 게임 서버에 접속합니다. <br>
UGameplayStatics의 OpenLevel 함수로 레벨을 해당 온라인 스테이지로 이동하여 게임을 시작합니다. <br>
<br>

# [실행 파일]
- 실행 파일 다운로드: (배포하면 저작권에 문제될 것 같아서 현재 링크를 지습니다.) <br>
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
- 개발자 MainServer(데디케이티드) IP: 58.233.177.231 (제 컴퓨터의 IP가 변경되면 수정하여 다시 표기하겠습니다.) <br>
- 개발자 MainServer(데디케이티드) Port: 8000 
1. 모두 Game.exe 실행 <br>
2. 온라인 클릭 <br>
3. 각자 ID 입력 <br>
4. 127.0.0.1 대신 개발자 MainServer IP를 입력. <br>
5. 접속 <br>
6. 방장이 게임방 생성 (그 순간 방장이 GameServer(리슨서버)를 생성하므로 공유기로 인해 내부(사설)/외부(공인) IP로 나눠진 경우, 참가자가 찾을 수가 없어서 그 사람은 방장을 할 수 없습니다.)<br>
7. 방장이 방을 만들면 나머지는 새로고침하고 생성된 게임방에 참가 <br>
8. 방장이 스테이지 설정 <br>
9. 시작 <br>
10. 네트워크 액세스 허용 <br>
- 방장 IP: 공유기나 내부 네트워크를 사용해서 인터넷에 접속할 경우 사설 IP(Private IP)라고 하는 특정 주소 범위(192.168.0.1 ~ 192.168.255.254)가 내부적으로 사용되고, 공인 IP 주소를 찾기 힘든 경우가 있습니다.
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

# [사용한 애셋들의 출처(저작권) 명시]
1. 3D 모델 & 애니메이션 (3D Models & Animations)
- EpicGames 무료 에셋
- Mixamo: https://www.mixamo.com/
- Free3d: https://free3d.com/ko/
- Turbosquid: https://www.turbosquid.com/
- Sketchfab: https://sketchfab.com/
- Cgtrader: https://www.cgtrader.com/
<br>

2. 이미지 (Image)
- Main UI by Yareni [Personal Use]: https://www.cleanpng.com/png-starcraft-ii-legacy-of-the-void-dota-2-defense-of-799826/preview.html
- Mineral Icon by Rudityas W Anggoro [Free for commercial use]: https://icon-icons.com/ko/%EC%95%84%EC%9D%B4%EC%BD%98/%EC%9D%98-%EB%B3%B4%EC%84%9D-graceful-%EB%AF%B8%EB%84%A4%EB%9E%84-%EB%8F%8C/112703
- Organic Icon by Vandelay Design [Free for commercial use]: https://icon-icons.com/ko/%EC%95%84%EC%9D%B4%EC%BD%98/%EC%9C%A0%EA%B8%B0%EB%86%8D-%EC%8B%9D%ED%92%88-%EC%95%8C%EB%A0%88%EB%A5%B4%EA%B8%B0-%EC%9C%A0%EB%B0%9C-%ED%95%AD%EC%9B%90/49217
- Energy Icon by Jeremiah [Attribution 4.0 International (CC BY 4.0)]: https://icon-icons.com/ko/%EC%95%84%EC%9D%B4%EC%BD%98/%ED%99%98%EA%B2%BD-%EC%8B%9C%EC%8A%A4%ED%85%9C-%EC%A0%84%EB%A0%A5-%EC%97%90%EB%84%88%EC%A7%80/103835
- Pioneer Icon by Cosmin Negoita [Free for commercial use]: https://icon-icons.com/ko/%EC%95%84%EC%9D%B4%EC%BD%98/%EA%B3%B5%EA%B0%84%EC%9D%B4-%EC%9A%B0%EC%A3%BC-%EB%B9%84%ED%96%89%EC%82%AC/115164
- Pioneer2 Icon by Chanut is Industries [Attribution 3.0 Unported (CC BY 3.0)]: http://www.iconarchive.com/show/outer-space-icons-by-chanut/Pioneer-icon.html
- Victory & Defeat by cameron2000 [can be used for personal use unless stated otherwise]: https://favpng.com/png_view/league-of-legends-league-of-legends-age-of-empires-ii-desktop-wallpaper-twitch-wallpaper-png/CwBPyf0F
- Continue Button by YAWD [Non-commercial use]: https://ya-webdesign.com/image/continue-button-png/1845688.html
<br>

3. 사운드 (Sound)
- 발사체 (Projectile): EpicGames 무료 에셋
- MainScreen BGM by Benjamin Tissot [FREE License with Attribution]: https://www.bensound.com/royalty-free-music/track/epic
- InGame BGM by Benjamin Tissot [FREE License with Attribution]: https://www.bensound.com/royalty-free-music/track/evolution-epic-nature
<br>
 
4. 폰트 (Font)
- Neo둥근모 by Dalgona [SIL Open Font License 1.1]: https://dalgona.github.io/neodgm/ or
  https://github.com/Dalgona/neodgm/releases or https://neodgm.dalgona.dev/
<br>

5. 코드 (Code)
- UnrealEngine Documents.
- Google Searches.
- Youtube Lectures.
- 설정 (Settings): https://www.youtube.com/watch?v=U_nCvY1OsdE
- 네트워크 기초 (Basic Network): https://github.com/LimSungMin/SungminWorld
<br>
