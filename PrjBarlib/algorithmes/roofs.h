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


int show(string name, Mat img, int wait = -1)
{
	cv::namedWindow(name, cv::WINDOW_KEEPRATIO);
	cv::imshow(name, img);
	if (wait >= 0)
		return waitKey(wait);
	else
		return -1;
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
	//bcstruct.maxRadius = 50;
	//bcstruct.maxLen.set(25);
	//bcstruct.colorRange = 25;
	bcstruct.addStructure(ProcType::Radius, ColorType::native, ComponentType::Component);

	//path = "D:/Learning/BAR/base/ident.png";
	//path = "D:/Learning/datasets/Floodnet/train-20220527T055812Z-001/train/train-org-img/6344.jpg";
	//path = "D:/1.png";
	Mat img = cv::imread(path, cv::IMREAD_COLOR);

	if (img.rows >= 3000)
	{
		cv::resize(img, img, cv::Size(img.cols / 2, img.rows / 2));
	}
	Mat back = img;

	show("baeck", back, 1);
	//cv::imwrite("source.png", back);

	//back.at<uchar>(0, 0) = 0;
	//back = 255 - back;
	//cvtColor(img, back, COLOR_BGR2GRAY);
	BarMat wrap(back);
	Barcontainer* containet = barcodeFactory.createBarcode(&wrap, bcstruct);
	Baritem* item = containet->getItem(0);

	Mat binmap(img.rows, img.cols, CV_8UC3, Scalar(0));
	img.copyTo(binmap);

	//item->sortBySize();
	//barlinevector& bar2 = item->getRootNode()->children;
	barlinevector& bar2 = item->barlines;
	frange = bar2.size();
	int add = 0;
	for (int i = 0; i < bar2.size(); ++i)
	{
		barline* line = bar2[i];
		int minlen = 0;
		barvector& points = line->matr;

		if (points.size() < wrap.length() * 0.01 || points.size() > wrap.length() * 0.9)
			continue;

		//if (line->start < 10)
		//	continue;
		//assert(line->len() < 6);
		for (size_t k = 0; k < points.size(); ++k)
		{
			const barvalue& p = points[k];
			//if (points[k].value > 100)
			//	continue;
			//if (line->len() > 100)
			binmap.at<Vec3b>(p.getY(), p.getX()) = colors[i % collen];
			//binmap.at<Vec3b>(p.getY(), p.getX()) = line->end().toCvVec();
			//binmap.at<Vec3b>(p.getY(), p.getX()) = Vec3b(255, 255,255);
			//binmap.at<uchar>(p.getY(), p.getX()) = (uchar)(float)line->start;
		}
	}

	show("result", binmap, 0);

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

			/*if (bar2[ind]->getPointsSize() < 300)
			{
				continue;
			}*/

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

		ds = show("result", workingimg, 0);
	}

	show("da", binmap, 1);


	//Mat kernel(3, 3, CV_8U);
	//cv::morphologyEx(binmap, binmap, cv::MORPH_ERODE, kernel);
	//cv::medianBlur(binmap, binmap, 3);
}


string getV(float x, float y, float z)
{
	string s = "v ";
	s += to_string(x);
	s += " ";
	s += to_string(y);
	s += " ";
	s += to_string(z);
	s += "\n";

	return s;
}

string getF(int v1, int v2, int v3, int v4)
{
	string s = "f ";
	s += to_string(v1);
	s += " ";
	s += to_string(v2);
	s += " ";
	s += to_string(v3);
	s += " ";
	s += to_string(v4);
	s += "\n";

	return s;
}

string getVt(float x, float z)
{
	string s = "vt ";
	s += to_string(x);
	s += " ";
	s += to_string(z);
	s += "\n";

	return s;
}


string getVVt(float x, float y, float z, int wid, int hei)
{
	string s = "v ";
	s += to_string(x);
	s += " ";
	s += to_string(y);
	s += " ";
	s += to_string(z);
	s += "\n";

	//s += "vt ";
	//s += to_string(x / wid);
	//s += " ";
	//s += to_string(1.f - (float)z / hei);
	//s += "\n";

	return s;
}

int lastX = 0;
int lastZ = 0;
//
//void writePixel(ofstream& out, int x, int z, Mat imp, float scale)
//{
//
//}
//
//void writeFaces(ofstream& out, int& ind, int x, int z, Mat imp)
//{
//	uchar y = imp.at<uchar>(x, z);
//
//	float yof = y * 0.02;
//	int hei = imp.cols;
//
//
//	out << getF(ind++, ind++, ind++, ind++);
//
//	if (imp.at<uchar>(x + 1, z) != y)
//		out << getF(ind++, ind++, ind++, ind++);
//
//	if (imp.at<uchar>(x, z + 1) != y)
//		out << getF(ind++, ind++, ind++, ind++);
//
//	if (imp.at<uchar>(x + 1, z + 1) != y)
//		out << getF(ind++, ind++, ind++, ind++);
//}

void writeObject(string path, Mat imp)
{
	int nameOf = path.rfind('/') + 1;
	int dotPoz = path.rfind('.');
	string objName = path.substr(nameOf, dotPoz - nameOf);
	ofstream out;
	out.open(path);


	out << "o " << objName << endl;

	float min = 9999;
	float max = -9999;
	int k = 0;

	int wid = imp.cols;
	int hei = imp.rows;
	float scale = 0.5;
	int ind = 1;
	float jds = 0.02;

	for (int h = 0; h < hei; h += 1)
	{
		for (int w = 0; w < wid; w += 1)
		{
			int x = w;
			int z = h;
			uchar y = imp.at<uchar>(h, w);


			float xof = x * scale;
			float zof = z * scale;
			float yof = y * jds;

			int oo = ind++;
			int po = ind++;
			int op = ind++;
			int pp = ind++;

			out << getVVt(xof, yof, zof, wid, hei);
			out << getVVt(xof + scale, yof, zof, wid, hei);
			out << getVVt(xof, yof, zof + scale, wid, hei);
			out << getVVt(xof + scale, yof, zof + scale, wid, hei);
			out << getF(oo, op, pp, po);

			if (x + 1 < wid)
			{
				uchar yn = imp.at<uchar>(h, w + 1);
				float ynof = yn * jds;
				if (yn != y)
				{
					out << getVVt(xof + scale, ynof, zof, wid, hei);
					out << getVVt(xof + scale, ynof, zof + scale, wid, hei);
					out << getF(po, pp, ind++, ind++);
				}
			}

			if (h + 1 < hei)
			{
				uchar yn = imp.at<uchar>(h + 1, w);
				float ynof = yn * jds;
				if (yn != y)
				{
					out << getVVt(xof, ynof, zof + scale, wid, hei);
					out << getVVt(xof + scale, ynof, zof + scale, wid, hei);
					out << getF(op, pp, ind++, ind++);
				}
			}
		}
	}

	//int ind = 1;
	//for (int h = 0; h < height - 1; h += 1)
	//{
	//	for (int w = 0; w < width - 1; w += 1)
	//	{
	//		writeFaces(out, ind, w, h, imp);
	//	}
	//}


	out.close();
}

void get3d(Mat img, int maxLen, string outpath, bool writeInRange)
{
	BarcodeCreator barcodeFactory;
	BarConstructor bcstruct;
	bcstruct.returnType = bc::ReturnType::barcode2d;
	bcstruct.createBinaryMasks = true;
	bcstruct.visualize = false;
	bcstruct.waitK = 1;
	bcstruct.addStructure(ProcType::f0t255, ColorType::native, ComponentType::Component);

	Mat back = img;

	BarMat wrap(back);
	Barcontainer* containet = barcodeFactory.createBarcode(&wrap, bcstruct);
	Baritem* item = containet->getItem(0);

	//Mat binmap(img.rows, img.cols, CV_8UC1, Scalar(wrap.max().getAvgUchar()));
	Mat binmap(img.rows, img.cols, CV_8UC1, Scalar(0));
	barlinevector& bar2 = item->barlines;
	int add = 0;
	for (int i = 0; i < bar2.size(); ++i)
	{
		barline* line = bar2[i];
		barvector& points = line->matr;

		if (writeInRange)
		{
			if (line->len() >= maxLen)
				continue;
		}
		else
		{
			if (line->len() < maxLen)
				continue;
		}

		++add;

		for (size_t k = 0; k < points.size(); ++k)
		{
			const barvalue& p = points[k];
			//if (points[k].value > 100)
			//	continue;
			//if (line->len() > 100)
			uchar& v = binmap.at<uchar>(p.getY(), p.getX());

			v += p.value.getAvgUchar();
		}
	}

	cout << add << " " << writeInRange << " "  << wrap.max().getAvgUchar()  << endl;

	writeObject(outpath, binmap);
}

void getResults()
{
	string pathas = "D:/Learning/papers/CO_compressing/base16_inv.png";
	//string path = "D:/Learning/papers/CO_compressing/base16t.png";
	Mat img = cv::imread("noise.png", cv::IMREAD_GRAYSCALE);

	//for (size_t i = 1; i < img.rows - 1; i++)
	//{
	//	for (size_t j = 1; j < img.cols - 1; j++)
	//	{
	//		img.at<uchar>(i, j) = rand() % 100;
	//	}
	//}
	//cv::imwrite("noise.png", img);

	get3d(img, 255, "D:/Learning/papers/CO_compressing/full.obj", true);
	get3d(img, 50, "D:/Learning/papers/CO_compressing/50_main.obj", true);
	get3d(img, 50, "D:/Learning/papers/CO_compressing/50_ost.obj", false);

	get3d(img, 20, "D:/Learning/papers/CO_compressing/20_main.obj", true);
	get3d(img, 20, "D:/Learning/papers/CO_compressing/20_ost.obj", false);

	return;

	string path = "D:/Programs/C++/Barcode/PrjBarlib/researching/e.png";
	Mat bin_etalon = cv::imread(path, IMREAD_COLOR);
	show("source", bin_etalon);
	binarymatr(path);

	show("bin decress", bin_etalon);
}