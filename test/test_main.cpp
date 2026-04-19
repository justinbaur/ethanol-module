#include <unity.h>
#include "../src/include/ethanol_math.h"

void setUp(void) {}
void tearDown(void) {}

void test_zero_hz_returns_zero(void) {
  TEST_ASSERT_EQUAL(0, hz_to_ethanol(0));
}

void test_below_baseline_returns_zero(void) {
  TEST_ASSERT_EQUAL(0, hz_to_ethanol(49));
}

void test_at_baseline_returns_zero(void) {
  TEST_ASSERT_EQUAL(0, hz_to_ethanol(50));
}

void test_midpoint_returns_fifty(void) {
  TEST_ASSERT_EQUAL(50, hz_to_ethanol(100));
}

void test_at_max_returns_hundred(void) {
  TEST_ASSERT_EQUAL(100, hz_to_ethanol(150));
}

void test_above_max_clamps_to_hundred(void) {
  TEST_ASSERT_EQUAL(100, hz_to_ethanol(200));
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_zero_hz_returns_zero);
  RUN_TEST(test_below_baseline_returns_zero);
  RUN_TEST(test_at_baseline_returns_zero);
  RUN_TEST(test_midpoint_returns_fifty);
  RUN_TEST(test_at_max_returns_hundred);
  RUN_TEST(test_above_max_clamps_to_hundred);
  return UNITY_END();
}
