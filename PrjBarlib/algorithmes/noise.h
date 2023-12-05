#pragma once

#include "prep.h"

//float getSKO(Bmat8 m1, Bmat8 m2, int dif = 1)
//{
//	// https://wikimedia.org/api/rest_v1/media/math/render/svg/56c53fa1ededf246dc1a602acd88f7c6e68b27fd
//	assert(m1.length() == m2.length());
//	unsigned long long f = 0;
//	for (size_t i = 0; i < m1.length(); i++)
//	{
//		f += (int)sqr(m1.getLiner(i) - m2.getLiner(i));
//	}
//	return  sqrt(static_cast<long double>(f) / static_cast<long double>(m1.length() - dif));
//}
//
//
//float getPSNR(Bmat8 m1, Bmat8 m2, int N = 255)
//{
//	// https://wikimedia.org/api/rest_v1/media/math/render/svg/56c53fa1ededf246dc1a602acd88f7c6e68b27fd
//	float sko = getSKO(m1, m2, 0);
//	return 10 * log10f((float)sqr(N) / sko);
//}
using std::to_string;
using std::string;
using cv::Mat;

void removeNoise(Mat img, const string& prefix, bool simple)
{
	bc::BarConstructor bcont;
	bcont.addStructure(simple ? bc::ProcType::f255t0 : bc::ProcType::Radius, bc::ColorType::gray, bc::ComponentType::Component);
	bcont.createBinaryMasks = true;
	bcont.createGraph = false;
	bcont.returnType = bc::ReturnType::barcode2d;

	// ###### PARAMS #####
	bool rev = true;
	const int NSIZE = 1;
	// ####################

	Bmat8 input(img, BarType::BYTE8_1);

	bc::BarcodeCreator test;
	auto* cont = test.createBarcode(&input, bcont);
	auto* item = cont->getItem(0);

	//cv::Mat out(input.hei(), input.wid(), CV_8UC1, cv::Scalar(0));

	//out = input.mat.clone();
	int tableStep[] = { 1,2,4,8, 16, 32, 40, 50, 64 };
	//int tableStep[] = { 30};
	auto delLym = [](uchar& val)
	{
		val = 0;
		if (val <= 2)
			val = 0;
		else if (val <= 8)
			val /= 2;
		else if (val <= 16)
			val /= 4;
		else if (val <= 32)
			val /= 8;
		else if (val <= 64)
			val /= 16;
		//else if (val < 128)
		//	val /= 32;
		//else
		//	val /= 64;
	};

	auto delLym2 = [](uchar& val)
	{
		val = 0;
		if (val <= 2)
			val = 0;
		else if (val <= 8)
			val /= 2;
		else if (val <= 16)
			val /= 3;
		else if (val <= 32)
			val /= 4;
		else if (val <= 64)
			val /= 5;
		//else if (val < 128)
		//	val /= 32;
		//else
		//	val /= 64;
	};

	double minVal;
	double maxVal;
	cv::minMaxLoc(img, &minVal, &maxVal);
	uchar mxu = maxVal;
	uchar mnu = minVal;
	for (int i : tableStep)
	{
		Mat test(img.rows, img.cols, CV_8UC1, cv::Scalar(mnu));
		//Mat test = img.clone();
		for (auto& line : item->barlines)
		{
			auto& matr = line->matr;
			if (line->len().getAvgUchar() <= i)
			{
				for (size_t k = 0; k < matr.size(); ++k)
				{
					auto& p = matr[k];
					uchar v = p.value.getAvgUchar();
					delLym(v);
					uchar& val = test.at<uchar>(p.getY(), p.getX());
					assert(val >= v);
					val -= v;
				}
			}
			else
			{
				for (size_t k = 0; k < matr.size(); ++k)
				{
					auto& p = matr[k];
					uchar& val = test.at<uchar>(p.getY(), p.getX());
					uchar v = p.value.getAvgUchar();
					//assert(val >= v);
					assert((int)val + v <= 255);
					val += v;
				}
			}
		}
		string outPath = prefix + "_" + to_string(i) + ".png";
		//string outPath = prefix + ".png";
		std::cout << outPath << std::endl;
		cv::imwrite(outPath, test);
	}
	//if (!rev)
		//out = 255 - out;

	delete cont;
}

void testAll()
{
	string base = "D:/papers/compression/src/";
	string pathToSave = "D:/papers/compression/research/bar/";
	for (int i = 0; i < 6; ++i)
	//int i = 7;
	{
		string imgpath = base + std::to_string(i) + ".png";
		Mat img = cv::imread(imgpath, cv::IMREAD_GRAYSCALE);
		string pref = pathToSave + std::to_string(i);
		removeNoise(img, pref + "f", true);
		//removeNoise(img, pref + "r", false);
	}
}
