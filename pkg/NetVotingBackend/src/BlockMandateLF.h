#pragma once
#include "ABlockMandate.h"
namespace Voting {
class TransactionQueue;
class BlockMandateLF : public ABlockMandate {
private:
  std::uint64_t curBlockId;
  const std::shared_ptr<TransactionQueue> queue;
  BlockMandateLF(std::uint64_t curBlockId);

public:
  static std::unique_ptr<BlockMandateLF> Create(std::uint64_t curBlockId);
  ~BlockMandateLF();
  BlockMandateLF(const BlockMandateLF &) = delete;
  BlockMandateLF(BlockMandateLF &&) = delete;
  BlockMandateLF &operator=(const BlockMandateLF &) = delete;
  BlockMandateLF &operator=(BlockMandateLF &&) = delete;
  std::uint64_t GetCurBlockId() const override;
  bool AddTransaction(std::unique_ptr<VoteTransaction> &&vote) override;
  bool AddTransaction(const VoteTransaction &vote) override;
  bool AddTransaction(VoteTransaction &&vote) override;
  // thread safe because queue is const
  std::shared_ptr<TransactionQueue> GetQueue();
};

} // namespace Voting
