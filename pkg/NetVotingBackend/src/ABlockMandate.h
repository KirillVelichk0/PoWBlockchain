#pragma once
#include <cstdint>
#include <memory>
namespace Voting {
class VoteTransaction;
class ABlockMandate {
public:
  ABlockMandate() = default;
  virtual ~ABlockMandate() {}
  virtual std::uint64_t GetCurBlockId() const = 0;
  virtual bool AddTransaction(std::unique_ptr<VoteTransaction> &&vote) = 0;
  virtual bool AddTransaction(const VoteTransaction &vote) = 0;
  virtual bool AddTransaction(VoteTransaction &&vote) = 0;
};
} // namespace Voting
