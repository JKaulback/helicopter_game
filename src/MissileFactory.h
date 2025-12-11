#include "Missile.h"
#include <memory>
#include <functional>
#include <vector>
#include "raylib.h"

class MissileFactory {
public:
    static std::unique_ptr<Missile> CreateRandomMissile(Vector2 position);
};
