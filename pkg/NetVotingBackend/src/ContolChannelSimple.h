#pragma once
#include "AControlChannel.h"
namespace Voting {
class ABlockController;
class ControllChannelSimple : public AControllChannel {
private:
  std::shared_ptr<ABlockController> blockController;

private:
  ControllChannelSimple(std::shared_ptr<ABlockController> controller);

public:
  std::shared_ptr<ABlockMandate> GetBlockMandate() override;
  ~ControllChannelSimple();
  static std::shared_ptr<ControllChannelSimple>
  Create(std::shared_ptr<ABlockController> controller);

public:
  ControllChannelSimple(const ControllChannelSimple &) = delete;
  ControllChannelSimple(ControllChannelSimple &&) = delete;
  ControllChannelSimple &operator=(const ControllChannelSimple &) = delete;
  ControllChannelSimple &operator=(ControllChannelSimple &&) = delete;
};
} // namespace Voting
