#pragma once

#include "actors.hpp"

#include <functional>
#include <iostream>
#include <list>
#include <mdspan>
#include <optional>
#include <random>
#include <unordered_map>
#include <vector>

struct Map {
  using SpanType = std::mdspan<Actor *, std::dextents<std::size_t, 2>>;

  Map(int rows, int columns) : rows(rows), columns(columns) {
    map.resize(rows * columns);
    map_span = SpanType{map.data(), rows, columns};
  }

  int getRows() const { return rows; }

  int getColumns() const { return columns; }

  std::list<Actor> &getActors() { return actors; }

  void addActor(Actor actor, int row, int column) {
    actor_to_it[&actors.back()] = actors.insert(actors.end(), std::move(actor));
    actor_to_pos[&actors.back()] = {row, column};
    map_span[row, column] = &actors.back();
  }

  void moveActor(Actor *actor, int new_row, int new_column) {
    auto [previous_row, previous_column] = actor_to_pos.at(actor);

    map_span[previous_row, previous_column] = nullptr;
    map_span[new_row, new_column] = actor;
    actor_to_pos.at(actor) = {new_row, new_column};
  }

  void removeActor(Actor *actor) {
    auto [previous_row, previous_column] = actor_to_pos.at(actor);
    auto it = actor_to_it.at(actor);

    actor_to_it.erase(actor);
    actor_to_pos.erase(actor);
    map_span[previous_row, previous_column] = nullptr;
    actors.erase(it);
  }

  bool isEmpty(int row, int column) { return map_span[row, column] == nullptr; }

  Actor *operator[](int row, int column) { return map_span[row, column]; }

  const Actor *operator[](int row, int column) const {
    return map_span[row, column];
  }

  std::vector<std::tuple<int, int>> get_neighbours(int row, int column) {
    auto is_in_bounds = [&](int coordinate, int upper) {
      return coordinate >= 0 && coordinate < upper;
    };
    if (!is_in_bounds(row, rows) || !is_in_bounds(column, columns)) {
      return {};
    }

    std::vector<std::tuple<int, int>> neighbours;
    neighbours.reserve(8);

    std::initializer_list<std::tuple<int, int>> potential_neighbours = {
        {row - 1, column - 1}, {row - 1, column},    {row - 1, column + 1},
        {row, column - 1},     {row, column + 1},    {row + 1, column - 1},
        {row + 1, column},     {row + 1, column + 1}};
    for (auto [n_row, n_column] : potential_neighbours) {
      if (is_in_bounds(n_row, rows) && is_in_bounds(n_column, columns)) {
        neighbours.push_back({n_row, n_column});
      }
    }

    return neighbours;
  }

  using Neighbour = std::tuple<Actor *, int, int>;

  std::optional<Neighbour>
  get_neighbour_by_predicate(Actor *actor,
                             std::function<bool(const Actor *)> predicate) {
    auto [row, column] = actor_to_pos.at(actor);
    auto neighbours = get_neighbours(row, column);

    auto empty_neighbours = [&]() {
      std::vector<Neighbour> res;
      for (auto [row, column] : neighbours) {
        Actor *value = operator[](row, column);
        if (predicate(value)) {
          res.push_back({std::move(value), row, column});
        }
      }
      return res;
    }();

    if (empty_neighbours.empty()) {
      return std::optional<Neighbour>{};
    }

    std::random_device r;
    std::default_random_engine e(r());
    std::uniform_int_distribution<int> idx_dist(0, empty_neighbours.size() - 1);

    return std::optional<Neighbour>{empty_neighbours[idx_dist(e)]};
  }

  std::optional<Neighbour> get_empty_neighbour(Actor *actor) {
    return get_neighbour_by_predicate(
        actor, [](const auto &cell) { return cell == nullptr; });
  }

private:
  std::size_t rows, columns;
  std::list<Actor> actors;
  std::unordered_map<Actor *, std::list<Actor>::iterator> actor_to_it;
  std::unordered_map<Actor *, std::tuple<std::size_t, std::size_t>>
      actor_to_pos;
  std::vector<Actor *> map;
  SpanType map_span;

  friend std::ostream &operator<<(std::ostream &os, const Map &obj);
};

std::ostream &operator<<(std::ostream &os, const Map &map) {
  for (int i = 0; i != map.getRows(); ++i) {
    os << "|";
    for (int j = 0; j != map.getColumns(); ++j) {
      auto cell = map[i, j];
      if (cell == nullptr) {
        os << ActorBase::State::EMPTY << "|";
      } else {
        os << cell->getState() << "|";
      }
    }
    os << '\n';
  }
  return os;
}