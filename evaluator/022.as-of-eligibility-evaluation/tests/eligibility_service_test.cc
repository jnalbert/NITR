#include <cassert>
#include <cstdint>
#include <limits>
#include <vector>

#include "eligibility_service.h"

namespace {

constexpr std::int64_t kFarFutureStart = std::numeric_limits<std::int64_t>::max() - 1;
constexpr std::int64_t kFarFutureEnd = std::numeric_limits<std::int64_t>::max();
constexpr std::int64_t kAlwaysActiveEnd = 4102444800;  // 2100-01-01 00:00:00 UTC.

void TestEnabledRuleInsideWindowIsEligible() {
  const nitr::case022::PromotionRule rule{
      "always-active",
      0,
      kAlwaysActiveEnd,
      true,
  };

  const nitr::case022::EligibilityService service;
  assert(service.IsEligible(rule));
}

void TestDisabledRuleIsIneligible() {
  const nitr::case022::PromotionRule rule{
      "disabled-always-active",
      0,
      kAlwaysActiveEnd,
      false,
  };

  const nitr::case022::EligibilityService service;
  assert(!service.IsEligible(rule));
}

void TestFutureWindowIsNotEligibleYet() {
  const nitr::case022::PromotionRule rule{
      "future-rule",
      kFarFutureStart,
      kFarFutureEnd,
      true,
  };

  const nitr::case022::EligibilityService service;
  assert(!service.IsEligible(rule));
}

void TestExpiredWindowIsNotEligible() {
  const nitr::case022::PromotionRule rule{
      "expired-rule",
      0,
      1,
      true,
  };

  const nitr::case022::EligibilityService service;
  assert(!service.IsEligible(rule));
}

void TestCountEligibleReturnsCorrectCount() {
  const std::vector<nitr::case022::PromotionRule> rules{
      nitr::case022::PromotionRule{"always-active", 0, kAlwaysActiveEnd, true},
      nitr::case022::PromotionRule{"disabled-always-active", 0, kAlwaysActiveEnd, false},
      nitr::case022::PromotionRule{"future-rule", kFarFutureStart, kFarFutureEnd, true},
      nitr::case022::PromotionRule{"expired-rule", 0, 1, true},
  };

  const nitr::case022::EligibilityService service;
  assert(service.CountEligible(rules) == 1U);
}

}  // namespace

int main() {
  TestEnabledRuleInsideWindowIsEligible();
  TestDisabledRuleIsIneligible();
  TestFutureWindowIsNotEligibleYet();
  TestExpiredWindowIsNotEligible();
  TestCountEligibleReturnsCorrectCount();
  return 0;
}
