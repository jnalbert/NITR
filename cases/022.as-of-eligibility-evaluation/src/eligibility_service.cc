#include "eligibility_service.h"

#include <chrono>

namespace nitr::case022 {

namespace {

std::int64_t CurrentUnixTimestamp() {
  const auto now = std::chrono::system_clock::now();
  const auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
  return seconds.time_since_epoch().count();
}

}  // namespace

bool EligibilityService::IsEligible(const PromotionRule& rule) const {
  if (!rule.enabled) {
    return false;
  }

  const std::int64_t now_timestamp = CurrentUnixTimestamp();
  return rule.start_timestamp <= now_timestamp &&
         now_timestamp <= rule.end_timestamp;
}

std::size_t EligibilityService::CountEligible(
    const std::vector<PromotionRule>& rules) const {
  std::size_t eligible_count = 0U;

  for (const PromotionRule& rule : rules) {
    if (IsEligible(rule)) {
      ++eligible_count;
    }
  }

  return eligible_count;
}

}  // namespace nitr::case022
