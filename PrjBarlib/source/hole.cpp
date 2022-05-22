#include "hole.h"
#include "barcodeCreator.h"


bc::Hole::Hole(point p1, BarcodeCreator* factory) : Component(factory)
{
	isValid = false;
	add(p1);
	index = factory->components.size() - 1;
}


bc::Hole::Hole(point p1, point p2, point p3, BarcodeCreator* factory) : Component(factory)
{
	isValid = true;
	//    zeroStart = p1;
		// ++factory->lastB;

	add(p1);
	add(p2);
	add(p3);
}


bc::Hole::~Hole()
{
	if (!isValid)
		Component::factory->components[index] = nullptr;
}


bool bc::Hole::getIsOutside() const
{
	return isOutside;
}


void bc::Hole::setShadowOutside(bool outside)
{
	isOutside = outside;
}


void bc::Hole::setOutside()
{
	if (!isOutside) {
		isOutside = true;
		if (isValid)
		{
			this->end = this->factory->curbright;
			Component::kill();
			Component::lived = true;

			// --this->factory->lastB;
		}
	}
}


void bc::Hole::kill()
{
	if (!isOutside && isValid)
	{
		this->end = this->factory->curbright;
		// --this->factory->lastB;

		Component::kill();

	}
	Component::lived = false;
}


inline bool bc::Hole::isContain(int x, int y)
{
	if (Component::factory->IS_OUT_OF_REG(x, y))
		return false;
	return Component::factory->getComp(Component::factory->GETOFF(x, y)) == this;
}


bool bc::Hole::isContain(bc::point p)
{
	if (Component::factory->IS_OUT_OF_REG(p.x, p.y))
		return false;
	return Component::factory->getComp(Component::factory->GETOFF(p.x, p.y)) == this;
}


bool bc::Hole::tryAdd(const point& p)
{
	if (isValid == false)
	{
		/*addCoord(p);
		return true;*/
		return false;
	}
	static char poss[9][2] = { { -1,0 },{ -1,-1 },{ 0,-1 },{ 1,-1 },{ 1,0 },{ 1,1 },{ 0,1 },{ -1,1 },{ -1,0 } };
	for (size_t i = 0; i < 8; ++i)
	{
		//Она соединяется только с соседними ближйми ребрами
		if (this->isContain((point)p + poss[i]) &&
			(this->isContain((point)p + poss[i + 1]) || (i % 2 == 0 && this->isContain((point)p + poss[i + 2])))
			)//есть ли нужное ребро
		{
			this->add(p);
			return true;
		}
	}
	return false;
}


inline void bc::Hole::add(const point& p)
{
	bool outDo = isOutside;
	//auto temp = bc::Component::factory->getComp();
	bc::Component::add(bc::Component::factory->GETOFF(p.x, p.y), p);
	//    setB(p);

	if (!isOutside)//ребро должно быть на границе
	{
		if ((p.x == 0 || p.x == this->factory->wid - 1) &&
			(this->isContain(p.x, p.y - 1) || this->isContain(p.x, p.y + 1)))
		{
			isOutside = true;
		}
		else if ((p.y == 0 || p.y == this->factory->hei - 1) &&
			(this->isContain(p.x - 1, p.y) || this->isContain(p.x + 1, p.y)))
		{
			isOutside = true;
		}

		if (isOutside != outDo)
		{
			this->end = this->factory->curbright;
			// --this->factory->lastB;
			Component::kill();
			Component::lived = true;
		}
	}
	//    ++size;
}


bool bc::Hole::checkValid(point p)
{
	if (this->getTotalSize() < 3)
		return false;

	if (isValid)
		return true;

	static char poss[9][2] = { { -1,0 },{ -1,-1 },{ 0,-1 },{ 1,-1 },{ 1,0 },{ 1,1 },{ 0,1 },{ -1,1 },{ -1,0 } };
	//эти сочетания могу образовывать дубли, поэтому перед добавление СЛЕДУЕТ ПРОВЕРЯТЬ, был ли уже добавлен такой треугольник

	//точка p в цетре. Сканирем ребра вокруг точки
	for (size_t i = 0; i < 8; ++i)
	{
		//Она соединяется только с соседними ближйми ребрами
		//FIXME Same expression on both sides of '||'.
		if (this->isContain(p + poss[i]) && (this->isContain(p + poss[i + 1]) || this->isContain(p + poss[i + 1])))//есть ли нужное ребро
		{
			isValid = true;
			return true;
		}
	}
	return false;
}


//явяется ли точка точкой соединения двух дыр - рис1

bool bc::Hole::findCross(point p, bc::Hole* hole)
{
	static char poss[5][2] = { { -1,0 },{ 0,-1 },{ 1,0 },{ 0,1 },{ -1,0 } };
	static char poss2[5][2] = { { -1,-1 },{ 1,-1 },{ 1,1 },{ -1,1 } };
	//static char poss[9][2] = { { -1,0 },{ -1,-1 },{ 0,-1 },{ 1,-1 },{ 1,0 },{ 1,1 },{ 0,1 },{ -1,1 }};
	if (this->isContain(p) && hole->isContain(p))
	{
		//ВАЖНО! Две дыры можно соединить если у ниъ есть 2 общие точки. Если мы знаем одну, то вторая должна быть четко над/под/слева/справа от предыдужей.
		//При дургом расопложении дыры не соединятся
		for (size_t i = 0; i < 4; ++i)
		{
			if (this->isContain(p + poss[i]) && hole->isContain(p + poss[i]))//если есть ребро, оразующиееся из разных дыр
				return true;

			//************cdoc 15**************
			if (this->isContain(p + poss[i]) && hole->isContain(p + poss[i + 1]))//если есть ребро, оразующиееся из разных дыр
				return true;
			if (this->isContain(p + poss[i + 1]) && hole->isContain(p + poss[i]))//если есть ребро, оразующиееся из разных дыр
				return true;
			//*********************************
		}
		//**********CDOC 590**************
		for (size_t i = 0; i < 4; ++i)
		{
			if (this->isContain(p + poss2[i]) && hole->isContain(p + poss2[i]))//если есть ребро, оразующиееся из разных дыр
				return true;
		}
		//********************************
	}
	return false;
}

