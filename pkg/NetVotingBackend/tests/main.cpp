#include <drogon/HttpAppFramework.h>
#define DROGON_TEST_MAIN
#include "../src/BlockControllerLF.h"
#include "../src/ContolChannelSimple.h"
#include "../src/Transaction.h"
#include <drogon/drogon_test.h>
std::shared_ptr<Voting::ABlockController>
PrepareController(std::string &result) {
  std::uint64_t id = 3;
  std::function<void(std::string &&)> callback = [&result](std::string &&data) {
    result = std::move(data);
  };
  std::vector<std::shared_ptr<Voting::VoteTransaction>> data;
  Voting::VoteTransaction vote;
  vote.vote = 2;
  vote.voterId = "1234.12341324";
  vote.votingId = 10;
  vote.timestamp = 23414;
  vote.signature = {1, 32, 1, 0, 132, 1, 34, 5, 17, 2};
  data.push_back(std::make_shared<Voting::VoteTransaction>(vote));
  return Voting::BlockControllerLF::Create(id, data, callback);
}
std::shared_ptr<Voting::AControllChannel>
PrepareChannel(std::shared_ptr<Voting::ABlockController> controller) {
  return Voting::ControllChannelSimple::Create(controller);
}
DROGON_TEST(SimpleHandlingTest) {
  std::string handlingData;
  CHECK(handlingData.size() == 0);
  auto controller = PrepareController(handlingData);
  auto channel = PrepareChannel(controller);
  auto createVote1 = []() {
    Voting::VoteTransaction vote;
    vote.vote = 3;
    vote.voterId = "1224.12341324";
    vote.votingId = 10;
    vote.timestamp = 23414;
    vote.signature = {1, 32, 1, 0, 132, 1, 34, 5, 17, 2};
    return vote;
  };
  auto vote1 = createVote1();
  CHECK(channel != nullptr);
  CHECK(controller->GetCurBlockMandate() != nullptr);
  CHECK(channel->GetBlockMandate() != nullptr);
  for (int i = 0; i < 10; i++) {
    channel->GetBlockMandate()->AddTransaction(vote1);
  }
  controller->ProcessBlockCreationEvent();
  CHECK(handlingData.size() != 0);
  LOG_INFO << handlingData;
  channel->GetBlockMandate()->AddTransaction(createVote1());
  controller->ProcessBlockCreationEvent();
}
int main(int argc, char **argv) {
  // drogon::app().setLogPath("./");
  return drogon::test::run(argc, argv);
}
