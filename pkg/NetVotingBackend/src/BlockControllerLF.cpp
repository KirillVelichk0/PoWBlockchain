#include "BlockControllerLF.h"
#include "ATransactionsContainer.h"
#include <atomic>
#include <thread>
// Просто на потестить. Потом уберу
namespace Voting {
void BlockControllerLF::CheckAndProcessNewTransactionsImpl(
    std::shared_ptr<BlockMandateLF> mandate) {
  bool goNext = true;
  auto queue = mandate->GetQueue();
  while (goNext) {
    auto transaction = queue->pop();
    if (transaction == nullptr) {
      goNext = false;
    } else {
      this->transactions->AddTransaction(std::move(transaction));
    }
  }
}
BlockControllerLF::BlockControllerLF(
    std::uint64_t curBlockId,
    const std::vector<std::shared_ptr<VoteTransaction>>
        &notProcessedTransactions)
    : curBlockId(curBlockId), curMandate(BlockMandateLF::Create(curBlockId)),
      transactions(CreateStandartTransactionContainer()) {
  for (auto &transaction : notProcessedTransactions) {
    this->transactions->AddTransaction(transaction);
  }
}
std::unique_ptr<BlockControllerLF>
BlockControllerLF::Create(std::uint64_t curBlockId,
                          const std::vector<std::shared_ptr<VoteTransaction>>
                              &notProcessedTransactions) {
  return std::unique_ptr<BlockControllerLF>(
      new BlockControllerLF(curBlockId, notProcessedTransactions));
}
std::shared_ptr<ABlockMandate> BlockControllerLF::GetCurBlockMandate() const {
  std::shared_ptr<BlockMandateLF> safeCopy;
  safeCopy = std::atomic_load_explicit(&safeCopy, std::memory_order_release);
  return std::dynamic_pointer_cast<ABlockMandate>(safeCopy);
}
void BlockControllerLF::ProcessBlockCreationEvent() {
  this->curBlockId++;
  std::weak_ptr<BlockMandateLF> weakOldMandate;
  {
    std::shared_ptr<BlockMandateLF> safeCopy;
    // переменная меняется только в этом потоке. По идее, можно считывать
    // неатомарно, но не будем так делать
    safeCopy = std::atomic_load_explicit(&safeCopy, std::memory_order_relaxed);
    weakOldMandate = safeCopy;
  }

  std::shared_ptr<BlockMandateLF> newMandate =
      BlockMandateLF::Create(this->curBlockId);
  std::atomic_store_explicit(&this->curMandate, newMandate,
                             std::memory_order_acquire);
  while (true) {
    auto oldMandate = weakOldMandate.lock();
    if (oldMandate == nullptr) {
      break;
    } else {
      this->CheckAndProcessNewTransactionsImpl(oldMandate);
      std::this_thread::yield();
    }
  }
}
void BlockControllerLF::CheckAndProcessNewTransactions() {
  std::shared_ptr<BlockMandateLF> safeCopy;
  safeCopy = std::atomic_load_explicit(&safeCopy, std::memory_order_release);
  this->CheckAndProcessNewTransactionsImpl(safeCopy);
}

} // namespace Voting
