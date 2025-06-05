#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <map>
using namespace std;

const int DEGREE = 3;  // B+Tree 차수 (최대 키 수 = DEGREE - 1)

#pragma pack(push, 1)
struct Student {
    unsigned ID;
    char name[20];      // null-terminated 아님
    float score;
    unsigned advisorID;
};
#pragma pack(pop)

struct BPlusNode {
    bool isLeaf;
    vector<float> keys;
    vector<BPlusNode*> children;     // Internal node용
    vector<int> values;              // Leaf node용 (레코드 위치 등)
    BPlusNode* next;                 // Leaf 노드 전용 (순차접근)

    BPlusNode(bool leaf) : isLeaf(leaf), next(nullptr) {}
};

class BPlusTree {
    BPlusNode* root;

public:
    BPlusTree() {
        root = new BPlusNode(true);
    }

    void insert(float key, int value) {
        if (root->keys.size() == DEGREE - 1) {
            BPlusNode* newRoot = new BPlusNode(false);
            newRoot->children.push_back(root);
            splitChild(newRoot, 0);
            root = newRoot;
        }
        insertNonFull(root, key, value);
    }

    int searchRangeWithKeys(float minKey, float maxKey) {
        int result = 0;
        BPlusNode* node = root;

        // 루트가 null인지 확인
        if (!node) return 0;

        // 첫 번째 리프 노드 찾기
        while (!node->isLeaf) {
            int i = 0;
            while (i < node->keys.size() && minKey >= node->keys[i]) i++;
            if (i >= node->children.size()) {
                return 0; // 안전장치
            }
            node = node->children[i];
            if (!node) return 0; // null 체크
        }

        // 리프 노드들을 순회하며 범위 내 키 카운트
        while (node) {
            for (int i = 0; i < node->keys.size(); ++i) {
                if (node->keys[i] > maxKey)
                    return result;
                if (node->keys[i] >= minKey)
                    result++;
            }
            node = node->next;
        }
        return result;
    }

    // 트리를 파일로 저장 (단순화된 버전)
    void saveToFile(const string& filename) {
        ofstream fout(filename, ios::binary);
        if (!fout.is_open()) {
            cerr << "인덱스 파일을 생성할 수 없습니다: " << filename << endl;
            return;
        }

        // 단순히 모든 리프 노드의 키-값 쌍만 저장
        vector<pair<float, int>> allData;
        collectAllData(root, allData);

        // 데이터 개수 저장
        int dataCount = allData.size();
        fout.write(reinterpret_cast<const char*>(&dataCount), sizeof(int));

        // 모든 키-값 쌍 저장
        for (const auto& [key, value] : allData) {
            fout.write(reinterpret_cast<const char*>(&key), sizeof(float));
            fout.write(reinterpret_cast<const char*>(&value), sizeof(int));
        }

        fout.close();
        cout << "인덱스 파일이 성공적으로 저장되었습니다: " << filename << endl;
    }

    // 파일에서 트리 로드 (단순화된 버전)
    static BPlusTree* loadFromFile(const string& filename) {
        ifstream fin(filename, ios::binary);
        if (!fin.is_open()) {
            cerr << "인덱스 파일을 열 수 없습니다: " << filename << endl;
            return nullptr;
        }

        BPlusTree* tree = new BPlusTree();

        // 데이터 개수 읽기
        int dataCount;
        fin.read(reinterpret_cast<char*>(&dataCount), sizeof(int));

        if (dataCount <= 0) {
            cerr << "잘못된 데이터 개수: " << dataCount << endl;
            delete tree;
            fin.close();
            return nullptr;
        }

        // 모든 키-값 쌍 읽어서 새 트리에 삽입
        for (int i = 0; i < dataCount; i++) {
            float key;
            int value;
            fin.read(reinterpret_cast<char*>(&key), sizeof(float));
            fin.read(reinterpret_cast<char*>(&value), sizeof(int));

            if (fin.fail()) {
                cerr << "파일 읽기 오류 발생" << endl;
                delete tree;
                fin.close();
                return nullptr;
            }

            tree->insert(key, value);
        }

        fin.close();
        return tree;
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
                if (key >= node->keys[i])
                    i++;
            }
            insertNonFull(node->children[i], key, value);
        }
    }

    // 모든 리프 노드의 데이터를 수집
    void collectAllData(BPlusNode* node, vector<pair<float, int>>& data) {
        if (!node) return;

        if (node->isLeaf) {
            for (int i = 0; i < node->keys.size(); i++) {
                data.push_back({node->keys[i], node->values[i]});
            }
        } else {
            for (BPlusNode* child : node->children) {
                collectAllData(child, data);
            }
        }
    }
};

// 인덱스 파일에서 범위 검색
int rangeSearchFromIndexFile(const string& idxFilename, float minScore, float maxScore) {
    BPlusTree* tree = BPlusTree::loadFromFile(idxFilename);
    if (!tree) {
        cerr << "Failed to load B+ tree from " << idxFilename << endl;
        return 0;
    }

    int count = tree->searchRangeWithKeys(minScore, maxScore);
    
    delete tree;
    return count;
}

int main(int argc, char* argv[]) {
    // 명령행 인수 확인
    if (argc != 2) {
        cerr << "사용법: " << argv[0] << " <query.txt>" << endl;
        return 1;
    }

    string queryFile = argv[1];
    
    BPlusTree tree;
    // 파일 열기
    ifstream fin("student.dat", ios::binary);
    if (!fin.is_open()) {
        cerr << "student.dat 파일을 열 수 없습니다." << endl;
        return 1;
    }

    fin.seekg(0, ios::end);
    streampos fileSize = fin.tellg();
    fin.seekg(0, ios::beg);

    const size_t recordSize = sizeof(Student);
    size_t recordCount = fileSize / recordSize;

    cout << "총 " << recordCount << "개의 학생 레코드를 처리합니다." << endl;

    for (size_t i = 0; i < recordCount; i++) {
        streampos pos = i * recordSize;

        Student s;
        fin.seekg(pos);
        fin.read(reinterpret_cast<char*>(&s), recordSize);

        if (fin.fail()) {
            cerr << "레코드 " << i << " 읽기 실패" << endl;
            break;
        }

        tree.insert(s.score, static_cast<int>(pos));
    }
    fin.close();

    // 생성된 B+ 트리를 student.idx 파일로 저장
    tree.saveToFile("student.idx");

    // query.txt 파일에서 min, max score 읽기
    ifstream queryIn(queryFile);
    if (!queryIn.is_open()) {
        cerr << queryFile << " 파일을 열 수 없습니다." << endl;
        return 1;
    }

    float minScore, maxScore;
    if (!(queryIn >> minScore >> maxScore)) {
        cerr << queryFile << " 파일에서 점수 범위를 읽을 수 없습니다." << endl;
        cerr << "파일 형식: <최소점수> <최대점수>" << endl;
        queryIn.close();
        return 1;
    }
    queryIn.close();

    // 범위 검색 테스트
    string indexFile = "student.idx";

    cout << "점수 범위 " << minScore << " ~ " << maxScore 
         << " 사이의 학생을 검색합니다..." << endl;

    int count = rangeSearchFromIndexFile(indexFile, minScore, maxScore);
    cout << "조건에 맞는 학생 수: " << count << "명" << endl;

    return 0;
}