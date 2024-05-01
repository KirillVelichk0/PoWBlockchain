#pragma once
#include <boost/lockfree/queue.hpp>
#include <cstdint>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <vector>
namespace Voting {
struct VoteTransaction {
  std::uint32_t vote;
  std::uint64_t votingId;
  std::uint64_t timestamp;
  std::string voterId; // public.x + . + public.y
  std::vector<unsigned char> signature;
};

// Эта реализация ну такая, и ее потом лучше заменить.
//
// Есть lockfree очередь, она очень хорошая, быстрая, но имеет некоторый
// максимальный размер Есть обычная очередь под мьютексом, она вспомогательная.
//
// При обычном сценарии работы будет использоваться именно основная очередь.
// Однако, при крайне большой нагрузке атомарная очередь может достигнуть своего
// лимита (обычно 2^16), на этот случай пригодится вспомогательная
class TransactionQueue {
private:
  boost::lockfree::queue<VoteTransaction *> queue;
  std::queue<VoteTransaction *> additionalQueue;
  std::mutex additionalQueueMutex;
  TransactionQueue();

public:
  static std::unique_ptr<TransactionQueue> Create();
  ~TransactionQueue();
  TransactionQueue(const TransactionQueue &) = delete;
  TransactionQueue &operator=(const TransactionQueue &) = delete;
  TransactionQueue(TransactionQueue &&) = delete;
  TransactionQueue &operator=(TransactionQueue &&) = delete;
  bool push(std::unique_ptr<VoteTransaction> &&vote);
  std::unique_ptr<VoteTransaction> pop();
};
} // namespace Voting
