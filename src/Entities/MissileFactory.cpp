#include "MissileFactory.h"
#include <vector>
#include <functional>

std::unique_ptr<Missile> MissileFactory::CreateRandomMissile(Vector2 position) {
    static const std::vector<std::function<std::unique_ptr<Missile>(Vector2)>> factories = {
        [](Vector2 pos) { return std::make_unique<OscillatorMissile>(pos); },
        [](Vector2 pos) { return std::make_unique<LooperMissile>(pos); },
        [](Vector2 pos) { return std::make_unique<SeekerMissile>(pos); },
        [](Vector2 pos) { return std::make_unique<StandardMissile>(pos); }
    };

    int r = GetRandomValue(0, (int)factories.size() - 1);
    return factories[r](position);
}
