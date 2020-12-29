//#include <opencv2/opencv.hpp>
//#include <opencv2/imgproc.hpp>
//
//
//#include "barcodeCreator.h" 
//
//#include <iostream>
//#include <filesystem>
//#include <string>
//#include <fstream>
//
///////////////////////////////
///////////////////////////////
///////////////////////////////  at ((row,col) = (hei,wid)
///////////////////////////////
///////////////////////////////
//
//namespace fs = std::filesystem;
//
//typedef cv::Point3_<uint8_t> Pixel;
//using std::string;
//typedef std::pair<bc::BarcodeTwo*, bool> net;
//
////#define inputArg 3 analysis\images\polus.png analysis\datasets
//#define inputArg 3 analysis\images\winter2.png analysis\datasets
//#define inputArg 3 analysis\images\big.tif analysis\datasets
//
//
//namespace bc
//{
//	struct info
//	{
//		Barcontainer* bar;
//		int type;
//
//		info(Barcontainer* bar, int type)
//		{
//			this->bar = bar;
//			this->type = type;
//		}
//	};
//
//	struct info2
//	{
//		bc::BarcodeTwo* bar;
//		int type;
//
//		info2(BarcodeTwo* bar, int type)
//		{
//			this->bar = bar;
//			this->type = type;
//		}
//	};
//	int procR(int r, int b)
//	{
//		return (int)round(100.0 * r / (r + b));
//	}
//	int procB(int r, int b)
//	{
//		return (int)round(100.0 * b / (r + b));
//	}
//	void doMagick(string s)
//	{
//		using recursive_directory_iterator = fs::recursive_directory_iterator;
//		barcodeCreator bc;
//		std::vector <info2> etalons, test;
//		std::vector<bc::BarcodeTwo*> planes, notPlanes;
//		float porog = 0.6;
//		int i1 = 0, j1 = 0;
//
//
//		int pr = 30; bool norm = false;
//		int testCount = 15;
//		int traainCount = 1000;
//
//	/*	for (const auto& dirEntry : recursive_directory_iterator(s))
//		{
//			string path = dirEntry.path().string();
//			cv::Mat m = cv::imread(path, cv::IMREAD_COLOR);
//			bc::BarcodeTwo* b = bc.createTwoSlbarcode(m, false);
//			b->preprocessBar(pr, norm);
//
//			if (dirEntry.path().filename().string()[0] == '1')
//				planes.push_back(b);
//			else
//				notPlanes.push_back(b);
//
//		}*/
//
//		int halfFilling = 0;
//
//		for (size_t i = 0; i < testCount; i++)
//		{
//			int poz;
//			poz = rand() % planes.size();
//			etalons.push_back(info2(planes[poz], 1));
//			planes.erase(planes.begin() + poz);
//
//			poz = rand() % notPlanes.size();
//			etalons.push_back(info2(notPlanes[poz], 0));
//			notPlanes.erase(notPlanes.begin() + poz);
//		}
//
//		//pretCount = notPlanes.size() * 0.2;
//		for (size_t i = 0; i < planes.size() && i < traainCount; i++)
//			test.push_back(info2(planes[i], 1));
//
//		for (size_t i = 0; i < notPlanes.size() && i < traainCount; i++)
//			test.push_back(info2(notPlanes[i], 0));
//
//		notPlanes.clear();
//		planes.clear();
//
//		int r = 0, b = 0, e = 0;
//		for (size_t i = 0; i < test.size(); i++)
//		{
//			float mfs[2] = { 0,0 };
//			for (size_t j = 0; j < etalons.size(); j++)
//			{
//				float fs = Barcode::compireBarcodes(etalons[j].bar, test[i].bar, CompireFunction::CommonToLen);
//				mfs[etalons[j].type] = MAX(fs, mfs[etalons[j].type]);
//			}
//			int retType = 1;
//			if (mfs[0] > mfs[1])
//				retType = 0;
//
//			if (test[i].type == retType)
//				++r;
//			else
//				++b;
//		}
//
//		std::cout << "Right detect: " << procR(r,b) << "%\n";
//		std::cout << "False detect: " << procB(r, b) << "%\n";
//		system("pause");
//	}
//
//	bool replace(std::string& str, const std::string& from, const std::string& to) {
//		size_t start_pos = str.find(from);
//		if (start_pos == std::string::npos)
//			return false;
//		str.replace(start_pos, from.length(), to);
//		return true;
//	}
//
//	void split(string str, std::vector<string>& strings)
//	{
//		std::istringstream f(str);
//		string s;
//		while (getline(f, s, ' '))
//			strings.push_back(s);
//	}
//	int gt(string s)
//	{
//		return atoi(s.c_str());
//	}
//
//	void doMagickDOTA()
//	{
//		string s = "D:\\Programs\\C++\\Barcode\\analysis\\datasets\\DOTA\\images";
//		using recursive_directory_iterator = fs::recursive_directory_iterator;
//		barcodeCreator* bc = new barcodeCreator();
//		std::vector<std::vector <Barcontainer*>*> elems;
//
//		std::unordered_map<string, int> categorues;
//
//		float porog = 0.6;
//		int i = 0, j = 0, tic = 0;
//		int trainCount = 15;
//		int testCount = 1000;
//		int itmsLimit = 3;
//		int pr = 30; bool norm = true;
//		int  total = testCount + trainCount;
//		int totalCount = 0, totalSeled = 0;
//
//		std::vector<bc::barstruct> structure;
//		structure.push_back(barstruct(ProcType::f0t255, ColorType::gray, ComponentType::Hole));
//		structure.push_back(barstruct(ProcType::f255t0, ColorType::gray, ComponentType::Hole));
//		for (const auto& dirEntry : recursive_directory_iterator(s))
//		{
//			string path = dirEntry.path().string();//D:\Programs\C++\Barcode\analysis\datasets\DOTA\labels\P0013.txt
//			std::cout << path << "...";
//			cv::Mat source = cv::imread(path, cv::IMREAD_GRAYSCALE);
//			replace(path, "images", "labels");
//			replace(path, "png", "txt");
//
//			std::ifstream infile(path);
//			std::string line;
//
//			std::getline(infile, line);
//			std::getline(infile, line);
//			bool stop = false;
//			while (std::getline(infile, line))//1321.0 44.0 1338.0 37.0 1339.0 70.0 1329.0 58.0 large-vehicle 1
//			{
//				std::vector<string> lids;
//				split(line, lids);
//
//				string s = (lids[8]);
//				int dif = gt(lids[9]);
//				if (dif == 1)
//					continue;
//				if (categorues.find(s) == categorues.end())
//				{
//					if (categorues.size() >= itmsLimit)
//						continue;
//
//					categorues.insert(std::pair<string, int>(s, i++));
//					elems.push_back(new std::vector<Barcontainer*>);
//					++j;
//				}
//				int index = categorues[s];
//				auto invec = elems[index];
//				if (invec->size() == total)
//					continue;
//
//				//else if (++count[index] == trainCount + testCount)
//				//	--j;
//
//				std::vector<cv::Point> points;
//				points.push_back(cv::Point(gt(lids[0]), gt(lids[1])));
//				points.push_back(cv::Point(gt(lids[2]), gt(lids[3])));
//				points.push_back(cv::Point(gt(lids[4]), gt(lids[5])));
//				points.push_back(cv::Point(gt(lids[6]), gt(lids[7])));
//
//				/*cv::line(source, points[0], points[1], cv::Scalar(0, 255, 0), 2);
//				cv::line(source, points[1], points[2], cv::Scalar(0, 255, 0), 2);
//				cv::line(source, points[2], points[3], cv::Scalar(0, 255, 0), 2);
//				cv::line(source, points[3], points[0], cv::Scalar(0, 255, 0), 2);*/
//
//				cv::Rect ds = cv::boundingRect(points);
//				if (ds.x + ds.width > source.cols)
//					continue;
//
//				if (ds.y + ds.height > source.rows)
//					continue;
//
//				if (ds.x < 0)
//					ds.x = 0;
//
//				if (ds.y < 0)
//					ds.y = 0;
//				if (ds.width > 400 || ds.height > 400)
//					continue;
//
//				cv::Mat m = source(ds);
//				cv::resize(m, m, cv::Size(32, 32));
//				auto b = bc->createBarcode(m, structure);
//				b->preprocessBar(pr, norm);
//				invec->push_back(b);
//				++tic;
//
//
//				//float r = Barcontainer::compireBarcodes(b, b, bc::CompireFunction::CommonToLen);
//				if (invec->size() == total)
//					--j;
//				/*			if (train.size() == 109)
//							{
//								cv::imshow("res",m);
//								cv::waitKey(0);
//							}*/
//				if (j == 0 && itmsLimit >= categorues.size())
//				{
//					stop = true;
//					break;
//				}
//			}
//			std::cout << " done: " << tic << "/" << categorues.size() * (total) << std::endl;
//
//			if (stop)
//				break;
//
//			/*cv::namedWindow("res", cv::WINDOW_NORMAL);
//			cv::imshow("res", source);
//			cv::waitKey(0);*/
//		}
//		delete bc;
//
//		std::vector<info> train, test;
//		for (size_t i = 0; i < elems.size(); i++)
//		{
//			auto vect = elems[i];
//			for (size_t j = 0; j < testCount && vect->size() > 0; j++)
//			{
//				int poz = rand() % vect->size();
//				Barcontainer* ba = vect->operator[](poz);
//				test.push_back(info(ba, i));
//				vect->erase(vect->begin() + poz);
//			}
//			for (size_t j = 0; j < vect->size(); j++)
//			{
//				Barcontainer* ba = vect->operator[](j);
//				train.push_back(info(ba, i));
//			}
//			vect->clear();
//			delete vect;
//		}
//
//		int r = 0, b = 0, e = 0;
//		int si = categorues.size();
//		float* mx = new float[si];
//		for (size_t i = 0; i < test.size(); i++)
//		{
//			for (size_t k = 0; k < si; k++)
//				mx[k] = 0;
//
//			for (size_t j = 0; j < train.size(); j++)
//			{
//				float fs = Barcontainer::compireBarcodes(test[i].bar, train[j].bar, CompireFunction::CommonToLen);
//
//				mx[train[j].type] = MAX(fs, mx[train[j].type]);
//			}
//			if (i % 10 == 0)
//				std::cout << i << "/" << test.size() << std::endl;
//			int max = std::distance(mx, std::max_element(mx, mx + si));
//			if (test[i].type == max)
//				++r;
//			else
//				++b;
//		}
//		delete mx;
//
//		std::cout << "Right detect: " << (int)round((((float)r) / (r + b)) * 100) << "%\n";
//		std::cout << "False detect: " << (int)round((((float)b) / (r + b)) * 100) << "%\n";
//		//std::cout << "Not rec: " << (int)round((((float)e) / test.size()) * 100) << "%\n";
//		system("pause");
//	}
//
//	//barcodeCreator barcodeCreator;
//	void reSave(std::string imgs_path, bool setToZero)
//	{
//		std::string foldName = imgs_path.substr(imgs_path.find_last_of('/'));
//		std::unordered_map<std::string, bc::barcodeCreator> bsv;
//		for (const auto& entry : fs::directory_iterator(imgs_path))
//		{
//			/*std::string name = bc::getFileName(entry.path().filename().string());
//
//			name = entry.path().string();
//			cv::Mat mat = cv::imread(name, cv::IMREAD_GRAYSCALE);
//
//			uchar set = 255;
//			if (setToZero)
//			{
//				set = 0;
//			}
//			for (size_t i = 0; i < mat.rows; i++)
//			{
//				for (size_t j = 0; j < mat.cols; j++)
//				{
//					uchar p = mat.at<uchar>(i, j);
//					if (p > 180 && p < 200)
//					{
//						mat.at<uchar>(i, j) = 0;
//					}
//				}
//			}
//			name = name.replace(name.rfind(foldName), (foldName.size()), foldName + "_wf");
//			cv::imwrite(name, mat);*/
//		}
//	}
//
//	void checkSinge(std::string img_path, std::string save_path, bool normlize)
//	{
//		cv::Mat mat = cv::imread(img_path);
//		barcodeCreator bs;
//
//		//bs.detectBs(mat, normlize);
//		std::vector<bc::barcodeCreator> bsv;
//
//		bsv.push_back(bs);
//		//bc::saveCoofs(bsv, save_path);
//	}
//
//	enum mod
//	{
//		detectPix,
//		detect256,
//		detectAll,
//		processImage,
//		trainNet,
//		createType,
//		test
//	};
//}
////2 build/polus.png 40 4 build/lables.txt build/traningData.txt
//int main(int argc, char* argv[])
//{
//	setlocale(0, "rus");
//	srand(time(0));
//
//	//bc::doMagick("D:\\Programs\\C++\\Barcode\\analysis\\shipnet\\shipsnet");
//	bc::doMagick("D:\\Programs\\C++\\Barcode\\analysis\\planesnet\\planesnet");
//	//bc::doMagickDOTA();
//	return 0;
//
//	int i;
//
//	bc::barcodeCreator testt;
//	cv::Mat r;
//	bc::mod mode = bc::processImage;
//	int k = 40;//40 26
//	int d = 4;//4
//
//	/*int labels[k] =
//	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1, 1, 1, 1, 1, 1, 1, -1, -1, -1, -1, -1, -1, -1 };
//	float td[k][d] =
//	{ {133,32,24,99},{125,32,18,113},{107,51,0,149},{108,52,0,148},{97,51,0,159},{124,48,2,130},{109,37,12,135},{140,24,24,92},{162,8,43,51},{124,7,45,87},{87,49,0,169},{139,29,20,97},{163,7,45,48},{159,26,24,73},{167,7,65,31},{179,9,44,33},{164,4,60,38},{172,0,0,27},{173,12,57,26},{170,3,57,34},{159,0,0,45},{173,3,47,36},{165,2,53,38},{169,0,0,30},{164,3,71,41},{161,7,57,38}
//	,{106,40,0,150},{116,24,0,140},{127,35,0,129},{106,36,0,150},{88,22,0,168},{43,24,0,213},{84,35,0,172},{130,12,42,126},{212,17,6,40},{189,44,0,67},{179,29,21,57},{117,48,0,139},{167,40,0,89},{187,41,0,69}
//	};*/
//	//std::vector<float*> trainingData(td, td + sizeof(td) / sizeof(td[0]));
//
//	int* labels = nullptr;
//	float* td = nullptr;
//	std::vector<float*> trainingData;
//
//	std::string m1_detect_folder, m1_saveCoeffs;
//	std::string m2_imgPath, m2_saveImagesPath, m2_saveCoefsPath, m2_detectImgPath, m2_labelsPath, m2_coefsPath;
//	std::string m4_path;
//
//	argc = 2;
//	//char* c = (char*)"D:\\Programs\\C++\\Barcode\\analysis\\planesnet\\planesnet";;
//	//argv[0] = "";
//
//	//if (argc < 6 && argc != 4)
//	//{
//	//	std::cout << "Ожидаются следующие параметры:\n";
//	//	std::cout << "Режим: 1 - проверить все файлы в дирректории и сохранить их коэффициенты, 2 - поиск на изображении объетов\n";
//	//	std::cout << "Параметры для режима 1:\n";
//	//	std::cout << "1)Путь к папке с изображениями\n";
//	//	std::cout << "2)Путь и имя файла с для выгрузки коэффициентов\n";
//
//	//	
//	//	std::cout << "Параметры для режима 2:\n";
//	//	std::cout << "1)Путь к изображению\n";
//	//	std::cout << "2)Кол-во элементов обучающей выборки\n";
//	//	std::cout << "3)Глубина выборки(кол-во коэффициентов: 2,3,4)\n";
//	//	std::cout << "4)Путь к файлу с метками\n";
//	//	std::cout << "5)Путь к файлу с коэффциентами\n";
//	//	std::cout << "6)(Опционально)Путь для сохранения(По умолчанию: '-')\n";
//	//	std::cout << "7)(Опционально)Изображение, на котором надо выделить все объекты(По умолчанию совпадает с исходным изображением)\n";
//
//	//	
//	//	std::cout << "Параметры для режима 3:\n";
//	//	std::cout << "1)Путь к изображению\n";
//	//	std::cout << "2)Путь для сохранения\n";
//	//	std::cout << "3)(Опционально)Изображение, на котором надо выделить все объекты(По умолчанию совпадает с исходным изображением)\n";
//	//	
//	//	
//	//	std::cout << "Параметры для режима 4:\n";
//	//	std::cout << "1)Путь к json\n";
//	//	//system("pause");
//	//	return 0;
//	//}
//	//else
//	{
//		if (argc == 4 && argv[1][0] == '1')
//		{
//			mode = bc::detect256;
//			m1_detect_folder = argv[2];
//			std::ifstream filea(m1_detect_folder);
//			m1_saveCoeffs = argv[3];
//		}
//		if (argc >= 7 && argv[1][0] == '2')
//		{
//			mode = bc::processImage;
//			m2_imgPath = argv[2];//1
//			k = atoi(argv[3]);//2
//			d = atoi(argv[4]);//3
//			if (k < 0 || d < 2 || d > 4)
//			{
//				return -1;
//			}
//			m2_labelsPath = argv[5];//4
//			m2_coefsPath = argv[6];//5
//			if (argc > 7 && argv[7][0] != '-')
//			{
//				m2_saveImagesPath = argv[8];//6
//			}
//			if (argc > 8 && argv[8][0] != '-')
//			{
//				m2_detectImgPath = argv[9];//6
//			}
//			else
//				m2_detectImgPath = m2_imgPath;
//
//
//			std::ifstream file(m2_labelsPath);
//			if (file.is_open())
//			{
//				labels = new int[k];//40
//				for (int i = 0; i < k; ++i)
//				{
//					file >> labels[i];
//				}
//				file.close();
//			}
//			else
//			{
//				return -1;
//			}
//
//			std::ifstream file2(m2_coefsPath);
//			if (file2.is_open())
//			{
//				std::string line;
//				while (std::getline(file2, line))
//				{
//					int l = 0;
//					if (line == "" || line == "\n" || line == "\r\n")
//					{
//						break;
//					}
//					td = new float[d];//4
//					std::string token;
//					std::istringstream tokenStream(line);
//					while (std::getline(tokenStream, token, ' '))
//					{
//						td[l++] = std::stof(token);
//						if (l == d)
//						{
//							break;
//						}
//					}
//					trainingData.push_back(td);
//				}
//				file2.close();
//			}
//			else
//			{
//				delete[] labels;
//				return -1;
//			}
//
//		}
//		else if ((argc == 4 || argc == 5) && argv[1][0] == '3')
//		{
//			mode = bc::createType;
//			m2_imgPath = argv[2];
//
//			m2_saveImagesPath = argv[3];//1
//
//			if (argc == 5)
//				m2_detectImgPath = argv[4];
//			else
//				m2_detectImgPath = m2_imgPath;
//		}
//		else if ((argc == 3) && argv[1][0] == '4')
//		{
//			mode = bc::trainNet;
//			m4_path = argv[2];
//		}
//	}
//	mode = bc::trainNet;
//	m4_path = "";
//
//	//mode = bc::test;
//	switch (mode)
//	{
//		//case bc::detect256:
//		//	//bc::checkAll("analysis/data/all/data", "analysis/data/all/all256_coofs.json");
//		//	bc::checkAll(m1_detect_folder, m1_saveCoeffs);
//		//	break;
//		//case bc::processImage:
//		//	//bc::detect(
//		//	//	"analysis/data/polus.png", "analysis/data/polus.png",
//		//	//	"analysis/data/all/data/", "analysis/coeffs/all256_coofs.json",
//		//	//	k, d, labels, trainingData);
//		//	r = bc::detect(
//		//		m2_imgPath, m2_detectImgPath,
//		//		m2_saveCoefsPath,
//		//		k, d, labels, trainingData);
//
//		//	//polus2 проверить 8289
//		//	//9208
//		///*	r = bc::detect(
//		//		"analysis/data/polus2.png", "analysis/data/polus2.png",
//		//		"", "",
//		//		k, d, labels, trainingData);*/
//
//		//		/*	r = bc::detect(
//		//				"analysis/data/winter2.jpg", "analysis/data/winter2.jpg",
//		//				"", "",
//		//				k, d, labels, trainingData);*/
//
//		//				//r = bc::detect(
//		//				//	"analysis/data/polus2shrt.png", "analysis/data/polus2shrt.png",
//		//				//	"", "",
//		//				//	k, d, labels, trainingData);
//		//				//detect("analysis/data/winter2.jpg", "analysis/data/winter2.jpg");
//
//		//	cv::namedWindow("Result", cv::WINDOW_NORMAL);
//		//	cv::imshow("Result", r);
//		//	cv::waitKey(0);
//		//	break;
//
//		//case bc::createType:
//		//	r = bc::detect(m2_imgPath, m2_detectImgPath, m2_saveImagesPath, 0, 0, nullptr, trainingData);
//		//	break;
//	case bc::test:
//
//		/*	cv::Mat testmat = cv::imread("test5.png", cv::IMREAD_ANYCOLOR);
//			int w = testmat.cols, h = testmat.rows;
//			bc::barcodeCreator test;*/
//			//test.(testmat);
//
//
//			//uchar* data = new uchar[36]{
//			//63,121,73,14,120,135,
//			//237,90,194,136,4,43,
//			//90,212,193,199,88,154,
//			//51,150,98,239,42,68,
//			//65,141,145,34,203,167,
//			//158,234,20,145,80,176
//			//};
//			//cv::Mat test36(6, 6, CV_8UC1, data);
//			//testt.detectBs(test36);
//
//			//uchar* data2 = new uchar[25]{
//			//63,121,73,14,120,
//			//237,90,194,136,4,
//			//90,212,193,199,88,
//			//51,150,98,239,42,
//			//65,141,145,34,203
//			//};
//			//cv::Mat test25(5, 5, CV_8UC1, data2);
//			//testt.detectBs(test25, true);
//
//			//uchar* data3 = new uchar[16]{
//			//63,121,73,14,
//			//237,90,194,136,
//			//90,212,193,199,
//			//51,150,98,239
//			//};
//			//cv::Mat test16(4, 4, CV_8UC1, data3);
//			//testt.detectBs(test16, true);
//
//			//uchar* data4 = new uchar[9]{
//			//63,121,73,
//			//237,90,194,
//			//90,212,193
//			//};
//			//cv::Mat test9(3, 3, CV_8UC1, data4);
//			//testt.detectBs(test9, true);
//			//
//
//
//		break;
//	case bc::mod::trainNet:
//		//bc::doMagick();
//		//bc::doMagickDOTA();
//
//		break;
//	}
//	for (size_t i = 0; i < trainingData.size(); i++)
//	{
//		delete[] trainingData[i];
//	}
//	trainingData.clear();
//	delete[] labels;
//	return 0;
//}
