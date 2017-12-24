#pragma once

#include <Windows.h>
#include <stdio.h>

template <typename ...ArgTypes>
bool Message(char *message, ArgTypes... args) {
    static char text[512];
    sprintf_s(text, message, args...);
    MessageBoxA(NULL, text, "Message", 0);
    return false;
}

template <typename ...ArgTypes>
bool Message(char const *message, ArgTypes... args) {
    static char text[512];
    sprintf_s(text, message, args...);
    MessageBoxA(NULL, text, "Message", 0);
    return false;
}

bool inline Message(char *message) {
    MessageBoxA(NULL, message, "Message", 0);
    return true;
}

template <typename ...ArgTypes>
int ErrorCode(int code, char const *message, ArgTypes... args) {
    Message(message, args...);
    return code;
}

int ErrorCode(int code, char const *message) {
    Message(message);
    return code;
}