#pragma once
#include "entity.hpp"

class PredatorFish : public Entity {
public:
    PredatorFish() = default;
    Type type()  const noexcept override { return Type::Predator; }
    const char* symbol() const noexcept override { return "𓆜"; }
};
