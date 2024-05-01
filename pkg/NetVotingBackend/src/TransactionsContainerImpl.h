#pragma once
#include "ATransactionsContainer.h"
#include <drogon/drogon.h>
namespace Voting {
bool TryToAppenTransactionToJson(Json::Value &jsonObj,
                                 const VoteTransaction &transaction);
class TransactionsContainerImpl : public ATransactionsContainer {
private:
  Json::Value curValue;

public:
  ~TransactionsContainerImpl() = default;
  bool AddTransaction(std::unique_ptr<VoteTransaction> &&vote) override;
  bool AddTransaction(std::shared_ptr<VoteTransaction> vote) override;
  std::string GetFinalTransactionBlock() override;
  void ResetTransactionBlock() override;
};
} // namespace Voting
