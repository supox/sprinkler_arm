#pragma once

#ifdef _TESTS

void TestHTTPParser();
void TestIrrigationList();
void TestValvesManager();
void TestSensors();
void CheckUART();
void TestGsmStatusParser();
void TestJsonTime();
void TestTimes();
void TestScheduler();
#endif

#ifdef _TEST_GSM_MODEM
void TestGsmModem();
#endif

#ifdef _TEST_RTC
void TestRTC();
#endif

