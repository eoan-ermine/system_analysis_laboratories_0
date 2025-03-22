#pragma once

#include "actors.hpp"
#include "map.hpp"
#include "statistics.hpp"

class Game {
  using PendingMovesType = std::unordered_map<Actor *, bool>;

public:
  struct Settings {
    int rows, columns;
    int rabbits, wolves_m, wolves_f;
    double max_health;
  };

  Game(Settings settings)
      : settings(std::move(settings)), map(settings.rows, settings.columns) {
    std::random_device r;
    std::default_random_engine e(r());
    std::uniform_int_distribution<int> uniform_row_dist(0, settings.rows - 1);
    std::uniform_int_distribution<int> uniform_column_dist(0, settings.columns -
                                                                  1);

    auto generate_n = [&](int count, auto value) {
      while (count) {
        auto row = uniform_row_dist(e), column = uniform_column_dist(e);
        if (!map.isEmpty(row, column)) {
          continue;
        }
        map.addActor(value, row, column);
        count--;
      }
    };

    generate_n(settings.rabbits, Rabbit{settings.max_health});
    generate_n(settings.wolves_m, MaleWolf{settings.max_health});
    generate_n(settings.wolves_f, FemaleWolf{settings.max_health});

    statistics.add(Statistics::StatisticsEntryType{
        {ActorBase::State::RABBIT, settings.rabbits},
        {ActorBase::State::WOLF_M, settings.wolves_m},
        {ActorBase::State::WOLF_F, settings.wolves_f}});
  }

  const Map &getMap() const { return map; }

  const Statistics &getStatistics() const { return statistics; }

  void step() {
    PendingMovesType pending_moves;

    std::vector<Actor *> actors;
    Statistics::StatisticsEntryType statistics_entry;
    for (auto &actor : map.getActors()) {
      actors.push_back(&actor);
      pending_moves.insert({&actor, true});
      statistics_entry[actor.getState()] += 1;
    }
    statistics.add(std::move(statistics_entry));

    for (auto actor : actors) {
      if (!pending_moves.at(actor)) {
        continue;
      }

      if (actor->getState() == ActorBase::State::RABBIT &&
          rabbit_step(actor, pending_moves)) {
        continue;
      }
      if (actor->getState() > ActorBase::State::WOLF &&
          wolf_step(actor, pending_moves)) {
        continue;
      }
      if (actor->getState() == ActorBase::State::WOLF_M &&
          wolf_m_step(actor, pending_moves)) {
        continue;
      }

      if (auto neighbour = map.get_empty_neighbour(actor); neighbour) {
        auto [_, new_row, new_column] = *neighbour;
        map.moveActor(actor, new_row, new_column);
      }
    }

    for (auto actor : actors) {
      if (actor->getState() > ActorBase::State::WOLF) {
        actor->setHealth(actor->getHealth() - 1);

        if (actor->getHealth() == 0) {
          map.removeActor(actor);
        }
      }
    }
  }

  void stop() {
    Statistics::StatisticsEntryType statistics_entry;
    for (auto &actor : map.getActors()) {
      statistics_entry[actor.getState()] += 1;
    }
    statistics.add(std::move(statistics_entry));
  }

private:
  bool rabbit_step(Actor *actor, PendingMovesType &pending_moves) {
    /* если возле волка/волчицы имеется кролик и их здоровье <=50%
    (то есть прошло не менее X/2 шагов с еды), то он/она перемещаются
    на клетку кролика и съедают его;
    */

    std::random_device r;
    std::default_random_engine e(r());
    std::uniform_int_distribution<int> new_dist(0, 100);
    if (new_dist(e) < 10) {
      if (auto new_neighbour = map.get_empty_neighbour(actor); new_neighbour) {
        auto [_, child_row, child_column] = *new_neighbour;
        map.addActor(Rabbit{settings.max_health}, child_row, child_column);
      }
      return true;
    }
    return false;
  }

  bool wolf_step(Actor *actor, PendingMovesType &pending_moves) {
    if (actor->getHealth() <= (settings.max_health / 2)) {
      auto rabbit_target =
          map.get_neighbour_by_predicate(actor, [&](auto cell) {
            return cell != nullptr &&
                   cell->getState() == ActorBase::State::RABBIT;
          });

      if (rabbit_target) {
        auto [rabbit_actor, rabbit_row, rabbit_column] = *rabbit_target;

        map.removeActor(rabbit_actor);
        map.moveActor(actor, rabbit_row, rabbit_column);
        actor->setHealth(settings.max_health);

        return true;
      }
    }
    return false;
  }

  bool wolf_m_step(Actor *actor, PendingMovesType &pending_moves) {
    /* если возле волка имеется волчица и их здоровье >=75%
    (то есть прошло менее X/4 шагов с еды), а также есть свободная клетка рядом,
    они производят 1 ед. потомства
    */
    if (actor->getHealth() >= (75 * (settings.max_health / 100))) {
      auto female_target =
          map.get_neighbour_by_predicate(actor, [&](auto cell) {
            return cell != nullptr &&
                   cell->getState() == ActorBase::State::WOLF_F &&
                   cell->getHealth() >= (75 * (settings.max_health / 100));
          });

      if (female_target) {
        auto [female_actor, female_row, female_column] = *female_target;
        if (auto empty_neighbour = map.get_empty_neighbour(actor);
            empty_neighbour) {
          auto [_, child_row, child_column] = *empty_neighbour;

          std::random_device r;
          std::default_random_engine e(r());
          std::uniform_int_distribution<int> binary_dist(0, 1);

          if (binary_dist(e)) {
            map.addActor(MaleWolf{settings.max_health}, child_row,
                         child_column);
          } else {
            map.addActor(FemaleWolf{settings.max_health}, child_row,
                         child_column);
          }

          pending_moves.at(female_actor) = false;
          return true;
        }
      }
    }
    return false;
  }

  Settings settings;
  Map map;
  Statistics statistics;
};