#include "TransactionsContainerImpl.h"
#include <drogon/utils/Utilities.h>
#include <sstream>
#include <trantor/utils/Logger.h>
namespace Voting {
bool TryToAppenTransactionToJson(Json::Value &jsonObj,
                                 const VoteTransaction &transaction) {
  try {
    bool result = false;
    std::string votingIdStr = std::to_string(transaction.votingId);
    auto &actualValue = jsonObj[votingIdStr][transaction.voterId];
    if (!actualValue.empty()) {
      LOG_ERROR << "Cant append transaction with voting id " << votingIdStr
                << " and voter id " << transaction.voterId
                << " because it exists";
    } else {
      result = true;
      actualValue["vote"] = transaction.vote;
      actualValue["timestamp"] = transaction.timestamp;
      auto signatureEncoded =
          drogon::utils::base64Encode(transaction.signature.data(),
                                      transaction.signature.size(), true, true);
      actualValue["signature"] = std::string(transaction.signature.begin(),
                                             transaction.signature.end());
    }
    return result;
  } catch (std::exception &error) {
    LOG_ERROR << "Cant process appending transaction with voting id "
              << transaction.votingId << " and voter id " << transaction.voterId
              << " because catched exception " << error.what();
    throw error;
  } catch (...) {
    LOG_ERROR << "Cant process appending transaction with voting id "
              << transaction.votingId << " and voter id " << transaction.voterId
              << " because catched unknown exception";
    throw;
  }
}
bool TransactionsContainerImpl::AddTransaction(
    std::unique_ptr<VoteTransaction> &&vote) {
  return this->AddTransaction(
      std::shared_ptr<VoteTransaction>(std::move(vote)));
}
bool TransactionsContainerImpl::AddTransaction(
    std::shared_ptr<VoteTransaction> vote) {
  return TryToAppenTransactionToJson(this->curValue, *vote.get());
}
std::string TransactionsContainerImpl::GetFinalTransactionBlock() {
  std::ostringstream oss;
  oss << this->curValue;
  return oss.str();
}
void TransactionsContainerImpl::ResetTransactionBlock() {
  this->curValue = Json::Value();
}
std::unique_ptr<ATransactionsContainer> CreateStandartTransactionContainer() {
  return std::unique_ptr<ATransactionsContainer>(
      new (TransactionsContainerImpl));
}
} // namespace Voting
