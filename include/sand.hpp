#pragma once
#include "entity.hpp"

class Sand : public Entity {
public:
    Type type()  const noexcept override { return Type::Sand; }
    const char* symbol() const noexcept override { return " "; }
};
