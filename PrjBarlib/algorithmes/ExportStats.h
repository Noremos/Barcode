#pragma once

#include "prep.h"
#include "D:\Programs\QT\binbar\MatrImg.h"

using namespace bc;
using namespace cv;

Barcontainer* run(bc::DatagridProvider& mainMat, ProcType procType, ColorType colType, ComponentType compType)
{
	bc::BarcodeCreator bc;
	bc::BarConstructor constr;
	constr.createBinaryMasks = true;
	constr.createGraph = false;
	constr.returnType = bc::ReturnType::barcode2d;
	constr.addStructure((bc::ProcType)procType, (bc::ColorType)colType, (bc::ComponentType)compType);
	return bc.createBarcode(&mainMat, constr);
}


void processMain(const DatagridProvider& mainMat, Barcontainer* barcode, int mode, MatrImg& mat)
{
	mat = MatrImg(mainMat.wid(), mainMat.hei(), mainMat.channels());

	auto bar = barcode->getItem(0);
	int st = 0;
	int ed = bar->barlines.size();

	switch (mode)
	{
	case 2: {
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
			}
		}
		break;
	}
	case 3:
	{
		mat.fill(0);
		MatrImg matCOunt(mainMat.wid(), mainMat.hei(), 1);
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
				Barscalar newV = pm.value - b->start;
				Barscalar res = v + pm.value;

				mat.set(x, y, res);
				matCOunt.add(x, y, Barscalar(1, BarType::BYTE8_1));
			}
		}

		for (int i = 0; i < mat.length(); i++)
			mat.setLiner(i, mat.getLiner(i) / matCOunt.getLiner(i));
		break;
	}
	case 4:
	{
		mat = MatrImg(mainMat.wid(), mainMat.hei(), 1);

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
	case 5:
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
				Barscalar res = v + pm.value;
				mat.set(x, y, res);
			}
		}
		break;
	}
	case 6:
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
				Barscalar newV = pm.value - b->start;
				Barscalar res = newV > v ? newV : v;
				mat.set(x, y, res);
			}
		}
		break;
	}
	case 7:
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
		break;
	}

	case 8:
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
		break;
	}
	case 9:
	{
		Barscalar minf, maxf;
		mainMat.maxAndMin(minf, maxf);
		mat.fill(bar->getType() == BarType::BYTE8_3 ? maxf : round(maxf.getAvgFloat()));
		MatrImg matCOunt(mainMat.wid(), mainMat.hei(), 1);
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

		for (size_t i = 0; i < mat.length(); i++)
		{
			auto v = matCOunt.getLiner(i);
			if (v != 0)
				mat.setLiner(i, mat.getLiner(i) / matCOunt.getLiner(i));
		}
		break;
	}
	}
}

void proc(int id, Mat& imgo, ProcType procType, ColorType colType, ComponentType compType)
{
	bc::BarMat wrap(imgo);

	std::unique_ptr<Barcontainer> barcode;
	barcode.reset(run(wrap, procType, colType, compType));

	//for (size_t i = 2; i <= 8; i++)
	int i = 9;
	{
		string basePath = "D:\\Programs\\C++\\Barcode\\PrjBarlib\\researching\\res\\";
		basePath += std::to_string(i) + "_m" + to_string(id) + "_prc" + std::to_string((int)procType) +
			"col" + std::to_string((int)colType) +
			"cmp" + std::to_string((int)compType) +
			".png";

		MatrImg out;
		processMain(wrap, barcode.get(), i, out);
		if (out.channels() == 1)
		{
			Mat img(out.wid(), out.hei(), CV_8UC1);
			for (size_t w = 0; w < out.wid(); w++)
			{
				for (size_t h = 0; h < out.hei(); h++)
				{
					img.at<uchar>(h, w) = out.get(w, h).getByte8();
				}
			}
			cv::imwrite(basePath, img);
		}
		else
		{
			Mat img(out.wid(), out.hei(), CV_8UC3);
			for (size_t w = 0; w < out.wid(); w++)
			{
				for (size_t h = 0; h < out.hei(); h++)
				{
					img.at<Vec3b>(h, w) = out.get(w, h).toCvVec();
				}
			}
			cv::imwrite(basePath, img);
		}
	}
}


void getResultsExperts()
{
	string basePath = "D:\\Programs\\C++\\Barcode\\PrjBarlib\\researching\\tiles\\";
	for (size_t i = 0; i <= 15; i++)
	{
		string path = basePath + std::to_string(i) + "_set.png";
		Mat img = cv::imread(path, cv::IMREAD_COLOR);
		if (img.rows == 0)
			continue;

		bc::BarMat wrap(img);
		//proc(img, ProcType::f0t255, ColorType::rgb, ComponentType::Component);
		//proc(img, ProcType::f255t0, ColorType::rgb, ComponentType::Component);
		//proc(i, img, ProcType::f0t255, ColorType::gray, ComponentType::Component);
		//proc(i, img, ProcType::f255t0, ColorType::gray, ComponentType::Component);
		//proc(i, img, ProcType::Radius, ColorType::rgb, ComponentType::Component);
		//proc(i, img, ProcType::Radius, ColorType::gray, ComponentType::Component);
		proc(i, img, ProcType::f0t255, ColorType::gray, ComponentType::Hole);
	}
}
