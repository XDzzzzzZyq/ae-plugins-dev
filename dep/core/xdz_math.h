
#include "glm/glm.hpp"

constexpr bool operator<(const glm::vec2& _a, const glm::vec2& _b) {
	return _a.x < _b.x || _a.y < _b.y;
}

constexpr bool operator>(const glm::vec2& _a, const glm::vec2& _b) {
	return _a.x > _b.x || _a.y > _b.y;
}

#include <cmath>
#include <random>

inline double random01(unsigned int seed) {
	std::default_random_engine generator(seed);
	const std::uniform_real_distribution<double> distribution(0.0, 1.0);
	return distribution(generator);
}

#define PI 3.14159265358979323846
inline double hash01(double in, double seed) noexcept {
	return fmod(in*seed*1.145141919810, PI) / PI;
}