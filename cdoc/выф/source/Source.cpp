#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>


#include "timeline.h"
#include "detectKnolls.h"

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


//#define inputArg 3 analysis\images\polus.png analysis\datasets
#define inputArg 3 analysis\images\winter2.png analysis\datasets
#define inputArg 3 analysis\images\big.tif analysis\datasets

namespace bc
{

	//timeline Timeline;
	void reSave(std::string imgs_path, bool setToZero)
	{
		std::string foldName = imgs_path.substr(imgs_path.find_last_of('/'));
		std::unordered_map<std::string, bc::timeline> bsv;
		for (const auto& entry : fs::directory_iterator(imgs_path))
		{
			std::string name = bc::getFileName(entry.path().filename().string());

			name = entry.path().string();
			cv::Mat mat = cv::imread(name, cv::IMREAD_GRAYSCALE);

			uchar set = 255;
			if (setToZero)
			{
				set = 0;
			}
			for (size_t i = 0; i < mat.rows; i++)
			{
				for (size_t j = 0; j < mat.cols; j++)
				{
					uchar p = mat.at<uchar>(i, j);
					if (p > 180 && p < 200)
					{
						mat.at<uchar>(i, j) = 0;
					}
				}
			}
			name = name.replace(name.rfind(foldName), (foldName.size()), foldName + "_wf");
			cv::imwrite(name, mat);
		}
	}

	void checkSinge(std::string img_path, std::string save_path, bool normlize)
	{
		cv::Mat mat = cv::imread(img_path);
		timeline bs;

		bs.detectBs(mat, normlize);
		std::vector<bc::timeline> bsv;

		bsv.push_back(bs);
		bc::saveCoofs(bsv, save_path);
	}

	enum mod
	{
		detectPix,
		detect256,
		detectAll,
		processImage,
		createType,
		test
	};
}
//2 build/polus.png 40 4 build/lables.txt build/traningData.txt
int main(int argc, char* argv[])
{
	setlocale(0, "rus");

	int i;

	bc::timeline testt;
	cv::Mat r;
	bc::mod mode = bc::processImage;
	int k = 40;//40 26
	int d = 4;//4

	/*int labels[k] =
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1, 1, 1, 1, 1, 1, 1, -1, -1, -1, -1, -1, -1, -1 };
	float td[k][d] =
	{ {133,32,24,99},{125,32,18,113},{107,51,0,149},{108,52,0,148},{97,51,0,159},{124,48,2,130},{109,37,12,135},{140,24,24,92},{162,8,43,51},{124,7,45,87},{87,49,0,169},{139,29,20,97},{163,7,45,48},{159,26,24,73},{167,7,65,31},{179,9,44,33},{164,4,60,38},{172,0,0,27},{173,12,57,26},{170,3,57,34},{159,0,0,45},{173,3,47,36},{165,2,53,38},{169,0,0,30},{164,3,71,41},{161,7,57,38}
	,{106,40,0,150},{116,24,0,140},{127,35,0,129},{106,36,0,150},{88,22,0,168},{43,24,0,213},{84,35,0,172},{130,12,42,126},{212,17,6,40},{189,44,0,67},{179,29,21,57},{117,48,0,139},{167,40,0,89},{187,41,0,69}
	};*/
	//std::vector<float*> trainingData(td, td + sizeof(td) / sizeof(td[0]));

	int* labels = nullptr;
	float* td = nullptr;
	std::vector<float*> trainingData;

	std::string m1_detect_folder, m1_saveCoeffs;
	std::string m2_imgPath, m2_saveImagesPath, m2_saveCoefsPath, m2_detectImgPath, m2_labelsPath, m2_coefsPath;
	if (argc < 6 && argc != 4)
	{
		std::cout << "Ожидаются следующие параметры:\n";
		std::cout << "Режим: 1 - проверить все файлы в дирректории и сохранить их коэффициенты, 2 - поиск на изображении объетов\n";
		std::cout << "Параметры для режима 1:\n";
		std::cout << "1)Путь к папке с изображениями\n";
		std::cout << "2)Путь и имя файла с для выгрузки коэффициентов\n";

		std::cout << "Параметры для режима 2:\n";
		std::cout << "1)Путь к изображению\n";
		std::cout << "2)Кол-во элементов обучающей выборки\n";
		std::cout << "3)Глубина выборки(кол-во коэффициентов: 2,3,4)\n";
		std::cout << "4)Путь к файлу с метками\n";
		std::cout << "5)Путь к файлу с коэффциентами\n";
		std::cout << "6)(Опционально)Путь для сохранения(По умолчанию: '-')\n";
		std::cout << "7)(Опционально)Изображение, на котором надо выделить все объекты(По умолчанию совпадает с исходным изображением)\n";


		std::cout << "Параметры для режима 3:\n";
		std::cout << "1)Путь к изображению\n";
		std::cout << "2)Путь для сохранения\n";
		std::cout << "3)(Опционально)Изображение, на котором надо выделить все объекты(По умолчанию совпадает с исходным изображением)\n";
		//system("pause");
		return 0;
	}
	else
	{
		if (argc == 4 && argv[1][0] == '1')
		{
			mode = bc::detect256;
			m1_detect_folder = argv[2];
			std::ifstream filea(m1_detect_folder);
			m1_saveCoeffs = argv[3];
		}
		if (argc >= 7 && argv[1][0] == '2')
		{
			mode = bc::processImage;
			m2_imgPath = argv[2];//1
			k = atoi(argv[3]);//2
			d = atoi(argv[4]);//3
			if (k < 0 || d < 2 || d > 4)
			{
				return -1;
			}
			m2_labelsPath = argv[5];//4
			m2_coefsPath = argv[6];//5
			if (argc > 7 && argv[7][0] != '-')
			{
				m2_saveImagesPath = argv[8];//6
			}
			if (argc > 8 && argv[8][0] != '-')
			{
				m2_detectImgPath = argv[9];//6
			}
			else
				m2_detectImgPath = m2_imgPath;


			std::ifstream file(m2_labelsPath);
			if (file.is_open())
			{
				labels = new int[k];//40
				for (int i = 0; i < k; ++i)
				{
					file >> labels[i];
				}
				file.close();
			}
			else
			{
				return -1;
			}

			std::ifstream file2(m2_coefsPath);
			if (file2.is_open())
			{
				std::string line;
				while (std::getline(file2, line))
				{
					int l = 0;
					if (line == "" || line == "\n" || line == "\r\n")
					{
						break;
					}
					td = new float[d];//4
					std::string token;
					std::istringstream tokenStream(line);
					while (std::getline(tokenStream, token, ' '))
					{
						td[l++] = std::stof(token);
						if (l == d)
						{
							break;
						}
					}
					trainingData.push_back(td);
				}
				file2.close();
			}
			else
			{
				delete[] labels;
				return -1;
			}

		}
		else if ((argc == 4 || argc == 5) && argv[1][0] == '3')
		{
			mode = bc::createType;
			m2_imgPath = argv[2];

			m2_saveImagesPath = argv[3];//1

			if (argc == 5)
				m2_detectImgPath = argv[4];
			else
				m2_detectImgPath = m2_imgPath;
		}
	}
	//mode = bc::test;
	switch (mode)
	{
	case bc::detect256:
		//bc::checkAll("analysis/data/all/data", "analysis/data/all/all256_coofs.json");
		bc::checkAll(m1_detect_folder, m1_saveCoeffs);
		break;
	case bc::processImage:
		//bc::detect(
		//	"analysis/data/polus.png", "analysis/data/polus.png",
		//	"analysis/data/all/data/", "analysis/coeffs/all256_coofs.json",
		//	k, d, labels, trainingData);
		r = bc::detect(
			m2_imgPath, m2_detectImgPath,
			m2_saveCoefsPath,
			k, d, labels, trainingData);

		//polus2 проверить 8289
		//9208
	/*	r = bc::detect(
			"analysis/data/polus2.png", "analysis/data/polus2.png",
			"", "",
			k, d, labels, trainingData);*/

			/*	r = bc::detect(
					"analysis/data/winter2.jpg", "analysis/data/winter2.jpg",
					"", "",
					k, d, labels, trainingData);*/

					//r = bc::detect(
					//	"analysis/data/polus2shrt.png", "analysis/data/polus2shrt.png",
					//	"", "",
					//	k, d, labels, trainingData);
					//detect("analysis/data/winter2.jpg", "analysis/data/winter2.jpg");

		cv::namedWindow("Result", cv::WINDOW_NORMAL);
		cv::imshow("Result", r);
		cv::waitKey(0);
		break;

	case bc::createType:
		r = bc::detect(m2_imgPath, m2_detectImgPath, m2_saveImagesPath, 0, 0, nullptr, trainingData);
		break;
	case bc::test:

		cv::Mat testmat = cv::imread("test5.png", cv::IMREAD_ANYCOLOR);
		int w = testmat.cols, h = testmat.rows;
		bc::timeline test;
		test.detectBs(testmat);


		//uchar* data = new uchar[36]{
		//63,121,73,14,120,135,
		//237,90,194,136,4,43,
		//90,212,193,199,88,154,
		//51,150,98,239,42,68,
		//65,141,145,34,203,167,
		//158,234,20,145,80,176
		//};
		//cv::Mat test36(6, 6, CV_8UC1, data);
		//testt.detectBs(test36);

		//uchar* data2 = new uchar[25]{
		//63,121,73,14,120,
		//237,90,194,136,4,
		//90,212,193,199,88,
		//51,150,98,239,42,
		//65,141,145,34,203
		//};
		//cv::Mat test25(5, 5, CV_8UC1, data2);
		//testt.detectBs(test25, true);

		//uchar* data3 = new uchar[16]{
		//63,121,73,14,
		//237,90,194,136,
		//90,212,193,199,
		//51,150,98,239
		//};
		//cv::Mat test16(4, 4, CV_8UC1, data3);
		//testt.detectBs(test16, true);

		//uchar* data4 = new uchar[9]{
		//63,121,73,
		//237,90,194,
		//90,212,193
		//};
		//cv::Mat test9(3, 3, CV_8UC1, data4);
		//testt.detectBs(test9, true);
		//


		break;

	}
	for (size_t i = 0; i < trainingData.size(); i++)
	{
		delete[] trainingData[i];
	}
	trainingData.clear();
	delete[] labels;
	return 0;
}
