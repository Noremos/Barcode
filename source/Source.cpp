
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>


#include "barcodeCreator.h" 

#include <iostream>
#include <filesystem>
#include <string>
#include <fstream>

/////////////////////////////
/////////////////////////////
/////////////////////////////  at ((row,col) = (hei,wid)
/////////////////////////////
/////////////////////////////

using std::string;
//typedef std::pair<bc::BarcodeTwo*, bool> net;

//#define inputArg 3 analysis\images\polus.png analysis\datasets
//#define inputArg 3 analysis\images\winter2.png analysis\datasets
//#define inputArg 3 analysis\images\big.tif analysis\datasets


namespace bc
{
	struct info
	{
		Barcontainer<uchar>* bar;
		int type;

		info(Barcontainer<uchar>* bar, int type)
		{
			this->bar = bar;
			this->type = type;
		}
	};
}
int main()
{
	bc::BarConstructor<uchar> bcont;
	bcont.addStructire(bc::ProcType::f0t255, bc::ColorType::gray, bc::ComponentType::Component);
	bcont.createBinayMasks = false;
	bcont.createGraph = false;
	bcont.createBinayMasks = false;
	bcont.returnType = bc::ReturnType::barcode2d;
	bcont.setStep(1);
	bcont.maxTypeValue.set(255);


	/*cv::Mat testmat = cv::imread("test5.png", cv::IMREAD_ANYCOLOR);
	int w = testmat.cols, h = testmat.rows;*/
	//, 1, testmat.data);
	bc::BarImg<uchar> img(1, 1);

	bc::BarcodeCreator<uchar> test;

	test.createBarcode(img,bcont);

	uchar* data = new uchar[36]{
	63,121,73,14,120,135,
	237,90,194,136,4,43,
	90,212,193,199,88,154,
	51,150,98,239,42,68,
	65,141,145,34,203,167,
	158,234,20,145,80,176
	};

	img.assignData(6, 6, 1, data);
	test.createBarcode(img,bcont);

	uchar* data2 = new uchar[25]{
	63,121,73,14,120,
	237,90,194,136,4,
	90,212,193,199,88,
	51,150,98,239,42,
	65,141,145,34,203
	};

	img.assignData(5, 5, 1, data2);
	test.createBarcode(img,bcont);

	uchar* data3 = new uchar[16]{
	63,121,73,14,
	237,90,194,136,
	90,212,193,199,
	51,150,98,239
	};

	img.assignData(4, 4, 1, data3);
	test.createBarcode(img,bcont);
	
	uchar* data4 = new uchar[9]{
	63,121,73,
	237,90,194,
	90,212,193
	};

	img.assignData(3, 3, 1, data3);
	test.createBarcode(img,bcont);

	return 0;
}
