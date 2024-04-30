#pragma once
#include "ABlockController.h"
#include "ATransactionsContainer.h"
#include "BlockMandateLF.h"
namespace Voting {
class BlockControllerLF : public ABlockController {
private:
  std::uint64_t curBlockId;
  std::shared_ptr<BlockMandateLF> curMandate;
  std::unique_ptr<ATransactionsContainer> transactions;
  void
  CheckAndProcessNewTransactionsImpl(std::shared_ptr<BlockMandateLF> mandate);
  BlockControllerLF(std::uint64_t curBlockId,
                    const std::vector<std::shared_ptr<VoteTransaction>>
                        &notProcessedTransactions);

public:
  static std::unique_ptr<BlockControllerLF>
  Create(std::uint64_t curBlockId,
         const std::vector<std::shared_ptr<VoteTransaction>>
             &notProcessedTransactions);
  ~BlockControllerLF() = default;
  BlockControllerLF(const BlockControllerLF &) = delete;
  BlockControllerLF(BlockControllerLF &&) = delete;
  BlockControllerLF &operator=(const BlockControllerLF &) = delete;
  BlockControllerLF &operator=(BlockControllerLF &&) = delete;
  std::shared_ptr<ABlockMandate> GetCurBlockMandate() const override;
  // Должен отрабатывать только в 1 потоке
  void ProcessBlockCreationEvent() override;
  // Должен отрабатывать только в 1 потоке
  void CheckAndProcessNewTransactions() override;
};
} // namespace Voting
