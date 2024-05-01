// #pragma once

// #include "GenCommon.h"
// #include "barImg.h"
// #include "barcodeCreator.h"

// #include "D:\Programs\QT\binbar\MatrImg.h"

// #define CREATE_PINDEX(X, Y, WID) (((Y) * (WID)) + (X))

// enum CommandProc
// {
// 	Restore = 0,
// 	setStart = 1,
// 	LMaxMB = 2,
// 	countTrue = 3,
// 	avgAsl = 4,
// 	sumStart = 5,
// 	LMaxDiv = 6,
// 	LChnls = 7,
// 	NoneAS = 8,
// 	MMAX
// };


// struct NNA
// {
// 	//bc::BarType bartype;
// 	//bc::ColorType color;
// 	uchar commands[8];
// 	// 0 - bartype
// 	// 1 - color
// 	// ... comms

// 	float accure = 0.f;

// 	void initRandom()
// 	{
// 		commands[0] = rand() % 5;
// 		commands[1] = 1 + rand() % 2;
// 		for (size_t i = 2; i < 8; i++)
// 		{
// 			commands[i] = rand() % CommandProc::MMAX;
// 		}
// 	}

// 	bc::BarConstructor getConstr()
// 	{
// 		bc::BarConstructor consrt;
// 		consrt.addStructure((bc::ProcType)commands[0], (bc::ColorType)commands[1], bc::ComponentType::Component);

// 		return consrt;
// 	}


// 	void calc(bc::Baritem* bar, CommandProc mode, MatrImg& in, MatrImg& mat)
// 	{
// 		Barscalar amins, amaxs;
// 		in.maxAndMin(amins, amaxs);

// 		int st = 0;
// 		int ed = bar->barlines.size();

// 		switch (mode)
// 		{
// 		case Restore:
// 		{
// 			mat.fill(bar->getType() == BarType::BYTE8_3 ? amaxs : round(amaxs.getAvgFloat()));
// 			for (int i = st; i < ed; ++i)
// 			{
// 				auto& b = bar->barlines[i];
// 				const auto& matr = b->getMatrix();
// 				for (const auto& pm : matr)
// 				{
// 					int x = pm.getX();
// 					int y = pm.getY();
// 					mat.minus(x, y, pm.value);
// 				}
// 			}
// 			break;
// 		}

// 		case setStart:
// 		{
// 			mat.fill(0);

// 			for (int i = st; i < ed; ++i)
// 			{
// 				auto& b = bar->barlines[i];
// 				const auto& matr = b->getMatrix();
// 				for (const auto& pm : matr)
// 				{
// 					int x = pm.getX();
// 					int y = pm.getY();
// 					mat.set(x, y, b->start);
// 					// Barscalar v = mat.get(x, y);
// 					// mat.set(x, y,  b->start > v ? b->start : v);
// 				}
// 			}
// 			break;
// 		}
// 		case LMaxMB: // LMaxMB
// 		{
// 			mat.fill(0);

// 			for (int i = st; i < ed; ++i)
// 			{
// 				auto& b = bar->barlines[i];
// 				const auto& matr = b->getMatrix();
// 				for (const auto& pm : matr)
// 				{
// 					int x = pm.getX();
// 					int y = pm.getY();
// 					Barscalar v = mat.get(x, y);
// 					Barscalar newV = pm.value;
// 					Barscalar res = newV > v ? newV : v;
// 					mat.set(x, y, res);
// 				}
// 			}

// 			//			for (size_t i = 0; i < mat.length(); i++)
// 			//			{
// 			//				auto v = matCOunt.getLiner(i);
// 			//				if (v!= 0)
// 			//					mat.setLiner(i, mat.getLiner(i) / matCOunt.getLiner(i));
// 			//			}
// 			break;
// 		}
// 		case avgAsl: // count True
// 		{
// 			mat = MatrImg(in.wid(), in.hei(), 1);

// 			int maxv = 0;
// 			for (int i = st; i < ed; ++i)
// 			{
// 				auto& b = bar->barlines[i];
// 				const auto& matr = b->getMatrix();
// 				for (const auto& pm : matr)
// 				{
// 					int x = pm.getX();
// 					int y = pm.getY();
// 					Barscalar v = mat.get(x, y);
// 					if (v == 255)
// 						continue;

// 					v += 1;
// 					if (v > maxv)
// 						maxv = v.getAvgUchar();
// 					mat.set(x, y, v);
// 				}
// 			}
// 			float cf = 255.f / maxv;

// 			for (size_t i = 0; i < mat.length(); ++i)
// 			{
// 				Barscalar v = mat.getLiner(i);
// 				float vc = v.getAvgFloat() * cf;

// 				mat.setLiner(i, vc > 255 ? 255 : vc);

// 			}
// 			break;
// 		}
// 		case countTrue:// avg
// 		{
// 			mat.fill(bar->getType() == BarType::BYTE8_3 ? amaxs : round(amaxs.getAvgFloat()));
// 			MatrImg matCOunt(in.wid(), in.hei(), 1);
// 			matCOunt.fill(0);

// 			for (int i = st; i < ed; ++i)
// 			{
// 				auto& b = bar->barlines[i];
// 				const auto& matr = b->getMatrix();
// 				for (const auto& pm : matr)
// 				{
// 					int x = pm.getX();
// 					int y = pm.getY();
// 					mat.minus(x, y, pm.value);

// 					matCOunt.add(x, y, Barscalar(1, BarType::BYTE8_1));
// 				}
// 			}

// 			//		mat.assignCopyOf(mainMat);
// 			for (size_t i = 0; i < mat.length(); i++)
// 			{
// 				auto v = matCOunt.getLiner(i);
// 				if (v != 0)
// 					mat.setLiner(i, mat.getLiner(i) / matCOunt.getLiner(i));
// 			}
// 			break;
// 		}
// 		case sumStart: // sum-start
// 		{
// 			mat.fill(0);

// 			for (int i = st; i < ed; ++i)
// 			{
// 				auto& b = bar->barlines[i];
// 				const auto& matr = b->getMatrix();
// 				for (const auto& pm : matr)
// 				{
// 					int x = pm.getX();
// 					int y = pm.getY();
// 					Barscalar v = mat.get(x, y);
// 					Barscalar newV = pm.value > b->start ? pm.value - b->start : b->start - pm.value;
// 					mat.set(x, y, newV > v ? newV : v);
// 				}
// 			}
// 			break;
// 		}
// 		case LMaxDiv:
// 		{
// 			mat.fill(0);
// 			MatrImg matCOunt(in.wid(), in.hei(), 1);
// 			matCOunt.fill(0);

// 			for (int i = st; i < ed; ++i)
// 			{
// 				auto& b = bar->barlines[i];
// 				const auto& matr = b->getMatrix();
// 				for (const auto& pm : matr)
// 				{
// 					int x = pm.getX();
// 					int y = pm.getY();
// 					Barscalar v = mat.get(x, y);
// 					Barscalar newV = pm.value;
// 					Barscalar res = newV > v ? newV : v;
// 					mat.set(x, y, res);

// 					matCOunt.add(x, y, Barscalar(1, BarType::BYTE8_1));
// 				}
// 			}
// 			for (size_t i = 0; i < mat.length(); i++)
// 			{
// 				auto v = matCOunt.getLiner(i);
// 				if (v != 0)
// 					mat.setLiner(i, mat.getLiner(i) / matCOunt.getLiner(i));
// 			}
// 			break;
// 		}
// 		case LChnls:
// 		{
// 			mat.fill(0);
// 			MatrImg matCOunt(in.wid(), in.hei(), 1);
// 			matCOunt.fill(0);

// 			for (int i = st; i < ed; ++i)
// 			{
// 				auto& b = bar->barlines[i];
// 				const auto& matr = b->getMatrix();
// 				for (const auto& pm : matr)
// 				{
// 					int x = pm.getX();
// 					int y = pm.getY();
// 					Barscalar v = mat.get(x, y);
// 					Barscalar newV = pm.value;
// 					Barscalar res = newV > v ? newV : v;
// 					mat.set(x, y, res);


// 					matCOunt.add(x, y, Barscalar(1, BarType::BYTE8_1));
// 				}
// 			}

// 			//		mat.assignCopyOf(mainMat);
// 			for (size_t i = 0; i < mat.length(); i++)
// 			{
// 				auto v = matCOunt.getLiner(i);
// 				if (v != 0)
// 					mat.setLiner(i, mat.getLiner(i) / matCOunt.getLiner(i));
// 			}
// 			break;
// 		}
// 		case NoneAS:
// 			break;
// 		}
// 	}


// 	float check(const bc::DatagridProvider& img, const bc::DatagridProvider& mask)
// 	{
// 		int avgmi = 0;
// 		int avgumi = 0;
// 		double avgMask = 0;
// 		double avgUnMask = 0;
// 		for (size_t i = 0; i < img.wid() * img.hei(); ++i)
// 		{
// 			Barscalar incSet = img.getLiner(i);
// 			bool maskMark = mask.getLiner(i) == 255;
// 			if (maskMark)
// 			{
// 				avgMask += incSet.getAvgFloat();
// 				++avgmi;
// 			}
// 			else
// 			{
// 				avgUnMask += incSet.getAvgFloat();
// 				++avgumi;
// 			}
// 		}

// 		avgMask /= avgmi;
// 		avgUnMask /= avgumi;

// 		return abs(avgUnMask - avgMask);
// 	}

// 	void calcutaAccur(bc::Barcontainer* bar, const bc::DatagridProvider* img, const bc::DatagridProvider& mask)
// 	{
// 		MatrImg res(img->wid(), img->hei(), img->channels());
// 		size_t total = img->wid() * img->hei();
// 		for (size_t i = 0; i < total; ++i)
// 		{
// 			res.setLiner(i, img->getLiner(i));
// 		}

// 		for (size_t i = 0; i < 8; i++)
// 		{
// 			MatrImg mat(img->wid(), img->hei(), img->channels());
// 			this->calc(bar->getItem(0), (CommandProc)this->commands[i], res, mat);
// 			res = mat;
// 		}

// 		accure = check(res, mask);
// 		delete bar;
// 	}

// 	MatrImg getResult(bc::Barcontainer* bar, const bc::DatagridProvider* img)
// 	{
// 		MatrImg res(img->wid(), img->hei(), img->channels());
// 		for (size_t i = 0; i < img->wid() * img->hei(); ++i)
// 		{
// 			res.setLiner(i, img->getLiner(i));
// 		}

// 		for (size_t i = 0; i < 8; i++)
// 		{
// 			MatrImg mat(img->wid(), img->hei(), img->channels());
// 			this->calc(bar->getItem(0), (CommandProc)this->commands[i], res, mat);
// 			res = mat;
// 		}

// 		delete bar;

// 		return res;
// 	}

// 	int maxMin(float f)
// 	{
// 		int af = (int)round(f);
// 		return MIN(8, MAX(af, 0));
// 	}

// 	void mutate(float chance, float rate)
// 	{
// 		if (chance <= MRNG::randf())
// 			commands[0] += 1 + maxMin(2 * MRNG::randf(-rate, rate));
// 		if (chance <= MRNG::randf())
// 			commands[1] += maxMin(5 * MRNG::randf(-rate, rate));
// 		for (char i = 2; i < 8; i++)
// 		{
// 			if (chance <= MRNG::randf())
// 				commands[i] += maxMin(MMAX * MRNG::randf(-rate, rate));
// 		}
// 	}

// 	static void combine(const NNA& fs, const NNA& sc, NNA& out)
// 	{
// #define CHLBIT(C, R) (C & R != 0 ? 1 : 0)
// 		int k = 0;
// 		// ��������� ������ ���������� ��� �������� ���������
// 		for (size_t i = 0; i < 8; i++)
// 		{
// 			out.commands[i] = MRNG::randi(0, 100) < 50 ? fs.commands[i] : sc.commands[i];
// 		}
// 	}
// };
