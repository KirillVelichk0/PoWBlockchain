#include "Transaction.h"
namespace Voting {
std::unique_ptr<TransactionQueue> TransactionQueue::Create() {
  return std::unique_ptr<TransactionQueue>(new TransactionQueue);
}
TransactionQueue::~TransactionQueue() {
  while (this->pop() != nullptr) {
  }
}
bool TransactionQueue::push(std::unique_ptr<VoteTransaction> &&vote) {
  if (vote == nullptr) {
    return false;
  }
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
  VoteTransaction *ptr = result.get();
  bool isOk = this->queue.pop(ptr); // если исключение, то ничего не поделаешь
                                    // (с учетом всех ограничений не должно)
  if (!isOk) {
    result.reset();
  }
  return result;
}
} // namespace Voting
