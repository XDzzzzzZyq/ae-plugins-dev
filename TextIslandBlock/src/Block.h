#pragma once

#include "glm/glm.hpp"
#include <iostream>

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

public:

	glm::vec2 b_min{ 9999 };
	glm::vec2 b_max{ .0   };

public:

	Block();

	friend std::ostream& operator<<(std::ostream& stream, const Block& block)
	{
		stream << "min: " << block.b_min.x << ", " << block.b_min.y << std::endl;
		stream << "max: " << block.b_max.x << ", " << block.b_max.y << std::endl;
		return stream;
	}
	
public:

	void UpdateMin(const glm::vec2& _min);
	void UpdateMax(const glm::vec2& _max);

};


