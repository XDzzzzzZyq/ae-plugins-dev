#include "Block.h"

Block::Block()
{

}

void Block::UpdateMin(const glm::vec2& _min)
{
	if (_min.x < b_min.x) b_min.x = _min.x;
	if (_min.y < b_min.y) b_min.y = _min.y;
}

void Block::UpdateMax(const glm::vec2& _max)
{
	if (_max.x > b_max.x) b_max.x = _max.x;
	if (_max.y > b_max.y) b_max.y = _max.y;
}

void Block::Reset()
{
	b_min = glm::vec2{ 9999. };
	b_max = glm::vec2{ 0. };
}
