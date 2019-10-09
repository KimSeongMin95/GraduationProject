# GraduationProject
[홍익대 세종캠 게임소프트웨어 B477004 김성민] 졸업 프로젝트 저장소입니다.

UnrealEngine 4.23 버전으로 PC 3D RTS 게임을 기획하고 개발중에 있습니다.

지도: 강신진 교수님

기간: 2019-09-18 ~ 통과

문의: 010-8865-0312




[프로젝트 폴더 관리]
Content/: 에셋 파일들 존재 (제거X)
Config/: 프로젝트의 설정값들 보관 (제거X)
Intermediate/: 프로젝트 관리에 필요한 임시파일들을 저장. 매번 새로 만들어주기 때문에 (제거O)
Saved/: 에디터 작업 중에 생성된 결과물들 저장. 게임의 Save파일, 스크린샷, 게임내 생성 파일, 로그파일 (제거X)
Binaries/: C++ 코드가 컴파일된 결과물이 저장. C++ 코드를 컴파일 할 때마다 새로 생성. (제거O)
Binaries/Win64/에서 Game, Game.exp, Game.lib, Game.pdb, Game.target 는 에디터에서 Launch하면 새로 생성되고 나머지는 에디터에서 컴파일하면 생성됨.
Source/: C++ 코드가 저장되는 폴더 (제거X)
VS/: VisualStudio 실행하면 생성되는데 언리얼 코드가 전부 들어있으므로 제거하면 안됨. (제거X)
