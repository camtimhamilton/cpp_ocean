#pragma once
#include "entity.hpp"

class Algae : public Entity {
public:
    Algae() = default;
    Type type()  const noexcept override { return Type::Algae; }
    const char* symbol() const noexcept override { return "~"; }
};
