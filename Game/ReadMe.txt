1. 언리얼엔진 필수 요소
    /Build
    /Config
    /Content    // 대용량이라 구글 드라이브에서 따로 다운 받아야 합니다.
    /Source
    /Game.uproject

2. 추가 요소
    /Server    // Version: Visual Studio 2019

3. 진행
    구글 드라이브에서 /Content 폴더를 다운받아서 같은 경로에 넣어줍니다.
    Game.uproject 파일에 우클릭하여 [Generate Visual Studio project files]를 좌클릭합니다.
    작업이 끝나고 생성된 Game.sln 파일을 실행하여 빌드 환경을 Development Editor와 Win64로 변경한 후, Ctrl + F5로 컴파일합니다.
    이제 마지막으로 Game.uproject 파일을 실행합니다.
