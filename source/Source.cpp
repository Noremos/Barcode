#include "barcodeCreator.h" 

#include <iostream>
#include <string>

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
	bcont.createGraph = true;
	bcont.createBinayMasks = true;
	bcont.returnType = bc::ReturnType::barcode2d;
	bcont.setStep(1);
	bcont.setMaxValue(255);


	/*cv::Mat testmat = cv::imread("test5.png", cv::IMREAD_ANYCOLOR);
	int w = testmat.cols, h = testmat.rows;*/
	//, 1, testmat.data);
	bc::BarImg<uchar> img(1, 1);

	bc::BarcodeCreator<uchar> test;

	uchar* data = new uchar[36]{
	63,121,73,14,120,135,
	237,90,194,136,4,43,
	90,212,193,199,88,154,
	51,150,98,255,42,68,
	65,141,145,34,203,167,
	158,234,20,145,80,176
	};

	uchar backup[36];

	memcpy(backup, data, 36);

	img.assignData(6, 6, 1, data);
	auto* ret = test.createBarcode(img, bcont);

	memset(data, 255, 36);

	
	auto* it = ret->getItem(0);

	auto& lines = it->barlines;
	for (size_t i = 0; i < lines.size(); i++)
	{
		auto& matr = lines[i]->matr;
		for (size_t k = 0; k < matr.size(); k++)
		{
			auto& p = matr[k];
			data[p.point.getLiner(6)] -=p.value;
		}
	}
	int r = memcmp(data, backup, 36);
	assert(r == 0);

	//uchar* data2 = new uchar[25]{
	//63,121,73,14,120,
	//237,90,194,136,4,
	//90,212,193,199,88,
	//51,150,98,239,42,
	//65,141,145,34,203
	//};

	//img.assignData(5, 5, 1, data2);
	//test.createBarcode(img,bcont);

	//uchar* data3 = new uchar[16]{
	//63,121,73,14,
	//237,90,194,136,
	//90,212,193,199,
	//51,150,98,239
	//};

	//img.assignData(4, 4, 1, data3);
	//test.createBarcode(img,bcont);
	//
	//uchar* data4 = new uchar[9]{
	//63,121,73,
	//237,90,194,
	//90,212,193
	//};

	//img.assignData(3, 3, 1, data3);
	//test.createBarcode(img,bcont);

	return 0;
}
