#include <iostream>

#include "../src/Log.h"

int main() {
    INIT_LOG;
    ADD_CONSOLE_LOG;
    ADD_FILE_LOG("log.txt");
    ADD_COMMON_LOG_LEVELS;
    SEVERITY_FILTER_TRACE;
    LOG(1) << "TEST LOG 1";
    LOG(0) << "TEST LOG 2";
    LOG("periodic") << "TEST LOG 3";
    LOG("aaa") << "TEST LOG 4";
    LOG_ERROR << "TEST LOG 5";
    return 0;
}
