#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <chrono>

using namespace std;

queue<int> numbers;
mutex mtx;
bool dataReady = false;

bool isPrime(int n) {
    if (n < 2) {
        return false;
    }

    for (int i = 2; i <= n / i; i++) {
        if (n % i == 0) {
            return false;
        }
    }

    return true;
}

void DataPreparation() {
    int num;

    cout << "Enter integers (0 to finish):" << endl;

    while (cin >> num) {
        if (num == 0) {
            break;
        }

        {
            lock_guard<mutex> lock(mtx);
            numbers.push(num);
        }
    }

    {
        lock_guard<mutex> lock(mtx);
        dataReady = true;
    }

    cout << "Data preparation finished." << endl;
}

void DataProcessing() {
    unique_lock<mutex> lock(mtx);

    while (!dataReady) {
        lock.unlock();

        this_thread::sleep_for(chrono::milliseconds(100));

        lock.lock();
    }

    cout << "Prime numbers: ";

    while (!numbers.empty()) {
        int num = numbers.front();
        numbers.pop();

        if (isPrime(num)) {
            cout << num << " ";
        }
    }

    cout << endl;
}

int main() {
    thread preparation(DataPreparation);
    preparation.detach();

    thread processing(DataProcessing);
    processing.join();

    cout << "Program finished." << endl;

    return 0;
}
