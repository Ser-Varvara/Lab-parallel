
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

using namespace std;

mutex mtx;
condition_variable cv;

int i = 0;
bool stopWaiting = false;

void Waits(int id) {
    unique_lock<mutex> lock(mtx);

    cout << "Thread " << id << " is waiting..." << endl;

    cv.wait(lock, [] {
        return i == 1 || stopWaiting;
    });

    if (!stopWaiting) {
        cout << "Message from Thread " << id << endl;
    }
}

void Notify() {
    this_thread::sleep_for(chrono::milliseconds(100));

    {
        lock_guard<mutex> lock(mtx);
        i = 1;
        cout << "Notify: i = 1, waking one thread." << endl;
    }

    cv.notify_one();
}

int main() {
    thread t1(Waits, 1);
    thread t2(Waits, 2);
    thread t3(Waits, 3);

    thread notifyThread(Notify);

    notifyThread.join();

    this_thread::sleep_for(chrono::milliseconds(200));

    {
        lock_guard<mutex> lock(mtx);
        stopWaiting = true;
    }

    cv.notify_all();

    t1.join();
    t2.join();
    t3.join();

    cout << "All threads finished." << endl;

    return 0;
}
