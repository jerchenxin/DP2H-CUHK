//
// Created by ChenXin on 2021/3/24.
//

#include "timer.h"

void cx::Timer::StartTimer(std::string eventName) {
    eventMap[eventName] = std::chrono::high_resolution_clock::now();
}

unsigned long long cx::Timer::EndTimer(std::string eventName) {
    if (eventMap.find(eventName) == eventMap.end()) {
        printf("event not exists\n");
        exit(88);
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - eventMap[eventName]);
    return diff.count();
}

void cx::Timer::EndTimerAndPrint(std::string eventName) {
    if (eventMap.find(eventName) == eventMap.end()) {
        printf("event not exists\n");
        exit(88);
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - eventMap[eventName]);
    std::cout << std::endl << "#### " << eventName << " : " << diff.count() * 1.0 / 1e3 << " seconds ####" << std:: endl;
    std::cout << "#### " << eventName << " : " << diff.count() << " milliseconds ####" << std:: endl << std::endl;
}

