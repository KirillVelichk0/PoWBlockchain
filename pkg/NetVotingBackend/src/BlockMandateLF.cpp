#include "BlockMandateLF.h"
namespace Voting {
BlockMandateLF::BlockMandateLF(std::uint64_t curBlockId)
    : curBlockId(curBlockId), queue(TransactionQueue::Create()) {}
std::unique_ptr<BlockMandateLF>
BlockMandateLF::Create(std::uint64_t curBlockId) {
  return std::unique_ptr<BlockMandateLF>(new BlockMandateLF(curBlockId));
}

std::uint64_t BlockMandateLF::GetCurBlockId() const { return this->curBlockId; }
bool BlockMandateLF::AddTransaction(std::unique_ptr<VoteTransaction> &&vote) {
  return this->queue->push(std::move(vote));
}
bool BlockMandateLF::AddTransaction(const VoteTransaction &vote) {
  std::unique_ptr<VoteTransaction> transactionCopy(new VoteTransaction(vote));
  return this->AddTransaction(std::move(transactionCopy));
}
bool BlockMandateLF::AddTransaction(VoteTransaction &&vote) {
  std::unique_ptr<VoteTransaction> transactionMoved(
      new VoteTransaction(std::move(vote)));
  return this->AddTransaction(std::move(transactionMoved));
}
std::shared_ptr<TransactionQueue> BlockMandateLF::GetQueue() {
  return this->queue;
}
} // namespace Voting
