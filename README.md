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

2. 이미지 (Image)
- Main UI(Personal Use): https://www.cleanpng.com/png-starcraft-ii-legacy-of-the-void-dota-2-defense-of-799826/preview.html
- Resource Icon
Mineral: https://icon-icons.com/ko/%EC%95%84%EC%9D%B4%EC%BD%98/%EC%9D%98-%EB%B3%B4%EC%84%9D-graceful-%EB%AF%B8%EB%84%A4%EB%9E%84-%EB%8F%8C/112703
Organic: https://icon-icons.com/ko/%EC%95%84%EC%9D%B4%EC%BD%98/%EC%9C%A0%EA%B8%B0%EB%86%8D-%EC%8B%9D%ED%92%88-%EC%95%8C%EB%A0%88%EB%A5%B4%EA%B8%B0-%EC%9C%A0%EB%B0%9C-%ED%95%AD%EC%9B%90/49217
Energy(Attribution 4.0 International (CC BY 4.0)): https://icon-icons.com/ko/%EC%95%84%EC%9D%B4%EC%BD%98/%ED%99%98%EA%B2%BD-%EC%8B%9C%EC%8A%A4%ED%85%9C-%EC%A0%84%EB%A0%A5-%EC%97%90%EB%84%88%EC%A7%80/103835
Pioneer: https://icon-icons.com/ko/%EC%95%84%EC%9D%B4%EC%BD%98/%EA%B3%B5%EA%B0%84%EC%9D%B4-%EC%9A%B0%EC%A3%BC-%EB%B9%84%ED%96%89%EC%82%AC/115164
Pioneer2(Attribution 3.0 Unported (CC BY 3.0)): http://www.iconarchive.com/show/outer-space-icons-by-chanut/Pioneer-icon.html
- Victory & Defeat: https://favpng.com/png_view/league-of-legends-league-of-legends-age-of-empires-ii-desktop-wallpaper-twitch-wallpaper-png/CwBPyf0F
- Continue: https://ya-webdesign.com/image/continue-button-png/483820.html
- Dialog: Ther Are Billions Source

3. 사운드 (Sound)
- 발사체 (Projectile): EpicGames 무료 에셋
- They Are Billions Incoming Menace: https://www.youtube.com/watch?v=mcnrX_N_yws&list=PLqcUjE-I-0rKNzEtu2oGt-Et8S3uE8Gb7
- They Are Billions Reconquest: https://www.youtube.com/watch?v=8LC6XfP0PAo&list=PLqcUjE-I-0rKNzEtu2oGt-Et8S3uE8Gb7&index=17
- They Are Billions Strong Walls: https://www.youtube.com/watch?v=zEzBG3MgPJU&list=PLqcUjE-I-0rKNzEtu2oGt-Et8S3uE8Gb7&index=4
- They Are Billions The Goddess of Destiny: https://www.youtube.com/watch?v=Gtsdh-Vq2tM&list=PLqcUjE-I-0rKNzEtu2oGt-Et8S3uE8Gb7&index=15
- They Are Billions The New Empire: https://www.youtube.com/watch?v=GQt-tQ-Y6ZQ&list=PLqcUjE-I-0rKNzEtu2oGt-Et8S3uE8Gb7&index=7
- They Are Billions The Throne Room: https://www.youtube.com/watch?v=mcnrX_N_yws&list=PLqcUjE-I-0rKNzEtu2oGt-Et8S3uE8Gb7&index=1
- They Are Billions They Are Billions!: https://www.youtube.com/watch?v=ZFbV-JPc6cI&list=PLqcUjE-I-0rKNzEtu2oGt-Et8S3uE8Gb7&index=2
- They Are Billions We Are Done For: https://www.youtube.com/watch?v=i72RR9ZFFUs&list=PLqcUjE-I-0rKNzEtu2oGt-Et8S3uE8Gb7&index=6
- Star Trek 2009: https://www.youtube.com/watch?v=BlHyt_I1rFc
 
4. 폰트 (Font)
- Neo둥근모(SIL Open Font License 1.1): https://dalgona.github.io/neodgm/
  https://github.com/Dalgona/neodgm/releases

5. 코드 (Code)
- UnrealEngine Documents.
- Google Searches.
- Youtube Lectures.
- 설정 (Settings): Youtube Lecture.
- 네트워크 기초 (Basic Network): https://github.com/LimSungMin/SungminWorld
