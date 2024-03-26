#pragma once
#include <memory>
#include <optional>
#include <source_location>
#include <string_view>
namespace BlockChainCore {
class NestedError {
private:
  struct NestedErrorInternal {
    std::string curErrorMsg;
    std::shared_ptr<NestedErrorInternal> nested;
    std::source_location loc;
  };
  std::shared_ptr<NestedErrorInternal> internal;
  NestedError(std::shared_ptr<NestedErrorInternal>);

public:
  [[nodiscard]] std::source_location GetLocation() const noexcept;
  [[nodiscard]] std::string_view CurErrorMsg() const noexcept;
  [[nodiscard]] std::optional<NestedError> GetNested() const noexcept;
  NestedError(std::string_view curErrorMsg, const std::source_location &loc);
  NestedError(std::string_view curErrorMsg, const NestedError &nested,
              const std::source_location &loc);
  [[nodiscard]] std::string GetFullErrorMsg() const noexcept;
};
} // namespace BlockChainCore
