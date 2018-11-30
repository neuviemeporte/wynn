#include "gtest/gtest.h"
#include "ixplog_active.h"

QTSLogger *APP_LOGSTREAM = nullptr;

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    QTSLogger log("wynn_test_log.txt");
    APP_LOGSTREAM = &log;
    return RUN_ALL_TESTS();
}
