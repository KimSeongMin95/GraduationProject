# GraduationProject  
[프로젝트 개요]<br>
--- 홍익대 세종캠 게임소프트웨어 B477004 김성민 졸업 프로젝트 저장소입니다.<br>
--- UnrealEngine 4.23 버전으로 PC 3D RTS 게임을 기획하고 개발중에 있습니다.<br>
--- 지도: 강신진 교수님<br>
--- 기간: 2019-09-18 ~ 통과<br>
--- 문의: 010-8865-0312<br>
<br>
<br>
[프로젝트 폴더 관리]<br>
--- .vs/: VisualStudio 실행하면 생성. 제거 후 디버깅에서 Development Editor로 변경해주고 Win64 해주면 되지만 (제거X)<br>
------ 그냥 .vs/Game/v15/에서 ipch/ 폴더만 삭제하는게 좋음. (제거O)
--- Binaries/: C++ 코드가 컴파일된 결과물이 저장. C++ 코드를 컴파일 할 때마다 새로 생성. (제거O)<br>
------ Binaries/Win64/에서 Game, Game.exp, Game.lib, Game.pdb, Game.target 는 에디터에서 Launch하면 새로 생성되고 나머지는 에디터에서 컴파일하면 생성됨.<br>
--- Build/: ???<br>
--- Config/: 프로젝트의 설정값들 보관 (제거X)<br>
--- Content/: 에셋 파일들 존재 (제거X)<br>
--- Intermediate/: 프로젝트 관리에 필요한 임시파일들을 저장. 제거하면 VisualStudio에서 프로젝트 다 날라감. (제거X)<br>
--- Saved/: 에디터 작업 중에 생성된 결과물들 저장. 게임의 Save파일, 스크린샷, 게임내 생성 파일, 로그파일 (제거X)<br>
--- Source/: C++ 코드가 저장되는 폴더 (제거X)<br>
--- Game.sln: VisualStudio 솔루션. 제거해도 생성 가능 (제거X)<br>
--- Game: 언리얼엔진 (제거X)<br>
<br>
<br>
```c
#include<stdio.h>
int main(void)
{
printf("Hello Interface!\n");
return 0;
}
```
