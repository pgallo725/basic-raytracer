#include "Random.h"

// Initialize the random number generator
thread_local std::mt19937_64 Random::m_generator = std::mt19937_64();
