#pragma once
#include "Transaction.h"
#include <memory>
namespace Voting {
class ATransactionsContainer {
public:
  virtual ~ATransactionsContainer() = 0;
  virtual bool AddTransaction(std::unique_ptr<VoteTransaction> &&vote) = 0;
  virtual bool AddTransaction(std::shared_ptr<VoteTransaction> vote) = 0;
  virtual std::string GetFinalTransactionBlock() = 0;
  virtual void ResetTransactionBlock() = 0;
};
std::unique_ptr<ATransactionsContainer> CreateStandartTransactionContainer();
} // namespace Voting
