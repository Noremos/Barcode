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

int pr = 30; bool normA = true;

const int N = 16;

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
		float res = classes[type].compireFull(bar0, cp) * 0.5;
		res += classes[type + N].compireFull(bar255, cp) * 0.5;
		if (res > 0.9)
			return true;

		int maxInd = type;
		float maxP = res;
		for (size_t i = 0; i < N; i++)
		{
			float ps = classes[type].compireFull(bar0, cp) * 0.5 + classes[type + N].compireFull(bar255, cp) * 0.5;
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

void getSet(string path, barclassificator& data, char diff = '0')
{
	string whiteList = "plane, ship, storage tank, tennis court, basketball court, bridge, small vehicle, helicopter, container crane";

	std::vector<string> splited;
	split(whiteList, splited);

	std::unordered_map<string, int> categorues;
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
		categorues[c.substr(soff)] = l;
	}


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

	for (const auto& dirEntry : recursive_directory_iterator(coords))
	{
		string coordsPath = dirEntry.path().string();//D:\Programs\C++\Barcode\analysis\datasets\DOTA\labels\P0013.txt
		std::cout << coordsPath << "...";

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
			if (categorues.find(s) == categorues.end())
			{
				/*				if (categorues.size() >= itmsLimit)
									continue;*/

				categorues.insert(std::pair<string, int>(s, categoruesSize++));
			}

			int index = categorues[s];

			int x = atoi(lids[0].c_str());
			int y = atoi(lids[1].c_str());
			cv::Rect ds(x, y, atoi(lids[2].c_str()) - x, atoi(lids[5].c_str()) - y);
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


	string pathtrain = "D:\\Learning\\papers\\CO2\\train";
	string pathvalidation = "D:\\Learning\\papers\\CO2\\validation";

	getSet(pathtrain, train, '0');

	getSet(pathvalidation, validation, '0');

	bc::Barcontainer<uchar> testcont;
	int correct = 0;
	int total = 0;
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
		std::cout << " done for" << i << ": " << correct << "/" << total << std::endl;
	}

	system("pause");
}
//
//int main(int argc, char* argv[])
//{
//	setlocale(0, "rus");
//	srand(time(0));
//
//	doMagickDOTA();
//	return 0;
//}
