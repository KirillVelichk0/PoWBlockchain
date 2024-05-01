#pragma once
#include "ABlockMandate.h"
namespace Voting {
class AControllChannel {
public:
  virtual ~AControllChannel() = 0;
  virtual std::shared_ptr<ABlockMandate> GetBlockMandate() = 0;
};
} // namespace Voting
