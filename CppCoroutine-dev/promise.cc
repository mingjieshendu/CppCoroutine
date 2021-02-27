#include <functional>
#include <cstdio>
#include <vector>
#include <queue>
#include <memory>

#include "Coroutine.h"

using norlit::coroutine::Coroutine;

template<typename T>
class Promise {
    enum Status {
        PENDING,
        RESOLVED
    };
  public:
    Status status = PENDING;
    T value;

    std::vector<std::function<void(T)>> success;

    void then(std::function<void(T)> func) {
        switch (status) {
        case PENDING:
            success.push_back(func);
            break;
        case RESOLVED:
            func(value);
            break;
        default:
            break;
        }
    }

    void resolve(T arg) {
        status = RESOLVED;
        value = arg;
        for (auto& func : success) {
            func(arg);
        }
        success.clear();
    }

    T await() {
        Coroutine&& fiber{ Coroutine::current() };
        T ret;
        then([&] (T r) {
            ret = r;
            fiber.resume();
        });
        Coroutine::yield();
        return ret;
    }
};

std::queue<std::function<void()>> eventQueue;

void invokeLater(std::function<void()> func) {
    eventQueue.push(std::move(func));
}

void asyncMain() {
    std::shared_ptr<Promise<int>> p{ new Promise<int>{} };
    invokeLater([=] () {
        p->then([] (int val) {
            printf("This is evaluated last\n");
        });

        printf("This is evaluated second\n");
        p->resolve(1);
    });
    printf("This is evaluated first\n");
    printf("Promise result = %d\n", p->await());
    printf("This is evaluated third\n");
}

int main(void) {
    invokeLater(asyncMain);
    while (!eventQueue.empty()) {
        auto func = eventQueue.back();
        Coroutine{ [func](void*) {
            func();
            return nullptr;
        } } .resume();
        eventQueue.pop();
    }

	system("pause");
    return 0;
}