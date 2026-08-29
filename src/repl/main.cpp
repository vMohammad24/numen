#include "history-path.hpp"
#include "numen/numen.hpp"
#include "rang/rang.hpp"
#include <cerrno>
#include <filesystem>
#include <iostream>
#include <replxx.hxx>
#include <string>
#ifdef BUILD_CURRENCY_PROVIDER
#include "vicinae-currency-provider.hpp"
#endif

namespace fs = std::filesystem;

void process(numen::Numen &calc, const std::string &s) {
  std::string_view line{s};

  bool ast = false;
  if (line.starts_with("/ast")) {
    line = line.substr(4);
    ast = true;
  }

  if (ast) {
    calc.printAST(std::string{line});
  } else {
    auto res = calc.evaluate(line);

    if (!res) {
      std::cout << "Error: " << res.error() << "\n";
    } else {
      std::cout << line << " = " << rang::fg::green << res.value() << rang::fg::reset << "\n";
    }
  }
}

int main(int ac, char **av) try {
  numen::Numen calc{};

#ifdef BUILD_CURRENCY_PROVIDER
  calc.setCurrencyProvider(std::make_unique<VicinaeCurrencyProvider>());
  calc.updateRates();
#endif

  if (ac == 2) {
    process(calc, av[1]);
    return 0;
  }

  replxx::Replxx rx;
  rx.set_max_history_size(512);

  const fs::path history = historyPath();
  if (!history.empty()) {
    std::error_code ec;
    fs::create_directories(history.parent_path(), ec);
    rx.history_load(history.string());
  }

  for (;;) {
    const char *raw = rx.input("$> ");
    if (!raw) {
      if (errno == EAGAIN) continue; // interrupted (e.g. window resize)
      break;
    }

    std::string line{raw};
    if (line.empty()) continue;

    process(calc, line);
    rx.history_add(line);
  }

  if (!history.empty()) rx.history_save(history.string());
} catch (const std::exception &e) {
  std::cerr << "fatal: " << e.what() << "\n";
  return 1;
}
