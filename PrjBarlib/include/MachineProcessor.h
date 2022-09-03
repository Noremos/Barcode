#pragma once

#include "presets.h"
#include "barstrucs.h"
#include "barImg.h"
#include "CellMachine.h"

using IncludeCell = CellMachine*;

using Cells = std::vector<CellMachine>;

#define CREATE_PINDEX(X, Y, WID) (((Y) * (WID)) + (X))
class MachinePRocessor
{
	std::vector<IncludeCell> included;
	std::vector<IncludeCell> machines;

	enum NextPoz : char
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
		IndexCov(uint _offset = 0, float _dist = 0, NextPoz _vert = middleRight) : offset(_offset), dist(_dist), poz(_vert)
		{}

		bc::poidex getNextPoindex(int wid) const
		{
			int x = offset % wid;
			int y = offset / wid;
			switch (poz)
			{
			case middleRight:
				return CREATE_PINDEX(x + 1, y, wid);
			case bottomRight:
				return CREATE_PINDEX(x + 1, y + 1, wid);
			case bottomCenter:
				return CREATE_PINDEX(x, y + 1, wid);
			case bottomLeft:
				return CREATE_PINDEX(x - 1, y + 1, wid);
			}
		}
	};

	IndexCov* sortPixelsByRadius(const bc::DatagridProvider& workingImg, size_t& totalSize, float maxRadius, size_t minCount)
	{
		int wid = workingImg.wid();
		int hei = workingImg.hei();

		totalSize = 4 * static_cast<size_t>(wid) * hei + wid + hei;
		float dist;
		IndexCov* data = new IndexCov[totalSize];
		// Сичтаем расстояние между всеми соседними пикселями для каждого пикселя.
		// Чтобы не считать повтороно, от текущего проверяем только уникальные - в форме отражённой по вертикали буквы "L"

		int k = 0;
		for (int h = 0; h < hei - 1; ++h)
		{
			for (int w = 0; w < wid - 1; ++w)
			{
				bc::poidex offset = wid * h + w;
				Barscalar cur = workingImg.get(w, h);
				Barscalar next;

				// rigth
				// c n
				// 0 0
				next = workingImg.get(w + 1, h);
				dist = cur.val_distance(next);

				data[k++] = IndexCov(offset, dist, middleRight);

				// bottom
				// c 0
				// n 0
				next = workingImg.get(w, h + 1);
				dist = cur.val_distance(next);

				data[k++] = IndexCov(offset, dist, bottomCenter);

				// bottom rigth
				// c 0
				// 0 n
				next = workingImg.get(w + 1, h + 1);
				dist = cur.val_distance(next);

				data[k++] = IndexCov(offset, dist, bottomRight);


				// 0 c
				// n 0
				cur = workingImg.get(w + 1, h);
				next = workingImg.get(w, h + 1);
				dist = cur.val_distance(next);
				offset = wid * h + w + 1;

				data[k++] = IndexCov(offset, dist, bottomLeft);
			}
		}

		int wd = wid - 1;
		for (int h = 0; h < hei - 1; ++h)
		{
			bc::poidex offset = wid * h + wd;
			Barscalar cur = workingImg.get(wd, h);
			Barscalar next;
			next = workingImg.get(wd, h + 1);
			dist = cur.val_distance(next);
			data[k++] = IndexCov(offset, dist, bottomCenter);
		}

		int hd = hei - 1;
		for (int w = 0; w < wid - 1; ++w)
		{
			bc::poidex offset = wid * hd + w;
			Barscalar cur = workingImg.get(w, hd);
			Barscalar next;
			next = workingImg.get(w + 1, hd);
			dist = cur.val_distance(next);
			data[k++] = IndexCov(offset, dist, middleRight);
		}
		// Тип не важен потому что соединяем не по яркости

		std::sort(data, data + k, [](const IndexCov& a, const IndexCov& b) {
			return a.dist < b.dist;
			});

		totalSize = MIN(minCount, k);
		size_t i;
		for (i = 0; i < k; i++)
		{
			if (data[i].dist > maxRadius && i > minCount)
			{
				totalSize = i;
				break;
			}
		}

		//totalSize = MIN(totalSize, 500);
		return data;
	}

	int wid;
	//bc::poidex curpoindex;
	//bc::point curpix;
	//Barscalar curbright;
	bool simpleUchar;
	CellMachine* seed = nullptr;

	bc::point getPoint(bc::poidex i) const
	{
		return bc::point(i % wid, i / wid);
	}

	bc::barvalue getBarvalue(const bc::DatagridProvider& img, bc::poidex ind)
	{
		return bc::barvalue(getPoint(ind), img.getLiner(ind));
	}

	void createMachine(const bc::DatagridProvider& img, bc::poidex& ind)
	{
		CellMachine* no = new CellMachine(getBarvalue(img, ind), seed, simpleUchar);
		included[ind] = no;
		machines.push_back(no);

		auto curpoint = getPoint(ind);
		auto col = no->color.toCvVec();
		auto drawCol = drawMat.at<cv::Vec3b>(curpoint.y, curpoint.x);
		cv::Vec3b newCol = (drawCol / 2) + (col / 2);
		drawMat.at<cv::Vec3b>(curpoint.y, curpoint.x) = newCol;
	}

	Mat drawMat;
	float maxRadius = 50;
	int minMachines = 200;
	void process(const bc::DatagridProvider& img)
	{
		drawMat = convertRGBProvider2Mat(&img);
		//cv::cvtColor(drawMat, drawMat, cv::COLOR_BGR2BGRA);
		int len = img.wid() * img.hei();

		size_t toPocrss;
		wid = img.wid();
		simpleUchar = img.type == BarType::BYTE8_1;
		IndexCov* toPorcData = sortPixelsByRadius(img, toPocrss, maxRadius, minMachines);
		included.resize(toPocrss);
		std::fill_n(included.data(), toPocrss, nullptr);

		for (size_t i = 0; i < toPocrss; i++)
		{
			auto fiirstPoi = toPorcData[i].offset;
			auto secondPoi = toPorcData[i].getNextPoindex(wid);

			CellMachine* first = included[fiirstPoi];
			CellMachine* second = included[secondPoi];

			if (first != nullptr && second == nullptr)
			{
				if (!first->tryAdd(getBarvalue(img, secondPoi), len))
				{
					createMachine(img, secondPoi);
				}
			}
			else if (first == nullptr && second != nullptr)
			{
				if (!second->tryAdd(getBarvalue(img, fiirstPoi), len))
				{
					createMachine(img, secondPoi);
				}
			}
			else if (first == nullptr && second == nullptr)
			{
				createMachine(img, fiirstPoi);
				CellMachine* first = included[fiirstPoi];
				bool res = first->tryAdd(getBarvalue(img, secondPoi), len);
				if (!res)
				{
					createMachine(img, secondPoi);
				}
			}
		}
		cv::namedWindow("cells", cv::WINDOW_NORMAL);
		cv::imshow("cells", drawMat);
		cv::waitKey(1);
		delete[] toPorcData;
	}

	void calculateAccure(const bc::DatagridProvider& mask, Cells& out, float procToGet = 0.05)// 5%
	{
		for (size_t i = 0; i < machines.size(); i++)
		{
			machines[i]->calculateAccure(mask);
		}

		std::sort(machines.begin(), machines.end(), [](const CellMachine* a, const CellMachine* b) {
			return a->accure > b->accure;
			});

		int count = machines.size() * procToGet + 1;
		for (size_t i = 0; i < count; i++)
		{
			out.push_back(machines[i]->clear());
		}
	}

	int calculateTotalAccure(const bc::DatagridProvider& mask)
	{
		int totalAccur = 0;
		for (size_t i = 0; i < included.size(); i++)
		{
			if (included[i])
			{
				bool maskMark = mask.getLiner(i) == 255;
				totalAccur += (maskMark ? 1 : -1);
			}
		}

		return totalAccur;
	}

	static void combine(const Cells& first, const Cells& second, Cells& out)
	{
		int countF = first.size();
		int countS = second.size();

		for (size_t i = 0; i < countF; i++)
		{
			for (size_t j = 0; j < countS; j++)
			{
				CellMachine no;
				CellMachine::combineOne(first[i], second[j], no);
				out.push_back(no);
			}
		}
	}

	~MachinePRocessor()
	{
		for (size_t i = 0; i < machines.size(); i++)
		{
			delete machines[i];
		}
	}

public:
	static void fullProcess2(const bc::DatagridProvider& img, const bc::DatagridProvider& mask)
	{
		// 1 first random run, get the best;
		// 2 second run, get the best
		// 3 Compbine frist and sec -> combined.
		// 4 Process with output machines
		// 5 New random run, get the best
		// 6 Combine combined and random, get the best
		// 7 Go to step 4
		srand(time(NULL));

		Cells baseCells;
		for (size_t i = 0; i < 100; i++)
		{
			CellMachine mach(img.type == BarType::BYTE8_3);
			MachinePRocessor procBase;
			procBase.seed = &mach;
			procBase.process(img);
			mach.accure = procBase.calculateTotalAccure(mask);
			baseCells.push_back(mach);
		}

		std::cout << "Got " << baseCells.size() << " base cells" << std::endl;

		for (size_t i = 0; i < 200; i++)
		{
			Cells randomCells;
			for (size_t k = 0; k < 5; k++)
			{
				CellMachine mach(img.type == BarType::BYTE8_3);

				MachinePRocessor procRandom;
				procRandom.seed = &mach;
				procRandom.process(img);
				mach.accure = procRandom.calculateTotalAccure(mask);
				randomCells.push_back(mach);
			}
			std::cout << "Got " << randomCells.size() << " random cells" << std::endl;


			Cells out;
			MachinePRocessor::combine(baseCells, randomCells, out);
			std::cout << "Get " << out.size() << " inhed cells" << std::endl;
			randomCells.clear();
			baseCells.clear();

			std::cout << "Calculating accure of the inheds cell... " << std::endl;

			for (size_t k = 0; k < out.size(); k++)
			{
				MachinePRocessor proc;
				proc.seed = &out.at(k);
				proc.process(img);
				out.at(k).accure = proc.calculateTotalAccure(mask);
			}

			std::cout << "Sorting inheds cell... " << std::endl;
			std::sort(out.begin(), out.end(), [](const CellMachine& a, const CellMachine& b) {
				return a.accure > b.accure;
				});

			int nsize = out.size() * 0.10;
			std::cout << "Get 10% of inheds cell. The new size is " << nsize << std::endl;

			if (nsize == 0)
			{
				baseCells = out;
				break;
			}
			out.resize(nsize);
		}

		CellMachine outCell = baseCells[0];
		{

			MachinePRocessor proc;
			proc.seed = &outCell;
			proc.process(img);
			waitKey(0);
		}
	}

	static void fullProcess(const bc::DatagridProvider& img, const bc::DatagridProvider& mask)
	{
		// 1 first random run, get the best;
		// 2 second run, get the best
		// 3 Compbine frist and sec -> combined.
		// 4 Process with output machines
		// 5 New random run, get the best
		// 6 Combine combined and random, get the best
		// 7 Go to step 4


		Cells baseCells;
		{
			MachinePRocessor procBase;
			procBase.process(img);
			procBase.calculateAccure(mask, baseCells);
		}

		for (size_t id = 0; id < 200; id++)
		{
			Cells randomCells;
			{
				MachinePRocessor procRandom;
				procRandom.process(img);
				procRandom.calculateAccure(mask, randomCells);
			}

			Cells out;
			MachinePRocessor::combine(baseCells, randomCells, out);

			for (size_t i = 0; i < out.size(); i++)
			{
				MachinePRocessor proc;
				proc.seed = &out.at(i);
				proc.process(img);
				out.at(i).accure = proc.calculateTotalAccure(mask);
			}
			std::sort(out.begin(), out.end(), [](const CellMachine& a, const CellMachine& b) {
				return a.accure > b.accure;
				});

			int nsize = out.size() * 0.10;
			if (nsize == 0)
			{
				return;
			}
			out.resize(nsize);
			baseCells = out;
		}
	}
};