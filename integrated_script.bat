@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

:: 스크립트 설정
set CONVERTER_CPP=student_converter.cpp
set MAIN_CPP=main.cpp
set CONVERTER_EXEC=student_converter.exe
set MAIN_EXEC=student_search.exe
set STUDENT_TXT=student.txt
set QUERY_FILE=query.txt

echo === 통합 학생 데이터 처리 스크립트 ===
echo 1. student.txt → student.dat 변환
echo 2. B+ Tree 인덱스 생성 및 범위 검색
echo.

:: ========== STEP 1: Student Converter ==========
echo [STEP 1] Student Data Converter

:: 1-1. student_converter.cpp 파일 존재 확인
if not exist "%CONVERTER_CPP%" (
    echo [오류] %CONVERTER_CPP% 파일이 존재하지 않습니다.
    pause
    exit /b 1
)

:: 1-2. student_converter 컴파일
echo student_converter 컴파일 중...
g++ -std=c++11 -O2 -o "%CONVERTER_EXEC%" "%CONVERTER_CPP%"
if errorlevel 1 (
    echo [오류] student_converter 컴파일 실패!
    pause
    exit /b 1
)
echo [성공] student_converter 컴파일 완료!

:: 1-3. student.txt 파일 확인 및 생성
if not exist "%STUDENT_TXT%" (
    echo %STUDENT_TXT% 파일이 없습니다. 샘플 파일을 생성합니다.
    (
        echo # Student data format: ID name score advisorID
        echo 20211001 김철수 85.5 1001
        echo 20211002 이영희 92.0 1002
        echo 20211003 박민수 78.3 1001
        echo 20211004 최수진 88.7 1003
        echo 20211005 정대현 91.2 1002
        echo 20211006 한소영 76.8 1001
        echo 20211007 임준호 89.4 1003
        echo 20211008 송미경 94.1 1002
        echo 20211009 오세훈 82.6 1001
        echo 20211010 윤채연 87.9 1003
        echo 20211011 장성호 93.2 1002
        echo 20211012 김유진 79.8 1001
        echo 20211013 이동욱 86.5 1003
        echo 20211014 박서연 90.1 1002
        echo 20211015 최민준 84.7 1001
    ) > "%STUDENT_TXT%"
    echo [성공] 샘플 %STUDENT_TXT% 파일 생성 완료
)

:: 1-4. 입력 파일 미리보기
echo 입력 파일 미리보기:
powershell -Command "Get-Content '%STUDENT_TXT%' | Select-Object -First 10"
for /f %%i in ('findstr /v /c:"#" "%STUDENT_TXT%" ^| findstr /v /r "^$" ^| find /c /v ""') do set student_count=%%i
echo ... (총 !student_count!개 학생 데이터)
echo.

:: 1-5. 기존 student.dat 백업
if exist "student.dat" (
    echo 기존 student.dat를 student.dat.backup으로 백업합니다.
    copy student.dat student.dat.backup >nul
)

:: 1-6. student_converter 실행
echo student.txt → student.dat 변환 중...
echo --------------------------------
"%CONVERTER_EXEC%" "%STUDENT_TXT%"
set CONVERTER_RESULT=!errorlevel!
echo --------------------------------

if !CONVERTER_RESULT! equ 0 (
    echo [성공] student.dat 변환 완료!
    if exist "student.dat" (
        dir student.dat
    )
) else (
    echo [오류] student.dat 변환 중 오류 발생
    pause
    exit /b 1
)

echo.

:: ========== STEP 2: Main Program ==========
echo [STEP 2] B+ Tree Index ^& Range Search

:: 2-1. main.cpp 파일 존재 확인
if not exist "%MAIN_CPP%" (
    echo [오류] %MAIN_CPP% 파일이 존재하지 않습니다.
    pause
    exit /b 1
)

:: 2-2. main 프로그램 컴파일
echo main 프로그램 컴파일 중...
g++ -std=c++11 -O2 -o "%MAIN_EXEC%" "%MAIN_CPP%"
if errorlevel 1 (
    echo [오류] main 프로그램 컴파일 실패!
    pause
    exit /b 1
)
echo [성공] main 프로그램 컴파일 완료!

:: 2-3. query.txt 파일 확인 및 생성
if not exist "%QUERY_FILE%" (
    echo %QUERY_FILE% 파일이 없습니다. 기본 쿼리 파일을 생성합니다.
    echo 80.0 90.0 > "%QUERY_FILE%"
    echo [성공] 기본 쿼리 파일 생성 완료 ^(점수 범위: 80.0 ~ 90.0^)
)

:: 2-4. 쿼리 파일 내용 출력
echo 쿼리 파일 내용:
type "%QUERY_FILE%"
echo.

:: 2-5. main 프로그램 실행
echo B+ Tree 인덱스 생성 및 범위 검색 실행 중...
echo ================================
"%MAIN_EXEC%" "%QUERY_FILE%"
set MAIN_RESULT=!errorlevel!
echo ================================

if !MAIN_RESULT! equ 0 (
    echo [성공] 프로그램 실행 완료!
) else (
    echo [오류] 프로그램 실행 중 오류 발생 ^(종료 코드: !MAIN_RESULT!^)
)

:: ========== 결과 요약 ==========
echo.
echo === 실행 결과 요약 ===
echo 생성된 파일들:

:: 생성된 파일들 확인
set files_found=0
if exist "student.dat" (
    echo - student.dat
    set files_found=1
)
if exist "student.idx" (
    echo - student.idx
    set files_found=1
)
if exist "%CONVERTER_EXEC%" (
    echo - %CONVERTER_EXEC%
    set files_found=1
)
if exist "%MAIN_EXEC%" (
    echo - %MAIN_EXEC%
    set files_found=1
)

if !files_found! equ 0 (
    echo 생성된 파일이 없습니다.
)

:: ========== 정리 옵션 ==========
echo.
set /p response="실행 파일들을 삭제하시겠습니까? (y/N): "
if /i "!response!"=="y" (
    del "%CONVERTER_EXEC%" 2>nul
    del "%MAIN_EXEC%" 2>nul
    echo [성공] 실행 파일 삭제 완료
)

echo 모든 작업 완료!
echo.
pause
