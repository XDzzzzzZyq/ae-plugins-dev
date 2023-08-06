#include "Block.h"

Block::Block()
{

}

void Block::UpdateMin(const glm::vec2& _max)
{
	if (_max < b_max) b_max = _max;
}

void Block::UpdateMax(const glm::vec2& _max)
{
	if (_max > b_max) b_max = _max;
}
