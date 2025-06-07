#pragma once
#include <cstdlib>

class MathUtils {
public:
    static float randomFloat(float min, float max) {
        return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
        //return (min + 1) + (((float)rand()) / (float)RAND_MAX) * (max - (min + 1));
    }
};
