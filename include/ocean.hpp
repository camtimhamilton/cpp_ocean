#pragma once
#include <cstddef>
#include <memory>
#include "entity.hpp"

class Ocean {
public:
    Ocean(std::size_t width, std::size_t height);
    Ocean(const Ocean& other);
    Ocean& operator=(const Ocean& other);
    Ocean(Ocean&& other) noexcept;
    Ocean& operator=(Ocean&& other) noexcept;
    ~Ocean();

    void init();  
    void print() const;  
    void setEntity(std::size_t x,
                   std::size_t y,
                   std::unique_ptr<Entity> entity);
    void tick();  

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
