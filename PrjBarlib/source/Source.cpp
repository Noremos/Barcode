#include "barcodeCreator.h"

#include <iostream>
#include <string>

#include "../algorithmes/detection.h"
#include "../algorithmes/noise.h"
#include "../algorithmes/roofs.h"
#include "../algorithmes/ExportStats.h"
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
	//testImg("D:\\Programs\\Python\\barcode\\roofs\\t2\\ident-simple4.png"); // bigimg.jpg

	// testSimple();
}

//
//void compireMethods()
//{
//	bool rev = true;
//	const int NSIZE = 4;
//	// ####################
//
//	//cv::Mat mat = cv::imread("D:\\2.png", cv::IMREAD_GRAYSCALE);
//	cv::Mat source = cv::imread("D:\\Learning\\papers\\CO3\\testimg\\dog.jpg", cv::IMREAD_GRAYSCALE);
//	if (source.rows == 0)
//	{
//		std::cerr << "The image not found";
//		return;
//	}
//
//	Mat noiseMat = source.clone();
//	Bmat8 noised(noiseMat);
//
//	float p = 0.1;
//
//	noise(noise_typ::gauss, noised, p);
//	//noiseMat = noised.mat;
//
//	Mat denoisedOut, denoisedMedian;
//	noiseMat.copyTo(denoisedOut);
//
//	// median
//	denoisedMedian = medianBlur(noiseMat, 3);
//
//	// out method
//	denoisedOut = removeNoise(denoisedOut);
//	denoisedOut = 255 - denoisedOut;
//	denoisedOut = removeNoise(denoisedOut);
//	denoisedOut = 255 - denoisedOut;
//
//	// end ------------------------------------------------------
//
//	cout << "Our SKO: " << getSKO(source, denoisedOut) << endl;
//	cout << "Our PSNR: " << getPSNR(source, denoisedOut) << endl;
//	cout << "Med SKO: " << getSKO(source, denoisedMedian) << endl;
//	cout << "Med PSNR: " << getPSNR(source, denoisedMedian) << endl;
//
//	cv::namedWindow("source", cv::WINDOW_NORMAL);
//	cv::imshow("source", source);
//
//	cv::namedWindow("noised", cv::WINDOW_NORMAL);
//	cv::imshow("noised", noiseMat);
//
//	cv::namedWindow("our", cv::WINDOW_NORMAL);
//	cv::imshow("our", denoisedOut);
//
//	cv::namedWindow("median", cv::WINDOW_NORMAL);
//	cv::imshow("median", denoisedMedian);
//
//	cv::waitKey(0);
//}
//
//void prepSegm()
//{
//	//string path = "D:/Programs/Python/barcode/roofs/imgs/8.bmp";
//	string path = "D:/Learning/BAR/base/1.png";
//	segment(path);
//	//segment<uchar>(path);
//}


bc::BarConstructor defineConstr()
{
	bc::BarConstructor bcont;
	bcont.addStructure(bc::ProcType::f0t255, bc::ColorType::gray, bc::ComponentType::Component);
	bcont.createBinaryMasks = true;
	bcont.createGraph = false;
	bcont.createBinaryMasks = false;
	bcont.returnType = bc::ReturnType::barcode2d;
	//bcont.setMaxPorog(1);
	//bcont.maxTypeValue.set(255);

	return bcont;
}

bc::BarImg restore255ToBarimg(bc::Barcontainer* cont, int wid, int hei, Barscalar maxval)
{
	auto* it = cont->getItem(0);
	auto& lines = it->barlines;
	bc::BarImg img(wid, hei);
	for (size_t i = 0; i < wid * hei; i++)
	{
		img.setLiner(i, maxval);
	}

	for (size_t i = 0; i < lines.size(); i++)
	{
		Barscalar start = lines[i]->start;
		Barscalar end = lines[i]->end();
		auto& matr = lines[i]->matr;
		for (size_t k = 0; k < matr.size(); k++)
		{
			auto& p = matr[k];
			//assert(start <= p.value && p.value <= end);
			img.minus(p.getPoint(), p.value);
		}
	}
	return img;
}


void compiteBarAndBar(BarImg& img0, BarImg& img1)
{
	for (int i = 0; i < img0.hei(); i++)
	{
		for (int j = 0; j < img0.wid(); j++)
		{
			Barscalar av = img0.get(j, i);
			Barscalar bv = img1.get(j, i);
			cout << (int)bv.data.b1 << " ";
			assert(av == bv);
		}
	}
}

void printImg(BarImg& img0)
{
	for (int i = 0; i < img0.hei(); i++)
	{
		for (int j = 0; j < img0.wid(); j++)
		{
			std::cout << std::setw(4);
			cout << (int)img0.get(j, i).getByte8();
		}
		std::cout << std::endl;
	}
}
#include "MachineProcessor.h"
#include "CellAutomatProcessor.h"


void tryImg()
{
	bc::BarcodeCreator test;
	auto bcont = defineConstr();
	bcont.structure[0].proctype = bc::ProcType::experiment;
	//bcont.visualize = true;
	const int k = 500;
	uchar maxv = 0;
	bc::BarImg img(k, k);
	uchar* data5 = new uchar[k * k]
	{
		1, 1, 6,
		1, 9, 3,
		5, 3, 3,
	};

	for (size_t i = 0; i < k; i++)
	{
		for (size_t j = 0; j < k; j++)
		{
			img.set(i, j, rand() % 255);
		}
	}

	Barcontainer* ret = test.createBarcode(&img, bcont);
	delete ret;
}


MatrImg drawImg(DatagridProvider* img, const bc::Baritem::barsplitvec& input)
{
	MatrImg res(img->wid(), img->hei(), img->channels());
	for (size_t i = 0; i < img->length(); i++)
		res.setLiner(i, img->getLiner(i));

	float totSize = img->length();
	int oked = 0;
	for (auto& line : input)
	{
		auto& matr = line.line->matr;
		float sizes = matr.size();
		Barscalar color = Barscalar(MRNG::randi(0, 255), MRNG::randi(0, 255), MRNG::randi(0, 255));
		for (size_t w = 0; w < matr.size(); ++w)
		{
			res.set(matr[w].getX(), matr[w].getY(), color);
		}
	}

	return res;
}

void entrotySplit(DatagridProvider* img)
{
	BarcodeCreator creator;
	bc::BarConstructor consrt;
	consrt.addStructure((bc::ProcType::invertf0), (bc::ColorType::rgb), bc::ComponentType::Component);
	consrt.createBinaryMasks = true;
	consrt.createGraph = true;
	consrt.returnType = ReturnType::barcode2d;

	bc::Barcontainer* bar = creator.createBarcode(img, consrt);

	Baritem::barsplitvec low, high;
	bar->getItem(0)->splitByValue(low, high);
	MatrImg lowImg = drawImg(img, low);
	MatrImg highImg = drawImg(img, high);

	cv::namedWindow("low", cv::WINDOW_NORMAL);
	cv::imshow("low", bc::convertProvider2Mat(&lowImg));

	cv::namedWindow("hith", cv::WINDOW_NORMAL);
	cv::imshow("hith", bc::convertProvider2Mat(&highImg));
	cv::waitKey(0);
	delete bar;
}


int main()
{
	//MasResultsExperts();
	//return 0;
	//getResults23();
	//return 0;
	////tryImg();
	////return 0;
	//srand(time(0));

	string pbase = "D:/Programs/C++/Barcode/PrjBarlib/researching/tiles/";
	//string path = "D:/Learning/papers/CO_compressing/base16t.png";
	Mat imgs = cv::imread(pbase + "5_set.png", cv::IMREAD_COLOR);
	Mat mask = cv::imread(pbase + "5_bld.png", cv::IMREAD_GRAYSCALE);
	bc::BarMat imgsWrap(imgs, BarType::BYTE8_3);
	bc::BarMat maskWrap(mask, BarType::BYTE8_1);

	entrotySplit(&imgsWrap);
	//MachComb(imgsWrap, maskWrap);
	return 0;



	////CellAutomatProcessor::fullProcess2(imgsWrap, maskWrap);
	//MachinePRocessor::fullProcess2(imgsWrap, maskWrap);

	//return 0;
	//getResultsExperts();
	return 0;
	bc::BarcodeCreator test;
	auto bcont = defineConstr();
	bcont.structure[0].proctype = bc::ProcType::StepRadius;
	//bcont.structure[0].comtype = bc::ComponentType::Hole;
	bcont.visualize = false;
	const int k = 3;
	uchar maxv = 0;
	bc::BarImg img(k, k, 1);

	uchar* data5 = new uchar[k * k]
	{
		5, 0, 4,
		3, 3, 5,
		4, 3, 5,

		/*5, 0, 1,
		3, 6, 6,
		4, 2, 1,*/
	};

	//srand(1);
	for (size_t i = 0; i < k * k; i++)
	{
		img.setLiner(i, data5[i]);
		//data5[i] = rand() % 255;
		//if (data5[i] > maxv)
		//	maxv = data5[i];
	}
	//	const int k = 2;
	//	uchar maxv = 5; 
	//	uchar* data5 = new uchar[k * k]{
	//4,5,
	//5,5
	//	};

		//img.setDataU8(k, k, data5);
	printImg(img);

	Barcontainer* ret = test.createBarcode(&img, bcont);
	//BarImg out = restore255ToBarimg(ret, k, k, maxv);
	Baritem* nf = ret->getItem(0);

	for (size_t i = 0; i < nf->barlines.size(); i++)
	{
		uchar* dataE = new uchar[k * k]
		{
			0, 0, 0,
			0, 0, 0,
			0, 0, 0,
		};
		auto& d = nf->barlines[i]->getMatrix();
		for (size_t j = 0; j < d.size(); j++)
		{
			dataE[d[j].getY() * 3 + d[j].getX()] = d[j].value.getAvgUchar();
		}

		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				cout << (int)dataE[i * 3 + j] << " ";
			}
			std::cout << std::endl;
		}
	}

	std::cout << std::endl;
	//printImg(out);
	//compiteBarAndBar(img, out);
	// doMagickDOTA();
	//  testAll();
	//compireMethods();
	//prepSegm();
	//getResults();
	return 0;
}
