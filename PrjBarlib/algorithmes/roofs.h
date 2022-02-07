#pragma once

#include "prep.h"
using namespace bc;
using namespace cv;


int getFromHex(const char* s)
{
	string str = s;
	char* end;
	return strtol(str.substr(0, 2).c_str(), &end, 16);
}

cv::Vec3b getCol(const char* name)
{
	return cv::Vec3b(getFromHex(name), getFromHex(name + 3), getFromHex(name + 5));
}


cv::Vec3b colors[] =
{
	getCol("#00FF00"),
	getCol("#0000FF"),
	getCol("#FF0000"),
	getCol("#01FFFE"),
	getCol("#FFA6FE"),
	getCol("#FFDB66"),
	getCol("#006401"),
	getCol("#010067"),
	getCol("#95003A"),
	getCol("#007DB5"),
	getCol("#FF00F6"),
	getCol("#FFEEE8"),
	getCol("#774D00"),
	getCol("#90FB92"),
	getCol("#0076FF"),
	getCol("#D5FF00"),
	getCol("#FF937E"),
	getCol("#6A826C"),
	getCol("#FF029D"),
	getCol("#FE8900"),
	getCol("#7A4782"),
	getCol("#7E2DD2"),
	getCol("#85A900"),
	getCol("#85A990")
};

void experemental6();


template<class T>
void segment(string& path, bool revert = false, int radius = 0)
{
	experemental6();
	return;
	path = "D:/Learning/BAR/base/1.png";
	path = "D:/Programs/Python/barcode/roofs/imgs/5.bmp";
	//path = "D:/Programs/Python/barcode/roofs/imgs/5m.bmp";

	bool is3d = typeid(T) != typeid(uchar);
	Mat img = cv::imread(path, cv::IMREAD_COLOR);


	if (!is3d)
	{
		cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);
	}


	cv::namedWindow("nasl", cv::WINDOW_NORMAL);
	cv::resizeWindow("nasl", 720, 480);
	cv::imshow("nasl", img);


	//int ncols = 300;
	//float coofs = img.cols / ncols;
	//int nrows = img.rows / coofs;
	//int ocols = img.cols;
	//int orows = img.rows;
	//cv::resize(img, img, cv::Size(ncols, nrows));


	Mat backback;
	img.copyTo(backback);

	BarcodeCreator<T> barcodeFactory;
	//if (is3d) :
	//    barcodeFactory = BarcodeCreator3d()
	//    bcstruct = BarConstructor3d()
	//else:
	//barcodeFactory = BarcodeCreator()

	BarConstructor<T> bcstruct;

	bcstruct.returnType = bc::ReturnType::barcode2d;
	bcstruct.createBinaryMasks = true;
	bcstruct.createGraph = true;
	bcstruct.attachMode = AttachMode::morePointsEatLow;
	//bcstruct.attachMode = AttachMode::createNew;
	bcstruct.visualize = false;
	//bcstruct.stepPorog.set(60.0);
	bcstruct.waitK = 0;
	if (radius > 0)
		bcstruct.setStep(radius);

	Mat back;
	img.copyTo(back);
	//cv::resize(back, back, cv::Size(ncols, nrows));


	if (is3d)
		bcstruct.addStructure(ProcType::f0t255, ColorType::native, ComponentType::RadiusComp);
	else
		bcstruct.addStructure(ProcType::f255t0, ColorType::gray, ComponentType::Component);

	BarMat<T> val(back);
	Barcontainer<T>* containet = barcodeFactory.createBarcode(&val, bcstruct);
	Baritem<T>* item = containet->getItem(0);
	item->sortBySize();
	bc::barlinevector<T>& bar = item->barlines;


	int ll = bar.size();
	int collen = sizeof(colors) / sizeof(Vec3b);
	int k = 0;
	for (size_t i = 0; i < ll; i++)
	{
		barline<T>* line = bar[i];
		barvector<T>& points = line->matr;

		BarRect v = line->getBarRect();

		//if (v.coof() < 1.5)
		//	continue;


		if (points.size() < 100 || points.size() > val.length() * 0.9)
			continue;

		Vec3b col = colors[k % collen];
		Scalar scal(col.val[0], col.val[1], col.val[2]);
		for (barvalue<T>& p : points)
		{
			//size_t ny = p.getY(), yend = p.getY() * coofs;
			//size_t nx = p.getX(), xend = p.getX() * coofs;
			//cv::rectangle(backback, cv::Rect(nx, ny, xend - nx, yend - ny), scal, 2);
			backback.at<cv::Vec3b>(p.getY(), p.getX()) = col;
		}
		k++;
	}


	cv::namedWindow("result", cv::WINDOW_NORMAL);
	cv::imshow("result", backback);
	int ds = cv::waitKey(0);
	int ind = 0;
	while (ds != 'q')
	{
		Mat workingimg;
		img.copyTo(workingimg);
		switch (ds)
		{
		case 'd':
		case 'D':
			ind += 1;
			if (ind >= ll)
				ind = ll - 1;
			break;
		case 'a':
		case 'A':
			ind -= 1;
			if (ind < 0)
				ind = 0;
			break;
		}

		barline<T>* line = bar[ind];
		barvector<T>& points = line->matr;

		for (barvalue<T>& p : points)
		{
			workingimg.at<Vec3b>(p.getY(), p.getX()) = colors[k % collen];
		}
		cv::namedWindow("result", cv::WINDOW_NORMAL);
		cv::imshow("result", workingimg);
		ds = cv::waitKey(0);
	}

	delete containet;

}


void experemental6()
{
	BarcodeCreator<uchar> barcodeFactory;

	BarConstructor<uchar> bcstruct;

	bcstruct.returnType = bc::ReturnType::barcode2d;
	bcstruct.createBinaryMasks = true;
	bcstruct.createGraph = true;
	bcstruct.attachMode = AttachMode::morePointsEatLow;
	//bcstruct.attachMode = AttachMode::createNew;
	bcstruct.visualize = false;
	//bcstruct.stepPorog.set(60.0);
	bcstruct.waitK = 0;

	bcstruct.addStructure(ProcType::f255t0, ColorType::gray, ComponentType::Component);

	//bc::BarImg<uchar> img(1, 1);

	//uchar* data6 = new uchar[36]{
	//	'7','1','1','1','1','1',
	//	'2','2','7','8','1','1',
	//	'6','1','8','8','1','&',
	//	'8','1','8','9','2','1',
	//	'8','1','8','8','3','1',
	//	'9','3','1','2','2','1',
	//};

	//img.assignRawData(6, 6, 1, data6);

	//Mat backback(img.hei(), img.wid(), CV_8UC3);
	
	string path = "D:/Learning/BAR/base/1.png";
	path = "D:/Learning/BAR/base/ident.png";

	path = "D:/Programs/Python/barcode/roofs/imgs/4.bmp";

	Mat img = cv::imread(path, cv::IMREAD_COLOR);

	cv::namedWindow("nasl", cv::WINDOW_NORMAL);
	cv::resizeWindow("nasl", 720, 480);
	cv::imshow("nasl", img);


	int ncols = img.cols / 2;
	float coofs = img.cols / ncols;
	int nrows = img.rows / coofs;
	int ocols = img.cols;
	int orows = img.rows;

	cv::resize(img, img, cv::Size(ncols, nrows));


	Mat back;
	cv::cvtColor(img, back, cv::COLOR_BGR2GRAY);
	//img.copyTo(img);
	BarMat<uchar> wrap(back);

	Mat backback;
	img.copyTo(backback);

	Barcontainer<uchar>* containet = barcodeFactory.createBarcode(&wrap, bcstruct);
	Baritem<uchar>* item = containet->getItem(0);
	item->sortBySize();
	bc::barlinevector<uchar>& bar = item->barlines;


	int collen = sizeof(colors) / sizeof(Vec3b);
	int k = 0;
	size_t ll = bar.size();
	BarRoot<uchar>* root = item->getRootNode();
	//ll = root->children.size();
	for (size_t i = 0; i < ll; i++)
	{
		barline<uchar>* line = bar[i];
		//line = root->children[i];
		barvector<uchar>& points = line->matr;

		BarRect v = line->getBarRect();

		//if (v.coof() < 1.5)
		//	continue;


		if (points.size() < 100 || points.size() > wrap.length() * 0.6)
			continue;
		if (line->getDeath() != 2)
			continue;

		Vec3b col = colors[k % collen];
		Scalar scal(col.val[0], col.val[1], col.val[2]);
		for (barvalue<uchar>& p : points)
		{
			//size_t ny = p.getY(), yend = p.getY() * coofs;
			//size_t nx = p.getX(), xend = p.getX() * coofs;
			//cv::rectangle(backback, cv::Rect(nx, ny, xend - nx, yend - ny), scal, 2);
			backback.at<cv::Vec3b>(p.getY(), p.getX()) = col;
		}
		k++;
	}


	cv::namedWindow("result", cv::WINDOW_NORMAL);
	cv::imshow("result", backback);
	int ds = cv::waitKey(0);
	int ind = 0;
	bool inc = false;
	while (ds != 'q')
	{
		Mat workingimg;
		//(img.hei(), img.wid(), CV_8UC3);
		img.copyTo(workingimg);
		//Mat workingimg(img.hei(), img.wid(), CV_8UC3);

		switch (ds)
		{
		case 'd':
		case 'D':
			ind += 1;
			if (ind >= ll)
				ind = ll - 1;

			inc = true;
			break;
		case 'a':
		case 'A':
			ind -= 1;
			if (ind < 0)
				ind = 0;

			inc = false;
			break;
		}

		barline<uchar>* line = bar[ind];
		//line = root->children[ind];

		barvector<uchar>& points = line->matr;
		if (ind < ll - 1 && points.size() < 50)
			ind += inc ? 1 : -1;

		for (barvalue<uchar>& p : points)
		{
			workingimg.at<Vec3b>(p.getY(), p.getX()) = Vec3b(255, 0, 0);
		}

		cout << "Start-end: " << (int)line->start << "-" << (int)line->len() << "|" << line->getDeath() << " : " << (line->parent == nullptr ? "emply" : "has parent") << endl;

		cv::namedWindow("result", cv::WINDOW_NORMAL);
		cv::imshow("result", workingimg);
		ds = cv::waitKey(0);
	}

	delete containet;

}
