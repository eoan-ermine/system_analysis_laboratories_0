#pragma once

#include <ostream>
#include <utility>
#include <variant>

struct ActorBase {
  enum State : int {
    EMPTY,
    RABBIT,
    WOLF,
    WOLF_M,
    WOLF_F,
  } state = EMPTY;
  double health{0};

  ActorBase() {}
  ActorBase(State state, double health) : state{state}, health{health} {}

  bool operator==(const ActorBase &) const = default;
};

struct Rabbit : public ActorBase {
  using ActorBase::ActorBase;
  Rabbit(double health) : ActorBase(State::RABBIT, health) {}
};

struct MaleWolf : public ActorBase {
  using ActorBase::ActorBase;
  MaleWolf(double health) : ActorBase(State::WOLF_M, health) {}
};

struct FemaleWolf : public ActorBase {
  using ActorBase::ActorBase;
  FemaleWolf(double health) : ActorBase(State::WOLF_F, health) {}
};

struct Actor {
  Actor() {}

  template <typename T> Actor(T actor) : actor_(std::move(actor)){};

  ActorBase::State getState() const {
    return std::visit([](auto &&arg) -> ActorBase::State { return arg.state; },
                      actor_);
  }

  void setState(ActorBase::State new_state) {
    std::visit([new_state](auto &&arg) { arg.state = new_state; }, actor_);
  }

  double getHealth() const {
    return std::visit([](auto &&arg) -> double { return arg.health; }, actor_);
  }

  void setHealth(double new_health) {
    std::visit([new_health](auto &&arg) { arg.health = new_health; }, actor_);
  }

  bool operator==(const Actor &) const = default;

private:
  std::variant<ActorBase, Rabbit, MaleWolf, FemaleWolf> actor_;
};

std::ostream &operator<<(std::ostream &os, const ActorBase::State &obj) {
  switch (obj) {
  case ActorBase::State::EMPTY:
    os << std::string{"\033[1;30m"} + "E" + "\033[0m";
    break;
  case ActorBase::State::RABBIT:
    os << std::string{"\033[1;92m"} + "R" + "\033[0m";
    break;
  case ActorBase::State::WOLF_M:
    os << std::string{"\033[1;91m"} + "M" + "\033[0m";
    break;
  case ActorBase::State::WOLF_F:
    os << std::string{"\033[1;95m"} + "F" + "\033[0m";
    break;
  default:
    std::unreachable();
  }
  return os;
}
