#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cstring>
#include <string>
using namespace std;

const int BLOCK_SIZE = 4096;
const int DEGREE = 3;

#pragma pack(push, 1)
struct Student {
    unsigned ID;
    char name[20]; // null-terminated 아님
    float score;
    unsigned advisorID;
};
#pragma pack(pop)

const int STUDENT_SIZE = sizeof(Student);
const int STUDENTS_PER_BLOCK = BLOCK_SIZE / STUDENT_SIZE;

char studentBlock[BLOCK_SIZE];

void stringToCharArray(const string& str, char* arr, int maxLen) {
    int copyLen = min((int)str.length(), maxLen);
    memcpy(arr, str.c_str(), copyLen);
    if (copyLen < maxLen) {
        memset(arr + copyLen, 0, maxLen - copyLen);
    }
}

bool insertStudent(string name, unsigned ID, float score, unsigned advisorID) {
    ifstream checkFile("student.dat", ios::binary);
    if (checkFile.is_open()) {
        checkFile.seekg(0, ios::end);
        int studentCount = checkFile.tellg() / STUDENT_SIZE;
        checkFile.seekg(0, ios::beg);

        int blocks = (studentCount + STUDENTS_PER_BLOCK - 1) / STUDENTS_PER_BLOCK;
        for (int b = 0; b < blocks; ++b) {
            checkFile.read(studentBlock, BLOCK_SIZE);
            Student* students = (Student*)studentBlock;
            int count = min(STUDENTS_PER_BLOCK, studentCount - b * STUDENTS_PER_BLOCK);
            for (int i = 0; i < count; ++i) {
                if (students[i].ID == ID) {
                    checkFile.close();
                    return false;
                }
            }
        }
        checkFile.close();
    }

    Student s;
    s.ID = ID;
    stringToCharArray(name, s.name, 20);
    s.score = score;
    s.advisorID = advisorID;

    ofstream out("student.dat", ios::binary | ios::app);
    if (!out) return false;
    out.write((char*)&s, STUDENT_SIZE);
    out.close();
    return true;
}

bool parseStudentLine(const string& line, string& name, unsigned& ID, float& score, unsigned& advisorID) {
    istringstream iss(line);
    string token;
    if (!(iss >> token)) return false;
    if (isdigit(token[0])) {
        ID = stoul(token);
        if (!(iss >> name >> score >> advisorID)) return false;
    } else {
        name = token;
        if (!(iss >> ID >> score >> advisorID)) return false;
    }
    return true;
}

struct BPlusNode {
    bool isLeaf;
    vector<float> keys;
    vector<BPlusNode*> children;
    vector<int> values;
    BPlusNode* next;
    BPlusNode(bool leaf) : isLeaf(leaf), next(nullptr) {}
};

class BPlusTree {
    BPlusNode* root;
public:
    BPlusTree() { root = new BPlusNode(true); }

    void insert(float key, int value) {
        if (root->keys.size() == DEGREE - 1) {
            BPlusNode* newRoot = new BPlusNode(false);
            newRoot->children.push_back(root);
            splitChild(newRoot, 0);
            root = newRoot;
        }
        insertNonFull(root, key, value);
    }

    void saveToFile(const string& filename) {
        ofstream fout(filename, ios::binary);
        vector<pair<float, int>> allData;
        collectAllData(root, allData);
        int count = allData.size();
        fout.write((char*)&count, sizeof(int));
        for (auto& [k, v] : allData) {
            fout.write((char*)&k, sizeof(float));
            fout.write((char*)&v, sizeof(int));
        }
    }

    static BPlusTree* loadFromFile(const string& filename) {
        ifstream fin(filename, ios::binary);
        if (!fin.is_open()) return nullptr;
        BPlusTree* tree = new BPlusTree();
        int count;
        fin.read((char*)&count, sizeof(int));
        for (int i = 0; i < count; ++i) {
            float k;
            int v;
            fin.read((char*)&k, sizeof(float));
            fin.read((char*)&v, sizeof(int));
            tree->insert(k, v);
        }
        return tree;
    }

    int searchRangeWithKeys(float minKey, float maxKey) {
        BPlusNode* node = root;
        while (!node->isLeaf) {
            int i = 0;
            while (i < node->keys.size() && minKey >= node->keys[i]) i++;
            node = node->children[i];
        }

        int result = 0;
        while (node) {
            for (int i = 0; i < node->keys.size(); ++i) {
                if (node->keys[i] > maxKey) return result;
                if (node->keys[i] >= minKey) result++;
            }
            node = node->next;
        }
        return result;
    }

private:
    void splitChild(BPlusNode* parent, int index) {
        BPlusNode* child = parent->children[index];
        BPlusNode* sibling = new BPlusNode(child->isLeaf);
        int mid = DEGREE / 2;

        parent->keys.insert(parent->keys.begin() + index, child->keys[mid]);
        parent->children.insert(parent->children.begin() + index + 1, sibling);

        if (child->isLeaf) {
            sibling->keys.assign(child->keys.begin() + mid, child->keys.end());
            sibling->values.assign(child->values.begin() + mid, child->values.end());
            child->keys.resize(mid);
            child->values.resize(mid);
            sibling->next = child->next;
            child->next = sibling;
        } else {
            sibling->keys.assign(child->keys.begin() + mid + 1, child->keys.end());
            sibling->children.assign(child->children.begin() + mid + 1, child->children.end());
            child->keys.resize(mid);
            child->children.resize(mid + 1);
        }
    }

    void insertNonFull(BPlusNode* node, float key, int value) {
        if (node->isLeaf) {
            auto it = lower_bound(node->keys.begin(), node->keys.end(), key);
            int idx = it - node->keys.begin();
            node->keys.insert(it, key);
            node->values.insert(node->values.begin() + idx, value);
        } else {
            int i = 0;
            while (i < node->keys.size() && key >= node->keys[i]) i++;
            if (node->children[i]->keys.size() == DEGREE - 1) {
                splitChild(node, i);
                if (key >= node->keys[i]) i++;
            }
            insertNonFull(node->children[i], key, value);
        }
    }

    void collectAllData(BPlusNode* node, vector<pair<float, int>>& data) {
        if (!node) return;
        if (node->isLeaf) {
            for (int i = 0; i < node->keys.size(); ++i)
                data.push_back({node->keys[i], node->values[i]});
        } else {
            for (auto child : node->children)
                collectAllData(child, data);
        }
    }
};

// 실행 메인 함수
int main(int argc, char* argv[]) {
    string inputFile = "student.txt";
    string queryFile = "query.txt";

    if (argc >= 2) inputFile = argv[1];
    if (argc >= 3) queryFile = argv[2];

    remove("student.dat");
    ifstream in(inputFile);
    if (!in) {
        cerr << "학생 파일 열기 실패: " << inputFile << endl;
        return 1;
    }

    string line;
    while (getline(in, line)) {
        if (line.empty() || line[0] == '#') continue;
        string name; unsigned ID, advisorID; float score;
        if (parseStudentLine(line, name, ID, score, advisorID)) {
            insertStudent(name, ID, score, advisorID);
        }
    }
    in.close();

    // 인덱스 생성
    BPlusTree tree;
    ifstream dat("student.dat", ios::binary);
    dat.seekg(0, ios::end);
    int size = dat.tellg();
    dat.seekg(0);
    int count = size / STUDENT_SIZE;
    for (int i = 0; i < count; i++) {
        Student s;
        dat.seekg(i * STUDENT_SIZE);
        dat.read((char*)&s, STUDENT_SIZE);
        tree.insert(s.score, i * STUDENT_SIZE);
    }
    dat.close();
    tree.saveToFile("student.idx");

    // 쿼리 처리
    ifstream qf(queryFile);
    ofstream out("result.txt");
    float minScore, maxScore;
    while (qf >> minScore >> maxScore) {
        BPlusTree* indexTree = BPlusTree::loadFromFile("student.idx");
        int result = indexTree->searchRangeWithKeys(minScore, maxScore);
        out << result << endl;
        delete indexTree;
    }

    return 0;
}
