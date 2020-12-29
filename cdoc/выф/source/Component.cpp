#include "point.h"
#include "Component.h"
bc::Component::Component(int len)
{
	coords = pmap((size_t)len);
}

bc::Component::Component(int x, int y)
{
	coords = pmap();
    add(point(x, y));
}

bool bc::Component::isContain(int x, int y)
{
	auto  it = coords.find(point(x, y));
	return (it != coords.end());
}

bool bc::Component::isContain(point p)
{
	auto  it = coords.find(p);
	return (it != coords.end());
}

inline void bc::Component::add(point p)
{
	coords.insert(ppair(p, true));
}

bc::Component::~Component()
{
	coords.clear();
}
