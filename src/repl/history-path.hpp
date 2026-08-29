#pragma once

#include <filesystem>

// platform state dir + "numen/history"; empty if no base dir is resolvable
std::filesystem::path historyPath();
