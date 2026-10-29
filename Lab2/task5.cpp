
#include <iostream>
#include <future>
#include <chrono>
#include <cmath>
#include <vector>
#include <limits>

using namespace std;

unsigned int nthPrime(int n) {
    if (n < 1) {
        return 0;
    }

    vector<bool> primes;

    int found = 0;
    unsigned int number = 1;

    while (found < n) {
        ++number;

        bool isPrime = true;

        for (unsigned int i = 2; i <= number / i; ++i) {
            if (number % i == 0) {
                isPrime = false;
                break;
            }
        }

        if (isPrime) {
            ++found;
        }
    }

    return number;
}

int main() {
    int n, mode, choice;

    cout << "Enter the index of prime number: ";
    cin >> n;

    if (!cin || n <= 0 || n > 1000000) {
        cout << "Invalid index." << endl;
        return 1;
    }

    cout << "Select launch mode:" << endl;
    cout << "1 - deferred" << endl;
    cout << "2 - async" << endl;
    cin >> mode;

    if (!cin || (mode != 1 && mode != 2)) {
        cout << "Invalid mode." << endl;
        return 1;
    }

    auto policy = mode == 1
        ? launch::deferred
        : launch::async;

    auto start = chrono::steady_clock::now();

    future<unsigned int> result =
        async(policy, nthPrime, n);

    cout << "Choose a mathematical function:" << endl;
    cout << "1 - square root" << endl;
    cout << "2 - sine" << endl;
    cout << "3 - natural logarithm" << endl;
    cin >> choice;

    double value = static_cast<double>(n);

    if (choice == 1) {
        cout << "sqrt(" << n << ") = "
             << sqrt(value) << endl;
    } else if (choice == 2) {
        cout << "sin(" << n << ") = "
             << sin(value) << endl;
    } else if (choice == 3) {
        cout << "ln(" << n << ") = "
             << log(value) << endl;
    } else {
        cout << "Invalid function." << endl;
        return 1;
    }

    auto waitStart = chrono::steady_clock::now();

    unsigned int prime = result.get();

    auto finish = chrono::steady_clock::now();

    auto waitingTime =
        chrono::duration_cast<chrono::milliseconds>(
            finish - waitStart
        ).count();

    auto totalTime =
        chrono::duration_cast<chrono::milliseconds>(
            finish - start
        ).count();

    cout << "Prime number #" << n
         << " = " << prime << endl;

    cout << "Waiting time: "
         << waitingTime << " ms" << endl;

    cout << "Total time: "
         << totalTime << " ms" << endl;

    return 0;
}