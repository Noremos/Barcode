#include "detectKnolls.h"
#include <iostream>
#include <filesystem>
#include <string>

#include <array>
#include <fstream>
#include <stack>
#include <map>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/ml.hpp>

using cv::ml::SVM;
using cv::Mat;
namespace fs = std::filesystem;


auto createTrainSVM(const int k, const int d, int* labels, std::vector<float*> trainingData)
{
	Mat trainingDataMat(k, d, CV_32F);
	for (size_t i = 0; i < k; i++)
		for (size_t j = 0; j < d; j++)
			trainingDataMat.at<float>((int)i, (int)j) = trainingData[i][j];

	//memcpy(trainingDataMat.data, trainingData.data()[0], trainingData.size() * sizeof(float)*d);
	Mat labelsMat(k, 1, CV_32SC1, labels);
	// Train the SVM
	cv::Ptr<SVM> svm = SVM::create();
	svm->setType(SVM::Types::C_SVC);
	svm->setKernel(SVM::KernelTypes::RBF);//тоже норм

	svm->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER, 100, 1e-6));
	svm->train(trainingDataMat, cv::ml::SampleTypes::ROW_SAMPLE, labelsMat);
	return svm;
}

int checkDetail(cv::Ptr<SVM> svm, float x, float y)
{
	Mat sampleMat = (cv::Mat_<float>(1, 2) << x, y);
	float response = svm->predict(sampleMat);
	return (int)(response);
}

int checkDetail(cv::Ptr<SVM> svm, float x, float y, float z)
{
	Mat sampleMat = (cv::Mat_<float>(1, 3) << x, y, z);
	float response = svm->predict(sampleMat);
	return (int)(response);
}

int checkDetail(cv::Ptr<SVM> svm, float x, float y, float z, float t)
{
	Mat sampleMat = (cv::Mat_<float>(1, 4) << x, y, z, t);
	float response = svm->predict(sampleMat);
	return (int)(response);
}
std::string saveVector(std::vector<std::array<int, 2>> arr)
{
	std::string jsonStr = "[";
	if (arr.size() == 0)
		return "[]";
	for (int i = 0; i < arr.size() - 1; i++)
		jsonStr += std::to_string(arr[i][0]) + "," + std::to_string(arr[i][1]) + ",";

	jsonStr += std::to_string(arr[arr.size() - 1][0]) + "," + std::to_string(arr[arr.size() - 1][1]) + "]";
	return jsonStr;
}
void saveExp(std::vector<std::vector<std::array<int, 2>>> arr, std::string path)
{
	std::ofstream myfile;
	std::string jsonWid = "{\r\n";

	for (size_t i = 0; i < arr.size(); i++)
	{
		jsonWid += "\""+std::to_string(i) + "_bf\":" + saveVector(arr[i]);
		if (i != arr.size() - 1)
			jsonWid += ",\r\n";
	}
	jsonWid += "}";

	myfile.open(path + "parts_wid_ex_barcode.json");
	myfile << jsonWid;
	myfile.close();
}
void bc::saveCoofs(std::vector< bc::timeline> arr, std::string path)
{
	std::string jsonWid = "{\r\n";
	std::string jsonHei = "{\r\n";
	int k = 0;
	bool iswid = arr[0].widExists;
	for (size_t i = 0; i < arr.size(); i++)
	{
		const timeline* time = &arr[i];
		std::string bc1 = "";
		std::string bc2 = "";
		std::string num = std::to_string(i);
		int l = num.length();
		for (size_t k = 0; k <= 8 - l; k++)
			num = "0" + num;

		//hei
		for (size_t i = 0; i < time->bLen; ++i)
		{
			if (i != 0)
			{
				bc1 += ",";
			}
			bc1 += std::to_string(time->b0[i]);
		}

		//continue;
		///////////////////////
		if (i != 0)
		{
			jsonWid += ",";
			jsonHei += ",";
		}
		jsonHei += "\"" + num + "_b0\":[" + bc1 + "],\r\n";
		/////////////////////////

		bc1 = "";
		for (size_t i = 0; i < time->bLen; ++i)
		{
			if (i != 0)
			{
				bc1 += ",";
			}
			bc1 += std::to_string(time->b1[i]);
		}
		jsonHei += "\"" + num + "_b1\":[" + bc1 + "]\r\n";


		if (iswid)
		{

			//wid
			/*for (size_t i = 0; i < time->b0wid.size(); ++i)
			{
				if (i != 0)
				{
					bc2 += ",";
				}
				bc2 += std::to_string(time->b0wid[i]);
			}*/
			int l = time->b0fullwid[0].size();
			for (size_t i = 0; i < time->b0fullwid.size(); ++i)
			{
				if (i != 0)
				{
					bc2 += ",";
				}
				for (size_t k = 0; k < l; k++)
				{
					bc2 += std::to_string(time->b0fullwid[i][k]);
					if (k != l - 1)
					{
						bc2 += ",";
					}
				}
			}

			jsonWid += "\"" + num + "_b0\":[" + bc2 + "],\r\n";
			/////////////////////////

			bc2 = "";
			l = time->b1fullwid[0].size();
			for (size_t i = 0; i < time->b1fullwid.size(); ++i)
			{
				if (i != 0)
				{
					bc2 += ",";
				}
				for (size_t k = 0; k < l; k++)
				{
					bc2 += std::to_string(time->b1fullwid[i][k]);
					if (k != l - 1)
					{
						bc2 += ",";
					}
				}
			}
			/*for (size_t i = 0; i < time->b1wid.size(); ++i)
			{
				if (i != 0)
				{
					bc2 += ",";
				}
				bc2 += std::to_string(time->b1wid[i]);
			}*/
			jsonWid += "\"" + num + "_b1\":[" + bc2 + "]\r\n";
		}
	}

	jsonHei += "}";
	jsonWid += "}";


	std::ofstream myfile;

	if (iswid)
	{
		myfile.open(path + "parts_wid_sl_barcode.json");
		myfile << jsonWid;
		myfile.close();
	}

	myfile.open(path + "parts_hei_barcode.json");
	myfile << jsonHei;
	myfile.close();

	std::cout << "coofs saved" << std::endl;
}

void saveRects(std::vector<cv::Rect> arr, std::string path)
{
	std::string json = "{\r\n";
	int len = arr.size() - 1;
	for (size_t i = 0; i <= len; i++)
	{
		std::string bc = std::to_string(arr[i].x) + ", " + std::to_string(arr[i].y) + ", " + std::to_string(arr[i].width) + ", " + std::to_string(arr[i].height);
		std::string num = std::to_string(i);

		int l = num.length();
		for (size_t k = 0; k <= 8 - l; k++)
			num = "0" + num;

		///////////////////////


		json += "\"" + num + "\":[" + bc + (i != len ? "],\r\n" : "]\r\n");
		/////////////////////////
	}

	json += "}";


	std::ofstream myfile;
	myfile.open(path + "rects.json");
	myfile << json;
	myfile.close();
	std::cout << "Rects saved" << std::endl;
}

int findTrueAVG(Mat gray)
{
	static int hist[256];//256
	for (size_t i = 0; i < 256; ++i)
	{
		hist[i] = 0;
	}
	for (size_t i = 0; i < gray.cols; ++i)//wid
	{
		for (size_t j = 0; j < gray.rows; ++j)//hei
		{
			auto p = gray.at<uchar>((int)j, (int)i);
			++hist[p];//можно vector, но хз
		}
	}
	int imax = 0;
	for (size_t i = 0; i < 256; ++i)
		if (hist[i] > hist[imax])
			imax = (int)i;
	return imax;
}

int findSumAVG(Mat gray)
{
	int sum = 0;
	for (size_t i = 0; i < gray.cols; ++i)//wid
	{
		for (size_t j = 0; j < gray.rows; ++j)//hei
		{
			auto p = gray.at<uchar>((int)j, (int)i);
			sum += p;
		}
	}
 
	return sum/(gray.rows*gray.cols);
}
Mat myDetectInv(Mat gray, int* avg)
{
	int hei = gray.rows;
	int wid = gray.cols;
	int imax = findTrueAVG(gray);
	Mat new_img = gray.clone();

	for (size_t i = 0; i < wid; ++i)//wid
	{
		for (size_t j = 0; j < hei; ++j)//hei
		{
			auto p = gray.at<uchar>((int)j, (int)i);
			//p = abs(p-imax);
			if (p > imax)
			{
				p = p - imax;
			}
			else
				p = imax - p;

			if (p < 15)
			{
				p = 0;
			}
			else
				p = 255;
			new_img.at<uchar>((int)j, (int)i) = p;
		}
	}

	int erosion_size = 2;
	Mat el = cv::getStructuringElement(cv::MORPH_RECT,
		cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1),
		cv::Point(erosion_size, erosion_size));

	cv::dilate(new_img, new_img, el);
	//cv::namedWindow("test", cv::WINDOW_NORMAL);
	//cv::imshow("test", new_img);
	//cv::waitKey(0);
	//************************************************
	for (int i = 0; i < wid; ++i)
	{
		new_img.at<uchar>(0, i) = 255;
		new_img.at<uchar>(hei - 1, i) = 255;
	}
	for (int i = 0; i < hei; ++i)
	{
		new_img.at<uchar>(i, 0) = 255;
		new_img.at<uchar>(i, wid - 1) = 255;
	}
	*avg = imax;
	return new_img;
}

//del=2:2x2; del=3:3x3;
//checks - rot 
//0 1 2		0 1
//3   4		2 3
//5 6 7
bool checkHill(Mat hill, int n, bool* checks)
{
	//(n+(n-2))*2=4n-4
	int cn = 4 * n - 4;
	int partW = hill.cols / n;
	int partH = hill.rows / n;
	int* cont = new int[cn];
	int k = 0, tk = 0;

	int avg = findTrueAVG(hill);
	for (size_t i = 0; i < cn; ++i)
	{
		cont[i] = 0;
	}
	for (size_t i = 0; i < n; ++i)//hei
	{
		bool isMid = i > 0 && i < n - 1;
		for (size_t j = 0; j < n; ++j)//wid
		{
			if (isMid && j == 1)
			{
				j = n - 1;//нужен только контур, перескакиваем к правой стороне
				++tk;
			}
			for (size_t ip = i * partH; ip < (i + 1) * partH; ip++)
			{
				for (size_t jp = j * partW; jp < (j + 1) * partW; jp++)
				{
					uchar p = hill.at<uchar>(ip, jp);
					if (p > avg)
					{
						++cont[tk];
					}
				}
			}
		}
	}
	int sum = partW * partH * 1 / 2;
	for (size_t i = 0; i < cn; i++)
	{
		if (checks[i] && cont[i] < sum)
		{
			return false;
		}
	}
	return true;
}

Mat bc::detect(std::string path, std::string mark_path, std::string savePath,
	int k, int d, int* labels, std::vector<float*> trainingData)
{
	fs::path fullname(path);
	std::string name = fullname.filename().string();
	size_t lastindex = name.find_last_of(".");
	name = name.substr(0, lastindex);
	Mat source = cv::imread(path);
	Mat markedS = cv::imread(mark_path);
	Mat marked = markedS.clone();

	Mat gray = cv::imread(path, cv::IMREAD_GRAYSCALE);
	int wid = gray.cols;
	int hei = gray.rows;
	if (gray.rows == 0 || marked.rows == 0)
	{
		throw "file not found";
	}

	//*****************************************\\
	//*****Ќјƒќ ¬џƒ≈Ћ»“№ „≈–Ќџћ Ќј Ѕ≈Ћќћ*******\\
	//*****************************************\\
	
	int avg;
	Mat normlImg = myDetectInv(gray, &avg);


	//************************************************
	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(normlImg, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);//simple - only end points

	//************************************************ 
	cv::Ptr<cv::ml::SVM> svm;

	if (labels != nullptr)
		svm = createTrainSVM(k, d, labels, trainingData);

	//*******************************************************
	int coun = -1;
	std::cout << contours.size() << std::endl;
	bool sv = !fs::exists(savePath + "/parts");
	if (!savePath.empty())
	{
		savePath += "/";
		savePath = savePath + name;
		if (!fs::exists(savePath))
			fs::create_directory(savePath);
		savePath += "/";
		if (!fs::exists(savePath + "parts"))
			fs::create_directory(savePath + "parts");
	}

	std::vector<timeline> bs;
	std::vector<std::vector<std::array<int, 2>>> b1s;
	std::vector<cv::Rect> rects;
	int minim = std::min(gray.rows, gray.cols) / 10;
	for (auto contur : contours)
	{
		cv::Rect rect = cv::boundingRect(contur);
		rect.x = MAX(rect.x - 5, 0);
		rect.y = MAX(rect.y - 5, 0);
		rect.width = rect.width + 5;
		rect.height = rect.height + 5;

		int e = rect.x + rect.width - wid;
		if (e >= 0)
			rect.width -= e;

		e = rect.y + rect.height - hei;
		if (e >= 0)
			rect.height -= e;

		//*******************************************************
		if (rect.width * rect.height > 40000 || rect.width * rect.height < 250)//<min(min(img.rows,img.cols)/10)
			continue;
		//*******************************************************
		++coun;
		//if (coun != 293) continue;

		Mat temp = gray(rect);
		timeline coofs;
		std::cout << coun << std::endl;
		coofs.detectBs(temp, !savePath.empty());
		b1s.push_back(coofs.detectExp(temp, avg));
		//coofs.detectBs(normlImg(rect), !savePath.empty());
		if (labels != nullptr)
		{
			auto pointsb0 = (coofs.b0);
			auto pointsb1 = (coofs.b1);
			int fa = findMaxZone(pointsb0);
			int fb = findMaxZone(pointsb1);
			int fsa = findStart(pointsb0);
			int fsb = findStart(pointsb1);
			float el0, el1;

			el0 = (float)fsa;
			el1 = (float)fb;


			int res = 0;
			if (d == 2)
				res = checkDetail(svm, (float)fsa, (float)fb);
			if (d == 3)
				res = checkDetail(svm, (float)fsa, (float)fb, (float)fa);
			if (d == 4)
				res = checkDetail(svm, (float)fsa, (float)fb, (float)fsb, (float)fa);

			switch (res)
			{
			case 1:
				cv::rectangle(marked, rect, cv::Scalar(0, 255, 255), 5);
				break;
			default:
				cv::rectangle(marked, rect, cv::Scalar(255, 128, 255), 5);
				break;
			}
		}
		//*******************************************************
		if (!savePath.empty())
		{
			rects.push_back(rect);
			bs.push_back(coofs);

			if (sv)
				cv::imwrite(savePath + "parts/" + std::to_string(coun) + ".png", temp);
		}
	}
	if (!savePath.empty() && bs.size() != 0)
	{
		cv::imwrite(savePath + "result.jpg", marked);
		if (sv)
		{
			cv::imwrite(savePath + "source.png", source);
			if (!fs::exists(savePath + "marked.png"))
				cv::imwrite(savePath + "marked.png", markedS);
		}
		saveCoofs(bs, savePath);
		saveRects(rects, savePath);
		saveExp(b1s, savePath);
	}
	bs.clear();
	return marked;
}

int bc::findMaxZone(int* points, int len)
{
	int por = 1;
	int max = 0;
	int k = 0;
	for (int i = 0; i < len; i++)
	{
		if (points[i] >= por)
		{
			k++;
		}
		else
		{
			max = MAX(max, k);
			k = 0;
		}
	}
	max = MAX(max, k);
	return max;
}


int bc::findStart(int* points, int len)
{
	for (int i = 0; i < len; i++)
	{
		if (points[i] > 0)
			return i;
	}
	return len;
}

std::string bc::getFileName(std::string fullname)
{
	size_t lastindex = fullname.find_last_of(".");
	return fullname.substr(0, lastindex);
}

void bc::checkAll(std::string imgs_path, std::string save_path)
{
	std::vector<timeline> bsv;
	if (!fs::exists(imgs_path))
	{
		std::cout << "Directry is not exist";
		return;
	}
	for (const auto& entry : fs::directory_iterator(imgs_path))
	{
		Mat mat = cv::imread(entry.path().string());
		std::cout << entry.path() << std::endl;
		timeline bs;

		bs.detectBs(mat, true);
		bsv.push_back(bs);
	}
	saveCoofs(bsv, save_path);
}
