#include "point.h"

bc::point::point()
{
	x = -1; y = -1;
}

bc::point::point(int x, int y) :x(x), y(y)
{
}

void bc::point::init(int x, int y)
{
	this->x = x;
	this->y = y;
}

void bc::point::init(int p[2])
{
	this->x = p[0];
	this->y = p[1];
}

cv::Point bc::point::cvPoint()
{
	return cv::Point(x, y);
}