#include "barcodeCreator.h"

#include <iostream>
#include <string>

#include "../detection.h"
#include "tests.h"

void tetsAll()
{

	// TODO Move it to test project
	// testInitFromMat();

	// printf("raw data tests: star...");
	// checkImgFromData2();
	// checkImgFromData3();
	// checkImgFromData4();
	// checkImgFromData5();
	// checkImgFromData6();
	// printf("done\n\n");

	// printf("mat tests: star...");
	// checkSingleMat();
	// testMats();
	// printf("done\n\n");
	//
	// printf("maxLen test: sart...\n");
	// testMaxLen();
	// printf("done\n\n");

	// printf("BigImg test: start...\n");
	// checkBigImg();
	// printf("done\n\n");

	// printf("Check fix for zero len: sart...\n");
	// checkSameVals();
	// printf("done\n\n");

	/*printf("Check float imgs: sart...\n");
	testFloatMats();
	printf("done\n\n");*/

	// printf("Check problem float imgs: sart...\n");
	// testProblemFloatMats();
	// printf("done\n\n");

	// printf("Check big problem float imgs: sart...\n");
	// testBigProblemFloatMats();
	// printf("done\n\n");

	// testImg("../PrjTests/data/city.png");//bigimg.jpg
	testImg("D:\\Programs\\Python\\barcode\\roofs\\t2\\ident-simple4.png"); // bigimg.jpg

	// testSimple();
}

void simple()
{

	//for (int i = 0; i < img.wid(); i++)
	//{
	//	for (int j = 1; j < img.hei(); j++)
	//	{
	//		uchar clos = 0;
	//		int mindiff = 255;
	//		uchar uval = mat.at<uchar>(j, i);
	//		if (uval == 255 || uval == 0)
	//		{
	//			//int badCounter = 0;
	//			//uchar newbadval = 0;
	//			//int val = uval;
	//			//int sum = 0;
	//			//for (int ii = i - 1; ii <= i + 1; ii++)
	//			//{
	//			//	for (int jj = 0; jj <= j + 1; jj++)
	//			//	{
	//			//		if (ii >= 0 && ii < img.wid() && jj >= 0 && jj < img.hei())
	//			//		{
	//			//			int potval = mat.at<uchar>(jj, ii);
	//			//			//badCounter++;
	//			//			if (potval == 0 && val == 255)
	//			//			{
	//			//				sum += potval;
	//			//				badCounter++;
	//			//				uval = 255;
	//			//				continue;
	//			//			}
	//			//			if (potval == 255 && val == 0)
	//			//			{
	//			//				badCounter++;
	//			//				sum += potval;
	//			//				uval = 0;
	//			//				continue;
	//			//			}
	//			//			/*
	//			//			&& potval != 255)
	//			//			{
	//			//			if (abs(val - potval) < mindiff)
	//			//			{
	//			//				mindiff = abs(val - potval);
	//			//				clos = potval;
	//			//			}
	//			//			}*/
	//			//		}
	//			//	}
	//			//}
	//			//clos = sum == 0 ? uval : (sum / badCounter);
	//			mat.at<uchar>(j, i) = mat.at<uchar>(j - 1, i);;
	//			out.at<uchar>(j, i) = mat.at<uchar>(j - 1, i);;
	//		}
	//		else
	//		{
	//			out.at<uchar>(j, i) = uval;
	//		}
	//	}
	//}

}

cv::Mat proc(cv::Mat mat)
{
	bc::BarConstructor<uchar> bcont;
	bcont.addStructure(bc::ProcType::f0t255, bc::ColorType::gray, bc::ComponentType::Component);
	bcont.createBinaryMasks = true;
	bcont.createGraph = false;
	bcont.returnType = bc::ReturnType::barcode3d;

	// ###### PARAMS #####
	bool rev = true;
	const int NSIZE = 4;
	// ####################

	bc::BarMat<uchar> img(mat);

	bc::BarcodeCreator<uchar> test;
	auto* cont = test.createBarcode(&img, bcont);
	auto* item = cont->getItem(0);

	cv::Mat out(img.hei(), img.wid(), CV_8UC1, cv::Scalar(0));

	out = mat;

	for (auto& line : item->barlines)
	{
		auto& matr = line->matr;
		/*	for (size_t k = 0; k < matr.size(); k++)
		{
			auto& p = matr[k];

			out.at<uchar>(p.getY(), p.getX()) += p.value;
		}*/

		int k = 0;
		int sizeb3;
		int titalSize = 0;
		while (k < line->bar3d->size() && (sizeb3 = line->bar3d->at(k).count - titalSize) <= NSIZE)
		{
			for (int total = k + sizeb3; k < total; ++k)
			{
				auto& p = matr[k];
				uchar& val = out.at<uchar>(p.getY(), p.getX());
				//assert(val >= p.value);
				val += p.value;
			}
			titalSize += sizeb3;
			//break;

		}
		//cv::namedWindow("proc", cv::WINDOW_NORMAL);
		//cv::imshow("proc", out);
		//cv::waitKey(0);
	}
	//if (!rev)
		//out = 255 - out;

	delete cont;
	return out;
}

void saltAndPepper()
{
	bool rev = true;
	const int NSIZE = 4;
	// ####################

	//cv::Mat mat = cv::imread("D:\\2.png", cv::IMREAD_GRAYSCALE);
	cv::Mat mat = cv::imread("D:\\Learning\\papers\\CO3\\result.png", cv::IMREAD_GRAYSCALE);

	mat = proc(mat);
	mat = 255 - mat;
	mat = proc(mat);
	mat = 255 - mat;


	cv::namedWindow("res", cv::WINDOW_NORMAL);
	cv::imshow("res", mat);
	cv::waitKey(0);
	cv::imwrite("D:\\noiseless.png", mat);

}


int main()
{
	// doMagickDOTA();
	//  testAll();
	saltAndPepper();

	return 0;
}
