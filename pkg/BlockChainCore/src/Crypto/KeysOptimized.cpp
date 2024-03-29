#include "Crypto.h"
namespace BlockChainCore {
PublicKeyOptimizedImpl::PublicKeyOptimizedImpl(
    const std::shared_ptr<void> &impl)
    : impl(impl) {}
PublicKeyOptimizedImpl::PublicKeyOptimizedImpl(std::shared_ptr<void> &&impl)
    : impl(std::move(impl)) {}

PublicKeyOptimizedImpl::PublicKeyOptimizedImpl(
    const PublicKeyOptimizedImpl &key) noexcept
    : impl(key.impl) {}
PublicKeyOptimizedImpl &
PublicKeyOptimizedImpl::operator=(const PublicKeyOptimizedImpl &impl) noexcept {
  this->impl = impl.impl;
  return *this;
}
PublicKeyOptimizedImpl::PublicKeyOptimizedImpl(
    PublicKeyOptimizedImpl &&key) noexcept
    : impl(key.impl) {}
PublicKeyOptimizedImpl &
PublicKeyOptimizedImpl::operator=(PublicKeyOptimizedImpl &&key) noexcept {
  this->impl = std::move(key.impl);
  return *this;
}
[[nodiscard]] std::shared_ptr<void>
PublicKeyOptimizedImpl::GetImpl() const noexcept {
  return this->impl;
}
PrivateKeyOptimizedImpl::PrivateKeyOptimizedImpl(
    const std::shared_ptr<void> &impl)
    : impl(impl) {}
PrivateKeyOptimizedImpl::PrivateKeyOptimizedImpl(std::shared_ptr<void> &&impl)
    : impl(std::move(impl)) {}

PrivateKeyOptimizedImpl::PrivateKeyOptimizedImpl(
    const PrivateKeyOptimizedImpl &key) noexcept
    : impl(key.impl) {}
PrivateKeyOptimizedImpl &PrivateKeyOptimizedImpl::operator=(
    const PrivateKeyOptimizedImpl &key) noexcept {
  this->impl = key.impl;
  return *this;
}
PrivateKeyOptimizedImpl::PrivateKeyOptimizedImpl(
    PrivateKeyOptimizedImpl &&key) noexcept
    : impl(std::move(key.impl)) {}
PrivateKeyOptimizedImpl &
PrivateKeyOptimizedImpl::operator=(PrivateKeyOptimizedImpl &&key) noexcept {
  this->impl = std::move(key.impl);
  return *this;
}
[[nodiscard]] std::shared_ptr<void>
PrivateKeyOptimizedImpl::GetImpl() const noexcept {
  return this->impl;
}
} // namespace BlockChainCore
