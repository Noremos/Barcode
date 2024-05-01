#pragma once

#include "GenCommon.h"

#include "barImg.h"
#include "CellAutomat.h"

using IncludeACell = CellAutomat*;

using AutoCells = std::vector<CellAutomat>;

class CellAutomatProcessor
{
	std::vector<IncludeACell> included;
	std::vector<IncludeACell> machines;

	IndexCov* sortPixelsByRadius(const bc::DatagridProvider& workingImg, size_t& totalSize, float maxRadius, size_t minCount)
	{
		int wid = workingImg.wid();
		int hei = workingImg.hei();

		totalSize = 4 * static_cast<size_t>(wid) * hei;
		float dist;
		IndexCov* data = new IndexCov[totalSize];
		// Сичтаем расстояние между всеми соседними пикселями для каждого пикселя.
		// Чтобы не считать повтороно, от текущего проверяем только уникальные - в форме отражённой по вертикали буквы "L"

		int k = 0;
		for (int h = 1; h < hei - 2; ++h)
		{
			for (int w = 1; w < wid - 2; ++w)
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
	NN* seed = nullptr;
	bc::barvector matr;
	CellAutomat cllBas;

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
		bc::barvalue val = getBarvalue(img, ind);

		if (!tryAdd(seed, img, ind))
		{
			return false;
		}

		CellAutomat* no = &cllBas;
		included[ind] = no;
		//machines.push_back(no);

		auto curpoint = getPoint(ind);
		auto col = seed->color.toCvVec();
		auto& drawCol = drawMat.at<cv::Vec3b>(curpoint.y, curpoint.x);
		cv::Vec3b newCol = (drawCol / 2) + (col / 2);
		drawMat.at<cv::Vec3b>(curpoint.y, curpoint.x) = newCol;

		return true;
	}


	bool tryAdd(NN* no, const bc::DatagridProvider& img, bc::poidex& ind)
	{
		static char poss[9][2] = { { -1,-1 },{ 0,-1 },{ 1,-1 },{ -1,0 },{0, 0}, {1, 0}, { 1,1 },{ 0,1 },{ -1,1 } };
		std::array<Barscalar, 9> input;
		bc::point curpix = getPoint(ind);

		if (seed->width == 9)
		{
			// uchar
			float vals[9];
			for (char i = 0; i < 9; i++)
			{
				bc::point IcurPoint(curpix + poss[i]);
				input[i] = img.get(IcurPoint);
				vals[i] = static_cast<float>(input[i].data.b1) / 255.f;
			}
			if (seed->fit(vals))
			{
				return true;
			}
		}
		else
		{
			float vals[9 * 3];
			int k = 0;
			for (char i = 0; i < 9; i++)
			{
				bc::point IcurPoint(curpix + poss[i]);
				input[i] = img.get(IcurPoint);
				vals[k++] = static_cast<float>(input[i].data.b3[0]) / 255.f;
				vals[k++] = static_cast<float>(input[i].data.b3[1]) / 255.f;
				vals[k++] = static_cast<float>(input[i].data.b3[2]) / 255.f;
			}
			if (seed->fit(vals))
			{
				//matr.push_back(colval);
				return true;
			}
		}

		return false;
	}


public:
	Mat drawMat;
	float maxRadius = 50;
	int minMachines = 200;
	IndexCov* toPorcData;
	size_t toPocrss;

	void init(const bc::DatagridProvider& img)
	{
		drawMat = convertRGBProvider2Mat(&img);
		//cv::cvtColor(drawMat, drawMat, cv::COLOR_BGR2BGRA);
		int len = img.wid() * img.hei();

		wid = img.wid();
		simpleUchar = img.type == BarType::BYTE8_1;
		toPorcData = sortPixelsByRadius(img, toPocrss, maxRadius, minMachines);
		included.resize(len);
	}

	void process(const bc::DatagridProvider & img)
	{
		int len = img.wid() * img.hei();

		machines.clear();
		std::fill_n(included.data(), len, nullptr);
		for (size_t i = 0; i < toPocrss; i++)
		{
			auto fiirstPoi = toPorcData[i].offset;
			auto secondPoi = toPorcData[i].getNextPoindex(wid);

			CellAutomat* first = included[fiirstPoi];
			CellAutomat* second = included[secondPoi];

			if (first != nullptr && second == nullptr)
			{
				if (!tryAdd(seed, img, secondPoi))
				{
					createMachine(img, secondPoi);
				}
			}
			else if (first == nullptr && second != nullptr)
			{
				if (!tryAdd(seed, img, fiirstPoi))
				{
					createMachine(img, secondPoi);
				}
			}
			else if (first == nullptr && second == nullptr)
			{
				bool added = false;
				if (createMachine(img, fiirstPoi))
				{
					CellAutomat* first = included[fiirstPoi];
					added = tryAdd(seed, img, secondPoi);
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

	static void combine(const AutoCells& first, const AutoCells& second, AutoCells& out)
	{
		int countF = first.size();
		int countS = second.size();

		for (size_t i = 0; i < countF; i++)
		{
			for (size_t j = 0; j < countS; j++)
			{
				CellAutomat no;
				CellAutomat::combineOne(first[i], second[j], no);
				out.push_back(no);
			}
		}
	}

	~CellAutomatProcessor()
	{
		delete[] toPorcData;

		for (size_t i = 0; i < machines.size(); i++)
		{
			delete machines[i];
		}
	}

public:
	static void fullProcess2(const bc::DatagridProvider& img, const bc::DatagridProvider& mask)
	{
		srand(time(NULL));

		const int bestSize = 20;
		const int populationSize = 100;
		const int maxGenNumber = 10;

		bool is3d = img.type == BarType::BYTE8_3;
		std::vector<NN*> population;
		NN* bestCells[bestSize];
		for (char i = 0; i < bestSize; i++)
		{
			bestCells[i] = new NN();
		}

		int oldSize = population.size();
		population.resize(populationSize);
		for (size_t i = oldSize; i < populationSize; i++)
		{
			population[i] = new NN();
			if (is3d)
				population[i]->createLayers(9 * 3);
			else
				population[i]->createLayers(9);
		}

		std::cout << "Inited " << population.size() << " base population cells" << std::endl;
		for (size_t i = 0; i < maxGenNumber; i++)
		{
			std::cout << "Porcessing " << population.size() << " populations" << std::endl;

			CellAutomatProcessor procBase;
			procBase.init(img);

			for (auto* machine : population)
			{
				procBase.seed = machine;
				procBase.process(img);
				machine->accure = procBase.calculateTotalAccure(mask);
				std::cout << "done" << std::endl;
			}

			int getLEst = bestSize / 2;
			std::cout << "Sorting population cells... " << std::endl;
			std::sort(population.begin(), population.end(), [](const NN* a, const NN* b) {
				return a->accure > b->accure;
			});

			for (int j = 0; j < getLEst; j++)
			{
				bestCells[j]->copyFrom(*population[j]);
			}

			for (int i = bestSize / 2; i < bestSize; i++)
			{
				int ind0 = 0, ind1 = 0, ind2 = 0;
				while (ind0 == ind1 || ind0 == ind2 || ind1 == ind2)
				{
					ind0 = MRNG::randi(bestSize / 2, populationSize);
					ind1 = MRNG::randi(bestSize / 2, populationSize);
					ind2 = MRNG::randi(bestSize / 2, populationSize);
					NN* cells[3] = { population[ind0], population[ind1], population[ind2] };
					std::sort(cells, cells + 3, [](const NN* a, const NN* b) {
						return a->accure < b->accure;
						});
					bestCells[i]->copyFrom(*cells[0]);
				}

				//if (rand() % 100 < 10)
//	bestCells[j].mutate(0.5f, 0.4f);
//else
//	bestCells[j].mutate(0.01f, 0.1f);
				if (rand() % 100 < 10)
					bestCells[i]->Mutate(1.f / bestCells[i]->width, 0.1f);
			}

			std::cout << "Mutated " << bestSize << " cells" << std::endl;

			float avgFitness = 0.f;
			float maxS = -100000;

			for (NN* machine : population)
			{
				avgFitness += machine->accure;
				if (machine->accure > maxS)
					maxS = machine->accure;

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

					machine->reinit(bestCells[fs], bestCells[sc], 0.5f);
				}
				else
				{
					machine->copyFrom(*bestCells[rand() % bestSize]);
				}

			}
			avgFitness /= populationSize;
			std::cout << "Gen no. " << i << ". Avg futness: " << avgFitness << "; Max: " << maxS << std::endl;

			std::cout << "Created new population from best " << bestSize << " cells" << std::endl;
		}

		for (int i = 0; i < MIN(bestSize, 10); i++)
		{

			std::cout << "Best no. " << i << "Futness: " << bestCells[i]->accure << std::endl;
			bestCells[i]->accure = 11;
			CellAutomatProcessor procBase;
			procBase.seed = bestCells[i];
			procBase.process(img);
			cv::waitKey(0);
		}
	}
};
