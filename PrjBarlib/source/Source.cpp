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
		data->minus(p.getPoint(data->wid()), p.value);
	}

	for (size_t i = 0; i < rt->children.size(); i++)
	{
		::set(rt->children[i], data);
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


void compiteBarAndBar(bc::BarImg<float>& img, bc::BarImg<float>& mat)
{
	for (int i = 0; i < img.hei(); i++)
	{
		for (int j = 0; j < img.wid(); j++)
		{
			float av = round(img.get(j, i) * 10000);
			float bv = round(mat.get(j, i) * 10000);
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


template<class T>
bc::BarImg<T> restoreToBarimg(bc::Barcontainer<T>* cont, int wid, int hei, T maxval)
{
	auto* it = cont->getItem(0);
	auto& lines = it->barlines;
	bc::BarImg<T> img(wid, hei);
	for (size_t i = 0; i < wid * hei; i++)
	{
		img.setLiner(i, maxval);
	}

	for (size_t i = 0; i < lines.size(); i++)
	{
		T start = lines[i]->start;
		T end = start + lines[i]->len;
		auto& matr = lines[i]->matr;
		for (size_t k = 0; k < matr.size(); k++)
		{
			auto& p = matr[k];
			//assert(start <= p.value && p.value <= end);
			img.minus(p.getPoint(wid), p.value);
		}
	}
	return img;
}



template<class T>
bc::BarImg<T> restore255ToBarimg(bc::Barcontainer<T>* cont, int wid, int hei, T minval)
{
	auto* it = cont->getItem(0);
	auto& lines = it->barlines;
	bc::BarImg<T> img(wid, hei);
	for (size_t i = 0; i < wid * hei; i++)
	{
		img.setLiner(i, minval);
	}

	for (size_t i = 0; i < lines.size(); i++)
	{
		T start = lines[i]->start;
		T end = start + lines[i]->len;
		auto& matr = lines[i]->matr;
		for (size_t k = 0; k < matr.size(); k++)
		{
			auto& p = matr[k];
			//assert(start <= p.value && p.value <= end);
			img.add(p.getPoint(wid), p.value);
		}
	}
	return img;
}

Bimg8 restoreToBarimgFromGraph(bc::Barcontainer<uchar>* cont, int wid, int hei, uchar maxval)
{
	auto* it = cont->getItem(0);
	auto& lines = it->barlines;
	Bimg8 img(wid, hei);
	memset(img.getData(), maxval, static_cast<size_t>(wid) * hei);

	::set(it->getRootNode(), &img);

	return img;
}

void test(bool graph, Bbase8& testimg, bool createNew = false)
{
	bc::BarConstructor<uchar> bcont;
	bcont.addStructure(bc::ProcType::f0t255, bc::ColorType::gray, bc::ComponentType::Component);
	bcont.createBinaryMasks = true;
	bcont.createGraph = graph;
	bcont.returnType = bc::ReturnType::barcode2d;
	bcont.createNewComponentOnAttach = createNew;
	bcont.setStep(255);

	bc::BarcodeCreator<uchar> test;

	auto* ret = test.createBarcode(&testimg, bcont);

	//testimg.diagReverce = false;
	uchar max, min;
	testimg.maxAndMin(min, max);

	Bimg8 imgrest(1, 1);
	if (graph)
	{
		imgrest = restoreToBarimgFromGraph(ret, testimg.wid(), testimg.hei(), max);
	}
	else
	{
		imgrest = restoreToBarimg(ret, testimg.wid(), testimg.hei(), max);
	}

	Mat orig = bc::convertProvider2Mat(&testimg);
	Mat res = bc::convertProvider2Mat(&imgrest);

	cv::namedWindow("orig", cv::WINDOW_NORMAL);
	cv::imshow("orig", orig);
	cv::namedWindow("restored", cv::WINDOW_NORMAL);
	cv::imshow("restored", res);
	cv::waitKey(1);
	compiteBarAndBar(imgrest, testimg);

	ret->removePorog(100);

	delete ret;
}

void testf255t0(Bbase8& testimg)
{
	bc::BarConstructor<uchar> bcont;
	bcont.addStructure(bc::ProcType::f255t0, bc::ColorType::gray, bc::ComponentType::Component);
	bcont.createBinaryMasks = true;
	bcont.createGraph = false;
	bcont.returnType = bc::ReturnType::barcode2d;
	bcont.createNewComponentOnAttach = false;
	bcont.setStep(255);

	bc::BarcodeCreator<uchar> test;
	for (size_t i = 0; i < testimg.length(); i++)
	{
		uchar& b = testimg.getLiner(i);
		b = 255 - b;
	}
	auto* ret = test.createBarcode(&testimg, bcont);

	auto* it = ret->getItem(0);
	auto& lines = it->barlines;

	int wid = testimg.wid();
	int hei = testimg.hei();
	Bimg8 img(wid, hei);
	memset(img.getData(), 0, static_cast<size_t>(wid) * hei);

	for (size_t i = 0; i < lines.size(); i++)
	{
		auto& matr = lines[i]->matr;
		for (size_t k = 0; k < matr.size(); k++)
		{
			auto& p = matr[k];
			img.add(p.getPoint(wid), p.value);
		}
	}

	Mat orig = bc::convertProvider2Mat(&testimg);
	Mat res = bc::convertProvider2Mat(&img);
	cv::namedWindow("restored", cv::WINDOW_NORMAL);
	//cv::imshow("restored", res);
	cv::namedWindow("original", cv::WINDOW_NORMAL);
	//cv::imshow("original", orig);
	cv::waitKey(1);
	compiteBarAndBar(img, testimg);

	delete ret;
}

void testMat(bool graph, Mat testmat, bool createNew = false)
{
	Bmat8 img(testmat);
	test(graph, img, createNew);
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
		0,121,
		255,90
	};
	Bimg8 img(2, 2, 1, data);
	//testf255t0(img);

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
void testMats(bool createNew = false)
{
	string testsuit[]{ "as.png", "as3.png", "as4.png","as2.png","as1.png" };
	for (auto& test : testsuit)
	{
		Mat testmat = cv::imread((string)"../PrjTests/data/tests/" + test, cv::IMREAD_GRAYSCALE);
		//testmat = testmat(cv::Range(10, 20), cv::Range(7, 10));
		cv::namedWindow("orig", cv::WINDOW_NORMAL);
		//cv::imshow("orig", testmat);
		cv::waitKey(1);
		Bmat8 img(testmat);
		compiteBarAndMat(&img, testmat);

		//testf255t0(img);

		testMat(true, testmat);
		testMat(false, testmat);
		testMat(false, testmat, createNew);
	}
}

#include <fstream>
#include <iterator>
#include <vector>


void readImg(std::string path, std::vector<char>& vec)
{
	std::ifstream input(path, std::ios::binary);

	vec.assign(
		(std::istreambuf_iterator<char>(input)),
		(std::istreambuf_iterator<char>()));
}


union conv
{
	char data[4];
	float val;
	int ival;
};


void testProblemFloatMats()
{
	std::string pathFromGeo = "D:\\Programs\\Python\\barcode\\experiments\\geo\\imgs_one\\imgOut0.bf";

	std::string pathArctic = "D:\\Programs\\QT\\ArctivViewer\\ArcticViewer\\temp\\out.fd";

	std::vector<char> vec, vec2;

	readImg(pathFromGeo, vec);
	bc::BarImg<float> imgFromGeo((int)60, (int)60, 1, (uchar*)vec.data() + 2, false, false);


	readImg(pathArctic, vec2);
	bc::BarImg<float> findedImg((int)vec2[0], (int)vec2[1], 1, (uchar*)vec2.data() + 2, false, false);

	char c1[4]{ vec2[2],vec2[3] ,vec2[4] ,vec2[5] };
	char c2[4]{ vec2[5],vec2[4] ,vec2[3] ,vec2[2] };

	conv con1, con2;
	con1.data[0] = vec2[2];
	con1.data[1] = vec2[3];
	con1.data[2] = vec2[4];
	con1.data[3] = vec2[5];

	con2.data[3] = vec2[2];
	con2.data[2] = vec2[3];
	con2.data[1] = vec2[4];
	con2.data[0] = vec2[5];


	bc::BarConstructor<float> bcont;
	bcont.addStructure(bc::ProcType::f0t255, bc::ColorType::gray, bc::ComponentType::Component);
	bcont.createBinaryMasks = false;
	bcont.createGraph = false;
	bcont.returnType = bc::ReturnType::barcode3d;
	bcont.createNewComponentOnAttach = false;
	bcont.setStep(255);
	bc::BarcodeCreator<float> test;


	//auto* retFromQGIS = test.createBarcode(&imgFromGeo, bcont);
	//auto* qgis = retFromQGIS->getItem(0);
	//retFromQGIS->relen();
	//retFromQGIS->removePorog(3);

	auto* retFromArctic = test.createBarcode(&findedImg, bcont);
	retFromArctic->relen();
	retFromArctic->removePorog(3);
	auto* arcticItem = retFromArctic->getItem(0);


	delete retFromArctic;
	//float ret = qgis->compireFull(arcticItem, bc::CompireStrategy::CommonToLen);
	//std::cout << "Ret: " << ret << std::endl;
}


void testBigProblemFloatMats()
{
	std::string pathBig = "D:\\Programs\\QT\\ArctivViewer\\ArcticViewer\\temp\\outbig.bf";

	std::vector<char> vec;

	readImg(pathBig, vec);

	int w = 60, h = 60;
	int off = 2;
	if (vec[0] == 'b')
	{
		conv con1;
		con1.data[0] = vec[1];
		con1.data[1] = vec[2];
		con1.data[2] = vec[3];
		con1.data[3] = vec[4];
		w = con1.ival;

		con1.data[0] = vec[5];
		con1.data[1] = vec[6];
		con1.data[2] = vec[7];
		con1.data[3] = vec[8];
		h = con1.ival;
		off = 9;
	}

	bc::BarImg<float> imgFromGeo(w, h, 1, (uchar*)vec.data() + off, false, false);

	bc::BarcodeCreator<float> test;


	//auto* retFromQGIS = test.createBarcode(&imgFromGeo, bcont);
	//auto* qgis = retFromQGIS->getItem(0);
	//retFromQGIS->relen();
	//retFromQGIS->removePorog(3);

	float maxs = imgFromGeo.max();
	float mins = imgFromGeo.min();

	//imgFromGeo.addToMat(-mins);

	auto* retFromArctic = test.searchHoles(imgFromGeo.getData(), w, h);
	auto* arcticItem = retFromArctic->getItem(0);
	auto img = restore255ToBarimg(retFromArctic, w, h, mins);
	std::cout << arcticItem->barlines.size() << std::endl;

	//compiteBarAndBar(img, imgFromGeo);

	for (size_t i = 0; i < arcticItem->barlines.size(); i++)
	{
		float minT = arcticItem->barlines[i]->start;
		float maxT = arcticItem->barlines[i]->end();

		float bottomLvl = minT + (maxT - minT) / 10;
		auto& vecto = arcticItem->barlines[i]->matr;
		int coi = 0;
		for (size_t j = 0; j < vecto.size(); j++)
		{
			float val = vecto[j].value;
			if (val >= bottomLvl)
				++coi;
		}
		std::cout << "Ret: " << coi << std::endl;
	}
	//float ret = qgis->compireFull(arcticItem, bc::CompireStrategy::CommonToLen);
	//std::cout << "Ret: " << ret << std::endl;
}


void testFloatMats()
{
	for (int i = 0; i < 24; ++i)
	{
		std::string path = "D:\\Programs\\Python\\barcode\\experiments\\geo\\imgs\\imgOut";
		path += std::to_string(i);
		path += ".bf";
		std::ifstream input(path, std::ios::binary);

		std::vector<char> bytes(
			(std::istreambuf_iterator<char>(input)),
			(std::istreambuf_iterator<char>()));

		//testmat = testmat(cv::Range(10, 20), cv::Range(7, 10));
		bc::BarImg<float> baseimg((int)60, (int)60, 1, (uchar*)bytes.data() + 2, false, false);

		bc::BarConstructor<float> bcont;
		bcont.addStructure(bc::ProcType::f0t255, bc::ColorType::gray, bc::ComponentType::Component);
		bcont.createBinaryMasks = true;
		bcont.createGraph = false;
		bcont.returnType = bc::ReturnType::barcode3d;
		bcont.createNewComponentOnAttach = false;
		bcont.setStep(255);

		bc::BarcodeCreator<float> test;
		auto* ret = test.createBarcode(&baseimg, bcont);

		bc::Barcontainer<float>* sdd = (bc::Barcontainer<float>*)ret->clone();
		float re = sdd->compireFull(ret, bc::CompireStrategy::compire3dBrightless);

		bc::BarImg<float> retimg = restoreToBarimg(sdd, 60, 60, baseimg.max());

		compiteBarAndBar(retimg, baseimg);

		delete ret;
		input.close();
	}
}

#include <thread>


void checkBigImg()
{
	bc::BarConstructor<uchar> bcont;
	bcont.addStructure(bc::ProcType::f0t255, bc::ColorType::gray, bc::ComponentType::Component);
	bcont.createBinaryMasks = true;
	bcont.createGraph = false;
	bcont.returnType = bc::ReturnType::barcode2d;
	bcont.createNewComponentOnAttach = false;
	bcont.setStep(255);

	bc::BarcodeCreator<uchar> test;

	Mat testmat = cv::imread((string)"../PrjTests/data/bigimg.jpg", cv::IMREAD_GRAYSCALE);
	//Mat testmat = cv::imread((string)"D:\\Programs\\Python\\barcode\\lenna.png", cv::IMREAD_GRAYSCALE);
	bc::BarMat<uchar> mf(testmat);

	using std::chrono::high_resolution_clock;
	using std::chrono::duration_cast;
	using std::chrono::duration;
	using std::chrono::milliseconds;

	auto t1 = high_resolution_clock::now();
	auto* ret = test.createBarcode(&mf, bcont);
	auto t2 = high_resolution_clock::now();
	auto ms_int = duration_cast<milliseconds>(t2 - t1);
	std::cout << "time:" << (double)(ms_int.count()) / 1000 << std::endl;
	delete ret;
}

void caclSize(bc::Barcontainer<uchar>* ret)
{
	auto* item = ret->getItem(0);
	int totalSize = 0;
	totalSize += sizeof(*item) + 8;
	int more = 0;
	for (size_t i = 0; i < item->barlines.size(); i++)
	{
		int locsize = item->barlines[i]->matr.capacity() * sizeof(item->barlines[i]->matr[0]);
		more += locsize - item->barlines[i]->matr.size() * sizeof(item->barlines[i]->matr[0]);

		if (locsize / 1024 / 1024 > 0)
			std::cout << "loc size is:" << locsize / 1024 / 1024 << std::endl;
		totalSize += sizeof(*item->barlines[i]) + 8 + locsize;
	}
	std::cout << "Size is:" << totalSize / 1024 / 1024 << std::endl;
	std::cout << "Oversize is:" << more / 1024 / 1024 << std::endl;
}

void testImg(string path)
{
	bc::BarConstructor<uchar> bcont;
	bcont.addStructure(bc::ProcType::f0t255, bc::ColorType::gray, bc::ComponentType::Component);
	bcont.createBinaryMasks = true;
	bcont.createGraph = false;
	bcont.returnType = bc::ReturnType::barcode2d;
	bcont.createNewComponentOnAttach = false;
	bcont.setStep(255);

	bc::BarcodeCreator<uchar>* test = new bc::BarcodeCreator<uchar>();

	Mat testmat = cv::imread(path, cv::IMREAD_GRAYSCALE);
	//Mat testmat = cv::imread((string)"D:\\Programs\\Python\\barcode\\lenna.png", cv::IMREAD_GRAYSCALE);
	bc::BarMat<uchar> mf(testmat);

	auto* ret = test->createBarcode(&mf, bcont);
	delete test;
	caclSize(ret);
	delete ret;
}

void testMaxLen()
{
	bc::BarcodeCreator<uchar> bc;
	bc::BarConstructor<uchar> constr;
	constr.createBinaryMasks = true;
	constr.createGraph = false;
	constr.createNewComponentOnAttach = false;
	constr.returnType = bc::ReturnType::barcode2d;
	constr.addStructure(bc::ProcType::f0t255, bc::ColorType::gray, bc::ComponentType::Component);
	constr.setStep(255);
	constr.setMaxLen(20);

	Mat testmat = cv::imread((string)"../PrjTests/data/hole-test.png", cv::IMREAD_GRAYSCALE);
	bc::BarMat<uchar> gray(testmat);

	auto varcode = bc.createBarcode(&gray, constr);

	Bimg8 sd = restoreToBarimg(varcode, gray.wid(), gray.hei(), gray.max());
	compiteBarAndBar(sd, gray);
}

void checkSameVals()
{
	bc::BarcodeCreator<uchar> bc;
	bc::BarConstructor<uchar> constr;
	constr.createBinaryMasks = true;
	constr.createGraph = false;
	constr.createNewComponentOnAttach = false;
	constr.returnType = bc::ReturnType::barcode2d;
	constr.addStructure(bc::ProcType::f0t255, bc::ColorType::gray, bc::ComponentType::Component);
	constr.setStep(255);

	uchar data[6]
	{
		1, 2, 1,
		1, 1, 1
	};

	Bimg8 img(3, 2, 1, data, false, false);

	auto varcode = bc.createBarcode(&img, constr);

	assert(varcode->getItem(0)->barlines.size() == 1);
}

int main()
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

	testImg("../PrjTests/data/city.png");//bigimg.jpg
	return 0;
}
