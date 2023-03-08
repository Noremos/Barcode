// #pragma once

// #include "GenCommon.h"
// #include "barImg.h"
// #include "barcodeCreator.h"

// #include "D:\Programs\QT\binbar\MatrImg.h"

// #define CREATE_PINDEX(X, Y, WID) (((Y) * (WID)) + (X))



// struct InitConteiter
// {
// public:
// 	const bc::DatagridProvider& img;
// 	const bc::DatagridProvider& mask;
// 	barlinevector barlines;

// 	void set()
// 	{
// 		BarcodeCreator creator;
// 		bc::BarConstructor consrt;
// 		consrt.addStructure((bc::ProcType::invertf0), (bc::ColorType::rgb), bc::ComponentType::Component);
// 		consrt.createBinaryMasks = true;
// 		consrt.createGraph = true;
// 		consrt.returnType = ReturnType::barcode2d;

// 		bc::Barcontainer* bar = creator.createBarcode(&img, consrt);
// 		barlines = std::move(bar->getItem(0)->barlines);
// 		//bar->getItem(0)->barlines.clear();
// 		delete bar;
// 	}
// };


// class MPState
// {
// protected:
// 	void mutateColor(float chance, float rate)
// 	{
// 		for (char i = 0; i < 3; i++)
// 		{
// 			int ind = rand() % 3;
// 			int s = rand() % (int)(20 * rate);
// 			if (MRNG::randf() > chance)
// 			{
// 				if (color.data.b3[ind] + s < color.data.b3[ind])
// 				{
// 					color.data.b3[ind] = 255;
// 				}
// 				else
// 					color.data.b3[ind] = color.data.b3[ind] + s;
// 			}
// 			else
// 			{
// 				if (color.data.b3[ind] - s > color.data.b3[ind])
// 				{
// 					color.data.b3[ind] = 0;
// 				}
// 				else
// 					color.data.b3[ind] = color.data.b3[ind] - s;
// 			}
// 			if (rand() % 2 == 0)
// 				break;
// 		}

// 	}

// public:
// 	void initRandomColor()
// 	{
// 		color = Barscalar(rand() % 256, rand() % 256, rand() % 256);
// 	}


// 	Barscalar color;
// 	double accure = 0;
// 	virtual void calculate(const InitConteiter* content) = 0;

// 	virtual void initRandom(const InitConteiter* content) = 0;

// 	virtual void mutate(float rate) = 0;

// 	virtual void combine(const MPState* sc, MPState* out) = 0;

// 	virtual MatrImg getResult(const bc::DatagridProvider* img) = 0;

// 	void inheritColor(const MPState* sc, bool mutate)
// 	{
// 		color = rand() % 2 == 0 ? color : sc->color;
// 		if (mutate)
// 		{
// 			mutateColor(0.5, 0.5);
// 		}
// 	}
// };

// class SelectBestState : public MPState
// {
// public:
// 	// ������� ���������
// 	float totalMtrixCountMin; // ���-�� � ���������
// 	float totalMtrixCountMax; // ���-�� � ���������
// 	int minDepth;
// 	int maxDepth;
// 	Barscalar minStart; // Non-random
// 	Barscalar maxStart; // Non-random
// 	Barscalar minLen; // Non-random
// 	Barscalar maxLen; // Non-random

// private:
// 	BarType srcType;

// 	template<class T>
// 	static void mutateSingle(T& val, const T mine, T maxe, float chance, float rate)
// 	{
// 		T as = (maxe - mine);
// 		if (chance <= MRNG::randf())
// 		{
// 			T temp = val + MRNG::randf(-rate * as, rate * as);
// 			if (temp < mine)
// 				temp = mine;
// 			if (temp > maxe)
// 				temp = maxe;
// 			val = temp;
// 		}
// 	}

// 	static void mutateSingle(Barscalar& val, float chance, float rate)
// 	{
// 		if (chance <= MRNG::randf())
// 		{
// 			if (val.type == BarType::BYTE8_1)
// 			{
// 				val.data.b1 = (uchar)MIN(255, MAX(0, MRNG::randf(-rate * 255, rate * 255)));
// 			}
// 			else
// 			{
// 				for (char i = 0; i < 3; i++)
// 				{
// 					val.data.b3[i] = (uchar)MIN(255, MAX(0, (short)val.data.b3[i] +  MRNG::randf(-rate * 255, rate * 255)));
// 				}
// 			}
// 		}
// 	}


// 	static void mutateSingle(float& val, float chance, float rate)
// 	{
// 		if (chance <= MRNG::randf())
// 			val = MIN(1.0, MAX(0, val + MRNG::randf(-rate, rate)));
// 	}

// 	template<class TR>
// 	bool inRange(const TR& inp, const TR& ymin, const TR& ymax)
// 	{
// 		return ymin <= inp && inp <= ymax;
// 	}
// public:
// 	void preinit(BarType type)
// 	{
// 		srcType = type;
// 	}

// 	virtual void calculate(const InitConteiter* content)
// 	{
// 		accure = 0;

// 		const barlinevector& barlines = *lines;
// 		float totSize = content->img.length();
// 		for (barline* line : barlines)
// 		{
// 			float sizes = line->matr.size();
// 			if (inRange(sizes / totSize, totalMtrixCountMin, totalMtrixCountMax) &&
// 				inRange(line->getDeath(), minDepth, maxDepth) &&
// 				inRange(line->start, minStart, maxStart) &&
// 				inRange(line->len(), minLen, maxLen))
// 			{
// 				auto& matr = line->matr;
// 				for (size_t w = 0; w < matr.size(); ++w)
// 				{
// 					if (content->mask.get(matr[w].getPoint()) > 128)
// 					{
// 						accure += 1;
// 					}
// 					else
// 					{
// 						accure -= 2;
// 					}
// 				}
// 			}
// 		}
// 	}

// 	const barlinevector* lines;

// 	virtual void initRandom(const InitConteiter* content)
// 	{
// 		preinit(content->img.type);

// 		lines = &content->barlines;

// 		totalMtrixCountMin = 0;
// 		totalMtrixCountMax = 1.0;
// 		minDepth = 0;
// 		maxDepth = 20;
// 		minStart = Barscalar(0, srcType);;
// 		maxStart = Barscalar(255, srcType);
// 		minLen = Barscalar(0, srcType);;
// 		maxLen = Barscalar(255, srcType);

// 		initRandomColor();
// 	}

// 	virtual void mutate(float rate)
// 	{
// 		float chance = 1.f / 8;

// 		//const Barscalar minas(0, srcType);
// 		//const Barscalar maxas(255, srcType);

// 		mutateSingle(totalMtrixCountMin, chance, rate);
// 		mutateSingle(totalMtrixCountMax, chance, rate);
// 		if (totalMtrixCountMin > totalMtrixCountMax)
// 			totalMtrixCountMin = totalMtrixCountMax;
// 		mutateSingle(minDepth, 0, 20, chance, rate);
// 		mutateSingle(maxDepth, 0, 20, chance, rate);
// 		if (minDepth > maxDepth)
// 			minDepth = maxDepth;
// 		mutateSingle(minStart, chance, rate);
// 		mutateSingle(maxStart, chance, rate);
// 		if (minStart > maxStart)
// 			minStart = maxStart;
// 		mutateSingle(minLen, chance, rate);
// 		mutateSingle(maxLen, chance, rate);
// 		if (minLen > maxLen)
// 			minLen = maxLen;

// 		mutateColor(chance, rate);
// 	}

// 	void combine(const MPState* sc, MPState* out)
// 	{
// 		const SelectBestState* els = dynamic_cast<const SelectBestState*>(sc);
// 		SelectBestState* outm = dynamic_cast<SelectBestState*>(out);
// 		outm->totalMtrixCountMin = MRNG::coin() ? totalMtrixCountMin : outm->totalMtrixCountMin;
// 		outm->totalMtrixCountMax = MRNG::coin() ? totalMtrixCountMax : outm->totalMtrixCountMax;

// 		outm->minDepth = MRNG::coin() ? minDepth : outm->minDepth;
// 		outm->maxDepth = MRNG::coin() ? maxDepth : outm->maxDepth;

// 		outm->minStart = MRNG::coin() ? minStart : outm->minStart;
// 		outm->maxStart = MRNG::coin() ? maxStart : outm->maxStart;

// 		outm->minLen = MRNG::coin() ? minLen : outm->minLen;
// 		outm->maxLen = MRNG::coin() ? maxLen : outm->maxLen;

// 		outm->srcType = srcType;
// 		outm->lines = lines;
// 		out->color = color;
// 		out->inheritColor(sc, true);
// 	}

// 	MatrImg getResult(const bc::DatagridProvider* img)
// 	{
// 		MatrImg res(img->wid(), img->hei(), img->channels());
// 		//for (size_t i = 0; i < img->length(); i++)
// 		//{
// 		//	res.setLiner(i, img->getLiner(i));
// 		//}

// 		const barlinevector& barlines = *lines;
// 		float totSize = img->length();
// 		int oked = 0;
// 		for (barline* line : barlines)
// 		{
// 			float sizes = line->matr.size();
// 			if (inRange(sizes / totSize, totalMtrixCountMin, totalMtrixCountMax) &&
// 				inRange(line->getDeath(), minDepth, maxDepth) &&
// 				inRange(line->start, minStart, maxStart) &&
// 				inRange(line->len(), minLen, maxLen))
// 			{
// 				++oked;
// 				auto& matr = line->matr;
// 				for (size_t w = 0; w < matr.size(); ++w)
// 				{
// 					res.set(matr[w].getX(), matr[w].getY(), color);
// 				}
// 			}
// 		}
// 		std::cout << "ok: " << oked << "Col: " << color.getAvgFloat() << std::endl;

// 		return res;
// 	}
// };


// class MachinePRocessor
// {
// 	Mat drawMat;
// 	float maxRadius = 50;
// 	int minMachines = 200;

// public:
// 	//template<class T>
// 	static void fullProcess(const InitConteiter& cont)
// 	{
// 		srand(time(NULL));

// 		const int bestSize = 10;
// 		const int populationSize = 100;
// 		const int maxGenNumber = 200;
// 		const bool crossover = true;

// 		std::vector<SelectBestState> population;
// 		SelectBestState bestCells[bestSize];

// 		int oldSize = population.size();
// 		population.resize(populationSize);
// 		for (size_t i = oldSize; i < populationSize; i++)
// 		{
// 			population[i].initRandom(&cont);
// 		}

// 		std::cout << "Inited " << population.size() << " base population cells" << std::endl;
// 		for (size_t i = 0; i < maxGenNumber; i++)
// 		{
// 			std::cout << "Porcessing " << population.size() << " populations" << std::endl;

// 			for (auto& machine : population)
// 			{
// 				machine.calculate(&cont);
// 			}

// 			int getLEst = bestSize / 2;
// 			//int getLEst = 0;
// 			std::cout << "Sorting population cells... " << std::endl;
// 			std::sort(population.begin(), population.end(), [](const SelectBestState& a, const SelectBestState& b) {
// 				return a.accure > b.accure;
// 				});

// 			for (int j = 0; j < getLEst; j++)
// 			{
// 				bestCells[j] = population[j];
// 			}

// 			for (size_t i = bestSize / 2; i < bestSize; i++)
// 			{
// 				int ind0 = 0, ind1 = 0, ind2 = 0;
// 				while (ind0 == ind1 || ind0 == ind2 || ind1 == ind2)
// 				{
// 					ind0 = MRNG::randi(bestSize / 2, populationSize);
// 					ind1 = MRNG::randi(bestSize / 2, populationSize);
// 					ind2 = MRNG::randi(bestSize / 2, populationSize);
// 					SelectBestState* cells[3] = { &population[ind0],&population[ind1],&population[ind2] };
// 					std::sort(cells, cells + 3, [](const SelectBestState* a, const SelectBestState* b) {
// 						return a->accure < b->accure;
// 						});
// 					bestCells[i] = *cells[0];
// 				}
// 			}

// 			float avgFitness = 0.f;
// 			float maxS = -100000;
// 			for (auto& machine : population)
// 			{
// 				avgFitness += machine.accure;
// 				if (machine.accure > maxS)
// 					maxS = machine.accure;
// 			}
// 			avgFitness /= populationSize;
// 			std::cout << "Gen no. " << i << ". Avg futness: " << avgFitness << "; Max: " << maxS << std::endl;

// 			for (int j = 0; j < bestSize; j++)
// 			{
// 				bestCells[j] = population[j];
// 				if (MRNG::randf() <= 0.1f)
// 					bestCells[j].mutate(0.1f);

// 				//std::cout << "b no." << "j; a:" << bestCells[j].accure
// 				//	<< "; md: " << bestCells[j].maxDiff
// 				//	<< "; mtl:" << bestCells[j].maxTotalLen
// 				//	<< "; ms:" << bestCells[j].totalMtrixCount
// 				//	<< "; str:" << bestCells[j].start.text()
// 				//	<< endl;
// 			}
// 			std::cout << "Mutated " << bestSize << " cells" << std::endl;

// 			for (auto& machine : population)
// 			{
// 				if (MRNG::randf() >= 0.9f)
// 				{
// 					int fs = rand() % bestSize;
// 					fs = fs - fs % 2;
// 					int sc = rand() % bestSize;
// 					if (fs % 2 == 0)
// 					{
// 						fs += 1;
// 						if (fs >= bestSize)
// 							fs -= 2;
// 					}

// 					SelectBestState asds;
// 					bestCells[fs].combine(&bestCells[sc], &asds);
// 					machine = asds;
// 				}
// 				else
// 				{
// 					machine = bestCells[rand() % bestSize];
// 				}
// 			}
// 			std::cout << "Created new population from best " << bestSize << " cells" << std::endl;
// 		}

// 		cv::namedWindow("sa", cv::WINDOW_NORMAL);
// 		for (int i = 0; i < bestSize; i++)
// 		{
// 			std::cout << "Best no. " << i << "Futness: " << bestCells[i].accure << std::endl;
// 			auto& as = bestCells[i];
// 			MatrImg imsa = as.getResult(&cont.img);
// 			//sad.minDepth = 0;
// 			//sad.maxDepth = 11;
// 			//sad.totalMtrixCountMin = 0;
// 			//sad.totalMtrixCountMax = 0.9478;
// 			//sad.minStart = Barscalar(150, 0, 0);
// 			//sad.maxStart = Barscalar((uchar)'�', (uchar)'�', (uchar)'�');
// 			//sad.minLen = Barscalar(150, BarType::BYTE8_3);
// 			//sad.maxLen = Barscalar('\x15', '�', 255);
// 			//sad.initRandomColor();
// 			std::cout <<
// 				"Min d:" << as.minDepth <<
// 				"Max d:" << as.maxDepth <<
// 				"totalMtrixCountMin:" << as.totalMtrixCountMin <<
// 				"totalMtrixCountMax:" << as.totalMtrixCountMax <<
// 				"minStart:" << as.minStart.text() <<
// 				"maxStart:" << as.maxStart.text() <<
// 				"minLen:" << as.minLen.text() <<
// 				"maxLen:" << as.maxLen.text() << std::endl;

// 			cv::imshow("sa", bc::convertProvider2Mat(&imsa));
// 			cv::waitKey(0);
// 		}
// 	}
// };

// // Min d:0Max d:15totalMtrixCountMin:0totalMtrixCountMax:0.876223 minStart:(29,255,61)maxStart:(247,52,229)minLen:0maxLen:(240,236,255)
// //Min d:-2Max d:20totalMtrixCountMin:0totalMtrixCountMax:0.0719016minStart:(184,0,168)maxStart:236minLen:0maxLen:(252,235,37)
// void MachComb(const bc::DatagridProvider& img, const bc::DatagridProvider& mask)
// {
// 	InitConteiter intel{ img, mask };
// 	intel.set();
// 	MachinePRocessor::fullProcess(intel);
// }

// void MasResultsExperts()
// {
// 	string basePath = "D:\\Programs\\C++\\Barcode\\PrjBarlib\\researching\\tiles\\";
// 	for (size_t i = 0; i <= 15; i++)
// 	{
// 		string path = basePath + std::to_string(i) + "_set.png";
// 		Mat img = cv::imread(path, cv::IMREAD_COLOR);
// 		if (img.rows == 0)
// 			continue;

// 		string pathMask = basePath + std::to_string(i) + "_bld.png";

// 		Mat mask = cv::imread(pathMask, cv::IMREAD_GRAYSCALE);
// 		bc::BarMat imgWrap(img, BarType::BYTE8_3);
// 		bc::BarMat maskWrap(mask, BarType::BYTE8_1);

// 		SelectBestState sad;
// 		sad.minDepth = 0;
// 		sad.maxDepth = 15;
// 		sad.totalMtrixCountMin = 0;
// 		sad.totalMtrixCountMax = 876223;
// 		sad.minStart = Barscalar(29, 255, 61);
// 		sad.maxStart = Barscalar(247, 52, 229);
// 		sad.minLen = Barscalar(0, BarType::BYTE8_3);
// 		sad.maxLen = Barscalar(240, 236, 255);
// 		sad.initRandomColor();

// 		InitConteiter intel{ imgWrap, maskWrap };
// 		intel.set();
// 		sad.lines = &intel.barlines;
// 		MatrImg imsa = sad.getResult(&imgWrap);


// 		cv::namedWindow("sa", cv::WINDOW_NORMAL);
// 		cv::imshow("sa", bc::convertProvider2Mat(&imsa));
// 		cv::waitKey(0);

// 		//proc(img, ProcType::f0t255, ColorType::rgb, ComponentType::Component);
// 		//proc(img, ProcType::f255t0, ColorType::rgb, ComponentType::Component);
// 		//proc(i, img, ProcType::f0t255, ColorType::gray, ComponentType::Component);
// 		//proc(i, img, ProcType::f255t0, ColorType::gray, ComponentType::Component);
// 		//proc(i, img, ProcType::Radius, ColorType::rgb, ComponentType::Component);
// 		//proc(i, img, ProcType::Radius, ColorType::gray, ComponentType::Component);
// 	}
// }


// /*
// total Matr = [0, 0.9478]
// depth = [0,11]
// start = [150, ���]
// len = [0, \x15�]
// */
