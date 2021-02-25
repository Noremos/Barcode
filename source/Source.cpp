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


void set(bc::barline<uchar>* rt, uchar* data, int clend)
{
	auto& matr = rt->matr;
	for (size_t k = 0; k < matr.size(); k++)
	{
		auto& p = matr[k];
		data[p.point.getLiner(clend)] -= p.value;
	}

	for (size_t i = 0; i < rt->childrens.size(); i++)
	{
		set(rt->childrens[i], data, clend);
	}
}

void set(bc::BarRoot<uchar>* rt, uchar* data, int clend)
{
	for (size_t i = 0; i < rt->children.size(); i++)
	{
		set(rt->children[i], data, clend);
	}
}


int main()
{
	bc::BarConstructor<uchar> bcont;
	bcont.addStructire(bc::ProcType::f0t255, bc::ColorType::gray, bc::ComponentType::Component);
	bcont.createBinayMasks = true;
	bcont.createGraph = false;
	bcont.returnType = bc::ReturnType::barcode2d;
	bcont.setStep(0);


	/*cv::Mat testmat = cv::imread("test5.png", cv::IMREAD_ANYCOLOR);
	int w = testmat.cols, h = testmat.rows;*/
	//, 1, testmat.data);
	bc::BarImg<uchar> img(1, 1);

	bc::BarcodeCreator<uchar> test;

	uchar* data = new uchar[36]{
	63, 121, 73, 14, 120,135,
	237,90,  194,136,4,  43,
	90, 212, 193,199,88, 154,
	51, 150, 98, 255,42, 68,
	65, 141, 145,34, 203,167,
	158,234, 20, 145,80, 176
	};

	//uchar* data = new uchar[9]{
	//63,121,73,
	//237,90,194,
	//90,212,193
	//};
	const int lend = 6;
	const uchar max = 255;
	uchar backup[lend * lend];

	//memcpy(backup, data, 36);
	//img.assignData(6, 6, 1, data);

	memcpy(backup, data, lend * lend);
	img.assignData(lend, lend, 1, data);

	auto* ret = test.createBarcode(img, bcont);

	memset(data, max, lend * lend);

	auto* it = ret->getItem(0);

	auto& lines = it->barlines;
	for (size_t i = 0; i < lines.size(); i++)
	{
		auto& matr = lines[i]->matr;
		for (size_t k = 0; k < matr.size(); k++)
		{
			auto& p = matr[k];
			data[p.point.getLiner(lend)] -= p.value;
		}
	}
	int r = memcmp(data, backup, lend * lend);
	assert(r == 0);
	memset(data, max, lend * lend);

	if (bcont.createGraph)
	{
		set(it->getRootNode(), data, lend);
		r = memcmp(data, backup, lend * lend);
		assert(r == 0);
	}
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
