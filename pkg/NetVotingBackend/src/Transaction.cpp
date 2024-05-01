#include "Transaction.h"
#include <drogon/drogon.h>
#include <trantor/utils/Logger.h>
namespace Voting {
std::unique_ptr<TransactionQueue> TransactionQueue::Create() {
  return std::unique_ptr<TransactionQueue>(new TransactionQueue);
}
TransactionQueue::TransactionQueue() : queue(100) {}
TransactionQueue::~TransactionQueue() {
  LOG_DEBUG << "Lockfree queue destructor";
  while (this->pop() != nullptr) {
  }
}
bool TransactionQueue::push(std::unique_ptr<VoteTransaction> &&vote) {
  if (vote == nullptr) {
    return false;
  }
  LOG_DEBUG << "Lockfree queue push";
  bool result = true;
  VoteTransaction *ptr = vote.get();
  // Я не уверен, что queue.push не может выкидывать исключений
  auto safePush = [this](VoteTransaction *ptr, auto &queue) -> bool {
    try {
      if (!queue.push(ptr)) {
        LOG_DEBUG << "Using additionalQueue to push";
        std::lock_guard lock(this->additionalQueueMutex);
        this->additionalQueue.push(ptr);
      }
      return true;
    } catch (...) {
      return false;
    }
  };
  try {
    if (!safePush(vote.release(), this->queue)) {
      LOG_WARN << "Cant push data to lockfree queue";
      delete ptr;
      result = false;
    }
  } catch (...) {
    delete ptr;
  }
  return result;
}
std::unique_ptr<VoteTransaction> TransactionQueue::pop() {
  std::unique_ptr<VoteTransaction> result;
  VoteTransaction *ptr;
  auto safePop = [this](VoteTransaction *&ptr) -> bool {
    bool result = false;
    if (this->queue.pop(ptr)) {
      LOG_DEBUG << "Popped from main queue";
      result = true;
    } else {
      std::lock_guard lock(this->additionalQueueMutex);
      if (!this->additionalQueue.empty()) {
        ptr = this->additionalQueue.front();
        this->additionalQueue.pop();
        result = true;
        LOG_DEBUG << "Popped from main queue";
      }
    }
    return result;
  };
  bool isOk = safePop(ptr); // если исключение, то ничего не поделаешь
  LOG_DEBUG << "Lockfree queue pop"; // (с учетом всех ограничений не должно)
  if (isOk) {
    result.reset(ptr);
    LOG_DEBUG << "Success lockfree queue pop";
  } else {
    LOG_DEBUG << "Unsuccess lockfree queue pop";
  }
  return result;
}
} // namespace Voting
