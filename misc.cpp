#include <windows.h>
#include <iostream>
#include <string>
#include <chrono>
#include "chess.h"


using namespace std::chrono;

static inline bool starts_with(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && std::equal(prefix.begin(), prefix.end(), str.begin());
}

unsigned long getTimeInMilliseconds() {
    return GetTickCount();
}

unsigned long long getTimeInNanoseconds() {
    return duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count();
}

bool isInputWaiting() {
    static bool initialized = false;
    static HANDLE inputHandle;
    static bool isPipe = false;
    static DWORD consoleMode = 0;

    if (!initialized) {
        initialized = true;
        inputHandle = GetStdHandle(STD_INPUT_HANDLE);
        isPipe = !GetConsoleMode(inputHandle, &consoleMode);
        if (!isPipe) {
            SetConsoleMode(inputHandle, consoleMode & ~(ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT));
            FlushConsoleInputBuffer(inputHandle);
        }
    }

    if (isPipe) {
        DWORD bytesAvailable = 0;
        if (!PeekNamedPipe(inputHandle, nullptr, 0, nullptr, &bytesAvailable, nullptr)) {
            return true; // likely broken pipe, assume quit
        }
        return bytesAvailable > 0;
    } else {
        DWORD events = 0;
        GetNumberOfConsoleInputEvents(inputHandle, &events);
        return events > 1; // 1 is usually a dummy event
    }
}

void readInput(S_SEARCHINFO* info) {
    if (!isInputWaiting()) return;

    info->stopped = true;

    std::string input;
    std::getline(std::cin, input);

    if (!input.empty() && starts_with(input, "quit")) {
        info->quit = true;
    }
}
