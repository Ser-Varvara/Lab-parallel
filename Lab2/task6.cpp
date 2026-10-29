
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <deque>
#include <vector>
#include <string>
#include <utility>

using namespace std;

deque<packaged_task<unsigned int(int)>> tasks;
deque<int> indices;

mutex mtx;
condition_variable cv;
bool finished = false;

unsigned int nthPrime(int n) {
    unsigned int number = 1;
    int found = 0;

    while (found < n) {
        ++number;
        bool prime = true;

        for (unsigned int i = 2; i <= number / i; ++i) {
            if (number % i == 0) {
                prime = false;
                break;
            }
        }

        if (prime) {
            ++found;
        }
    }

    return number;
}

void Worker() {
    while (true) {
        packaged_task<unsigned int(int)> task;
        int n;

        {
            unique_lock<mutex> lock(mtx);

            cv.wait(lock, [] {
                return !tasks.empty() || finished;
            });

            if (tasks.empty() && finished) {
                break;
            }

            task = move(tasks.front());
            tasks.pop_front();

            n = indices.front();
            indices.pop_front();
        }

        task(n);
    }
}

int main() {
    vector<future<unsigned int>> results;
    vector<int> requestedIndices;

    thread worker(Worker);

    cout << "Enter prime indices (q to finish):" << endl;

    string input;

    while (cin >> input && input != "q") {
        int n;

        try {
            size_t pos;
            n = stoi(input, &pos);

            if (pos != input.size() ||
                n <= 0 || n > 1000000) {
                cout << "Invalid index." << endl;
                continue;
            }
        } catch (...) {
            cout << "Invalid input." << endl;
            continue;
        }

        packaged_task<unsigned int(int)> task(nthPrime);
        results.push_back(task.get_future());
        requestedIndices.push_back(n);

        {
            lock_guard<mutex> lock(mtx);
            tasks.push_back(move(task));
            indices.push_back(n);
        }

        cv.notify_one();
    }

    {
        lock_guard<mutex> lock(mtx);
        finished = true;
    }

    cv.notify_one();

    worker.join();

    cout << "Results:" << endl;

    for (size_t i = 0; i < results.size(); ++i) {
        cout << "Prime #" << requestedIndices[i]
             << " = " << results[i].get() << endl;
    }

    cout << "All tasks completed." << endl;

    return 0;
}
