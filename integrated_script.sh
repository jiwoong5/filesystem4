#!/bin/bash

# 스크립트 설정
CONVERTER_CPP="student_converter.cpp"
MAIN_CPP="main.cpp"
CONVERTER_EXEC="student_converter"
MAIN_EXEC="student_search"
STUDENT_TXT="student.txt"
QUERY_FILE="query.txt"

# 색상 코드
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

echo -e "${BLUE}=== 통합 학생 데이터 처리 스크립트 ===${NC}"
echo -e "${CYAN}1. student.txt → student.dat 변환${NC}"
echo -e "${CYAN}2. B+ Tree 인덱스 생성 및 범위 검색${NC}"
echo ""

# ========== STEP 1: Student Converter ==========
echo -e "${YELLOW}[STEP 1] Student Data Converter${NC}"

# 1-1. student_converter.cpp 파일 존재 확인
if [ ! -f "$CONVERTER_CPP" ]; then
    echo -e "${RED}오류: $CONVERTER_CPP 파일이 존재하지 않습니다.${NC}"
    exit 1
fi

# 1-2. student_converter 컴파일
echo -e "${YELLOW}student_converter 컴파일 중...${NC}"
g++ -std=c++11 -O2 -o "$CONVERTER_EXEC" "$CONVERTER_CPP"
if [ $? -ne 0 ]; then
    echo -e "${RED}student_converter 컴파일 실패!${NC}"
    exit 1
fi
echo -e "${GREEN}student_converter 컴파일 성공!${NC}"

# 1-3. student.txt 파일 확인 및 생성
if [ ! -f "$STUDENT_TXT" ]; then
    echo -e "${YELLOW}$STUDENT_TXT 파일이 없습니다. 샘플 파일을 생성합니다.${NC}"
    cat << 'EOF' > "$STUDENT_TXT"
# Student data format: ID name score advisorID
20211001 김철수 85.5 1001
20211002 이영희 92.0 1002
20211003 박민수 78.3 1001
20211004 최수진 88.7 1003
20211005 정대현 91.2 1002
20211006 한소영 76.8 1001
20211007 임준호 89.4 1003
20211008 송미경 94.1 1002
20211009 오세훈 82.6 1001
20211010 윤채연 87.9 1003
20211011 장성호 93.2 1002
20211012 김유진 79.8 1001
20211013 이동욱 86.5 1003
20211014 박서연 90.1 1002
20211015 최민준 84.7 1001
EOF
    echo -e "${GREEN}샘플 $STUDENT_TXT 파일 생성 완료${NC}"
fi

# 1-4. 입력 파일 미리보기
echo -e "${YELLOW}입력 파일 미리보기:${NC}"
head -10 "$STUDENT_TXT"
echo "... (총 $(grep -v '^#' "$STUDENT_TXT" | grep -v '^$' | wc -l)개 학생 데이터)"
echo ""

# 1-5. 기존 student.dat 백업
if [ -f "student.dat" ]; then
    echo -e "${YELLOW}기존 student.dat를 student.dat.backup으로 백업합니다.${NC}"
    cp student.dat student.dat.backup
fi

# 1-6. student_converter 실행
echo -e "${YELLOW}student.txt → student.dat 변환 중...${NC}"
echo "--------------------------------"
./"$CONVERTER_EXEC" "$STUDENT_TXT"
CONVERTER_RESULT=$?
echo "--------------------------------"

if [ $CONVERTER_RESULT -eq 0 ]; then
    echo -e "${GREEN}✓ student.dat 변환 완료!${NC}"
    if [ -f "student.dat" ]; then
        ls -lh student.dat
    fi
else
    echo -e "${RED}✗ student.dat 변환 중 오류 발생${NC}"
    exit 1
fi

echo ""

# ========== STEP 2: Main Program ==========
echo -e "${YELLOW}[STEP 2] B+ Tree Index & Range Search${NC}"

# 2-1. main.cpp 파일 존재 확인
if [ ! -f "$MAIN_CPP" ]; then
    echo -e "${RED}오류: $MAIN_CPP 파일이 존재하지 않습니다.${NC}"
    exit 1
fi

# 2-2. main 프로그램 컴파일
echo -e "${YELLOW}main 프로그램 컴파일 중...${NC}"
g++ -std=c++11 -O2 -o "$MAIN_EXEC" "$MAIN_CPP"
if [ $? -ne 0 ]; then
    echo -e "${RED}main 프로그램 컴파일 실패!${NC}"
    exit 1
fi
echo -e "${GREEN}main 프로그램 컴파일 성공!${NC}"

# 2-3. query.txt 파일 확인 및 생성
if [ ! -f "$QUERY_FILE" ]; then
    echo -e "${YELLOW}$QUERY_FILE 파일이 없습니다. 기본 쿼리 파일을 생성합니다.${NC}"
    echo "80.0 90.0" > "$QUERY_FILE"
    echo -e "${GREEN}기본 쿼리 파일 생성 완료 (점수 범위: 80.0 ~ 90.0)${NC}"
fi

# 2-4. 쿼리 파일 내용 출력
echo -e "${YELLOW}쿼리 파일 내용:${NC}"
cat "$QUERY_FILE"
echo ""

# 2-5. main 프로그램 실행
echo -e "${YELLOW}B+ Tree 인덱스 생성 및 범위 검색 실행 중...${NC}"
echo "================================"
./"$MAIN_EXEC" "$QUERY_FILE"
MAIN_RESULT=$?
echo "================================"

if [ $MAIN_RESULT -eq 0 ]; then
    echo -e "${GREEN}✓ 프로그램 실행 완료!${NC}"
else
    echo -e "${RED}✗ 프로그램 실행 중 오류 발생 (종료 코드: $MAIN_RESULT)${NC}"
fi

# ========== 결과 요약 ==========
echo ""
echo -e "${BLUE}=== 실행 결과 요약 ===${NC}"
echo -e "${YELLOW}생성된 파일들:${NC}"

# 생성된 파일들 확인
files_created=()
if [ -f "student.dat" ]; then
    files_created+=("student.dat")
fi
if [ -f "student.idx" ]; then
    files_created+=("student.idx")
fi
if [ -f "$CONVERTER_EXEC" ]; then
    files_created+=("$CONVERTER_EXEC")
fi
if [ -f "$MAIN_EXEC" ]; then
    files_created+=("$MAIN_EXEC")
fi

if [ ${#files_created[@]} -gt 0 ]; then
    ls -lh "${files_created[@]}"
else
    echo "생성된 파일이 없습니다."
fi

# ========== 정리 옵션 ==========
echo ""
echo -e "${YELLOW}실행 파일들을 삭제하시겠습니까? (y/N):${NC}"
read -r response
if [[ "$response" =~ ^[Yy]$ ]]; then
    rm -f "$CONVERTER_EXEC" "$MAIN_EXEC"
    echo -e "${GREEN}실행 파일 삭제 완료${NC}"
fi

echo -e "${BLUE}모든 작업 완료!${NC}"
