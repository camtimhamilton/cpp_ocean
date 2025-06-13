#include "ocean.hpp"
#include "algae.hpp"
#include "herbivore.hpp"
#include "predator.hpp"
#include <memory>
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    try {
        Ocean ocean(50, 10);
        ocean.init();
        
        ocean.setEntity( 8, 5, std::make_unique<HerbivoreFish>());
        ocean.setEntity(10, 6, std::make_unique<HerbivoreFish>());
        ocean.setEntity(12, 5, std::make_unique<HerbivoreFish>());
        
        ocean.setEntity(10, 5, std::make_unique<Algae>());

        ocean.setEntity( 5, 5, std::make_unique<PredatorFish>());

        std::cout << "Initial state:\n";
        ocean.print();

        int ticks = 0;
        const auto delay = std::chrono::milliseconds(500);
        while (ticks < 50) {
            ++ticks;
            std::this_thread::sleep_for(delay);
            ocean.tick();
            std::cout << "\nAfter tick " << ticks << ":\n";
            ocean.print();
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
