//#pragma once
//
//#include "prep.h"
//
//enum noise_typ
//{
//	gauss,
//	sNp,
//	impulse,
//	poisson,
//	speckle
//};
//
//// sigma: for gaus; for impule: sigma/(1-sigma) : 255/0, amount
//void noise(noise_typ type, Bmat8& image, float sigma, float amount = 0.004)
//{
//	int row = image.hei();
//	int col = image.wid();
//	int size = image.length();
//	cv::RNG cvrand;
//	switch (type)
//	{
//	case gauss:
//	{
//
//
//		//float max = -99999;
//		//float min = 99999;
//		//for (size_t i = 0; i < size; i++)
//		//{
//		//	uchar& val = image.getLiner(i);
//		//	float c = cvrand.gaussian(sigma) * 128;
//		//	int valu = val + c;
//		//	max = MAX(max, c);
//		//	min = MIN(min, c);
//		//	val = static_cast<uchar>(MAX(0, MIN(255,valu)));
//		//}
//		//std::cout << max << " " << min;
//
//		cv::Mat noise = Mat(image.mat.size(), CV_32F);
//		cv::normalize(image.mat, image.mat, 0.0, 1.0, cv::NormTypes::NORM_MINMAX, CV_32F);
//		cv::randn(noise, 0, sigma);
//		image.mat += noise;
//		for (size_t i = 0; i < row; i++)
//		{
//			for (size_t c = 0; c < col; c++)
//			{
//				float& val = image.mat.at<float>(i, c);
//				val = static_cast<uchar>(MAX(0, MIN(255, val * 255)));
//
//			}
//		}
//		image.mat.convertTo(image.mat, CV_8UC1);
//	}
//	case sNp:
//	{
//		int num_salt = static_cast<int>(amount * size * sigma);
//		for (size_t i = 0; i < num_salt; i++)
//		{
//			int x = cvrand.next() % col;
//			int y = cvrand.next() % row;
//			image.mat.at<uchar>(y, x) = 255;
//		}
//
//		num_salt = static_cast<int>(amount * size *  ( 1 - sigma));
//		for (size_t i = 0; i < num_salt; i++)
//		{
//			int x = cvrand.next() % col;
//			int y = cvrand.next() % row;
//			image.mat.at<uchar>(y, x) = 0;
//		}
//	}
//	case impulse:
//	{
//		int num_salt = static_cast<int>(amount * size);
//		for (size_t i = 0; i < num_salt; i++)
//		{
//			int x = cvrand.next() % col;
//			int y = cvrand.next() % row;
//			image.mat.at<uchar>(y, x) = 255;
//		}
//	}
//	//case poisson:
//
//	//	vals = len(np.unique(image))
//	//		vals = 2 * *np.ceil(np.log2(vals))
//	//		noisy = np.random.poisson(image * vals) / float(vals)
//	//		return noisy
//
//	//		break;
//	//case speckle:
//	//	row, col = image.shape
//	//		gauss = np.random.randn(row, col)
//	//		gauss = gauss.reshape(row, col)
//	//		noisy = image + image * gauss
//	//		break;
//	//default:
//	//	break;
//	}
//}
//
//
//float getSKO(Bmat8 m1, Bmat8 m2, int dif = 1)
//{
//	// https://wikimedia.org/api/rest_v1/media/math/render/svg/56c53fa1ededf246dc1a602acd88f7c6e68b27fd
//	assert(m1.length() == m2.length());
//	unsigned long long f = 0;
//	for (size_t i = 0; i < m1.length(); i++)
//	{
//		f += (int)sqr(m1.getLiner(i) - m2.getLiner(i));
//	}
//	return  sqrt(static_cast<long double>(f) / static_cast<long double>(m1.length() - dif));
//}
//
//
//float getPSNR(Bmat8 m1, Bmat8 m2, int N = 255)
//{
//	// https://wikimedia.org/api/rest_v1/media/math/render/svg/56c53fa1ededf246dc1a602acd88f7c6e68b27fd
//	float sko = getSKO(m1, m2, 0);
//	return 10 * log10f((float)sqr(N) / sko);
//}
//
//void simple(Bmat8 img, Mat mat)
//{
//	cv::Mat out(img.hei(), img.wid(), CV_8UC1, cv::Scalar(0));
//
//	for (int i = 0; i < img.wid(); i++)
//	{
//		for (int j = 1; j < img.hei(); j++)
//		{
//			uchar clos = 0;
//			int mindiff = 255;
//			uchar uval = mat.at<uchar>(j, i);
//			if (uval == 255 || uval == 0)
//			{
//				int badCounter = 0;
//				uchar newbadval = 0;
//				int val = uval;
//				int sum = 0;
//				for (int ii = i - 1; ii <= i + 1; ii++)
//				{
//					for (int jj = 0; jj <= j + 1; jj++)
//					{
//						if (ii >= 0 && ii < img.wid() && jj >= 0 && jj < img.hei())
//						{
//							int potval = mat.at<uchar>(jj, ii);
//							//badCounter++;
//							if (potval == 0 && val == 255)
//							{
//								sum += potval;
//								badCounter++;
//								uval = 255;
//								continue;
//							}
//							if (potval == 255 && val == 0)
//							{
//								badCounter++;
//								sum += potval;
//								uval = 0;
//								continue;
//							}
//							/*
//							&& potval != 255)
//							{
//							if (abs(val - potval) < mindiff)
//							{
//								mindiff = abs(val - potval);
//								clos = potval;
//							}
//							}*/
//						}
//					}
//				}
//				clos = sum == 0 ? uval : (sum / badCounter);
//				mat.at<uchar>(j, i) = mat.at<uchar>(j - 1, i);;
//				out.at<uchar>(j, i) = mat.at<uchar>(j - 1, i);;
//			}
//			else
//			{
//				out.at<uchar>(j, i) = uval;
//			}
//		}
//	}
//
//}
//
//cv::Mat medianBlur(Mat input_image, int n = 3)
//{
//	Mat out;
//	cv::medianBlur(input_image, out, n);
//	return out;
//}
//
//cv::Mat removeNoise(Mat img)
//{
//	bc::BarConstructor bcont;
//	bcont.addStructure(bc::ProcType::f0t255, bc::ColorType::gray, bc::ComponentType::Component);
//	bcont.createBinaryMasks = true;
//	bcont.createGraph = false;
//	bcont.returnType = bc::ReturnType::barcode3d;
//
//	// ###### PARAMS #####
//	bool rev = true;
//	const int NSIZE = 1;
//	// ####################
//
//	Bmat8 input(img, BarType::BYTE8_1);
//
//	bc::BarcodeCreator test;
//	auto* cont = test.createBarcode(&input, bcont);
//	auto* item = cont->getItem(0);
//
//	cv::Mat out(input.hei(), input.wid(), CV_8UC1, cv::Scalar(0));
//
//	out = input.mat.clone();
//
//	for (auto& line : item->barlines)
//	{
//		auto& matr = line->matr;
//		/*	for (size_t k = 0; k < matr.size(); k++)
//		{
//			auto& p = matr[k];
//
//			out.at<uchar>(p.getY(), p.getX()) += p.value;
//		}*/
//
//		int k = 0;
//		int sizeb3;
//		int titalSize = 0;
//		while (k < line->bar3d->size() && (sizeb3 = line->bar3d->at(k).count - titalSize) <= NSIZE)
//		{
//			for (int total = k + sizeb3; k < total; ++k)
//			{
//				auto& p = matr[k];
//				uchar& val = out.at<uchar>(p.getY(), p.getX());
//				//assert(val >= p.value);
//				val += (uchar)p.value;
//			}
//			titalSize += sizeb3;
//			//break;
//
//		}
//		//cv::namedWindow("proc", cv::WINDOW_NORMAL);
//		//cv::imshow("proc", out);
//		//cv::waitKey(0);
//	}
//	//if (!rev)
//		//out = 255 - out;
//
//	delete cont;
//	return out;
//}