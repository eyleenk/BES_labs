#include "unity.h"
#include "sequence.h"

#define RED   0x02   // PF1
#define GREEN 0x08   // PF3

void setUp(void) {}
void tearDown(void) {}

void testSetRed(void) {
    TEST_ASSERT_BITS_HIGH_MESSAGE(RED, SetRed(0x00), "Red LED should be on!");
    TEST_ASSERT_EQUAL_HEX32(0xFD | RED, SetRed(0xFD));
}

void testClearRed(void) {
    TEST_ASSERT_BITS_LOW_MESSAGE(RED, ClearRed(0xFF), "Red LED should be off!");
    TEST_ASSERT_EQUAL_HEX32(0xFF & ~RED, ClearRed(0xFF));
}

void testSetGreen(void) {
    TEST_ASSERT_BITS_HIGH_MESSAGE(GREEN, SetGreen(0x00), "Green LED should be on!");
    TEST_ASSERT_EQUAL_HEX32(0xF7 | GREEN, SetGreen(0xF7));
}

void testClearGreen(void) {
    TEST_ASSERT_BITS_LOW_MESSAGE(GREEN, ClearGreen(0xFF), "Green LED should be off!");
    TEST_ASSERT_EQUAL_HEX32(0xFF & ~GREEN, ClearGreen(0xFF));
}

void testSetYellow(void) {
    uint32_t v = SetGreen(SetRed(0x00));
    TEST_ASSERT_BITS_HIGH_MESSAGE(RED | GREEN, v, "Yellow (Green and Red) LED should be on!");
}

void testClearYellow(void) {
    uint32_t v = ClearGreen(ClearRed(0xFF));
    TEST_ASSERT_BITS_LOW_MESSAGE(RED | GREEN, v, "Yellow (Green and Red) LED should be off!");
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testSetRed);
    RUN_TEST(testClearRed);
    RUN_TEST(testSetGreen);
    RUN_TEST(testClearGreen);
    RUN_TEST(testSetYellow);
    RUN_TEST(testClearYellow);
    return UNITY_END();
}
