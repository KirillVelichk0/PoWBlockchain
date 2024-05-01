#pragma once
#include "ABlockMandate.h"
namespace Voting {
class AControllChannel {
public:
  AControllChannel() = default;
  virtual ~AControllChannel() {}
  virtual std::shared_ptr<ABlockMandate> GetBlockMandate() = 0;
};
} // namespace Voting
