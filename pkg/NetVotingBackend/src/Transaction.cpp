#include "Transaction.h"
#include <boost/lockfree/queue.hpp>
#include <drogon/drogon.h>
#include <trantor/utils/Logger.h>
namespace Voting {
// Эта реализация ну такая, и ее потом лучше заменить.
//
// Есть lockfree очередь, она очень хорошая, быстрая, но имеет некоторый
// максимальный размер Есть обычная очередь под мьютексом, она вспомогательная.
//
// При обычном сценарии работы будет использоваться именно основная очередь.
// Однако, при крайне большой нагрузке атомарная очередь может достигнуть своего
// лимита (обычно 2^16), на этот случай пригодится вспомогательная
struct TransactionQueue::QueueInternal {
  boost::lockfree::queue<VoteTransaction *> queue;
  std::queue<VoteTransaction *> additionalQueue;
  std::mutex additionalQueueMutex;
  QueueInternal();
  static std::unique_ptr<TransactionQueue::QueueInternal> CreateInternal() {
    return std::unique_ptr<QueueInternal>(new QueueInternal());
  }
};
std::unique_ptr<TransactionQueue> TransactionQueue::Create() {
  return std::unique_ptr<TransactionQueue>(new TransactionQueue);
}

TransactionQueue::TransactionQueue()
    : internal(QueueInternal::CreateInternal()) {}
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
        std::lock_guard lock(this->internal->additionalQueueMutex);
        this->internal->additionalQueue.push(ptr);
      }
      return true;
    } catch (...) {
      return false;
    }
  };
  try {
    if (!safePush(vote.release(), this->internal->queue)) {
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
    if (this->internal->queue.pop(ptr)) {
      LOG_DEBUG << "Popped from main queue";
      result = true;
    } else {
      std::lock_guard lock(this->internal->additionalQueueMutex);
      if (!this->internal->additionalQueue.empty()) {
        ptr = this->internal->additionalQueue.front();
        this->internal->additionalQueue.pop();
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
TransactionQueue::QueueInternal::QueueInternal() : queue(100) {}
} // namespace Voting
