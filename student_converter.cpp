#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <cstring>

using namespace std;

// 상수 정의
const int BLOCK_SIZE = 4096;
const int STUDENT_SIZE = sizeof(unsigned) + 20 + sizeof(float) + sizeof(unsigned); // 32 bytes
const int STUDENTS_PER_BLOCK = BLOCK_SIZE / STUDENT_SIZE; // 128 students per block

// 구조체 정의
struct Student {
    unsigned ID;
    char name[20];      // null-terminated 아님
    float score;
    unsigned advisorID;
};

// 통계 클래스
class InsertStats {
private:
    int totalAttempts;
    int successCount;
    int failureCount;
    
public:
    InsertStats() : totalAttempts(0), successCount(0), failureCount(0) {}
    
    void recordSuccess() {
        totalAttempts++;
        successCount++;
    }
    
    void recordFailure() {
        totalAttempts++;
        failureCount++;
    }
    
    void printInsertStats(const string& type) {
        cout << type << " Insert Statistics:" << endl;
        cout << "  Total Attempts: " << totalAttempts << endl;
        cout << "  Successful Inserts: " << successCount << endl;
        cout << "  Failed Inserts (Duplicate ID): " << failureCount << endl;
        cout << "  Success Rate: " << (totalAttempts > 0 ? (double)successCount / totalAttempts * 100 : 0) << "%" << endl;
        cout << endl;
    }
};

// 전역 변수
InsertStats studentInsertStats;
char studentBlock[BLOCK_SIZE];

// 유틸리티 함수
void stringToCharArray(const string& str, char* arr, int maxLen) {
    int copyLen = min((int)str.length(), maxLen);
    memcpy(arr, str.c_str(), copyLen);
    // 남은 공간을 0으로 채움 (null-terminated 아님)
    if (copyLen < maxLen) {
        memset(arr + copyLen, 0, maxLen - copyLen);
    }
}

int getStudentCount() {
    ifstream file("student.dat", ios::binary);
    if (!file.is_open()) {
        return 0;
    }
    
    file.seekg(0, ios::end);
    streampos fileSize = file.tellg();
    file.close();
    
    return fileSize / STUDENT_SIZE;
}

bool insertStudent(string name, unsigned ID, float score, unsigned advisorID) {
    // 중복 ID 체크
    int studentCount = getStudentCount();
    ifstream checkFile("student.dat", ios::binary);
    
    if (checkFile.is_open()) {
        int blocksToRead = (studentCount + STUDENTS_PER_BLOCK - 1) / STUDENTS_PER_BLOCK;
        
        for (int blockNum = 0; blockNum < blocksToRead; blockNum++) {
            checkFile.seekg(blockNum * BLOCK_SIZE);
            checkFile.read(studentBlock, BLOCK_SIZE);
            
            int studentsInBlock = min(STUDENTS_PER_BLOCK, studentCount - blockNum * STUDENTS_PER_BLOCK);
            Student* students = (Student*)studentBlock;
            
            for (int i = 0; i < studentsInBlock; i++) {
                if (students[i].ID == ID) {
                    checkFile.close();
                    studentInsertStats.recordFailure(); // 실패 기록
                    return false; // 중복 ID
                }
            }
        }
        checkFile.close();
    }
    
    // 새 학생 추가
    Student newStudent;
    newStudent.ID = ID;
    stringToCharArray(name, newStudent.name, 20);
    newStudent.score = score;
    newStudent.advisorID = advisorID;
    
    ofstream file("student.dat", ios::binary | ios::app);
    if (!file.is_open()) {
        studentInsertStats.recordFailure(); // 실패 기록
        return false;
    }
    
    file.write((char*)&newStudent, STUDENT_SIZE);
    file.close();
    
    studentInsertStats.recordSuccess(); // 성공 기록
    return true;
}

bool parseStudentLine(const string& line, string& name, unsigned& ID, float& score, unsigned& advisorID) {
    istringstream iss(line);
    string token;
    
    // 공백이나 탭으로 구분된 데이터 파싱
    if (!(iss >> token)) return false;
    
    // 첫 번째 토큰이 숫자인지 확인 (ID부터 시작하는 경우)
    if (isdigit(token[0])) {
        // 형식: ID name score advisorID
        ID = stoul(token);
        if (!(iss >> name >> score >> advisorID)) return false;
    } else {
        // 형식: name ID score advisorID
        name = token;
        if (!(iss >> ID >> score >> advisorID)) return false;
    }
    
    return true;
}

int main(int argc, char* argv[]) {
    string inputFile = "student.txt";
    
    // 명령행 인수가 있으면 파일명 변경
    if (argc > 1) {
        inputFile = argv[1];
    }
    
    cout << "=== Student Data Converter ===" << endl;
    cout << "입력 파일: " << inputFile << endl;
    cout << "출력 파일: student.dat" << endl;
    cout << endl;
    
    // 기존 student.dat 파일 삭제 (새로 시작)
    remove("student.dat");
    
    ifstream inFile(inputFile);
    if (!inFile.is_open()) {
        cerr << "오류: " << inputFile << " 파일을 열 수 없습니다." << endl;
        return 1;
    }
    
    string line;
    int lineNumber = 0;
    
    cout << "학생 데이터 변환 중..." << endl;
    
    while (getline(inFile, line)) {
        lineNumber++;
        
        // 빈 줄이나 주석 줄 건너뛰기
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        string name;
        unsigned ID, advisorID;
        float score;
        
        if (parseStudentLine(line, name, ID, score, advisorID)) {
            bool success = insertStudent(name, ID, score, advisorID);
            
            if (success) {
                cout << "✓ 라인 " << lineNumber << ": " << name << " (ID: " << ID << ") 추가됨" << endl;
            } else {
                cout << "✗ 라인 " << lineNumber << ": " << name << " (ID: " << ID << ") 추가 실패 (중복 ID)" << endl;
            }
        } else {
            cout << "✗ 라인 " << lineNumber << ": 파싱 오류 - " << line << endl;
            studentInsertStats.recordFailure();
        }
    }
    
    inFile.close();
    
    cout << endl;
    cout << "=== 변환 완료 ===" << endl;
    studentInsertStats.printInsertStats("Student");
    
    // 결과 파일 정보 출력
    int finalCount = getStudentCount();
    cout << "생성된 student.dat 파일 정보:" << endl;
    cout << "  총 학생 수: " << finalCount << "명" << endl;
    cout << "  파일 크기: " << (finalCount * STUDENT_SIZE) << " bytes" << endl;
    cout << "  사용된 블록 수: " << ((finalCount + STUDENTS_PER_BLOCK - 1) / STUDENTS_PER_BLOCK) << "개" << endl;
    
    return 0;
}
