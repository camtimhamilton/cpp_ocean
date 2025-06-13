#pragma once
#include "entity.hpp"

class HerbivoreFish : public Entity {
public:
    HerbivoreFish() = default;
    Type type()  const noexcept override { return Type::Herbivore; }
    const char* symbol() const noexcept override { return "𓆡"; }
};
