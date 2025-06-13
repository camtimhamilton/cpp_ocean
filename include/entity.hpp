#pragma once
#include <cstddef>
#include <string>

class Entity {
public:
    enum class Type { Sand, Algae, Herbivore, Predator };
    virtual ~Entity() = default;
    virtual Type type()  const noexcept = 0;
    virtual const char* symbol() const noexcept = 0;
};
