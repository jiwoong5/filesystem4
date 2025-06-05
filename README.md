B+ Tree Student Database System
학생 데이터베이스를 B+ Tree 인덱스를 사용하여 효율적으로 관리하고 점수 범위 검색을 수행하는 프로그램입니다.

📋 개요
이 프로그램은 학생 정보를 바이너리 파일로 저장하고, 점수(score) 필드에 대한 B+ Tree 인덱스를 생성하여 빠른 범위 검색을 지원합니다.

🏗️ 시스템 구조
데이터 구조
cpp
struct Student {
    unsigned ID;        // 학생 ID
    char name[20];      // 학생 이름 (null-terminated 아님)
    float score;        // 점수 (인덱스 키)
    unsigned advisorID; // 지도교수 ID
};
B+ Tree 설정
차수 (DEGREE): 3 (최대 키 수 = 2)
블록 크기: 4096KB
인덱스 키: 학생 점수 (score)
📁 파일 구조
├── student.txt          # 입력: 학생 데이터 (텍스트 형식)
├── query.txt           # 입력: 검색 쿼리 (점수 범위)
├── student.dat         # 생성: 바이너리 학생 데이터
├── student.idx         # 생성: B+ Tree 인덱스 파일
├── bplustree.cpp       # 메인 프로그램 소스코드
├── integrated_script.sh # Linux/Mac 실행 스크립트
├── integrated_script.bat # Windows 실행 스크립트
└── README.md           # 이 파일
🚀 사용 방법
Linux/Mac 환경
bash
chmod +x integrated_script.sh
./integrated_script.sh
Windows 환경
cmd
integrated_script.bat
📝 입력 파일 형식
student.txt
학생 정보를 한 줄에 하나씩 다음 형식으로 작성:

학생ID 학생이름 점수 지도교수ID
예시:

1001 JohnSmith 85.5 2001
1002 JaneWilson 92.3 2002
1003 MikeJohnson 78.9 2001
query.txt
검색할 점수 범위를 한 줄에 하나씩 다음 형식으로 작성:

최소점수 최대점수
예시:

80.0 90.0
85.0 95.0
70.0 80.0
⚙️ 프로그램 동작 과정
데이터 변환: student.txt → student.dat (바이너리 형식)
인덱스 생성: 점수 기준 B+ Tree 생성 → student.idx
범위 검색: query.txt의 각 쿼리에 대해 범위 내 학생 수 출력
📊 출력 형식
각 쿼리에 대해 다음과 같이 출력됩니다:

점수 범위 80 ~ 90 사이의 학생을 검색합니다...
조건에 맞는 학생 수: 1205명

점수 범위 85 ~ 95 사이의 학생을 검색합니다...
조건에 맞는 학생 수: 892명
🔧 컴파일 및 실행
수동 컴파일
bash
g++ -o bplustree bplustree.cpp -std=c++17
./bplustree
요구사항
C++17 이상 지원 컴파일러
표준 C++ 라이브러리
📈 성능 특징
시간 복잡도:
삽입: O(log n)
범위 검색: O(log n + k) (k = 결과 개수)
공간 복잡도: O(n)
블록 크기: 4096KB로 설정하여 디스크 I/O 최적화
🐛 주의사항
파일 경로: 실행 파일과 같은 디렉토리에 입력 파일들이 있어야 합니다.
데이터 형식: student.txt의 형식이 정확해야 합니다.
메모리: 대용량 데이터 처리 시 충분한 메모리가 필요합니다.
문자 인코딩: 학생 이름은 20바이트 고정 길이이며 null-terminated되지 않습니다.
🔍 예시 실행
bash
$ ./integrated_script.sh
총 18095개의 학생 레코드를 처리합니다.
인덱스 파일이 성공적으로 저장되었습니다: student.idx
점수 범위 80 ~ 90 사이의 학생을 검색합니다...
조건에 맞는 학생 수: 1205명
점수 범위 85 ~ 95 사이의 학생을 검색합니다...
조건에 맞는 학생 수: 892명
📞 문제 해결
일반적인 오류
"파일을 열 수 없습니다": 입력 파일이 존재하는지 확인
"세그멘테이션 폴트": 입력 데이터 형식 확인
"컴파일 오류": C++11 지원 컴파일러 사용 확인
디버깅
프로그램은 처리 과정에서 상세한 로그를 출력하므로, 오류 발생 시 출력 메시지를 확인하세요.

개발자: B+ Tree Database System
버전: 1.0
최종 수정: 2025년 6월


