#include "Transaction.h"
#include <drogon/drogon.h>
#include <trantor/utils/Logger.h>
namespace Voting {
std::unique_ptr<TransactionQueue> TransactionQueue::Create() {
  return std::unique_ptr<TransactionQueue>(new TransactionQueue);
}
TransactionQueue::TransactionQueue() : queue(100) {}
TransactionQueue::~TransactionQueue() {
  std::cout << "Queue destructor\n";
  while (this->pop() != nullptr) {
  }
}
bool TransactionQueue::push(std::unique_ptr<VoteTransaction> &&vote) {
  if (vote == nullptr) {
    return false;
  }
  std::cout << "Pushing\n";
  bool result = true;
  VoteTransaction *ptr = vote.get();
  // Я не уверен, что queue.push не может выкидывать исключений
  auto safePush = [](VoteTransaction *ptr, auto &queue) -> bool {
    try {
      return queue.push(ptr);
    } catch (...) {
      return false;
    }
  };
  try {
    if (!safePush(vote.release(), this->queue)) {
      std::cout << "Badpush\n";
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
  bool isOk = this->queue.pop(ptr); // если исключение, то ничего не поделаешь
  std::cout << "Pop!\n"; // (с учетом всех ограничений не должно)
  if (isOk) {
    result.reset(ptr);
  } else {
    std::cout << "BADPOP\n";
  }
  return result;
}
} // namespace Voting
