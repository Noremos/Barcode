#pragma once
#include "presets.h"
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
		totalMtrixCount = rand() % 90;
		initRandomColor();
	}

	void initRandomVec3b()
	{
		maxDiff = rand() % 384;
		maxTotalLen = rand() % 768;
		totalMtrixCount = rand() % 90;
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
		if (matr.size() / imgLen > totalMtrixCount)
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
			color.data.b3[1] += rand() % 10;
			color.data.b3[2] += rand() % 10;
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
		for (size_t i = 0; i < 3; i++) // dont change start
		{
			k = i * 2 + (rand() % 2);
			assert(k < 6);
			outChild.inheritParam(fs, sc, k);
		}
		outChild.inheritColor(fs, sc, true);
	}
};
