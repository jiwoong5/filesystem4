# 📚 B+ Tree Student Database System

학생 데이터베이스를 **B+ Tree 인덱스**를 사용하여 효율적으로 관리하고 점수 범위 검색을 수행하는 프로그램입니다.

---

## 📋 개요

이 프로그램은 학생 정보를 바이너리 파일에 저장하고, **점수(score)** 필드에 대한 **B+ Tree 인덱스**를 생성해서 **빠른 범위 검색**을 지원합니다.

---

## 🏧 시스템 구조

### 파일 구조

```
🔹 student.txt           # 입력: 학생 데이터 (텍스트 형식)
🔹 query.txt             # 입력: 검색 쿨리 (점수 범위)
🔹 student.dat           # 생성: 바이너리 학생 데이터
🔹 student.idx           # 생성: B+ Tree 인덱스 파일
🔹 bplustree.cpp         # 메인 프로그램 소스코드
🔹 integrated_script.sh  # Linux/Mac 시행 스크립트
🔹 integrated_script.bat # Windows 시행 스크립트
🔹 README.md             # 이 파일
```

### record 구조

```cpp
struct Student {
    unsigned ID;        // 학생 ID
    char name[20];      // 학생 이름 (null-terminated 아니면)
    float score;        // 점수 (인덱스 키)
    unsigned advisorID; // 지도교수 ID
};
```

### B+ Tree 설정

* **차수 (DEGREE)**: 3 (Internal/Leaf 노드는 최대 2개의 키 가짐)
* **블록 크기**: 4096KB
* **인덱스 키**: `score` (float)

---

## 🚀 사용 방법

```bash
git clone https://github.com/jiwoong5/filesystem4.git
```

### Linux/Mac

```bash
chmod +x integrated_script.sh
./integrated_script.sh
```

### Windows

```cmd
integrated_script.bat
```

---

## 📝 입력 파일 형식

### student.txt

```
아직은: 학생ID 학생이름 점수 지도교수ID
```

프리뷰 예시:

```
1001 JohnSmith 85.5 2001
1002 JaneWilson 92.3 2002
1003 MikeJohnson 78.9 2001
```

### query.txt

```
첫번째와 두번째 값이 최소~최대 점수 범위 
```

프리뷰 예시:

```
80.0 90.0
85.0 95.0
70.0 80.0
```

---

## ⚙️ 프로그램 동작 과정

1. `student.txt` → `student.dat` (바이너리 형식 변환)
2. B+ Tree 구성 및 인덱스 파일(`student.idx`) 생성
3. `query.txt`의 검색에 따른 결과 출력

---

## 📊 출력 형식

```
점수 범위 80 ~ 90 사이의 학생을 검색합니다...
조건에 맞는 학생 수: N명
```

---

## ⚙️ 컴파일 수단

```bash
g++ -o bplustree bplustree.cpp -std=c++11
./bplustree
```

### 요구 사항

* C++11 이상 지원 컴파일러
* 표준 C++ 라이브러리

---

## 📈 성능 특징

* 입력: `O(log n)`
* 범위 검색: `O(log n + k)` (k = 결과 개수)
* 단일 키 검색: `O(log n)`
* 삽입/삭제: `O(log n)`

---

## 🤞 주의사항

* **파일 경로**: 실행 파일과 같은 디렉토리에 요구 파일 필요
* **데이터 형식**: `student.txt`의 형식을 정확하게 입력
* **메모리**: 대용량 처리 시 부족한 메모리에 의해 segment fault 가능
* **문자 인코딩**: 학생이름 는 null x

---

## 🔍 실행 예시

```bash
$ ./integrated_script.sh
총 18095개의 학생 레코드를 처리합니다.
인덱스 파일이 성공적으로 저장되었습니다: student.idx

점수 범위 80 ~ 90 사이의 학생을 검색합니다...
조건에 맞는 학생 수: 1205명

점수 범위 85 ~ 95 사이의 학생을 검색합니다...
조건에 맞는 학생 수: 892명
```

---

## 📞 문제 해결

### 일반적인 오류

| 오류 메시지         | 원인 및 해결                |
| -------------- | ---------------------- |
| `파일을 열 수 없습니다` | 파일이 존재하는지 경로 확인        |
| `segmentation fault`   | 입력 데이터 형식 오류 여부 확인     |
| `컴파일 오류`       | C++11 지원 컴파일러 사용 여부 확인 |

### 디버그

* 처리 과정에서 상세 로그 메시지 확인
* 입력 파일 형식, 경로 검사

---

## 🧑‍💻 개발자 정보

* **프로젝트명**: filesystem4
* **버전**: 1.0
* **최종 수정**: 2025년 6월
