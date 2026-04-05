#include <cassert>
#include <cstdint>
#include <type_traits>

#include "eligibility_service.h"

namespace {

template <typename Service, typename = void>
struct SupportsIsEligibleAt : std::false_type {};

template <typename Service>
struct SupportsIsEligibleAt<
    Service,
    std::void_t<decltype(std::declval<const Service&>().IsEligibleAt(
        std::declval<const nitr::case022::PromotionRule&>(),
        std::declval<std::int64_t>()))>> : std::true_type {};

template <typename Service, typename = void>
struct SupportsIsEligibleWithTimestamp : std::false_type {};

template <typename Service>
struct SupportsIsEligibleWithTimestamp<
    Service,
    std::void_t<decltype(std::declval<const Service&>().IsEligible(
        std::declval<const nitr::case022::PromotionRule&>(),
        std::declval<std::int64_t>()))>> : std::true_type {};

template <typename Service>
bool EvaluateAt(const Service& service,
                const nitr::case022::PromotionRule& rule,
                std::int64_t timestamp) {
  if constexpr (SupportsIsEligibleAt<Service>::value) {
    return service.IsEligibleAt(rule, timestamp);
  } else if constexpr (SupportsIsEligibleWithTimestamp<Service>::value) {
    return service.IsEligible(rule, timestamp);
  } else {
    static_assert(SupportsIsEligibleAt<Service>::value ||
                      SupportsIsEligibleWithTimestamp<Service>::value,
                  "EligibilityService must support explicit-time evaluation");
    return false;
  }
}

void TestAsOfEvaluationInsideWindow() {
  const nitr::case022::PromotionRule rule{
      "launch-window",
      100,
      200,
      true,
  };

  const nitr::case022::EligibilityService service;
  assert(EvaluateAt(service, rule, 150));
}

void TestAsOfEvaluationAtInclusiveBoundaries() {
  const nitr::case022::PromotionRule rule{
      "inclusive-window",
      100,
      200,
      true,
  };

  const nitr::case022::EligibilityService service;
  assert(EvaluateAt(service, rule, 100));
  assert(EvaluateAt(service, rule, 200));
}

void TestAsOfEvaluationOutsideWindow() {
  const nitr::case022::PromotionRule rule{
      "outside-window",
      100,
      200,
      true,
  };

  const nitr::case022::EligibilityService service;
  assert(!EvaluateAt(service, rule, 99));
  assert(!EvaluateAt(service, rule, 201));
}

void TestAsOfEvaluationRespectsEnabledFlag() {
  const nitr::case022::PromotionRule rule{
      "disabled-rule",
      100,
      200,
      false,
  };

  const nitr::case022::EligibilityService service;
  assert(!EvaluateAt(service, rule, 150));
}

}  // namespace

int main() {
  TestAsOfEvaluationInsideWindow();
  TestAsOfEvaluationAtInclusiveBoundaries();
  TestAsOfEvaluationOutsideWindow();
  TestAsOfEvaluationRespectsEnabledFlag();
  return 0;
}
