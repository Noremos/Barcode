#pragma once

#include "GenCommon.h"
#include "barImg.h"
#include "CellMachine.h"

using IncludeCell = CellMachine*;

using Cells = std::vector<CellMachine>;

#define CREATE_PINDEX(X, Y, WID) (((Y) * (WID)) + (X))
class MachinePRocessor
{
	std::vector<IncludeCell> included;
	std::vector<IncludeCell> machines;

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

				data[k++] = IndexCov(offset, dist, NextPoz::middleRight);

				// bottom
				// c 0
				// n 0
				next = workingImg.get(w, h + 1);
				dist = cur.val_distance(next);

				data[k++] = IndexCov(offset, dist, NextPoz::bottomCenter);

				// bottom rigth
				// c 0
				// 0 n
				next = workingImg.get(w + 1, h + 1);
				dist = cur.val_distance(next);

				data[k++] = IndexCov(offset, dist, NextPoz::bottomRight);


				// 0 c
				// n 0
				cur = workingImg.get(w + 1, h);
				next = workingImg.get(w, h + 1);
				dist = cur.val_distance(next);
				offset = wid * h + w + 1;

				data[k++] = IndexCov(offset, dist, NextPoz::bottomLeft);
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
			data[k++] = IndexCov(offset, dist, NextPoz::bottomCenter);
		}

		int hd = hei - 1;
		for (int w = 0; w < wid - 1; ++w)
		{
			bc::poidex offset = wid * hd + w;
			Barscalar cur = workingImg.get(w, hd);
			Barscalar next;
			next = workingImg.get(w + 1, hd);
			dist = cur.val_distance(next);
			data[k++] = IndexCov(offset, dist, NextPoz::middleRight);
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

	bool createMachine(const bc::DatagridProvider& img, bc::poidex& ind)
	{
		barvalue val = getBarvalue(img, ind);
		if (seed && seed->start.val_distance(val.value) > seed->maxDiff)
		{
			return false;
		}

		CellMachine* no = new CellMachine(val, seed, simpleUchar);
		included[ind] = no;
		machines.push_back(no);

		auto curpoint = getPoint(ind);
		auto col = no->color.toCvVec();
		auto drawCol = drawMat.at<cv::Vec3b>(curpoint.y, curpoint.x);
		cv::Vec3b newCol = (drawCol / 2) + (col / 2);
		drawMat.at<cv::Vec3b>(curpoint.y, curpoint.x) = newCol;

		return true;
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
		included.resize(len);
		std::fill_n(included.data(), len, nullptr);

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
				bool added = false;
				if (createMachine(img, fiirstPoi))
				{
					CellMachine* first = included[fiirstPoi];
					added = first->tryAdd(getBarvalue(img, secondPoi), len);
				}
				if (!added)
				{
					createMachine(img, secondPoi);
				}
			}
		}
		if (seed && seed->accure > 10)
		{
			cv::namedWindow("cells", cv::WINDOW_NORMAL);
			cv::imshow("cells", drawMat);
			cv::waitKey(1);
		}
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
			bool incSet = included[i] != NULL;
			bool maskMark = mask.getLiner(i) == 255;
			if (maskMark != incSet)
			{
				--totalAccur;
			}
			else if (maskMark)
			{
				++totalAccur;
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

		const int bestSize = 20;
		const int populationSize = 100;
		const int maxGenNumber = 10;
		const bool crossover = true;

		bool is3d = img.type == BarType::BYTE8_3;
		vector<CellMachine> population;
		CellMachine bestCells[bestSize];

		//{
		//	MachinePRocessor procBase;
		//	procBase.process(img);
		//	procBase.calculateAccure(mask, population);
		//}

		int oldSize = population.size();
		population.resize(populationSize);
		for (size_t i = oldSize; i < populationSize; i++)
		{
			if (is3d)
				population[i].initRandomVec3b();
			else
				population[i].initRandomUchar();
		}

		std::cout << "Inited " << population.size() << " base population cells" << std::endl;
		for (size_t i = 0; i < maxGenNumber; i++)
		{
			std::cout << "Porcessing " << population.size() << " populations" << std::endl;

			for (auto& machine : population)
			{
				MachinePRocessor procBase;
				procBase.seed = &machine;
				procBase.process(img);
				machine.accure = procBase.calculateTotalAccure(mask);
				machine.clear();
			}

			int getLEst = bestSize / 2;
			//int getLEst = 0;
			std::cout << "Sorting population cells... " << std::endl;
			std::sort(population.begin(), population.end(), [](const CellMachine& a, const CellMachine& b) {
				return a.accure > b.accure;
				});
			for (int j = 0; j < getLEst; j++)
			{
				bestCells[j] = population[j];
			}

			for (size_t i = bestSize / 2; i < bestSize; i++)
			{
				int ind0 = 0, ind1 = 0, ind2 = 0;
				while (ind0 == ind1 || ind0 == ind2 || ind1 == ind2)
				{
					ind0 =  CellMachine::randi(bestSize / 2, populationSize);
					ind1 = CellMachine::randi(bestSize / 2, populationSize);
					ind2 = CellMachine::randi(bestSize / 2, populationSize);
					CellMachine* cells[3] = { &population[ind0],&population[ind1],&population[ind2] };
					std::sort(cells, cells + 3, [](const CellMachine* a, const CellMachine* b) {
						return a->accure < b->accure;
						});
					bestCells[i] = *cells[0];
				}
			}

			float avgFitness = 0.f;
			float maxS = -100000;
			for (auto& machine : population)
			{
				avgFitness += machine.accure;
				if (machine.accure > maxS)
					maxS = machine.accure;
			}
			avgFitness /= populationSize;
			std::cout << "Gen no. "<< i << ". Avg futness: " << avgFitness << "; Max: " << maxS << std::endl;

			for (int j = 0; j < bestSize; j++)
			{
				bestCells[j] = population[j];
				//if (rand() % 100 < 10)
				//	bestCells[j].mutate(0.5f, 0.4f);
				//else
				//	bestCells[j].mutate(0.01f, 0.1f);
				if (rand() % 100 < 10)
					bestCells[j].mutate(1.f / 4, 0.1f);

				std::cout << "b no." << "j; a:" << bestCells[j].accure
					<< "; md: " << bestCells[j].maxDiff
					<< "; mtl:" << bestCells[j].maxTotalLen
					<< "; ms:" << bestCells[j].totalMtrixCount
					<< "; str:" << bestCells[j].start.text()
					<< endl;
			}
			std::cout << "Mutated " << bestSize << " cells" << std::endl;

			for (auto& machine : population)
			{
				if (rand() % 100 >= 90)
				{
					int fs = rand() % bestSize;
					fs = fs - fs % 2;
					int sc = rand() % bestSize;
					if (fs % 2 == 0)
					{
						fs += 1;
						if (fs >= bestSize)
							fs -= 2;
					}

					CellMachine::combineOne(bestCells[fs], bestCells[sc], machine);
				}
				else
				{
					machine = bestCells[rand() % bestSize];
				}
			}
			std::cout << "Created new population from best " << bestSize << " cells" << std::endl;
		}

		for (int i = 0; i < bestSize; i++)
		{
			std::cout << "Best no. " << i << "Futness: " << bestCells[i].accure << std::endl;

			bestCells[i].accure = 11;
			MachinePRocessor procBase;
			procBase.seed = &bestCells[i];
			procBase.process(img);
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