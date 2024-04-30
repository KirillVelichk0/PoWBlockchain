#pragma once
#include "Transaction.h"
#include <cstdint>
namespace Voting {
class ABlockMandate {
public:
  virtual ~ABlockMandate() = 0;
  virtual std::uint64_t GetCurBlockId() const;
  virtual bool AddTransaction(std::unique_ptr<VoteTransaction> &&vote) = 0;
  virtual bool AddTransaction(const VoteTransaction &vote) = 0;
  virtual bool AddTransaction(VoteTransaction &&vote) = 0;
};
} // namespace Voting
