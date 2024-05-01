#pragma once
#include "barstrucs.h"
#include "barImg.h"

class CellMachine
{
	bc::barvector matr;

public:
	// Факторы поведения
	float maxDiff; // Разница между соседними
	float maxTotalLen; // Длина всей компоненты
	float totalMtrixCount; // Кол-во в процентах
	Barscalar start; // Non-random

	Barscalar color;
	// ---------------

	// Показатели приспобленности
	float accure;
	// ---------------

	
	CellMachine()
	{
		accure = 0;
	
		maxDiff = 0;
		maxTotalLen = 0;
		totalMtrixCount = 0;
		start = 0;
		color = 0;
	}

	CellMachine(bc::barvalue& satrt)
	{
		accure = 0;

		maxDiff = 0;
		maxTotalLen = 0;
		totalMtrixCount = 0;
		initRandomColor();

		matr.push_back(satrt);
		start = satrt.value;
	}


	CellMachine(bool onlyUchar)
	{
		accure = 0;
		if (onlyUchar)
			initRandomUchar();
		else
			initRandomVec3b();
	}

	CellMachine(const bc::barvalue& satrt, CellMachine* base, bool onlyUchar)
	{
		accure = 0;
		if (base)
		{
			maxDiff = base->maxDiff;
			maxTotalLen = base->maxTotalLen;
			totalMtrixCount = base->totalMtrixCount;
			color = base->color;
		}
		else if (onlyUchar)
			initRandomUchar();
		else
			initRandomVec3b();

		matr.push_back(satrt);
		start = satrt.value;
	}

	int matrSize()
	{
		return matr.size();
	}

	void calculateAccure(const bc::DatagridProvider& mask)
	{
		for (size_t w = 0; w < matr.size(); ++w)
		{
			if (mask.get(matr[w].getPoint()) == 255)
			{
				++accure;
			}
		}
	}

	void initRandomUchar()
	{
		maxDiff = rand() % 128;
		maxTotalLen = rand() % 256;
		totalMtrixCount = randf() * 0.5;
		uchar randCol = rand() % 256;
		start = Barscalar(randCol, BarType::BYTE8_3);
		initRandomColor();
	}

	void initRandomVec3b()
	{
		maxDiff = rand() % 128;
		maxTotalLen = rand() % 768;
		totalMtrixCount = randf() * 0.5;
		start = Barscalar(rand() % 256, rand() % 256, rand() % 256);
		initRandomColor();
	}

	void initRandomColor()
	{
		color = Barscalar(rand() % 256, rand() % 256, rand() % 256);
	}

	void inheritParam(const CellMachine& fs, const CellMachine& sc, int k)
	{
		switch (k)
		{
		case 0:
			maxDiff = fs.maxDiff;
			break;
		case 1:
			maxDiff = sc.maxDiff;
			break;
		case 2:
			maxTotalLen = fs.maxTotalLen;
			break;
		case 3:
			maxTotalLen = sc.maxTotalLen;
			break;
		case 4:
			totalMtrixCount = fs.totalMtrixCount;
			break;
		case 5:
			totalMtrixCount = sc.totalMtrixCount;
			break;
		case 6:
			start = fs.start;
			break;
		case 7:
			start = sc.start;
			break;
		default:
			assert(false);
			break;
		}
	}

	bool coin()
	{
		return rand() % 2 == 0;
	}

	// Living

	bool tryAdd(const bc::barvalue& colval, int imgLen)
	{
		if (matr.back().value.val_distance(colval.value) > maxDiff)
		{
			return false;
		}
		if (start.val_distance(colval.value) > maxTotalLen)
		{
			return false;
		}
		if ((float)matr.size() / (float)imgLen > totalMtrixCount)
		{
			return false;
		}
		
		matr.push_back(colval);
		return true;
	}

	CellMachine& clear()
	{
		matr.clear();
		return *this;
	}

	// Inherit

	void inheritColor(const CellMachine& fs, const CellMachine& sc, bool mutate)
	{
		color = coin() == 0 ? fs.color : sc.color;
		color = fs.accure > sc.accure ? fs.color : sc.color;
		if (mutate)
		{
			for (size_t i = 0; i < 3; i++)
			{
				int ind = rand() % 3;
				if (coin())
					color.data.b3[ind] += rand() % 10;
				else
					color.data.b3[ind] -= rand() % 10;

				if (coin())
					break;
			}
		}
	}

	static int randi(int LO, int HI)
	{
		return LO + (rand() % (HI - LO));
	}

	static float randf()
	{
		return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	}

	static float randf(float LO, float HI)
	{
		return LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
	}
	
	void mutate(float chance, float rate)
	{
		if (randf() < chance)
		{
			maxDiff += randf(-maxDiff * rate, maxDiff * rate);
		}

		if (randf() < chance)
		{
			maxTotalLen += randf(-maxTotalLen * rate, maxTotalLen * rate);
		}

		if (randf() < chance)
		{
			totalMtrixCount += randf(-totalMtrixCount * rate, totalMtrixCount * rate);
		}

		for (size_t i = 0; i < 3; i++)
		{
			int ind = rand() % 3;
			if (coin())
				color.data.b3[ind] += rand() % (int)(40 * rate);
			else
				color.data.b3[ind] -= rand() % (int)(40 * rate)	;

			if (coin())
				break;
		}

	}

	static void combine(const CellMachine& fs, const CellMachine& sc, std::vector<CellMachine>& children)
	{
#define CHLBIT(C, R) (C & R != 0 ? 1 : 0)
		int k = 0;
		for (char chl = 0; chl < 16; chl++) // Все биты установлены - это 15
		{
			// Перебором должны установить все возможны параметры
			CellMachine no = fs;
			no.inheritParam(fs, sc, 0 + CHLBIT(chl, 1));
			no.inheritParam(fs, sc, 2 + CHLBIT(chl, 2));
			no.inheritParam(fs, sc, 4 + CHLBIT(chl, 4));
			no.inheritParam(fs, sc, 6 + CHLBIT(chl, 8));
			no.inheritColor(fs, sc, true);
			children.push_back(no);
		}
	}

	static void combineOne(const CellMachine& fs, const CellMachine& sc, CellMachine& outChild)
	{
		int k = 0;
		for (size_t i = 0; i < 4; i++) // dont change start
		{
			k = i * 2 + (rand() % 2);
			assert(k < 8);
			outChild.inheritParam(fs, sc, k);
		}
		outChild.inheritColor(fs, sc, true);
	}
};
