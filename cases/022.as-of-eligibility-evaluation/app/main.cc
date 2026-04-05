#include <iostream>
#include <vector>

#include "eligibility_service.h"

int main() {
  const nitr::case022::PromotionRule summer_sale{
      "summer-sale",
      0,
      4102444800,
      true,
  };

  const nitr::case022::PromotionRule archived_offer{
      "archived-offer",
      0,
      1,
      true,
  };

  const nitr::case022::EligibilityService service;
  const std::vector<nitr::case022::PromotionRule> rules{
      summer_sale,
      archived_offer,
  };

  std::cout << "eligible rules: " << service.CountEligible(rules) << "\n";
  std::cout << "summer-sale eligible now: "
            << (service.IsEligible(summer_sale) ? "yes" : "no") << "\n";
  std::cout << "archived-offer eligible now: "
            << (service.IsEligible(archived_offer) ? "yes" : "no") << "\n";
  return 0;
}
