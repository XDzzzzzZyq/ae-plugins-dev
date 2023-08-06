#pragma once

#include "glm/glm.hpp"

constexpr bool operator<(const glm::vec2& _a, const glm::vec2& _b) {
	return _a.x < _b.x || _a.y < _b.y;
}

constexpr bool operator>(const glm::vec2& _a, const glm::vec2& _b) {
	return _a.x > _b.x || _a.y > _b.y;
}

class Block
{

public:

	enum BlockType
	{
		BType_NONE = -1, BType_RECT, BType_ELLIPS
	};

private:

	glm::vec2 b_min{ .0 };
	glm::vec2 b_max{ .0 };

public:

	Block();
	
public:

	void UpdateMin(const glm::vec2& _min);
	void UpdateMax(const glm::vec2& _max);

};

