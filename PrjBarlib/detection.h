#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>


#include "include/barcodeCreator.h" 

#include <iostream>
#include <filesystem>
#include <string>
#include <fstream>

/////////////////////////////
/////////////////////////////
/////////////////////////////  at ((row,col) = (hei,wid)
/////////////////////////////
/////////////////////////////

namespace fs = std::filesystem;

typedef cv::Point3_<uint8_t> Pixel;
using std::string;

int pr = 20; bool normA = true;

const int N = 9;

std::unordered_map<string, int> categorues;


class barclassificator
{
public:
	bc::Barcontainer<uchar> classes[N * 2];

	void addClass(bc::Barcontainer<uchar>* cont, int classInd)
	{
		classes[classInd].addItem(cont->exractItem(0));
		classes[classInd + N].addItem(cont->exractItem(1));
		delete cont;
	}

	//bool check(bc::Barcontainer<uchar>* cont)
	//{

	//}


	bool check(bc::Baritem<uchar>* bar0, bc::Baritem<uchar>* bar255, int type)
	{
		auto cp = bc::CompireStrategy::CommonToLen;
		float res = 0;

		//res = classes[type].compireBest(bar0, cp) * 0.5;
		//res += classes[type + N].compireBest(bar255, cp) * 0.5;
		//if (res > 0.9)
		//	return true;

		int maxInd = type;
		float maxP = res;
		for (size_t i = 0; i < N; i++)
		{
			float ps = classes[i].compireBest(bar0, cp) * 0.5 + classes[i + N].compireBest(bar255, cp) * 0.5;
			if (ps > maxP)
			{
				maxP = ps;
				maxInd = i;
			}
		}

		return type == maxInd;
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

void getSet(string path, barclassificator& data, char diff = '0')
{

	BarcodeCreator<uchar> bc;
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
	BarConstructor<uchar> consrt;
	consrt.createBinaryMasks = false;
	consrt.returnType = bc::ReturnType::barcode2d;
	consrt.addStructure(ProcType::f0t255, ColorType::gray, ComponentType::Hole);
	consrt.addStructure(ProcType::f255t0, ColorType::gray, ComponentType::Hole);
	consrt.visualize = false;
	consrt.waitK = 1;
	using recursive_directory_iterator = fs::recursive_directory_iterator;
	int k = 0;
	for (const auto& dirEntry : recursive_directory_iterator(coords))
	{
		string coordsPath = dirEntry.path().string();//D:\Programs\C++\Barcode\analysis\datasets\DOTA\labels\P0013.txt
		std::cout << coordsPath << "..." << endl;
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
				continue;

			cv::Rect ds(x, y, xend - x, yend - y);
			cv::Mat m = source(ds);
			cv::resize(m, m, cv::Size(32, 32));
			bc::BarMat<uchar> wrapper(m);
			auto b = bc.createBarcode(&wrapper, consrt);
			b->preprocessBar(pr, normA);
			data.addClass(b, index);
		}

	}
}

void doMagickDOTA()
{
	string s = "D:\\Programs\\C++\\Barcode\\analysis\\datasets\\DOTA\\images";
	barclassificator train;
	barclassificator validation;


	string whiteList = "plane, ship, storage tank, tennis court, basketball court, bridge, small vehicle, helicopter, container crane";

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


	string pathtrain = "D:\\Learning\\papers\\CO2\\train";
	string pathvalidation = "D:\\Learning\\papers\\CO2\\validation";

	getSet(pathtrain, train, '0');

	getSet(pathvalidation, validation, '0');

	bc::Barcontainer<uchar> testcont;
	int correct = 0;
	int total = 0;
	int cTotal = 0, cCurrect = 0;
	for (size_t i = 0; i < N; i++)
	{
		auto& barc0 = validation.classes[i];
		auto& barc1 = validation.classes[N + i];
		for (size_t j = 0; j < barc0.count(); j++)
		{
			auto* bar0 = barc0.getItem(j);
			auto* bar1 = barc1.getItem(j);
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

	std::cout << "\nTotal: " << cCurrect << "/" << cTotal << std::endl;
	system("pause");
}

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
