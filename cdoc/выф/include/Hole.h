#pragma once
#include "component.h"
namespace bc
{

	class EXPORT Hole : public Component
	{
		int width, height;
	public:
		bool isOutside = false;
		bool isValid = false;

		Hole(point p1, point p2, point p3, int wid, int hei);
		Hole(point p1, int wid, int hei);
		//******************STATIC SIDE**********************



		//***************************************************

		void addAround();
		bool tryAdd(point p);
		//add withot cheks. NOT SAFE. not add if valid
		void add(point p);
		//************************************************************************//
		bool checkValid(point p);
		//добовляет точку в дыру, если она там не содержится. Возвращает 

		//явяется ли точка точкой соединения двух дыр
		bool findCross(point p, Hole* hole);
		~Hole();
	};
}
