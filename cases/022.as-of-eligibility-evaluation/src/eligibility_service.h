#pragma once

#include <cstddef>
#include <vector>

#include "promotion_rule.h"

namespace nitr::case022 {

class EligibilityService {
 public:
  bool IsEligible(const PromotionRule& rule) const;
  std::size_t CountEligible(const std::vector<PromotionRule>& rules) const;
};

}  // namespace nitr::case022
