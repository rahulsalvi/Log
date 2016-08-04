#include <iostream>

#include "../src/Log.h"

int main() {
    INIT_LOG;
    ADD_COMMON_LOG_LEVELS;
    ADD_LOG_LEVEL(7, "L7")
    ADD_FILTERED_CONSOLE_LOG(SEVERITY == LEVEL(ERROR) || SEVERITY == LEVEL(INFO));
    ADD_FILE_LOG("log.txt");
    ADD_FILTERED_FILE_LOG("log2.txt", SEVERITY == LEVEL(TRACE));
    ADD_FILTERED_FILE_LOG("log3.txt", SEVERITY == LEVEL("L7"));
    ADD_FILTERED_FILE_LOG("log4.txt", SEVERITY == LEVEL(7) || SEVERITY == LEVEL(INFO));
    GLOBAL_FILTER(SEVERITY >= LEVEL(TRACE));
    LOG(1) << "TEST LOG 1";
    LOG(0) << "TEST LOG 2";
    LOG("periodic") << "TEST LOG 3";
    LOG("aaa") << "TEST LOG 4";
    LOG(ERROR) << "TEST LOG 5";
    LOG("L7") << "TEST LOG 6";
    LOG(7) << "TEST LOG 7";
    RESET_GLOBAL_FILTER;
    LOG(0) << "TEST LOG 8";
    LOG(INFO) << "TEST LOG 9";
    return 0;
}
