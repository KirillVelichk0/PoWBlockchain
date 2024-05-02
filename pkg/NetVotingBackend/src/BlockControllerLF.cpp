#include "BlockControllerLF.h"
#include "ATransactionsContainer.h"
#include "BlockMandateLF.h"
#include "Transaction.h"
#include <atomic>
#include <drogon/drogon.h>
#include <thread>
namespace Voting {
BlockControllerLF::~BlockControllerLF() {}
void BlockControllerLF::CheckAndProcessNewTransactionsImpl(
    std::shared_ptr<TransactionQueue> queue) {
  bool goNext = true;
  while (goNext) {
    LOG_DEBUG << "Trying Pop in transaction process";
    auto transaction = queue->pop();
    if (transaction == nullptr) {
      LOG_DEBUG << "UnsuccessTry in transaction process";
      goNext = false;
    } else {
      LOG_DEBUG << "SuccessTry in transaction process";
      this->transactions->AddTransaction(std::move(transaction));
    }
  }
}
BlockControllerLF::BlockControllerLF(
    std::uint64_t curBlockId,
    const std::vector<std::shared_ptr<VoteTransaction>>
        &notProcessedTransactions,
    std::function<void(std::string &&)> &callback)
    : curBlockId(curBlockId), curMandate(BlockMandateLF::Create(curBlockId)),
      transactions(CreateStandartTransactionContainer()), callback(callback) {
  for (auto &transaction : notProcessedTransactions) {
    this->transactions->AddTransaction(transaction);
  }
}
std::unique_ptr<BlockControllerLF>
BlockControllerLF::Create(std::uint64_t curBlockId,
                          const std::vector<std::shared_ptr<VoteTransaction>>
                              &notProcessedTransactions,
                          std::function<void(std::string &&)> &callback) {
  return std::unique_ptr<BlockControllerLF>(
      new BlockControllerLF(curBlockId, notProcessedTransactions, callback));
}
std::shared_ptr<ABlockMandate> BlockControllerLF::GetCurBlockMandate() const {
  std::shared_ptr<BlockMandateLF> safeCopy;
  safeCopy =
      std::atomic_load_explicit(&this->curMandate, std::memory_order_release);
  return std::dynamic_pointer_cast<ABlockMandate>(safeCopy);
}
void BlockControllerLF::ProcessBlockCreationEvent() {
  this->CheckAndProcessNewTransactions();
  this->curBlockId++;
  std::weak_ptr<BlockMandateLF> weakOldMandate;
  std::shared_ptr<TransactionQueue> oldQueue;
  {
    std::shared_ptr<BlockMandateLF> safeCopy;
    // переменная меняется только в этом потоке. По идее, можно считывать
    // неатомарно, но не будем так делать
    safeCopy =
        std::atomic_load_explicit(&this->curMandate, std::memory_order_relaxed);
    weakOldMandate = safeCopy;
    oldQueue = safeCopy->GetQueue();
  }

  std::shared_ptr<BlockMandateLF> newMandate =
      BlockMandateLF::Create(this->curBlockId);
  std::atomic_store_explicit(&this->curMandate, newMandate,
                             std::memory_order_acquire);
  while (true) {
    auto oldMandate = weakOldMandate.lock();
    this->CheckAndProcessNewTransactionsImpl(oldQueue);
    if (oldMandate == nullptr) {
      break;
    } else {
      std::this_thread::yield();
    }
  }
  callback(this->transactions->GetFinalTransactionBlock());
}
void BlockControllerLF::CheckAndProcessNewTransactions() {
  std::shared_ptr<BlockMandateLF> safeCopy;
  safeCopy =
      std::atomic_load_explicit(&this->curMandate, std::memory_order_release);
  this->CheckAndProcessNewTransactionsImpl(safeCopy->GetQueue());
}

} // namespace Voting
