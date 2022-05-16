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
	return cv::Vec3b(getFromHex(name + 1), getFromHex(name + 3), getFromHex(name + 5));
}


cv::Vec3b colors[] =
{
	getCol("#FF0000"),
	getCol("#00FF00"),
	getCol("#0000FF"),
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
const int collen = sizeof(colors) / sizeof(Vec3b);


Mat experemental6(const string& path, bool debug = false);
Mat experemental6()
{
	string path = "D:/Learning/BAR/base/1.png";
	path = "D:/Learning/BAR/base/ident.png";

	path = "D:/Programs/Python/barcode/roofs/imgs/4.bmp";
	return experemental6(path, true);
}


void segment(string& path, bool revert = false, int radius = 0)
{
	experemental6();
	return;
	path = "D:/Learning/BAR/base/1.png";
	path = "D:/Programs/Python/barcode/roofs/imgs/5.bmp";
	//path = "D:/Programs/Python/barcode/roofs/imgs/5m.bmp";

	bool is3d = false;// typeid(T) != typeid(uchar);
	Mat img = cv::imread(path, cv::IMREAD_COLOR);


	if (!is3d)
	{
		cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);
	}


	cv::namedWindow("nasl", cv::WINDOW_NORMAL);
	//cv::resizeWindow("nasl", 720, 480);
	cv::imshow("nasl", img);


	//int ncols = 300;
	//float coofs = img.cols / ncols;
	//int nrows = img.rows / coofs;
	//int ocols = img.cols;
	//int orows = img.rows;
	//cv::resize(img, img, cv::Size(ncols, nrows));


	Mat backback;
	img.copyTo(backback);

	BarcodeCreator barcodeFactory;
	//if (is3d) :
	//    barcodeFactory = BarcodeCreator3d()
	//    bcstruct = BarConstructor3d()
	//else:
	//barcodeFactory = BarcodeCreator()

	BarConstructor bcstruct;

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

	BarMat val(back, is3d ? BarType::BYTE8_3 : BarType::BYTE8_1);
	Barcontainer* containet = barcodeFactory.createBarcode(&val, bcstruct);
	Baritem* item = containet->getItem(0);
	item->sortBySize();
	bc::barlinevector& bar = item->barlines;


	int ll = bar.size();
	int k = 0;
	for (size_t i = 0; i < ll; i++)
	{
		barline* line = bar[i];
		barvector& points = line->matr;

		BarRect v = line->getBarRect();

		//if (v.coof() < 1.5)
		//	continue;


		if (points.size() < 100 || points.size() > val.length() * 0.9)
			continue;

		Vec3b col = colors[k % collen];
		Scalar scal(col.val[0], col.val[1], col.val[2]);
		for (barvalue& p : points)
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

		barline* line = bar[ind];
		barvector& points = line->matr;

		for (barvalue& p : points)
		{
			workingimg.at<Vec3b>(p.getY(), p.getX()) = colors[k % collen];
		}
		cv::namedWindow("result", cv::WINDOW_NORMAL);
		cv::imshow("result", workingimg);
		ds = cv::waitKey(0);
	}

	delete containet;

}
using btype = uchar;

Mat experemental6(const string& path, bool debug)
{
	BarcodeCreator barcodeFactory;

	BarConstructor bcstruct;

	bcstruct.returnType = bc::ReturnType::barcode2d;
	bcstruct.createBinaryMasks = true;
	bcstruct.createGraph = true;
	bcstruct.attachMode = AttachMode::morePointsEatLow;
	//bcstruct.attachMode = AttachMode::createNew;
	bcstruct.visualize = false;
	bcstruct.extracheckOnPixelConnect = false;
	bcstruct.waitK = 0;

	bcstruct.addStructure(ProcType::f255t0, ColorType::gray, ComponentType::Component);

	Mat img = cv::imread(path, cv::IMREAD_COLOR);
	//Mat img = cv::imread(path, cv::IMREAD_GRAYSCALE);
	//cv::threshold(img, img, 127, 255, cv::THRESH_OTSU);
	//return img;

	if (debug)
	{
		cv::namedWindow("nasl", cv::WINDOW_NORMAL);
		//cv::resizeWindow("nasl", 720, 480);
		//cv::resizeWindow("nasl", 720, 480);
		cv::imshow("nasl", img);
	}


	int ncols = img.cols / 2;
	//int ncols = 100;
	float coofs = img.cols / ncols;
	int nrows = img.rows / coofs;
	int ocols = img.cols;
	int orows = img.rows;

	cv::resize(img, img, cv::Size(ncols, nrows));


	Mat back;
	img.copyTo(back);
	cv::cvtColor(img, back, cv::COLOR_BGR2GRAY);
	BarMat wrap(back, BarType::BYTE8_1);

	Mat backback;
	img.copyTo(backback);

	Barcontainer* containet = barcodeFactory.createBarcode(&wrap, bcstruct);
	Baritem* item = containet->getItem(0);
	item->sortBySize();
	bc::barlinevector& bar = item->barlines;


	int collen = sizeof(colors) / sizeof(Vec3b);
	int k = 0;
	size_t ll = bar.size();
	BarRoot* root = item->getRootNode();
	Mat mainMask = Mat::zeros(img.rows, img.cols, CV_8UC1);

	for (size_t i = 0; i < ll; i++)
	{
		barline* line = bar[i];
		if (line->getDeath() > 0)
		{
			continue;
		}
		barvector points = line->getEnclusivePoints();// encluseve - ������ ����� ������
		for (barvalue& p : points)
		{
			mainMask.at<uchar>(p.getY(), p.getX()) = 255;
		}
	}
	if (debug)
	{
		cv::namedWindow("main mask", cv::WINDOW_NORMAL);
		cv::imshow("main mask", mainMask);
	}


	++k;

	const int deepSt = 2;
	const int deepEnd = 4;

	float avg = 0;
	int coun = 0;
	vector<vector<Point> > conres;


	for (size_t i = 0; i < ll; i++)
	{
		barline* line = bar[i];
		//barvector points = line->getExclusivePoints();
		barvector points = line->matr;

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

			int avlVal = (float)line->len() / 2;
			//if (mainMask.at<uchar>(p0.y, p0.x) == 255 && mainMask.at<uchar>(p1.y, p1.x) == 255 && mainMask.at<uchar>(p2.y, p2.x) == 255)
			{
				Mat objectsMask = Mat::zeros(img.rows, img.cols, CV_8UC1);
				int xs = points[0].getX(), xe = points[0].getX();
				int ys = points[0].getY(), ye = points[0].getY();
				for (barvalue& p : points)
				{
					if (p.getPyValue().value < avlVal)
						continue;
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
	if (debug)
	{
		Mat sad;
		img.copyTo(sad);
		cv::drawContours(sad, conres, -1, Scalar(0, 0, 255), 1);
		cv::namedWindow("object mask", cv::WINDOW_NORMAL);
		cv::imshow("object mask", sad);
	}

	k = 0;
	mainMask = Mat::zeros(img.rows, img.cols, CV_8UC1);

	float dev = 1.1f; // ��������� ����� ����������� �� �������� ( � ���������)
	avg /= (float)coun;
	const float defmin = avg * (1 - dev);// 0.5
	const float defmax = avg * (1 + dev);//1.5
	for (size_t i = 0; i < coun; i++)
	{
		int szsa = conres[i].size();

		if (defmin <= szsa && szsa <= defmax)
		{

			cv::drawContours(mainMask, conres, i, Scalar(255), cv::FILLED);
			if (debug)
				cv::drawContours(backback, conres, i, Scalar(0, 255, 0), 1);
			++k;
		}
		/*else
			cv::drawContours(backback, conres, i, Scalar(0, 0, 255), 2);*/

	}

	delete containet;

	if (debug)
	{
		cv::namedWindow("result", cv::WINDOW_NORMAL);
		cv::imshow("result", backback);

		cv::namedWindow("result mask", cv::WINDOW_NORMAL);
		cv::imshow("result mask", mainMask);

		int d = cv::waitKey(0);
	}

	return mainMask;
}


using std::filesystem::exists;

bool checkRect(const BarRect& rect, int wid, int hei)
{
	return (5 < rect.width && rect.width < wid / 2 && 5 < rect.height && rect.height < hei / 2);
}


void show(string name, Mat img, int wait = -1)
{
	cv::namedWindow(name, cv::WINDOW_KEEPRATIO);
	cv::imshow(name, img);
	if (wait >= 0)
		waitKey(wait);
}


void getCounturFormMatr(barline* line, int rows, int cols, vector<vector<Point> >& contoursRet)
{
	Mat objectsMask = Mat::zeros(rows, cols, CV_8UC1);
	for (barvalue& p : line->matr)
	{
		//if (p.getPyValue().value < avlVal)
		//	continue;
		int x = p.getX();
		int y = p.getY();
		objectsMask.at<uchar>(y, x) = 255;
	}

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(objectsMask, contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);

	if (contours.size() == 0)
		return;

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
	contoursRet.push_back(contours[mi]);
}

void binarymatrInner(const string& path, vector<vector<Point>> &contours, bool revert);
Mat binarymatr(const string& path)
{
	Mat restbgr = cv::imread(path, cv::IMREAD_COLOR);

	vector<vector<Point>> contours;
	binarymatrInner(path, contours, false);
	binarymatrInner(path, contours, true);
	drawContours(restbgr, contours, -1, Scalar(0, 0, 255), 1);
	return restbgr;
}

void binarymatrInner(const string& path, vector<vector<Point>>& contours, bool revert)
{
	int radius = 50;
	bool skipPar = false;
	int frange = 2;

	BarcodeCreator barcodeFactory;

	BarConstructor bcstruct;

	bcstruct.returnType = bc::ReturnType::barcode2d;
	bcstruct.createBinaryMasks = true;
	bcstruct.createGraph = true;
	bcstruct.attachMode = AttachMode::firstEatSecond;
	//bcstruct.attachMode = AttachMode::createNew;
	bcstruct.extracheckOnPixelConnect = false;
	//bcstruct.setStep(radius);
	//bcstruct.setMaxLen(25);
	//bcstruct.killOnMaxLen = true;;
	bcstruct.visualize = false;
	bcstruct.waitK = 1;
	bcstruct.maxRadius = 9999;
	//bcstruct.colorRange = 25;
	bcstruct.addStructure(ProcType::f0t255, ColorType::native, ComponentType::Component);

	Mat img = cv::imread(path, cv::IMREAD_COLOR);

	Mat back = img;

	show("baeck", back, 1);
	cv::imwrite("source.png", back);

	//back.at<uchar>(0, 0) = 0;
	//back = 255 - back;
	cvtColor(img, back, COLOR_BGR2GRAY);
	BarMat wrap(back);
	Barcontainer* containet = barcodeFactory.createBarcode(&wrap, bcstruct);
	Baritem* item = containet->getItem(0);

	Mat binmap(img.rows, img.cols, CV_8UC1, Scalar(0));
	img.copyTo(binmap);

	//barlinevector& bar2 = item->getRootNode()->children;
	barlinevector& bar2 = item->barlines;
	frange = bar2.size();
	int add = 0;
	for (int i = 0; i < bar2.size(); ++i)
	{
		barline* line = bar2[i];
		int minlen = 0;
		barvector& points = line->matr;

		//assert(line->len() < 6);
		for (size_t k = 0; k < points.size(); k++)
		{
			//if (line->len() > 100)
			//binmap.at<Vec3b>(points[k].getY(), points[k].getX()) = colors[i % collen];
			binmap.at<Vec3b>(points[k].getY(), points[k].getX()) = line->start.toCvVec();
			//binmap.at<uchar>(points[k].getY(), points[k].getX()) = (uchar)(float)line->start;
		}
	}

	show("fullres", binmap, 1);
	cv::waitKey(0);

	int ds = cv::waitKey(1);
	int ind = 0;
	while (ds != 'q')
	{
		add = 0;
		Mat workingimg;
		img.copyTo(workingimg);
		switch (ds)
		{
		case 'd':
		case 'D':
			add = 1;
			
			break;
		case 'a':
		case 'A':
			add = -1;

			break;
		}

		while (add != 0)
		{
			ind += add;
			if (ind < 0)
			{
				ind = 0;
				break;
			}

			if (ind >= frange)
			{
				ind = frange - 1;
				break;
			}

			if (bar2[ind]->getPointsSize() < 100)
			{
				continue;
			}

			break;
		}

		barline* line = bar2[ind];
		barvector& points = line->matr;

		for (barvalue& p : points)
		{
			////if (p.value < 50)				continue;

			workingimg.at<Vec3b>(p.getY(), p.getX()) = colors[ind % collen];
		}
		//break;

		cv::namedWindow("result", cv::WINDOW_NORMAL);
		cv::imshow("result", workingimg);
		ds = cv::waitKey(0);
	}


	//vector<vector<Point>> contours;

	for (int i = 0; i < frange; ++i)
	{
		barline* line = bar2[i];
		//BarRect r = line->getBarRect();
		int minlen = 0;
		//minlen = line->len() * 0.0;// +15;
		//minlen = 15;
		//if (line->getDeath() < 2)
		//	continue;
		//if (line->getDeath() > 3)
		//	continue;

		//if (line->len() < 70)
		//	continue;

		//if (line->start < 50)
		//	continue;
		//if (line->getPointsSize() > img.rows * img.cols * 0.5)
		//	continue;
		//if (line->getPointsSize() < 20)
		//	continue;
		//if (line->getPointsSize() > 1000)
		//	continue;
		//if (checkRect(r, img.cols, img.rows) && line->len() > 60)
		{
			barvector& points = line->matr;
			getCounturFormMatr(line, img.rows, img.cols, contours);
			float len = contours[contours.size() - 1].size();
			float s = (float)points.size() / len;
			//if (s > 5 or s < 1)
			//	continue;

			for (size_t i = 0; i < points.size(); i++)
			{
				//if (points[i].value <100)				continue;
				binmap.at<uchar>(points[i].getY(), points[i].getX()) = 255;
			}
		}
	}

	show("da", binmap, 1);

	//return contours;

	//Mat kernel(3, 3, CV_8U);

	//cv::morphologyEx(binmap, binmap, cv::MORPH_ERODE, kernel);
	////cv::morphologyEx(binmap, binmap, cv::MORPH_OPEN, kernel);
	//cv::medianBlur(binmap, binmap, 3);
	//return restbgr;
}

void getResults()
{
	string path = "D:/Programs/C++/Barcode/PrjBarlib/researching/e.png";
	Mat bin_etalon = cv::imread(path, IMREAD_COLOR);
	show("source", bin_etalon);
	//
	//for (size_t i = 0; i < bin_etalon.rows; i++)
	//{
	//	for (size_t j = 0; j < bin_etalon.cols; j++)
	//	{
	//		uchar& et = bin_etalon.at<uchar>(i, j);
	//		et -= et % 100;
	//	}
	//}

	vector<vector<Point>> contours;
	binarymatrInner(path, contours, false);

	show("bin decress", bin_etalon);

	//Mat b0, b255;

	//int fr = 0;
	//for (size_t i = 0; i < bin_etalon.rows; i++)
	//{
	//	for (size_t j = 0; j < bin_etalon.cols; j++)
	//	{
	//		fr += bin_etalon.at<uchar>(i, j);
	//	}
	//}

	//int avg = 127;
	//avg = ((int)((double)fr) / (bin_etalon.rows * bin_etalon.cols));

	//cv::threshold(bin_etalon, b0, avg, 255, THRESH_TOZERO);
	//cv::threshold(bin_etalon, b255, avg, 255, THRESH_TOZERO_INV);
	//show("0", b0);
	//show("255", b255);

	//experemental6(ds, true);
	//Mat bar_result = binarymatr(ds);
	//cv::namedWindow("barres", cv::WINDOW_NORMAL);
	//cv::imshow("barres", bar_result);
	cv::waitKey(0);
	return;

	const int N = 100;

	string mainPath = "./researching/tiles/";

	float totalCor = 0.f;
	int totalImgs = 0;
	for (size_t i = 0; i < N; i++)
	{
		const string setlPath = mainPath + to_string(i) + "_set.png";
		if (!exists(setlPath))
			break;

		const string binPath = mainPath + to_string(i) + "_bld.png";

		Mat bin_etalon = cv::imread(binPath, IMREAD_GRAYSCALE);
		Mat bar_result = binarymatr(setlPath);
		//Mat bar_result = experemental6(setlPath);

		//cv::resize(bar_result, bar_result, cv::Size(bar_result.cols * 2, bar_result.rows * 2));
		cv::resize(bar_result, bar_result, cv::Size(bin_etalon.cols, bin_etalon.rows));
		/*	assert(bin_etalon.cols == bar_result.cols);
			assert(bin_etalon.rows == bar_result.rows);*/

		int common = 0;
		int total = 0;
		for (size_t r = 0; r < bin_etalon.rows; r++)
		{
			for (size_t c = 0; c < bin_etalon.cols; c++)
			{
				uchar eval = bin_etalon.at<uchar>(r, c);
				uchar bval = bar_result.at<uchar>(r, c);

				common += ((eval == 255 && bval == 255) ? 1 : 0);
				total += ((eval == 255 || bval == 255) ? 1 : 0);
				//total += (eval == 255 ? 1 : 0);
			}
		}
		float cor = total > 0 ? static_cast<float>(common) / total : 0;
		std::cout << "For " << setlPath << ":" << cor << endl;

		cv::namedWindow("etalon", cv::WINDOW_NORMAL);
		cv::namedWindow("barres", cv::WINDOW_NORMAL);

		cv::imshow("etalon", bin_etalon);
		cv::imshow("barres", bar_result);

		//Mat org = cv::imread(setlPath, cv::IMREAD_GRAYSCALE);
		//cv::Canny(org, org, 238, 255);
		//cv::threshold(org, org, 127, 255, cv::THRESH_OTSU);
		//cv::namedWindow("canny", cv::WINDOW_NORMAL);
		//cv::imshow("canny", org);

		Mat orgn = cv::imread(setlPath, cv::IMREAD_COLOR);

		cv::namedWindow("origin", cv::WINDOW_NORMAL);
		cv::imshow("origin", orgn);

		cv::waitKey(0);

		totalCor += cor;
		++totalImgs;
	}

	totalCor /= totalImgs;
	std::cout << "Total cor: " << totalCor << endl;
}