#include "nested_error.h"
#include <fmt/core.h>
#include <fmt/format.h>
#include <source_location>
namespace BlockChainCore {
std::string_view NestedError::CurErrorMsg() const noexcept {
  return this->internal->curErrorMsg;
}
std::source_location NestedError::GetLocation() const noexcept {
  return this->internal->loc;
}
std::optional<NestedError> NestedError::GetNested() const noexcept {
  if (this->internal->nested == nullptr) {
    return {};
  }
  return NestedError(this->internal->nested);
}
NestedError::NestedError(std::string_view curErrorMsg,
                         const std::source_location &loc) {
  this->internal =
      std::shared_ptr<NestedErrorInternal>(new NestedErrorInternal);
  this->internal->curErrorMsg = std::string(curErrorMsg);
  this->internal->loc = loc;
}
NestedError::NestedError(std::string_view curErrorMsg,
                         const NestedError &nested,
                         const std::source_location &loc) {
  this->internal =
      std::shared_ptr<NestedErrorInternal>(new NestedErrorInternal);
  this->internal->nested = nested.internal;
  this->internal->curErrorMsg = std::string(curErrorMsg);
  this->internal->loc = loc;
}
std::string NestedError::GetFullErrorMsg() const noexcept {
  auto curInternal = this->internal;
  std::string result;
  int counter = 0;
  while (curInternal != nullptr) {
    result = fmt::format(
        "{0} {1}({2}:{3}) `{4}` : {5}\n", counter, curInternal->loc.file_name(),
        curInternal->loc.line(), curInternal->loc.column(),
        curInternal->loc.function_name(), curInternal->curErrorMsg);
    counter++;
    curInternal = curInternal->nested;
  }
  return result;
}
NestedError::NestedError(std::shared_ptr<NestedErrorInternal> nested)
    : internal(std::move(nested)) {}
} // namespace BlockChainCore
