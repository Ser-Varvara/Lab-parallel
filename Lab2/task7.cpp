
#include <iostream>
#include <thread>
#include <future>
#include <chrono>
#include <cmath>
#include <utility>

using namespace std;

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

void FirstThread(
    int n,
    promise<unsigned int> firstPromise,
    promise<unsigned int> secondPromise,
    promise<bool> signalPromise
) {
    unsigned int first = nthPrime(n);
    firstPromise.set_value(first);

    signalPromise.set_value(true);

    unsigned int second = nthPrime(n * 10);
    secondPromise.set_value(second);
}

void SecondThread(int n, future<bool> signal) {
    bool ready = signal.get();

    if (ready) {
        this_thread::sleep_for(chrono::seconds(2));

        cout << "Square root of " << n
             << " = " << sqrt(static_cast<double>(n))
             << endl;
    }
}

int main() {
    int n;

    cout << "Enter prime index n: ";
    cin >> n;

    if (!cin || n <= 0 || n > 100000) {
        cout << "Invalid index." << endl;
        return 1;
    }

    promise<unsigned int> firstPromise;
    promise<unsigned int> secondPromise;
    promise<bool> signalPromise;

    future<unsigned int> firstFuture =
        firstPromise.get_future();

    future<unsigned int> secondFuture =
        secondPromise.get_future();

    future<bool> signalFuture =
        signalPromise.get_future();

    thread t1(
        FirstThread,
        n,
        move(firstPromise),
        move(secondPromise),
        move(signalPromise)
    );

    thread t2(
        SecondThread,
        n,
        move(signalFuture)
    );

    cout << "Prime #" << n
         << " = " << firstFuture.get() << endl;

    cout << "Prime #" << n * 10
         << " = " << secondFuture.get() << endl;

    t1.join();
    t2.join();

    cout << "All threads finished." << endl;

    return 0;
}
