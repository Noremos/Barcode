#pragma once 
#include "timeline.h"

#include <math.h>
#include <stdlib.h>


namespace bc
{
	//struct EXPORT lbar
	//{
	//	timeline bs;
	//	int poz;
	//	cv::Rect rect;
	//    lbar(int start, int end, int height)
	//	{
	//		this->start = start;
	//		this->end = end;
	//		this->hei = height;
	//	}
	//};
	////typedef std::vector<bc::lbar> lBarcodes;
	//typedef std::vector<int> lBarcodes;


	/*
saveFragsPath = "analysis/data/all/data/"
 std::string saveCoofsPath = "analysis/coeffs/all256_coofs.json"
 int labels[k] =
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1, 1, 1, 1, 1, 1, 1, -1, -1, -1, -1, -1, -1, -1 };
	float trainingData[k][d] =
	{ {133,32,24,99},{125,32,18,113},{107,51,0,149},{108,52,0,148},{97,51,0,159},{124,48,2,130},{109,37,12,135},{140,24,24,92},{162,8,43,51},{124,7,45,87},{87,49,0,169},{139,29,20,97},{163,7,45,48},{159,26,24,73},{167,7,65,31},{179,9,44,33},{164,4,60,38},{172,0,0,27},{173,12,57,26},{170,3,57,34},{159,0,0,45},{173,3,47,36},{165,2,53,38},{169,0,0,30},{164,3,71,41},{161,7,57,38}
	,{106,40,0,150},{116,24,0,140},{127,35,0,129},{106,36,0,150},{88,22,0,168},{43,24,0,213},{84,35,0,172},{130,12,42,126},{212,17,6,40},{189,44,0,67},{179,29,21,57},{117,48,0,139},{167,40,0,89},{187,41,0,69}
	};
	k=40;d=4
*/
	cv::Mat detect(std::string path, std::string mark_path, std::string savePath, 
		int k, int d, int* labels, std::vector<float*> trainingData);

	void saveCoofs(std::vector<bc::timeline> arr, std::string);

	int findMaxZone(int* points, int len = 256);

	int findStart(int* points, int len = 256);

	void checkAll(std::string imgs_path, std::string save_path);
	std::string getFileName(std::string fullname);
}
