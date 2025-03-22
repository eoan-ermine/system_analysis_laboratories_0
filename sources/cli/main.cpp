#include <charconv>

#include <common/actors.hpp>
#include <common/game.hpp>
#include <common/map.hpp>

using namespace std::string_literals;
using namespace std::string_view_literals;

int main(int argc, char *argv[]) {
  if (argc != 7) {
    std::cout << "Usage: ./cli --rows=<int> --columns=<int> --rabbits=<int> "
                 "--wolves_m=<int> --wolves_f=<int> --max_health=<int>\n";
    return 1;
  }

  int rows{0}, columns{0};
  int rabbits{0}, wolves_m{0}, wolves_f{0};
  int max_health{0};
  std::unordered_map<std::string_view, std::reference_wrapper<int>> args = {
      {"rows", std::ref(rows)},         {"columns", std::ref(columns)},
      {"rabbits", std::ref(rabbits)},   {"wolves_m", std::ref(wolves_m)},
      {"wolves_f", std::ref(wolves_f)}, {"max_health", std::ref(max_health)}};
  for (std::size_t i = 1; i != argc; ++i) {
    std::string_view arg = argv[i];
    if (!arg.starts_with("--")) {
      continue;
    }
    arg.remove_prefix("--"sv.size());

    auto value_idx = arg.find("=");
    if (value_idx == std::string_view::npos) {
      continue;
    }

    auto value_view = arg.substr(value_idx);
    value_view.remove_prefix("="sv.size());
    arg = arg.substr(0, value_idx);

    if (args.count(arg)) {
      int value{0};
      std::from_chars(value_view.data(), value_view.data() + value_view.size(),
                      value);
      args.at(arg).get() = value;
    }
  }

  Game game(Game::Settings{.rows = rows,
                           .columns = columns,
                           .rabbits = rabbits,
                           .wolves_m = wolves_m,
                           .wolves_f = wolves_f,
                           .max_health = static_cast<double>(max_health)});
  std::cout << game.getMap() << std::endl;

  std::string action;
  int step = 0;
  while (true) {
    std::cout << "Enter your action: ";
    std::cin >> action;

    if (action == "n") {
      game.step();
      std::cout << game.getMap() << std::endl;
      step++;
    } else if (action == "e") {
      game.stop();
      break;
    } else {
      std::cout << "Wrong action: " << action << "\n";
    }
  }

  const auto &statistics = game.getStatistics();
  statistics.dump("statistics.dump");
}