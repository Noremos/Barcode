#pragma once

#include "GenCommon.h"
#include "barImg.h"
#include "barcodeCreator.h"

#include "D:\Programs\QT\binbar\MatrImg.h"

#define CREATE_PINDEX(X, Y, WID) (((Y) * (WID)) + (X))

enum CommandProc
{
	Restore = 0,
	setStart = 1,
	LMaxMB = 2,
	countTrue = 3,
	avgAsl = 4,
	sumStart = 5,
	LMaxDiv = 6,
	LChnls = 7,
	NoneAS = 8,
	MMAX
};


struct NNA
{
	//bc::BarType bartype;
	//bc::ColorType color;
	uchar commands[8];
	// 0 - bartype
	// 1 - color
	// ... comms

	float accure = 0.f;

	void initRandom()
	{
		commands[0] = rand() % 5;
		commands[1] = 1 + rand() % 2;
		for (size_t i = 2; i < 8; i++)
		{
			commands[i] = rand() % CommandProc::MMAX;
		}
	}

	bc::BarConstructor getConstr()
	{
		bc::BarConstructor consrt;
		consrt.addStructure((bc::ProcType)commands[0], (bc::ColorType)commands[1], bc::ComponentType::Component);

		return consrt;
	}


	void calc(bc::Baritem* bar, CommandProc mode, MatrImg& in, MatrImg& mat)
	{
		Barscalar amins, amaxs;
		in.maxAndMin(amins, amaxs);

		int st = 0;
		int ed = bar->barlines.size();

		switch (mode)
		{
		case Restore:
		{
			mat.fill(bar->getType() == BarType::BYTE8_3 ? amaxs : round(amaxs.getAvgFloat()));
			for (int i = st; i < ed; ++i)
			{
				auto& b = bar->barlines[i];
				const auto& matr = b->getMatrix();
				for (const auto& pm : matr)
				{
					int x = pm.getX();
					int y = pm.getY();
					mat.minus(x, y, pm.value);
				}
			}
			break;
		}

		case setStart:
		{
			mat.fill(0);

			for (int i = st; i < ed; ++i)
			{
				auto& b = bar->barlines[i];
				const auto& matr = b->getMatrix();
				for (const auto& pm : matr)
				{
					int x = pm.getX();
					int y = pm.getY();
					mat.set(x, y, b->start);
					// Barscalar v = mat.get(x, y);
					// mat.set(x, y,  b->start > v ? b->start : v);
				}
			}
			break;
		}
		case LMaxMB: // LMaxMB
		{
			mat.fill(0);

			for (int i = st; i < ed; ++i)
			{
				auto& b = bar->barlines[i];
				const auto& matr = b->getMatrix();
				for (const auto& pm : matr)
				{
					int x = pm.getX();
					int y = pm.getY();
					Barscalar v = mat.get(x, y);
					Barscalar newV = pm.value;
					Barscalar res = newV > v ? newV : v;
					mat.set(x, y, res);
				}
			}

			//			for (size_t i = 0; i < mat.length(); i++)
			//			{
			//				auto v = matCOunt.getLiner(i);
			//				if (v!= 0)
			//					mat.setLiner(i, mat.getLiner(i) / matCOunt.getLiner(i));
			//			}
			break;
		}
		case avgAsl: // count True
		{
			mat = MatrImg(in.wid(), in.hei(), 1);

			int maxv = 0;
			for (int i = st; i < ed; ++i)
			{
				auto& b = bar->barlines[i];
				const auto& matr = b->getMatrix();
				for (const auto& pm : matr)
				{
					int x = pm.getX();
					int y = pm.getY();
					Barscalar v = mat.get(x, y);
					if (v == 255)
						continue;

					v += 1;
					if (v > maxv)
						maxv = v.getAvgUchar();
					mat.set(x, y, v);
				}
			}
			float cf = 255.f / maxv;

			for (size_t i = 0; i < mat.length(); ++i)
			{
				Barscalar v = mat.getLiner(i);
				float vc = v.getAvgFloat() * cf;

				mat.setLiner(i, vc > 255 ? 255 : vc);

			}
			break;
		}
		case countTrue:// avg
		{
			mat.fill(bar->getType() == BarType::BYTE8_3 ? amaxs : round(amaxs.getAvgFloat()));
			MatrImg matCOunt(in.wid(), in.hei(), 1);
			matCOunt.fill(0);

			for (int i = st; i < ed; ++i)
			{
				auto& b = bar->barlines[i];
				const auto& matr = b->getMatrix();
				for (const auto& pm : matr)
				{
					int x = pm.getX();
					int y = pm.getY();
					mat.minus(x, y, pm.value);

					matCOunt.add(x, y, Barscalar(1, BarType::BYTE8_1));
				}
			}

			//		mat.assignCopyOf(mainMat);
			for (size_t i = 0; i < mat.length(); i++)
			{
				auto v = matCOunt.getLiner(i);
				if (v != 0)
					mat.setLiner(i, mat.getLiner(i) / matCOunt.getLiner(i));
			}
			break;
		}
		case sumStart: // sum-start
		{
			mat.fill(0);

			for (int i = st; i < ed; ++i)
			{
				auto& b = bar->barlines[i];
				const auto& matr = b->getMatrix();
				for (const auto& pm : matr)
				{
					int x = pm.getX();
					int y = pm.getY();
					Barscalar v = mat.get(x, y);
					Barscalar newV = pm.value > b->start ? pm.value - b->start : b->start - pm.value;
					mat.set(x, y, newV > v ? newV : v);
				}
			}
			break;
		}
		case LMaxDiv:
		{
			mat.fill(0);
			MatrImg matCOunt(in.wid(), in.hei(), 1);
			matCOunt.fill(0);

			for (int i = st; i < ed; ++i)
			{
				auto& b = bar->barlines[i];
				const auto& matr = b->getMatrix();
				for (const auto& pm : matr)
				{
					int x = pm.getX();
					int y = pm.getY();
					Barscalar v = mat.get(x, y);
					Barscalar newV = pm.value;
					Barscalar res = newV > v ? newV : v;
					mat.set(x, y, res);

					matCOunt.add(x, y, Barscalar(1, BarType::BYTE8_1));
				}
			}
			for (size_t i = 0; i < mat.length(); i++)
			{
				auto v = matCOunt.getLiner(i);
				if (v != 0)
					mat.setLiner(i, mat.getLiner(i) / matCOunt.getLiner(i));
			}
			break;
		}
		case LChnls:
		{
			mat.fill(0);
			MatrImg matCOunt(in.wid(), in.hei(), 1);
			matCOunt.fill(0);

			for (int i = st; i < ed; ++i)
			{
				auto& b = bar->barlines[i];
				const auto& matr = b->getMatrix();
				for (const auto& pm : matr)
				{
					int x = pm.getX();
					int y = pm.getY();
					Barscalar v = mat.get(x, y);
					Barscalar newV = pm.value;
					Barscalar res = newV > v ? newV : v;
					mat.set(x, y, res);


					matCOunt.add(x, y, Barscalar(1, BarType::BYTE8_1));
				}
			}

			//		mat.assignCopyOf(mainMat);
			for (size_t i = 0; i < mat.length(); i++)
			{
				auto v = matCOunt.getLiner(i);
				if (v != 0)
					mat.setLiner(i, mat.getLiner(i) / matCOunt.getLiner(i));
			}
			break;
		}
		case NoneAS:
			break;
		}
	}


	float check(const bc::DatagridProvider& img, const bc::DatagridProvider& mask)
	{
		int avgmi = 0;
		int avgumi = 0;
		double avgMask = 0;
		double avgUnMask = 0;
		for (size_t i = 0; i < img.wid() * img.hei(); ++i)
		{
			Barscalar incSet = img.getLiner(i);
			bool maskMark = mask.getLiner(i) == 255;
			if (maskMark)
			{
				avgMask += incSet.getAvgFloat();
				++avgmi;
			}
			else
			{
				avgUnMask += incSet.getAvgFloat();
				++avgumi;
			}
		}

		avgMask /= avgmi;
		avgUnMask /= avgumi;

		return abs(avgUnMask - avgMask);
	}

	void calcutaAccur(bc::Barcontainer* bar, const bc::DatagridProvider* img, const bc::DatagridProvider& mask)
	{
		MatrImg res(img->wid(), img->hei(), img->channels());
		size_t total = img->wid() * img->hei();
		for (size_t i = 0; i < total; ++i)
		{
			res.setLiner(i, img->getLiner(i));
		}

		for (size_t i = 0; i < 8; i++)
		{
			MatrImg mat(img->wid(), img->hei(), img->channels());
			this->calc(bar->getItem(0), (CommandProc)this->commands[i], res, mat);
			res = mat;
		}

		accure = check(res, mask);
		delete bar;
	}

	MatrImg getResult(bc::Barcontainer* bar, const bc::DatagridProvider* img)
	{
		MatrImg res(img->wid(), img->hei(), img->channels());
		for (size_t i = 0; i < img->wid() * img->hei(); ++i)
		{
			res.setLiner(i, img->getLiner(i));
		}

		for (size_t i = 0; i < 8; i++)
		{
			MatrImg mat(img->wid(), img->hei(), img->channels());
			this->calc(bar->getItem(0), (CommandProc)this->commands[i], res, mat);
			res = mat;
		}

		delete bar;

		return res;
	}

	int maxMin(float f)
	{
		int af = (int)round(f);
		return MIN(8, MAX(af, 0));
	}

	void mutate(float chance, float rate)
	{
		if (chance <= MRNG::randf())
			commands[0] +=  1 + maxMin(2 * MRNG::randf(-rate, rate));
		if (chance <= MRNG::randf())
			commands[1] += maxMin(5 * MRNG::randf(-rate, rate));
		for (char i = 2; i < 8; i++)
		{
			if (chance <= MRNG::randf())
				commands[i] += maxMin(MMAX * MRNG::randf(-rate, rate));
		}
	}

	static void combine(const NNA& fs, const NNA& sc, NNA& out)
	{
#define CHLBIT(C, R) (C & R != 0 ? 1 : 0)
		int k = 0;
			// Перебором должны установить все возможны параметры
		for (size_t i = 0; i < 8; i++)
		{
			out.commands[i] = MRNG::randi(0, 100) < 50 ? fs.commands[i] : sc.commands[i];
		}
	}
};

class MachinePRocessor
{
	Mat drawMat;
	float maxRadius = 50;
	int minMachines = 200;

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
		std::vector<NNA> population;
		NNA bestCells[bestSize];

		bc::BarcodeCreator crear;

		int oldSize = population.size();
		population.resize(populationSize);
		for (size_t i = oldSize; i < populationSize; i++)
		{
			population[i].initRandom();
		}

		std::cout << "Inited " << population.size() << " base population cells" << std::endl;
		for (size_t i = 0; i < maxGenNumber; i++)
		{
			std::cout << "Porcessing " << population.size() << " populations" << std::endl;

			for (auto& machine : population)
			{
				bc::Barcontainer* bar = crear.createBarcode(&img, machine.getConstr());
				machine.calcutaAccur(bar, &img, mask);
			}

			int getLEst = bestSize / 2;
			//int getLEst = 0;
			std::cout << "Sorting population cells... " << std::endl;
			std::sort(population.begin(), population.end(), [](const NNA& a, const NNA& b) {
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
					ind0 =  MRNG::randi(bestSize / 2, populationSize);
					ind1 = MRNG::randi(bestSize / 2, populationSize);
					ind2 = MRNG::randi(bestSize / 2, populationSize);
					NNA* cells[3] = { &population[ind0],&population[ind1],&population[ind2] };
					std::sort(cells, cells + 3, [](const NNA* a, const NNA* b) {
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
				if (MRNG::randf() <= 0.1f)
					bestCells[j].mutate(1.f / 8, 0.1f);

				//std::cout << "b no." << "j; a:" << bestCells[j].accure
				//	<< "; md: " << bestCells[j].maxDiff
				//	<< "; mtl:" << bestCells[j].maxTotalLen
				//	<< "; ms:" << bestCells[j].totalMtrixCount
				//	<< "; str:" << bestCells[j].start.text()
				//	<< endl;
			}
			std::cout << "Mutated " << bestSize << " cells" << std::endl;

			for (auto& machine : population)
			{
				if (MRNG::randf() >= 0.9f)
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

					NNA asds;
					NNA::combine(bestCells[fs], bestCells[sc], asds);
					machine = asds;
				}
				else
				{
					machine = bestCells[rand() % bestSize];
				}
			}
			std::cout << "Created new population from best " << bestSize << " cells" << std::endl;
		}

		bc::BarcodeCreator crear2;

		cv::namedWindow("sa", cv::WINDOW_NORMAL);
		for (int i = 0; i < bestSize; i++)
		{
			std::cout << "Best no. " << i << "Futness: " << bestCells[i].accure << std::endl;
			MatrImg mat(img.wid(), img.hei(), img.channels());
			mat.fill(0);

			bc::Barcontainer* barc = crear2.createBarcode(&img, bestCells[i].getConstr());
			bc::Baritem* bar = barc->getItem(0);

			MatrImg imsa = bestCells[i].getResult(barc, &img);

			cv::imshow("sa", bc::convertProvider2Mat(&imsa));
			cv::waitKey(0);
		}
	}
};