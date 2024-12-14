//
// Created by root on 24-12-6.
//
#include <iostream>
#include <cstring>
#include <ctime>
#include <random>
#include <algorithm>

using namespace std;

int n_num = 1000000000;

int main() {
    srand((unsigned int) time(NULL));

    int *data1 = new int[n_num];
    memset(data1, 0, sizeof(int));

    clock_t start1 = clock();
    for (int i = 0; i < n_num; i++) {
        data1[i]++;
    }
    clock_t end1 = clock();
    double duration1 = double(end1 - start1) / CLOCKS_PER_SEC;
    cout << "duration1 = " << duration1 << endl;

    delete[] data1;
    data1 = NULL;

    // 创建随机数生成器
    vector<int> v1;
    for (int i = 0; i < n_num; i++) {
        v1.push_back(i);
    }
    random_shuffle(v1.begin(), v1.end());

    int *data2 = new int[n_num];
    int *index2 = new int[n_num];
    memset(data2, 0, sizeof(int));
    memset(index2, 0, sizeof(int));

    for (int i = 0; i < n_num; i++) {
        index2[i] = v1[i];
    }

    clock_t start2 = clock();
    for (int i = 0; i < n_num; i++) {
        data2[index2[i]]++;
    }
    clock_t end2 = clock();
    double duration2 = double(end2 - start2) / CLOCKS_PER_SEC;
    cout << "duration2 = " << duration2 << endl;

    delete[] data2;
    delete[] index2;
    data2 = NULL;
    index2 = NULL;

    return 0;
}
