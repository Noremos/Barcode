#include "barcodeCreator.h" 

#include <iostream>
#include <string>


#include "../detection.h"
#include "tests.h"


void tetsAll()
{
	
	// TODO Move it to test project
	//testInitFromMat();

	//printf("raw data tests: star...");
	//checkImgFromData2();
	//checkImgFromData3();
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
	testImg("D:\\Programs\\Python\\barcode\\roofs\\t2\\ident-simple4.png");//bigimg.jpg


	//testSimple();
}
int main()
{
	//doMagickDOTA();
	return 0;
}
