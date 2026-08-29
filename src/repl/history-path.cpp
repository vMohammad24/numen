#include "history-path.hpp"
#include <cstdlib>

namespace fs = std::filesystem;

fs::path historyPath() {
#ifdef _WIN32
  const char *base = std::getenv("APPDATA");
  if (!base) return {};
  return fs::path{base} / "numen" / "history";
#else
  fs::path base;
  if (const char *state = std::getenv("XDG_STATE_HOME")) {
    base = state;
  } else if (const char *home = std::getenv("HOME")) {
    base = fs::path{home} / ".local" / "state";
  } else {
    return {};
  }
  return base / "numen" / "history";
#endif
}
