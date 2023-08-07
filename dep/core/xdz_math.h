
#include "glm/glm.hpp"
#include "glm/gtx/exterior_product.hpp"

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

#include <vector>
inline bool is_invert(const std::vector<glm::vec2>& verts) {
	float dir = 0;
	for (int i = 0; i < verts.size(); i++) {
		const glm::vec2 v1 = verts[(i + 1) % verts.size()] - verts[i];
		const glm::vec2 v2 = verts[(i + 2) % verts.size()] - verts[(i + 1) % verts.size()];

		dir += glm::asin(glm::cross(glm::normalize(v1), glm::normalize(v2)));
	}

	return dir > 0;
}