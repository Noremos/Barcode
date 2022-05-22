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


void binarymatr(string path)
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
	bcstruct.addStructure(ProcType::f0t255, ColorType::native, ComponentType::RadiusComp);

	//path = "D:/Learning/BAR/base/ident-simple4.png";
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

		if (points.size() < 100 || points.size() > wrap.length() * 0.9)
			continue;
		//assert(line->len() < 6);
		for (size_t k = 0; k < points.size(); k++)
		{
			//if (line->len() > 100)
			//binmap.at<Vec3b>(points[k].getY(), points[k].getX()) = colors[i % collen];
			binmap.at<Vec3b>(points[k].getY(), points[k].getX()) = line->start.toCvVec();
			//binmap.at<uchar>(points[k].getY(), points[k].getX()) = (uchar)(float)line->start;
		}
	}

	show("result", binmap, 1);
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

			if (bar2[ind]->getPointsSize() < 300)
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

		show("result", workingimg, 0);
	}

	show("da", binmap, 1);


	//Mat kernel(3, 3, CV_8U);
	//cv::morphologyEx(binmap, binmap, cv::MORPH_ERODE, kernel);
	//cv::medianBlur(binmap, binmap, 3);
}

void getResults()
{
	string path = "D:/Programs/C++/Barcode/PrjBarlib/researching/e.png";
	Mat bin_etalon = cv::imread(path, IMREAD_COLOR);
	show("source", bin_etalon);
	binarymatr(path);

	show("bin decress", bin_etalon);
}