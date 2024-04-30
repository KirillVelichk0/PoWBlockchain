#include <drogon/drogon.h>
#include <filesystem>
#include <string>
#include <trantor/utils/Logger.h>
namespace fs = std::filesystem;
int main() {
  auto pathToDrogonConf = (fs::current_path().parent_path())
                              .append("configs")
                              .append("drogon.json");
  if (!fs::exists(pathToDrogonConf)) {
    LOG_FATAL << "Cant get drogon config on path " << pathToDrogonConf.native()
              << "\n";
  }
  LOG_INFO << "Starting service\n";
  drogon::app().loadConfigFile(pathToDrogonConf.native()).run();

  return EXIT_SUCCESS;
}
