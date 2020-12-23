#include "Locator.hpp"

Locator::Locator()
{
}

Locator::~Locator()
{
	Clear();
}

void Locator::Clear()
{
	m_vInstances.clear();
	m_vCreators.clear();
}