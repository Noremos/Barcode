#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>


#include "include/barcodeCreator.h" 

#include <iostream>
#include <filesystem>
#include <string>
#include <fstream>
#include <opencv2/ml.hpp>
/////////////////////////////
/////////////////////////////
/////////////////////////////  at ((row,col) = (hei,wid)
/////////////////////////////
/////////////////////////////

namespace fs = std::filesystem;

typedef cv::Point3_<uint8_t> Pixel;
using std::string;
using namespace cv;

int pr = 10; bool normA = false;

//done for plane: 19 / 80
//done for  ship : 439 / 559
//done for  storage - tank : 930 / 1052
//done for  tennis - court : 4 / 17
//done for  basketball - court : 0 / 8
//done for  bridge : 29 / 39
//done for  helicopter : 0 / 1
//done for  container - crane : 0 / 0
//
//Total : 1421 / 1756
//Для продолжения нажмите любую клавишу . . .

const int N = 10;

std::unordered_map<string, int> categorues;
//class rfor
//{
//public:
//	rfor()
//	{
//
//#define ATTRIBUTES_PER_SAMPLE (4)
//		// Assumes training data (1000, 16x16x3) are in training_data
//		// Assumes training classifications (1000, 1) are in training_classifications
//
//		// All inputs are numerical. You can change this to reflect your data
//		Mat var_type = Mat(ATTRIBUTES_PER_SAMPLE, 1, CV_8U);
//		var_type.setTo(Scalar(CV_VAR_NUMERICAL)); // all inputs are numerical
//
//		// Output is a category; this is classification, not regression
//		var_type.at<uchar>(ATTRIBUTES_PER_SAMPLE, 0) = CV_VAR_CATEGORICAL;
//
//		// Train the classifier
//		ml::CVRTrees* rtree = new ml::RTrees();
//		rtree->train(training_data, CV_ROW_SAMPLE, training_classifications,
//			Mat(), Mat(), var_type);
//	}
//	void addClass(bc::Barcontainer* cont, int classInd)
//	{
//		classes[classInd].addItem(cont->exractItem(0));
//		classes[classInd + N].addItem(cont->exractItem(1));
//		delete cont;
//	}
//
//	//bool check(bc::Barcontainer* cont)
//	//{
//
//	//}
//
//
//	bool check(bc::Baritem* bar0, bc::Baritem* bar255, int type)
//	{
//		float res = 0;
//
//		//res = classes[type].compireBest(bar0, cp) * 0.5;
//		//res += classes[type + N].compireBest(bar255, cp) * 0.5;
//		//if (res > 0.9)
//		//	return true;
//
//		int maxInd = type;
//		float maxP = res;
//		for (size_t i = 0; i < N; i++)
//		{
//			float ps = classes[i].compireCTS(bar0) * 0.5 + classes[i + N].compireCTS(bar255) * 0.5;
//			if (ps > maxP)
//			{
//				maxP = ps;
//				maxInd = i;
//			}
//		}
//
//		return type == maxInd;
//	}
//
//
//};

class barclassificator
{
public:
	bc::Barcontainer classes[N * 2];

	void addClass(bc::Barcontainer* cont, int classInd)
	{
		classes[classInd].addItem(cont->exractItem(0));
		classes[classInd + N].addItem(cont->exractItem(1));
		delete cont;
	}

	//bool check(bc::Barcontainer* cont)
	//{

	//}


	int check(bc::Baritem* bar0, bc::Baritem* bar255, int type)
	{
		float res = 0;

		if (classes[type].count() == 0)
		{
			return -1;
		}
		//res = classes[type].compireBest(bar0, cp) * 0.5;
		//res += classes[type + N].compireBest(bar255, cp) * 0.5;
		//if (res > 0.9)
		//	return true;

		int maxInd = type;
		float maxP = res;
		for (size_t i = 0; i < N; i++)
		{
			float ps = classes[i].compireBest(bar0) * 0.5 + classes[i + N].compireBest(bar255) * 0.5;
			if (ps > maxP)
			{
				maxP = ps;
				maxInd = i;
			}
		}

		return type == maxInd ? 1 : 0;
	}

	~barclassificator()
	{
		/*for (size_t i = 0; i < N * 2; i++)
		{
			delete classes[i];
		}*/
	}
};

using namespace bc;
using namespace std;


int procR(int r, int b)
{
	return (int)round(100.0 * r / (r + b));
}
int procB(int r, int b)
{
	return (int)round(100.0 * b / (r + b));
}

bool replace(std::string& str, const std::string& from, const std::string& to) {
	size_t start_pos = str.find(from);
	if (start_pos == std::string::npos)
		return false;
	str.replace(start_pos, from.length(), to);
	return true;
}

void split(string str, std::vector<string>& strings, char splitCh = ' ')
{
	std::istringstream f(str);
	string s;
	while (getline(f, s, splitCh))
		strings.push_back(s);
}
int gt(string s)
{
	return atoi(s.c_str());
}

inline bool exists(const std::string& name) {
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

void getCategories()
{

}

void getSet(string path, barclassificator& data, char diff = '0', float* params = nullptr) 
{

	barcodeCreator bc;
	int categoruesSize = 0;

	string labelSubpath = "/labelTxt-v1.5/DOTA-v1.5_hbb/";
	string coords = path + labelSubpath;

	string subbaseIMG = "/images/";
	string subpath = subbaseIMG;
	int ispatr = 0;
	if (exists(path + subbaseIMG + "part1"))
	{
		subpath = subbaseIMG + "part1/images/";
		ispatr = 1;
	}

	std::vector<bc::barstruct> structure;

	std::vector<bc::barstruct> constr;
	constr.push_back(barstruct(ProcType::f0t255, ColorType::gray, ComponentType::Hole));
	constr.push_back(barstruct(ProcType::f255t0, ColorType::gray, ComponentType::Hole));
	using recursive_directory_iterator = fs::recursive_directory_iterator;
	int k = 0;
	cv::namedWindow("test", cv::WINDOW_NORMAL);

	for (const auto& dirEntry : recursive_directory_iterator(coords))
	{
		string coordsPath = dirEntry.path().string();//D:\Programs\C++\Barcode\analysis\datasets\DOTA\labels\P0013.txt
		//std::cout << coordsPath << "..." << endl;
		//k++;
		//if (k == 5000)
		//	break;
		string	imgpath = coordsPath;
		replace(imgpath, labelSubpath, subpath);
		replace(imgpath, "txt", "png");
		if (!exists(imgpath))
		{
			ispatr += 1;
			imgpath = coordsPath;

			subpath = subbaseIMG + "part" + std::to_string(ispatr) + "/images/";
			replace(imgpath, labelSubpath, subpath);
			replace(imgpath, "txt", "png");
		}

		cv::Mat source = cv::imread(imgpath, cv::IMREAD_GRAYSCALE);

		std::ifstream infile(coordsPath);
		std::string line;

		std::getline(infile, line);
		std::getline(infile, line);
		bool stop = false;
		//846.0 165.0 1108.0 165.0 1108.0 395.0 846.0 395.0 plane 1
		//||||D:\Learning\papers\CO2\train/labelTxt-v1.5/DOTA-v1.5_hbb/P1708.txt... - check this!!!!
		while (std::getline(infile, line))//1321.0 44.0 1338.0 37.0 1339.0 70.0 1329.0 58.0 large-vehicle 1 
		{
			std::vector<string> lids;
			split(line, lids);

			string s = (lids[8]);
			if (lids[9][0] == diff)
				continue;

			auto r = categorues.find(s);
			if (r == categorues.end())
				continue;

			int index = r->second;

			int x = atoi(lids[0].c_str());
			int y = atoi(lids[1].c_str());
			int xend = atoi(lids[2].c_str());
			int yend = atoi(lids[5].c_str());
			if (yend >= source.rows || xend >= source.cols)
			{
				continue;
			}
			
			int wid = xend - x;
			int hei = yend - y;

			//if (wid < 32 || hei < 32)
			//{
			//	continue;
			//}
			cv::Rect ds(x, y, wid, hei);
			cv::Mat m = source(ds);

			if (params)
			{
				if (params[0] > 0)
					cv::rotate(m, m, cv::ROTATE_90_CLOCKWISE);

				if (params[1] > 0)
					cv::resize(m, m, Size(m.cols * (1.0 - params[1]), m.rows));

				if (params[2] > 0)
				{
					ds.width *= (1.0 - params[2]);
					m = source(ds);
				}
			}

			//cv::resize(m, m, cv::Size(32, 32));
			//cv::imshow("test", m);
			//cv::waitKey(1);
			//bc::BarMat wrapper(m);
			auto b = bc.createBarcode(m, constr);
			b->preprocessBar(pr, normA);
			data.addClass(b, index);
		}

	}
}

void doMagickDOTA()
{
	string s = "D:\\Programs\\C++\\Barcode\\analysis\\datasets\\DOTA\\images";
	barclassificator train;


	string whiteList = "plane, ship, storage tank, tennis court, basketball court, bridge, helicopter, container crane";

	std::vector<string> splited;
	split(whiteList, splited, ',');
	categorues.clear();
	int l = 0;
	for (auto& c : splited)
	{
		int soff = c[0] == ' ' ? 1 : 0;
		for (size_t i = 1; i < c.length(); i++)
		{
			if (c[i] == ' ')
			{
				c[i] = '-';
			}
		}
		categorues[c.substr(soff)] = l++;
	}

	int cN = splited.size();
	assert(cN <= N);


	string pathtrain = "D:\\Learning\\papers\\CO2\\train";
	string pathvalidation = "D:\\Learning\\papers\\CO2\\validation";

	getSet(pathtrain, train, '0');


	bc::Barcontainer testcont;
	int correct = 0;
	int total = 0;
	int cTotal = 0, cCurrect = 0;

	float params[][3] = {{1, 0, 0}, {0,0.5,0}, {0,0,0.5}, {1, 0.5, 0.0}, {1, 0.0, 0.5}, {1, 0.5, 0.5}};
	for (size_t p = 0; p < sizeof(params) / (sizeof(int) * 3); p++)
	{
		barclassificator validation;

		getSet(pathvalidation, validation, '0', params[p]);

		cout << "Params: " << params[p][0] << " " << params[p][1] << " " << params[p][2] << endl;
		for (size_t i = 0; i < cN; i++)
		{
			auto& barc0 = validation.classes[i];
			auto& barc1 = validation.classes[N + i];
			for (size_t j = 0; j < barc0.count(); j++)
			{
				auto* bar0 = barc0.get(j);
				auto* bar1 = barc1.get(j);
				if (train.check(bar0, bar1, i))
				{
					++correct;
				}
				++total;
			}
			cTotal += total;
			cCurrect += correct;
			std::cout << " done for " << splited[i] << ": " << correct << "/" << total << std::endl;
			correct = 0;
			total = 0;
		}

		std::cout << "\nTotal: " << cCurrect << "/" << cTotal << "| " << static_cast<float>(cCurrect) / cTotal << std::endl << std::endl;
		cCurrect = 0;
		cTotal = 0;
	}
	system("pause");
}
//
//
//
//Params: 1 0 0
//done for plane : 20 / 80
//done for  ship : 434 / 559
//done for  storage - tank : 933 / 1052
//done for  tennis - court : 3 / 17
//done for  basketball - court : 0 / 8
//done for  bridge : 27 / 39
//done for  helicopter : 0 / 1
//done for  container - crane : 0 / 0
//
//Total : 1417 / 1756 | 0.806948
//
//Params : 0 0.5 0
//done for plane : 11 / 80
//done for  ship : 392 / 559
//done for  storage - tank : 939 / 1052
//done for  tennis - court : 2 / 17
//done for  basketball - court : 0 / 8
//done for  bridge : 24 / 39
//done for  helicopter : 0 / 1
//done for  container - crane : 0 / 0
//
//Total : 1368 / 1756 | 0.779043
//
//Params : 0 0 0.5
//done for plane : 12 / 80
//done for  ship : 382 / 559
//done for  storage - tank : 847 / 1052
//done for  tennis - court : 2 / 17
//done for  basketball - court : 0 / 8
//done for  bridge : 8 / 39
//done for  helicopter : 0 / 1
//done for  container - crane : 0 / 0
//
//Total : 1251 / 1756 | 0.712415
//
//Params : 1 0.5 0
//done for plane : 14 / 80
//done for  ship : 389 / 559
//done for  storage - tank : 945 / 1052
//done for  tennis - court : 1 / 17
//done for  basketball - court : 0 / 8
//done for  bridge : 24 / 39
//done for  helicopter : 0 / 1
//done for  container - crane : 0 / 0
//
//Total : 1373 / 1756 | 0.781891
//
//Params : 1 0 0.5
//done for plane : 10 / 80
//done for  ship : 378 / 559
//done for  storage - tank : 867 / 1052
//done for  tennis - court : 2 / 17
//done for  basketball - court : 0 / 8
//done for  bridge : 8 / 39
//done for  helicopter : 0 / 1
//done for  container - crane : 0 / 0
//
//Total : 1265 / 1756 | 0.720387
//
//Params : 1 0.5 0.5
//done for plane : 10 / 80
//done for  ship : 378 / 559
//done for  storage - tank : 867 / 1052
//done for  tennis - court : 2 / 17
//done for  basketball - court : 0 / 8
//done for  bridge : 8 / 39
//done for  helicopter : 0 / 1
//done for  container - crane : 0 / 0
//
//Total : 1265 / 1756 | 0.720387






/*

 done for plane: 0/80
 done for  ship: 70/559
 done for  storage-tank: 229/1052
 done for  tennis-court: 1/17
 done for  basketball-court: 1/8
 done for  bridge: 0/39
 done for  small-vehicle: 28203/33033
 done for  helicopter: 0/1
 done for  container-crane: 0/0

Total: 28504/34789
*/

//
//int main(int argc, char* argv[])
//{
//	setlocale(0, "rus");
//	srand(time(0));
//
//	doMagickDOTA();
//	return 0;
//}
//
//
//
//Params: 000000FF2F8FF8D0 000000FF2F8FF8DC 000000FF2F8FF8E8
//done for plane : 20 / 80
//done for  ship : 434 / 559
//done for  storage - tank : 933 / 1052
//done for  tennis - court : 3 / 17
//done for  basketball - court : 0 / 8
//done for  bridge : 27 / 39
//done for  helicopter : 0 / 1
//done for  container - crane : 0 / 0
//
//Total : 1417 / 1756 | 0.806948
//Params : 000000FF2F8FF8D0 000000FF2F8FF8DC 000000FF2F8FF8E8
//done for plane : 19 / 80
//done for  ship : 439 / 559
//done for  storage - tank : 930 / 1052
//done for  tennis - court : 4 / 17
//done for  basketball - court : 0 / 8
//done for  bridge : 29 / 39
//done for  helicopter : 0 / 1
//done for  container - crane : 0 / 0
//
//Total : 2838 / 3512 | 0.808087
//Params : 000000FF2F8FF8D0 000000FF2F8FF8DC 000000FF2F8FF8E8
//done for plane : 19 / 80
//done for  ship : 439 / 559
//done for  storage - tank : 930 / 1052
//done for  tennis - court : 4 / 17
//done for  basketball - court : 0 / 8
//done for  bridge : 29 / 39
//done for  helicopter : 0 / 1
//done for  container - crane : 0 / 0
//
//Total : 4259 / 5268 | 0.808466

//Params : 000000FF2F8FF8D0 000000FF2F8FF8DC 000000FF2F8FF8E8
//done for plane : 20 / 80
//done for  ship : 434 / 559
//done for  storage - tank : 933 / 1052
//done for  tennis - court : 3 / 17
//done for  basketball - court : 0 / 8
//done for  bridge : 27 / 39
//done for  helicopter : 0 / 1
//done for  container - crane : 0 / 0
//
//Total : 5676 / 7024 | 0.808087

// 
//Params : 000000FF2F8FF8D0 000000FF2F8FF8DC 000000FF2F8FF8E8
//done for plane : 20 / 80
//done for  ship : 434 / 559
//done for  storage - tank : 933 / 1052
//done for  tennis - court : 3 / 17
//done for  basketball - court : 0 / 8
//done for  bridge : 27 / 39
//done for  helicopter : 0 / 1
//done for  container - crane : 0 / 0
//
//Total : 7093 / 8780 | 0.807859

//Params : 000000FF2F8FF8D0 000000FF2F8FF8DC 000000FF2F8FF8E8
//done for plane : 20 / 80
//done for  ship : 434 / 559
//done for  storage - tank : 933 / 1052
//done for  tennis - court : 3 / 17
//done for  basketball - court : 0 / 8
//done for  bridge : 27 / 39
//done for  helicopter : 0 / 1
//done for  container - crane : 0 / 0
//
//Total : 8510 / 10536 | 0.807707
//Для продолжения нажмите любую клавишу . . .
