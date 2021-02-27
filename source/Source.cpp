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


typedef bc::BarImg<uchar> Bimg8;
typedef bc::BarMat<uchar> Bmat8;
typedef bc::DatagridProvider<uchar> Bbase8;

void set(bc::barline<uchar>* rt, Bimg8* data)
{
	auto& matr = rt->matr;
	for (size_t k = 0; k < matr.size(); k++)
	{
		auto& p = matr[k];
		data->minus(p.point, p.value);
	}

	for (size_t i = 0; i < rt->childrens.size(); i++)
	{
		set(rt->childrens[i], data);
	}
}

void compiteBarAndMat(bc::DatagridProvider<uchar>* img, Mat& mat)
{
	for (int i = 0; i < img->hei(); i++)
	{
		for (int j = 0; j < img->wid(); j++)
		{
			uchar av = img->get(j, i);
			uchar bv = mat.at<uchar>(i, j);
			assert(av == bv);
		}
	}
}

void compiteBarAndBar(Bbase8& img, Bbase8& mat)
{
	for (int i = 0; i < img.hei(); i++)
	{
		for (int j = 0; j < img.wid(); j++)
		{
			uchar av = img.get(j, i);
			uchar bv = mat.get(j, i);
			assert(av == bv);
		}
	}
}


void testInitFromMat()
{
	Mat testmat = cv::imread("test/test5.png", cv::IMREAD_GRAYSCALE);
	Bmat8 img(testmat);
	compiteBarAndMat(&img, testmat);
}


Bimg8 restreToBarimg(bc::Barcontainer<uchar>* cont, int wid, int hei, uchar maxval)
{
	auto* it = cont->getItem(0);
	auto& lines = it->barlines;
	Bimg8 img(wid, hei);
	memset(img.getData(), maxval, static_cast<size_t>(wid) * hei);

	for (size_t i = 0; i < lines.size(); i++)
	{
		auto& matr = lines[i]->matr;
		for (size_t k = 0; k < matr.size(); k++)
		{
			auto& p = matr[k];
			img.minus(p.point, p.value);
		}
	}
	return img;
}

Bimg8 restreToBarimgFromGraph(bc::Barcontainer<uchar>* cont, int wid, int hei, uchar maxval)
{
	auto* it = cont->getItem(0);
	auto& lines = it->barlines;
	Bimg8 img(wid, hei);
	memset(img.getData(), maxval, static_cast<size_t>(wid) * hei);

	set(it->getRootNode(), &img);

	return img;
}



void test(bool graph, Bbase8& testimg)
{
	bc::BarConstructor<uchar> bcont;
	bcont.addStructire(bc::ProcType::f0t255, bc::ColorType::gray, bc::ComponentType::Component);
	bcont.createBinayMasks = true;
	bcont.createGraph = graph;
	bcont.returnType = bc::ReturnType::barcode2d;
	bcont.setStep(255);

	bc::BarcodeCreator<uchar> test;

	auto* ret = test.createBarcode(&testimg, bcont);

	//testimg.diagReverce = false;
	uchar max = testimg.max();

	Bimg8 imgrest(1, 1);
	if (graph)
	{
		imgrest = restreToBarimgFromGraph(ret, testimg.wid(), testimg.hei(), max);
	}
	else
	{
		imgrest = restreToBarimg(ret, testimg.wid(), testimg.hei(), max);
	}

	Mat res = bc::convertProvider2Mat(&imgrest);

	//cv::namedWindow("orig", cv::WINDOW_NORMAL);
	//cv::imshow("orig", orig);
	cv::namedWindow("restored", cv::WINDOW_NORMAL);
	cv::imshow("restored", res);
	cv::waitKey(1);
	compiteBarAndBar(imgrest, testimg);
}

void testMat(bool graph, Mat testmat)
{
	Bmat8 img(testmat);
	test(graph, img);
}

void testData(bool graph, uchar* data, int lend)
{
	Bimg8 img(lend, lend, 1, data, false, false);
	test(graph, img);
}

void checkImgFromData2()
{
	const int lend = 2;
	uchar data[lend * lend]{
		63,121,
		237,90
	};

	testData(false, data, lend);
	testData(true, data, lend);
}

void checkImgFromData3()
{
	const int lend = 3;
	uchar data[lend * lend]{
		63,121,73,
		237,90,194,
		90,212,193
	};

	testData(false, data, lend);
	testData(true, data, lend);
}

void checkImgFromData4()
{
	const int lend = 4;
	uchar data[lend * lend]{
		63,121,73,14,
		237,90,194,136,
		90,212,193,199,
		51,150,98,239
	};

	testData(false, data, lend);
	testData(true, data, lend);
}

void checkImgFromData5()
{
	const int lend = 5;
	uchar data[lend * lend]{
		63,121,73,14,120,
		237,90,194,136,4,
		90,212,193,199,88,
		51,150,98,239,42,
		65,141,145,34,203
	};

	testData(false, data, lend);
	testData(true, data, lend);
}

void checkImgFromData6()
{
	const int lend = 6;
	uchar data[lend * lend]{
63, 121, 73, 14, 120,135,
237,90,  194,136,4,  43,
90, 212, 193,199,88, 154,
51, 150, 98, 255,42, 68,
65, 141, 145,34, 203,167,
158,234, 20, 145,80, 176
	};

	testData(false, data, lend);
	testData(true, data, lend);
}

void checkSingleMat()
{
	Mat temp(2, 2, CV_8UC1);
	//		63,121,
	// 237, 90
	temp.at<uchar>(0, 0) = 63;
	temp.at<uchar>(0, 1) = 121;
	temp.at<uchar>(1, 0) = 237;
	temp.at<uchar>(1, 1) = 90;
	bc::BarMat<uchar> it(temp);
	compiteBarAndMat(&it, temp);

	testMat(false, temp);
	testMat(true, temp);
}
void testMats()
{
	string testsuit[]{ "as4.png","as3.png","as2.png","as1.png","as.png" };
	for (auto& test : testsuit)
	{
		Mat testmat = cv::imread((string)"test/" + test, cv::IMREAD_GRAYSCALE);
		//testmat = testmat(cv::Range(10, 20), cv::Range(7, 10));
		cv::namedWindow("orig", cv::WINDOW_NORMAL);
		cv::imshow("orig", testmat);
		cv::waitKey(1);
		Bmat8 img(testmat);
		compiteBarAndMat(&img, testmat);

		testMat(true, testmat);
		testMat(false, testmat);
	}
}


int main()
{
	// TODO Move i to test project
	testInitFromMat();

	printf("raw data tests: star...");
	checkImgFromData2();
	checkImgFromData3();
	checkImgFromData4();
	checkImgFromData5();
	checkImgFromData6();
	printf("done\n\n");

	printf("mat tests: star...");
	checkSingleMat();
	testMats();
	printf("done\n\n");

	return 0;
}
