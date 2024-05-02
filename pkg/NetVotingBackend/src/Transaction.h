#pragma once
#include <cstdint>
#include <memory>
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

class TransactionQueue {
private:
  struct QueueInternal;
  std::unique_ptr<QueueInternal> internal;

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
