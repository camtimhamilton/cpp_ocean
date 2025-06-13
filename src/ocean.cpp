#include "ocean.hpp"
#include "sand.hpp"
#include "algae.hpp"
#include "herbivore.hpp"
#include "predator.hpp"

#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <array>
#include <random>
#include <limits>

struct Ocean::Impl {
    std::size_t width_, height_;
    std::vector<std::vector<std::unique_ptr<Entity>>> grid_;
    std::mt19937 eng;

    Impl(std::size_t w, std::size_t h)
        : width_(w)
        , height_(h)
        , grid_(h)               
        , eng(std::random_device{}())
    {
        
        for (std::size_t y = 0; y < height_; ++y) {
            grid_[y].resize(width_);
        }
    }

    void validate(std::size_t x, std::size_t y) const {
        if (x >= width_ || y >= height_)
            throw std::out_of_range("Ocean::setEntity: coordinates out of range");
    }

    Impl(const Impl& o)
      : width_(o.width_), height_(o.height_), grid_(height_), eng(std::random_device{}())
    {
      for (std::size_t y = 0; y < height_; ++y) {
        grid_[y].resize(width_);
        for (std::size_t x = 0; x < width_; ++x) {
          if (!o.grid_[y][x]) continue;
          switch (o.grid_[y][x]->type()) {
            case Entity::Type::Sand:
              grid_[y][x] = std::make_unique<Sand>(); break;
            case Entity::Type::Algae:
              grid_[y][x] = std::make_unique<Algae>(); break;
            case Entity::Type::Herbivore:
              grid_[y][x] = std::make_unique<HerbivoreFish>(); break;
            case Entity::Type::Predator:
              grid_[y][x] = std::make_unique<PredatorFish>(); break;
          }
        }
      }
    }

};

Ocean::Ocean(std::size_t width, std::size_t height)
    : pImpl(std::make_unique<Impl>(width, height))
{}

Ocean::Ocean(const Ocean& other)
    : pImpl(std::make_unique<Impl>(*other.pImpl))
{}

Ocean& Ocean::operator=(const Ocean& other) {
    if (this != &other)
        pImpl = std::make_unique<Impl>(*other.pImpl);
    return *this;
}

Ocean::Ocean(Ocean&&) noexcept = default;
Ocean& Ocean::operator=(Ocean&&) noexcept = default;
Ocean::~Ocean() = default;

void Ocean::init() {
    for (std::size_t y = 0; y < pImpl->height_; ++y)
        for (std::size_t x = 0; x < pImpl->width_; ++x)
            pImpl->grid_[y][x] = std::make_unique<Sand>();
}

void Ocean::print() const {
    
    std::cout << '+';
    for (std::size_t i = 0; i < pImpl->width_; ++i) std::cout << '-';
    std::cout << "+\n";

    
    for (std::size_t y = 0; y < pImpl->height_; ++y) {
        std::cout << '|';
        for (std::size_t x = 0; x < pImpl->width_; ++x) {
            if (pImpl->grid_[y][x]) {
                std::cout << pImpl->grid_[y][x]->symbol();
            } else {
                std::cout << "?";
            }
        }
        std::cout << "|\n";
    }

    
    std::cout << '+';
    for (std::size_t i = 0; i < pImpl->width_; ++i) std::cout << '-';
    std::cout << "+\n";
}

void Ocean::setEntity(std::size_t x, std::size_t y, std::unique_ptr<Entity> entity) {
    pImpl->validate(x, y);
    pImpl->grid_[y][x] = std::move(entity);
}

void Ocean::tick() {
    
    std::vector<std::pair<std::size_t, std::size_t>> algae;
    constexpr int dxA[4] = {1, -1, 0, 0}, dyA[4] = {0, 0, 1, -1};
    for (std::size_t y = 0; y < pImpl->height_; ++y) {
        for (std::size_t x = 0; x < pImpl->width_; ++x) {
            if (pImpl->grid_[y][x] && pImpl->grid_[y][x]->type() == Entity::Type::Algae) {
                algae.emplace_back(x, y);
            }
        }
    }
    for (auto [x, y] : algae) {
        for (int i = 0; i < 4; ++i) {
            std::size_t nx = x + dxA[i], ny = y + dyA[i];
            if (nx < pImpl->width_ && ny < pImpl->height_ &&
                pImpl->grid_[ny][nx] &&
                pImpl->grid_[ny][nx]->type() == Entity::Type::Sand)
            {
                pImpl->grid_[ny][nx] = std::make_unique<Algae>();
            }
        }
    }

    
    std::vector<std::pair<std::size_t, std::size_t>> herbi;
    constexpr int dxH[4] = {1, -1, 0, 0}, dyH[4] = {0, 0, 1, -1};
    std::uniform_int_distribution<int> coin(0, 1);
    for (std::size_t y = 0; y < pImpl->height_; ++y) {
        for (std::size_t x = 0; x < pImpl->width_; ++x) {
            if (pImpl->grid_[y][x] && pImpl->grid_[y][x]->type() == Entity::Type::Herbivore) {
                herbi.emplace_back(x, y);
            }
        }
    }
    for (auto [x, y] : herbi) {
        std::array<int, 4> dirs = {0, 1, 2, 3};
        std::shuffle(dirs.begin(), dirs.end(), pImpl->eng);
        for (int d : dirs) {
            std::size_t nx = x + dxH[d], ny = y + dyH[d];
            if (nx < pImpl->width_ && ny < pImpl->height_ &&
                pImpl->grid_[ny][nx] &&
                pImpl->grid_[ny][nx]->type() == Entity::Type::Algae)
            {
                if (coin(pImpl->eng))
                    pImpl->grid_[ny][nx] = std::make_unique<HerbivoreFish>();
                else
                    pImpl->grid_[ny][nx] = std::make_unique<Sand>();
                break;
            }
        }
    }

    
    std::uniform_real_distribution<float> spawn_chance(0.0f, 1.0f);
    std::vector<std::pair<std::size_t, std::size_t>> preds, foods = herbi;
    for (std::size_t y = 0; y < pImpl->height_; ++y) {
        for (std::size_t x = 0; x < pImpl->width_; ++x) {
            if (pImpl->grid_[y][x] && pImpl->grid_[y][x]->type() == Entity::Type::Predator) {
                preds.emplace_back(x, y);
            }
        }
    }

    for (auto [x, y] : preds) {
        if (foods.empty()) break;
        
        std::size_t bestDist = std::numeric_limits<std::size_t>::max();
        std::pair<std::size_t, std::size_t> target = foods[0];
        for (auto& f : foods) {
            std::size_t d = std::abs(int(f.first) - int(x)) + std::abs(int(f.second) - int(y));
            if (d < bestDist) {
                bestDist = d;
                target = f;
            }
        }
        
        int dx = 0, dy = 0;
        if (target.first != x)       dx = (target.first > x ? 1 : -1);
        else if (target.second != y)  dy = (target.second > y ? 1 : -1);

        std::size_t nx = x + dx, ny = y + dy;
        if (nx >= pImpl->width_ || ny >= pImpl->height_) continue;

        
        bool ate = pImpl->grid_[ny][nx] && pImpl->grid_[ny][nx]->type() == Entity::Type::Herbivore;
        
        pImpl->grid_[ny][nx] = std::move(pImpl->grid_[y][x]);
        
        if (ate && spawn_chance(pImpl->eng) < 0.3f) {
            pImpl->grid_[y][x] = std::make_unique<PredatorFish>();
        } else {
            pImpl->grid_[y][x] = std::make_unique<Sand>();
        }
    }
}
