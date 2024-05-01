#pragma once
#include "presets.h"
#include "barstrucs.h"

#define CREATE_PINDEX(X, Y, WID) (((Y) * (WID)) + (X))

enum class NextPoz : char
{
	middleRight = 1,
	bottomRight = 2,
	bottomCenter,
	bottomLeft
};

struct IndexCov
{
	bc::poidex offset = 0;
	float dist = 0;
	NextPoz poz;
	IndexCov(uint _offset = 0, float _dist = 0, NextPoz _vert = NextPoz::middleRight) : offset(_offset), dist(_dist), poz(_vert)
	{}

	bc::poidex getNextPoindex(int wid) const
	{
		int x = offset % wid;
		int y = offset / wid;
		switch (poz)
		{
		case NextPoz::middleRight:
			return CREATE_PINDEX(x + 1, y, wid);
		case NextPoz::bottomRight:
			return CREATE_PINDEX(x + 1, y + 1, wid);
		case NextPoz::bottomCenter:
			return CREATE_PINDEX(x, y + 1, wid);
		case NextPoz::bottomLeft:
			return CREATE_PINDEX(x - 1, y + 1, wid);
		}
	}
};


class MRNG
{
public:
	static float randf()
	{
		return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	}

	static bool coin()
	{
		return randf() < 0.5;
	}

	static int randi(int LO, int HI)
	{
		return LO + (rand() % (HI - LO));
	}

	static float randf(float LO, float HI)
	{
		return LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
	}
};
