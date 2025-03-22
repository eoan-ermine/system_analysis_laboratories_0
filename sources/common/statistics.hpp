#pragma once

#include "actors.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <vector>

static constexpr int START_OF_TEXT = static_cast<int>(-2);
static constexpr int GROUP_SEPARATOR = static_cast<int>(-35);
static constexpr int RECORD_SEPARATOR = static_cast<int>(-36);
static constexpr int END_OF_TRANSMISSION = static_cast<int>(-4);

struct Statistics {
  using StatisticsEntryType = std::unordered_map<ActorBase::State, int>;
  using StatisticsType = std::vector<StatisticsEntryType>;

  Statistics() {}

  void add(StatisticsEntryType entry) {
    statistics.push_back(std::move(entry));
  }

  const StatisticsType &getStatistics() const { return statistics; }

  static Statistics from(std::filesystem::path path) {
    StatisticsType statistics;
    StatisticsEntryType statistics_entry;

    std::ifstream stream{path, std::ios_base::binary};
    stream.exceptions(std::ifstream::failbit);

    int symbol;
    stream.read((char *)&symbol, sizeof(int));

    if (symbol != START_OF_TEXT) {
      throw std::runtime_error{"control != START_OF_TEXT"};
    }

    while (true) {
      int state;
      stream.read((char *)&state, sizeof(int));

      if (state == GROUP_SEPARATOR) {
        statistics.push_back(statistics_entry);
        statistics_entry.clear();
        continue;
      } else if (state == END_OF_TRANSMISSION) {
        break;
      }

      int count;
      stream.read((char *)&count, sizeof(int));

      stream.read((char *)&symbol, sizeof(int));
      if (symbol == RECORD_SEPARATOR) {
        statistics_entry[static_cast<ActorBase::State>(state)] = count;
      }
    }

    return Statistics{std::move(statistics)};
  }

  void dump(std::filesystem::path path) const {
    std::ofstream stream{path, std::ios_base::binary | std::ios_base::trunc};
    stream.write((char *)(&START_OF_TEXT), sizeof(int));
    for (auto &entry : statistics) {
      for (auto &[state, count] : entry) {
        stream.write((char *)(&state), sizeof(int));
        stream.write((char *)(&count), sizeof(int));
        stream.write((char *)(&RECORD_SEPARATOR), sizeof(int));
      }
      stream.write((char *)(&GROUP_SEPARATOR), sizeof(int));
    }
    stream.write((char *)(&END_OF_TRANSMISSION), sizeof(int));
  }

private:
  Statistics(StatisticsType statistics) : statistics(std::move(statistics)) {}

  StatisticsType statistics;
};