#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

using namespace std;

mutex mtx;
condition_variable cv;
int i = 0;

void Waits(int id) {
    unique_lock<mutex> lock(mtx);

    cout << "Thread " << id << " is waiting..." << endl;

    cv.wait(lock, [] {
        return i == 1;
    });

    cout << "Thread " << id << " finished waiting." << endl;
}

void Awake() {
    this_thread::sleep_for(chrono::milliseconds(100));

    cout << "First notification (i = 0)." << endl;
    cv.notify_all();

    this_thread::sleep_for(chrono::milliseconds(100));

    {
        lock_guard<mutex> lock(mtx);
        i = 1;
        cout << "Second notification (i = 1)." << endl;
    }

    cv.notify_all();
}

int main() {
    thread t1(Waits, 1);
    thread t2(Waits, 2);
    thread t3(Waits, 3);

    thread awakeThread(Awake);

    awakeThread.join();

    t1.join();
    t2.join();
    t3.join();

    cout << "All threads finished." << endl;

    return 0;
}
