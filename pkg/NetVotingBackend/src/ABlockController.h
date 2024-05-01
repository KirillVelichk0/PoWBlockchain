#pragma once
#include "ABlockMandate.h"
#include <memory>
namespace Voting {
class ABlockController {
public:
  ABlockController() = default;
  virtual ~ABlockController() {}
  virtual std::shared_ptr<ABlockMandate> GetCurBlockMandate() const = 0;
  virtual void ProcessBlockCreationEvent() = 0;
  virtual void CheckAndProcessNewTransactions() = 0;
};
} // namespace Voting
