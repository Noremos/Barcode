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
	bcstruct.extracheckOnPixelConnect = true;
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
	Mat mainMask = Mat::zeros(img.rows, img.cols, CV_8UC1);

	for (size_t i = 0; i < ll; i++)
	{
		barline<uchar>* line = bar[i];
		barvector<uchar> points = line->getEnclusivePoints();// encluseve - только точки чайлов
		for (barvalue<uchar>& p : points)
		{
			mainMask.at<uchar>(p.getY(), p.getX()) = 255;
		}
	}
	cv::namedWindow("main mask", cv::WINDOW_NORMAL);
	cv::imshow("main mask", mainMask);


	++k;

	//cv::namedWindow("result", cv::WINDOW_NORMAL);
	//cv::imshow("result", backback);
	//cv::waitKey(0);

	//vector<cv::Rect> rects;
	//for (size_t i = 0; i < contours.size(); i++)
	//{
	//	rects.push_back(cv::boundingRect(contours[i]));
	//}



	const int deepSt = 0;
	const int deepEnd = 3;

	float avg = 0;
	int coun = 0;
	vector<vector<Point> > conres;


	for (size_t i = 0; i < ll; i++)
	{
		barline<uchar>* line = bar[i];
		barvector<uchar> points = line->getExclusivePoints();

		if (points.size() < 50)// || points.size() > wrap.length() * 0.7)
			continue;


		int d = line->getDeath();
		if (deepSt <= d && d <= deepEnd)
		{
			int sz = points.size();

			cv::Point p0 = points.at(0).getPoint().cvPoint();
			cv::Point p1 = points.at(sz / 2).getPoint().cvPoint();
			cv::Point p2 = points.at(sz - 1).getPoint().cvPoint();
			int ks = 0;

			if (mainMask.at<uchar>(p0.y, p0.x) == 255 && mainMask.at<uchar>(p1.y, p1.x) == 255 && mainMask.at<uchar>(p2.y, p2.x) == 255)
			{
				Mat objectsMask = Mat::zeros(img.rows, img.cols, CV_8UC1);
				int xs = points[0].getX(), xe = points[0].getX();
				int ys = points[0].getY(), ye = points[0].getY();
				for (barvalue<uchar>& p : points)
				{
					int x = p.getX();
					int y = p.getY();
					objectsMask.at<uchar>(y, x) = 255;
					if (x < xs)
						xs = x;
					else if (x > xe)
						xe = x;

					if (y < ys)
						ys = y;
					else if (y > ye)
						ye = y;
				}

				//cv::namedWindow("object mask", cv::WINDOW_NORMAL);
				//cv::imshow("object mask", objectsMask);
				//waitKey(0);




				//int morph_size = 3;
				//Mat element = getStructuringElement(2, Size(2 * morph_size + 1, 2 * morph_size + 1), Point(morph_size, morph_size));
				//morphologyEx(objectsMask, objectsMask, cv::MORPH_CLOSE, element);

				//cv::namedWindow("object mask", cv::WINDOW_NORMAL);
				//cv::imshow("object mask", objectsMask);
				//waitKey(0);

				vector<vector<Point> > contours;
				vector<Vec4i> hierarchy;
				findContours(objectsMask, contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);


				if (contours.size() == 0)
					continue;

				int mi = 0;
				int mm = 0;
				for (size_t i = 0; i < contours.size(); i++)
				{
					if (contours[i].size() > mm)
					{
						mm = contours[i].size();
						mi = i;
					}
				}

				approxPolyDP(contours[mi], contours[mi], 2, true);
				//avg += boundingRect(contours[mi]).area();
				avg += contours[mi].size();
				coun++;


				conres.push_back(std::move(contours[mi]));
			}

		}
	}
	Mat sad;
	img.copyTo(sad);
	cv::drawContours(sad, conres, -1, Scalar(0, 0, 255), 1);
	cv::namedWindow("object mask", cv::WINDOW_NORMAL);
	cv::imshow("object mask", sad);

	k = 0;

	float dev = 1.1f; // насколько можно отклониться от среднего ( в процентах)
	avg /= (float)coun;
	const float defmin = avg * (1 - dev);// 0.5
	const float defmax = avg * (1 + dev);//1.5
	for (size_t i = 0; i < coun; i++)
	{
		//int szsa = boundingRect(conres[i]).area();
		int szsa = conres[i].size();

		if (defmin <= szsa && szsa <= defmax)
		{
			cv::drawContours(backback, conres, i, Scalar(0, 255, 0), 1);
			++k;
		}
		/*else
			cv::drawContours(backback, conres, i, Scalar(0, 0, 255), 2);*/

	}

	cv::namedWindow("result", cv::WINDOW_NORMAL);
	cv::imshow("result", backback);
	int d = cv::waitKey(0);
	delete containet;
}
