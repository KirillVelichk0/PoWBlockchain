#include "ABlockController.h"
#include "ContolChannelSimple.h"
namespace Voting {
ControllChannelSimple::~ControllChannelSimple() {}
ControllChannelSimple::ControllChannelSimple(
    std::shared_ptr<ABlockController> controller)
    : blockController(controller) {}
std::shared_ptr<ABlockMandate> ControllChannelSimple::GetBlockMandate() {
  return this->blockController->GetCurBlockMandate();
}
std::shared_ptr<ControllChannelSimple>
ControllChannelSimple::Create(std::shared_ptr<ABlockController> controller) {
  std::shared_ptr<ControllChannelSimple> result;
  if (controller != nullptr) {
    result = std::shared_ptr<ControllChannelSimple>(
        new ControllChannelSimple(controller));
  }
  return result;
}
} // namespace Voting
