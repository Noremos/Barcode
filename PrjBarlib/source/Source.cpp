#include "barcodeCreator.h" 

#include <iostream>
#include <string>

/////////////////////////////
/////////////////////////////
/////////////////////////////  at ((row,col) = (hei,wid)
/////////////////////////////
/////////////////////////////

using std::string;
using namespace bc;
//typedef std::pair<bc::BarcodeTwo*, bool> net;

//#define inputArg 3 analysis\images\polus.png analysis\datasets
//#define inputArg 3 analysis\images\winter2.png analysis\datasets
//#define inputArg 3 analysis\images\big.tif analysis\datasets

void testImg(string path)
{
	std::vector<bc::barstruct> constr;
	constr.push_back(barstruct(ProcType::f0t255, ColorType::gray, ComponentType::Hole));
	constr.push_back(barstruct(ProcType::f255t0, ColorType::gray, ComponentType::Hole));
	bc::barcodeCreator test;

	cv::Mat testmat = cv::imread(path, cv::IMREAD_GRAYSCALE);
	//Mat testmat = cv::imread((string)"D:\\Programs\\Python\\barcode\\lenna.png", cv::IMREAD_GRAYSCALE);
	auto* ret = test.createBarcode(testmat, constr);
	cv::namedWindow("test", cv::WINDOW_NORMAL);
	cv::imshow("test", testmat);
	cv::waitKey(0);
	//caclSize(ret);
	delete ret;
}

#include "../detection.h"

int main()
{
	//doMagickDOTA();
	// TODO Move it to test project
	//testInitFromMat();

	//printf("raw data tests: star...");
	//checkImgFromData2();
	//checkImgFromData4();
	//checkImgFromData5();
	//checkImgFromData6();
	//printf("done\n\n");

	//printf("mat tests: star...");
	//checkSingleMat();
	//testMats();
	//printf("done\n\n");
	//
	//printf("maxLen test: sart...\n");
	//testMaxLen();
	//printf("done\n\n");

	// printf("BigImg test: start...\n");
	// checkBigImg();
	// printf("done\n\n");

	//printf("Check fix for zero len: sart...\n");
	//checkSameVals();
	//printf("done\n\n");

	/*printf("Check float imgs: sart...\n");
	testFloatMats();
	printf("done\n\n");*/

	//printf("Check problem float imgs: sart...\n");
	//testProblemFloatMats();
	//printf("done\n\n");

	//printf("Check big problem float imgs: sart...\n");
	//testBigProblemFloatMats();
	//printf("done\n\n");

	//testImg("../PrjTests/data/city.png");//bigimg.jpg
	//testImg("D:\\Programs\\Python\\barcode\\roofs\\t2\\ident-simple4.png");//bigimg.jpg
	//
	testImg("D:\\Learning\\papers\\CO2\\python\\distortion\\0_0_0\\26.jpg");

	//testSimple();
	return 0;
}
