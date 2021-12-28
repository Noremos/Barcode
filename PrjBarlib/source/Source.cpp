#include "barcodeCreator.h"

#include <iostream>
#include <string>

#include "../algorithmes/detection.h"
#include "../algorithmes/noise.h"
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


void compireMethods()
{
	bool rev = true;
	const int NSIZE = 4;
	// ####################

	//cv::Mat mat = cv::imread("D:\\2.png", cv::IMREAD_GRAYSCALE);
	cv::Mat source = cv::imread("D:\\Learning\\papers\\CO3\\testimg\\dog.jpg", cv::IMREAD_GRAYSCALE);
	if (source.rows == 0)
	{
		std::cerr << "The image not found";
		return;
	}

	Mat noiseMat = source.clone();
	Bmat8 noised(noiseMat);
	
	float p = 0.1;

	noise(noise_typ::gauss, noised, p);
	//noiseMat = noised.mat;

	Mat denoisedOut, denoisedMedian;
	noiseMat.copyTo(denoisedOut);

	// median
	denoisedMedian = medianBlur(noiseMat, 3);

	// out method
	denoisedOut = removeNoise(denoisedOut);
	denoisedOut = 255 - denoisedOut;
	denoisedOut = removeNoise(denoisedOut);
	denoisedOut = 255 - denoisedOut;

	// end ------------------------------------------------------
	
	cout << "Our SKO: " << getSKO(source, denoisedOut) << endl;
	cout << "Our PSNR: " << getPSNR(source, denoisedOut) << endl;
	cout << "Med SKO: " << getSKO(source, denoisedMedian) << endl;
	cout << "Med PSNR: " << getPSNR(source, denoisedMedian) << endl;

	cv::namedWindow("source", cv::WINDOW_NORMAL);
	cv::imshow("source", source);

	cv::namedWindow("noised", cv::WINDOW_NORMAL);
	cv::imshow("noised", noiseMat);

	cv::namedWindow("our", cv::WINDOW_NORMAL);
	cv::imshow("our", denoisedOut);

	cv::namedWindow("median", cv::WINDOW_NORMAL);
	cv::imshow("median", denoisedMedian);

	cv::waitKey(0);
}


int main()
{
	// doMagickDOTA();
	//  testAll();
	compireMethods();

	return 0;
}
