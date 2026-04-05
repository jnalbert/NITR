#pragma once

#include <cstdint>
#include <string>

namespace nitr::case022 {

struct PromotionRule {
  std::string rule_id;
  std::int64_t start_timestamp;
  std::int64_t end_timestamp;
  bool enabled;  // False means the rule is deactivated and always ineligible.
};

}  // namespace nitr::case022
