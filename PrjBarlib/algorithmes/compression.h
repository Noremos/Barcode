#pragma once
#undef _HAS_STD_BYTE

#include "prep.h"

#undef max
//

using std::cout;
using std::cin;

namespace CN {
//
//	// Client side C/C++ program to demonstrate Socket programming
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment (lib, "Ws2_32.lib")
//#include <iostream>
////#include <string.h>
//#include <sstream>
#include <WinSock2.h>
#include <WS2tcpip.h>

//
	SOCKET _socket;
//
	enum class PY_SIGN : uchar
	{
		SEND_IMG = 0,

	};

	SOCKET connect()
	{
		WSADATA WsaData;
		bool err = WSAStartup(MAKEWORD(2, 2), &WsaData) == NO_ERROR;

		std::string ip = "127.0.0.1";
		int port = 8088;

		struct addrinfo* result = NULL,
			* ptr = NULL,
			hints;

		// Validate the parameters 
		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		// Resolve the server address and port
		int iResult = getaddrinfo(ip.c_str(), std::to_string(port).c_str(), &hints, &result);
		if (iResult != 0) {
			printf("getaddrinfo failed with error: %d\n", iResult);
			return false;
		}

		// Attempt to connect to an address until one succeeds
		for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
		{
			// Create a SOCKET for connecting to server
			_socket = socket(ptr->ai_family, ptr->ai_socktype,
				ptr->ai_protocol);
			if (_socket == INVALID_SOCKET)
			{
				printf("socket failed with error: %ld\n", WSAGetLastError());
				return false;
			}

			// Connect to server.
			iResult = connect(_socket, ptr->ai_addr, (int)ptr->ai_addrlen);
			if (iResult == SOCKET_ERROR)
			{
				closesocket(_socket);
				_socket = INVALID_SOCKET;
				continue;
			}
			break;
		}

		freeaddrinfo(result);

		if (_socket == INVALID_SOCKET)
		{
			printf("Unable to connect to server!\n");
			return false;
		}
		printf("Connecting esabled!\n");


		return true;
	}
}

//void sentMatGetRest(SOCKET connection, Mat img)
//{
//	std::string reply(15, ' ');
//	int totalSize = img.rows * img.cols + 4 + 4;
//
//	unique_ptr<uchar[]> data;
//	data.reset(new uchar[totalSize]);
//	memcpy(data.get(), &totalSize, 4);
//	memcpy(data.get() + 4, &img.cols, 4);
//	memcpy(data.get() + 8, &img.rows, 4);
//	memcpy(data.get() + 12, img.data, img.rows * img.cols);
//	send(connection, (const char*)data.get(), totalSize, 0);
//
//	auto bytes_recv = recv(connection, &reply.front(), reply.size(), 0);
//	if (bytes_recv == -1) {
//		std::cerr << "Error while receiving bytes\n";
//	}
//
//}

void cechStats(Mat org, Mat processed)
{
	int maxOtcl = 0;
	int avgSum = 0;
	int avgCount = 0;
	for (size_t r = 0; r < org.rows; r++)
	{
		for (size_t c = 0; c < org.cols; c++)
		{
			int diff = abs((int)org.at<uchar>(r, c) - (int)processed.at<uchar>(r, c));
			if (diff != 0)
			{
				avgSum += diff;
				++avgCount;
				maxOtcl = MAX(maxOtcl, diff);
			}
		}
	}

	float avg = (float)avgSum / avgCount;
}

#include "../3d/Wavelet/Operations.cpp"

cv::Mat precompressWave1(const Mat& img)
{
	Mat out = img.clone();
	int s = img.rows * img.cols;
	std::vector<double> data;
	for (size_t r = 0; r < img.rows; r++)
	{
		for (size_t c = 0; c < img.cols; c++)
		{
			data.push_back((double)img.at<uchar>(r, c));
		}
	}

	TransformStream(data.data(), s);
	int rc = 0;
	for (size_t r = 0; r < img.rows; r++)
	{
		for (size_t c = 0; c < img.cols; c++)
		{
			out.at<uchar>(r, c) = data[rc++];
		}
	}

	return out;
}

#include "../3d/stream-wave/src/wavelet_all.hpp"

cv::Mat precompressWave2(Mat& img)
{
	//main2da(img);

	//float samplerate_hz(100.);
	//float frequency_min = 0.1;
	//float frequency_max = 50.;
	//float bands_per_octave = 8;
	//wavelet::Filterbank cwt(samplerate_hz,
	//	frequency_min,
	//	frequency_max,
	//	bands_per_octave);
	//std::size_t numbands(cwt.size());

	//cwt.reset(); // Reset processing
	//for (size_t r = 0; r < img.rows; r++)
	//	for (size_t c = 0; c < img.cols; c++)
	//	{
	//		double value = img.at<uchar>(r, c);// Get data from a stream
	//		cwt.update(value);
	//		//for (unsigned int band = 0; band < numbands; band++) {
	//		//	std::cout << cwt.result_complex[band] << " ";
	//		//}
	//		//std::cout << std::endl;
	//	}
	//cwt.

	return img;
}

int intFromBytes(char* data)
{
	return *reinterpret_cast<int*>(data);
}

cv::Mat precompressWave3(const Mat& img)
{
	int headerSize = 1 + 4;
	int imgDataSize = img.rows * img.cols;
	int imgFullSize = imgDataSize + 4 + 4;
	int totalSize = headerSize + imgFullSize;

	unique_ptr<uchar[]> data;
	data.reset(new uchar[totalSize]);
	data[0] = (uchar)CN::PY_SIGN::SEND_IMG;
	memcpy(data.get() + 1, &imgFullSize, 4);
	memcpy(data.get() + 5, &img.rows, 4);
	memcpy(data.get() + 9, &img.cols, 4);
	memcpy(data.get() + 13, img.data, imgDataSize);

	CN::send(CN::_socket, (const char*)data.get(), totalSize, 0);
	data.release();

	char buffer[8192];
	memset(buffer, 0, 8192);
	int bytes_recv = 0;

	do
	{
		bytes_recv = CN::recv(CN::_socket, (char*)buffer, 9, 0);
	} while (bytes_recv <= 0);

	if (bytes_recv == -1)
		exit(-1);

	char type = buffer[0];
	int hei = intFromBytes(buffer + 1);
	int wid = intFromBytes(buffer + 5);
	//int wid = intFromBytes(buffer + 9);

	std::vector<uchar> recbuff(wid * hei, 0);

	int offset = 0;

	while (bytes_recv != 0)
	{
		bytes_recv = CN::recv(CN::_socket, (char*)buffer, 8192, 0);
		memcpy(recbuff.data() + offset, buffer, bytes_recv);
		offset += bytes_recv;
	}
	Mat reslt(hei, wid, CV_8UC1);
	for (size_t i = 0; i < recbuff.size(); i++)
	{
		int w = i % wid;
		int h = i / wid;

		reslt.at<uchar>(h, w) = recbuff[i];
	}

	return reslt;
}

cv::Mat precompressBar(const Mat& img, int len, bool onlyOneSize = true, Mat* outRet = NULL)
{
	bc::BarConstructor<uchar> bcont;
	bcont.addStructure(bc::ProcType::f0t255, bc::ColorType::gray, bc::ComponentType::Component);
	//bcont.addStructure(bc::ProcType::f255t0, bc::ColorType::gray, bc::ComponentType::Component);
	bcont.createBinaryMasks = true;
	bcont.createGraph = true;
	bcont.returnType = bc::ReturnType::barcode3d;
	// ###### PARAMS #####
	double min = 0, max = 0;
	cv::minMaxLoc(img, &min, &max);
	Mat temp = (uchar)max - img.clone();
	Bmat8 input(temp);

	bc::BarcodeCreator<uchar> test;
	auto* cont = test.createBarcode(&input, bcont);

	cv::Mat out(input.hei(), input.wid(), CV_8UC1, cv::Scalar(0));
	out = img.clone();

	for (size_t ik = 0; ik < cont->count(); ik++)
	{
		auto* item = cont->getItem(ik);
		for (auto& line : item->barlines)
		{
			bool remove = false;
			if (len == 0)
			{
				if (line->children.size() == 0)
				{
					remove = true;
				}
			}
			else
			{
				if (line->len() <= len)
				{
					remove = true;
				}
			}

			if (!remove)
				continue;

			// �������� ����� �� �����������
			auto& matr = line->matr;
			for (int k = 0; k < matr.size(); ++k)
			{
				auto& p = matr[k];
				//if (p.value > len)	continue;

				uchar& val = out.at<uchar>(p.getY(), p.getX());
				//assert(val >= p.value);
				val -= p.value;

				if (outRet)
					outRet->at<uchar>(p.getY(), p.getX()) += p.value;
			}
		}

		if (onlyOneSize)
			break;
	}

	delete cont;
	return out;
}

struct CompressRes
{
	int orgSize = 0;
	int comprSize = 0;

	int getProc()
	{
		return static_cast<int>(100.f * (float)comprSize / orgSize);
	}

	void printResult(string pred)
	{
		std::cout << pred << getProc() << "%" << " (" << comprSize << " / " << orgSize << ")" << endl;
	}
};

enum class ComprType
{
	png,
	haff,
	entropy,
	quby
};

/// PNG
size_t comprPng(const Mat& img)
{
	std::vector<uchar> buff;
	cv::imencode(".png", img, buff);
	return buff.size();
}

/// HUFFMAN

std::vector<char> getMatData(const Mat& img)
{
	int hei = img.rows;
	int wid = img.cols;
	//int wid = intFromBytes(buffer + 9);

	std::vector<char> recbuff(wid * hei);

	int offset = 0;

	for (size_t i = 0; i < hei; i++)
	{
		memcpy(recbuff.data() + offset, img.row(i).data, wid);
		offset += wid;
	}

	return recbuff;
}

// ENTROPY
struct EntropyCompression
{
	std::vector<char> data, untouched;

	void addTouched()
	{
		int untouch = untouched.size();
		if (untouch == 0)
			return;

		int sta = 0;

		char maxVal0 = 127;
		char maxVal1 = 127;
		bool doubleWord = false;
		int maxToADd = 127;
		if (untouch >= 127 * 3)
		{
			doubleWord = true;
			unsigned short mamu = USHRT_MAX;
			maxVal0 = ((char*)&mamu)[0];
			maxVal1 = ((char*)&mamu)[1];
			maxToADd = mamu;
		}

		while (untouch > maxToADd)
		{
			if (doubleWord)
			{
				data.push_back(0);
				data.push_back(maxVal0);
				data.push_back(maxVal1);
			}
			else
				data.push_back(maxVal0);

			data.insert(data.end(), untouched.begin() + sta, untouched.begin() + sta + MIN(maxToADd, untouch));
			untouch -= maxToADd;
			sta += maxToADd;
		}
		if (untouch > 0)
		{
			data.push_back(-untouch);
			data.insert(data.end(), untouched.begin() + sta, untouched.end());
		}

		untouched.clear();
	}


	size_t copress(const Mat& img)
	{
		bool oneByteSize = true;

		int s = img.rows * img.cols;

		for (size_t i = 0; i < s; i++)
		{
			int r = i / img.cols;
			int c = i % img.cols;
			uchar val = img.at<uchar>(i / img.cols, i % img.cols);
			int sd = 0;

			for (size_t k = i + 1; k < s; k++)
			{
				int rk = k / img.cols;
				int ck = k % img.cols;
				if (val == img.at<uchar>(rk, ck))
					++sd;
				else
					break;
			}

			if (sd > 2)
			{
				// ���� ���� ��� ��������
				addTouched();
				if (sd > 127 * 3)
				{
					ushort sdUsh = sd;

					while (sd > USHRT_MAX)
					{
						data.push_back(1);
						data.push_back(((char*)&sdUsh)[0]);
						data.push_back(((char*)&sdUsh)[1]);

						data.push_back(val);
						sd -= USHRT_MAX;
					}
				}
				else
				{
					while (sd > 127)
					{
						data.push_back(127);
						data.push_back(val);
						sd -= 127;
					}
				}

				if (sd > 0)
				{
					data.push_back(sd);
					data.push_back(val);
				}
				i += sd;
				//uchar testval = img.at<uchar>(i / img.cols, i % img.cols);
				//uchar testval2 = img.at<uchar>((i + 1) / img.cols, (i + 1) % img.cols);
				//testval += 0;
			}
			else
			{
				untouched.push_back(val);
			}
		}

		// �������� ������
		addTouched();


		return data.size();
	}

	void decompress(int sourceRows, int sourceCols)
	{
		// test: try decode

		int poiPoz = 0;
		Mat out(sourceRows, sourceCols, CV_8UC1);
		for (size_t i = 0; i < data.size(); i++)
		{
			char type = data[i];
			if (type > 0)
			{
				char val = data[++i];
				for (int total = poiPoz + type; poiPoz <= total; poiPoz++)
				{
					int r = poiPoz / sourceCols;
					int c = poiPoz % sourceCols;
					out.at<uchar>(r, c) = val;
				}
			}
			else
			{
				++i;
				for (int total = poiPoz + -type; poiPoz < total; ++poiPoz)
				{
					int r = poiPoz / sourceCols;
					int c = poiPoz % sourceCols;
					out.at<uchar>(r, c) = data[i++];
				}
				--i;
			}
		}

		show("test comr", out, 0);
	}
};
//
//if (oneByteSize)
//{
//	while (untouch > 127)
//	{
//		data.push_back(-127);
//		data.insert(data.end(), untouched.begin() + sta, untouched.begin() + sta + MIN(127, untouch));
//		untouch -= 127;
//		sta += 127;
//	}
//	if (untouch > 0)
//	{
//		data.push_back(-untouch);
//		data.insert(data.end(), untouched.begin() + sta, untouched.end());
//	}
//}
//else
//{
//	data.push_back(((char*)&untouch)[0]);
//	data.push_back(((char*)&untouch)[1]);
//	data.insert(data.end(), untouched.begin(), untouched.end());
//}

size_t entropy(const Mat& img)
{
	EntropyCompression compr;

	return compr.copress(img);
}

namespace huff
{
#include "../3d/Wavelet/Huffman.h"
	size_t huff(const Mat& img)
	{
		wlt_header_info out;
		std::vector<char> buff = getMatData(img);
		HuffEncode((uchar*)buff.data(), buff.size(), out);
		return buff.size();
	}
}

// union ComprInt
// {
// 	unsigned char bytes[4];
// 	uint fullval;
// };

// struct Encodebytes
// {
// 	std::vector<uchar> bytes;
// }

// #include <unordered_map>
// size_t quby(const Mat& img)
// {
// 	std::unordered_map<uint, Encodebytes> vals;

// 	for (size_t r = 0; r < img.rows - 1; r += 2)
// 	{
// 		for (size_t c = 0; c < img.cols - 1; c += 2)
// 		{
// 			ComprInt val;
// 			val.bytes[0] = img.at<uchar>(r, c);
// 			val.bytes[1] = img.at<uchar>(r, c + 1);
// 			val.bytes[2] = img.at<uchar>(r + 1, c);
// 			val.bytes[3] = img.at<uchar>(r + 1, c + 1);

// 		}
// 	}
// }

// COMMON FUNCTION

CompressRes checkCompression(const Mat& origin, const Mat& compressed, ComprType compr)
{
	CompressRes sizes;
	switch (compr)
	{
	case ComprType::png:
		sizes.orgSize = comprPng(origin);
		sizes.comprSize = comprPng(compressed);
		break;
	case ComprType::haff:
		sizes.orgSize = huff::huff(origin);
		sizes.comprSize = huff::huff(compressed);
		break;
	case ComprType::entropy:
		sizes.orgSize = entropy(origin);
		sizes.comprSize = entropy(compressed);
		break;
	case ComprType::quby:
		sizes.orgSize = entropy(origin);
		sizes.comprSize = entropy(compressed);
	default:
		break;
	}

	return sizes;
}

#include "../3d/Wavelet-Transform-2D/wt2d.h"
struct HaffCode
{
	uchar val = 0;
	int count = 0;
	uchar code = 255;
	HaffCode* par = nullptr;
};

struct resltSj
{
	uchar data[8];
	int len = 1;
};


size_t haaffman(const Mat& source, int maxLen)
{
	maxLen = 255;
	int s = source.rows * source.cols;
	HaffCode* proc = new HaffCode[maxLen];

	int maxCall = 0;
	for (size_t i = 0; i < s; i++)
	{
		int r = i / source.cols;
		int c = i % source.cols;
		uchar val = source.at<uchar>(r, c);

		++proc[val].count;

		if (val > maxCall)
		{
			maxCall = val;
		}
	}

	for (size_t i = 0; i < maxCall; i++)
	{
		//proc[i].ver /= s;
		proc[i].val = i;
	}

	sort(proc, proc + maxLen, [](const HaffCode& left, const HaffCode& right) {return left.val < right.val; });

	vector<HaffCode*> result;
	for (size_t i = 0; i < maxCall; i++)
	{
		result.push_back(&proc[i]);
	}

	std::vector<HaffCode*> stackDel;
	while (result.size() > 1)
	{
		vector<HaffCode*> resultSwitch;

		int rsize = result.size() - result.size() % 2;
		for (size_t i = 0; i < rsize; i += 2)
		{
			if (result[i]->count < result[i + 1]->count)
			{
				// 0 for more
				result[i]->code = 1;
				result[i + 1]->code = 0;
			}
			else
			{
				// 0 for more
				result[i]->code = 0;
				result[i + 1]->code = 1;
			}

			HaffCode* par = new HaffCode();
			par->count = result[i]->count + result[i + 1]->count;
			par->par = nullptr;

			stackDel.push_back(par);

			result[i]->par = par;
			result[i + 1]->par = par;
			resultSwitch.push_back(par);
		}
		if (result.size() % 2 == 1)
		{
			resultSwitch.push_back(result.back());
		}

		result = resultSwitch;
	}

	//HaffCode* proc = new Haff[maxLen];

	resltSj* resultData = new resltSj[maxLen];

	for (size_t i = 0; i < maxCall; i++)
	{
		uchar* datacode = resultData[proc[i].val].data;

		datacode[0] = proc[i].code;
		int dataInd = 1;
		HaffCode* coderef = &proc[i];
		while (coderef->par)
		{
			coderef = coderef->par;
			datacode[dataInd++] = coderef->code;
		}

		resultData[proc[i].val].len = dataInd;
	}

	vector<uchar> output;

	for (size_t i = 0; i < s; i++)
	{
		int r = i / source.cols;
		int c = i % source.cols;
		uchar val = source.at<uchar>(r, c);
		uchar* datacode = resultData[val].data;

		for (size_t l = 0; l < resultData[val].len; l++)
		{
			output.push_back(datacode[l]);
		}
	}

	for (size_t i = 0; i < stackDel.size(); i++)
	{
		delete stackDel[i];
	}

	delete[] resultData;
	delete[] proc;

	return output.size() / 8;
}

void processImage(const Mat& frame, ComprType comp)
{
	cv::namedWindow("source", cv::WINDOW_NORMAL);
	cv::imshow("source", frame);

	CN::SOCKET s = CN::connect();

	cout << "Original size: " << frame.rows * frame.cols << endl;

	Mat wavPre = precompressWave3(frame);
	CompressRes wavpng = checkCompression(frame, wavPre, comp);
	//CompressRes wavpng = checkCompression(frame, wavPre, ".png");
	cv::namedWindow("wave", cv::WINDOW_NORMAL);
	cv::imshow("wave", wavPre);
	wavpng.printResult("wave:");

	std::vector<Mat> out, result;
	cv::split(frame, out);

	vector<int> Ds{ 1, 2, 5, 10 , 15, 30};//, 30, 50 };// , 100, 150, 200, 255 };
	for (size_t j = 0; j < Ds.size(); j++)
	{
		cout << "For pre = " << Ds[j] << ": " << endl;
		//cv::imwrite("D:\\len.png", frame);

		CompressRes barpng;
		barpng.comprSize = 0;
		barpng.orgSize = 0;
		result.clear();
		for (int i = 0; i < out.size(); ++i)
		{
			Mat curFrame = out[i].clone();
			Mat outSec(curFrame.rows, curFrame.cols, CV_8UC1, cv::Scalar(0));
			Mat barPre = precompressBar(curFrame, Ds[j], false, &outSec);
			CompressRes temp = checkCompression(curFrame, barPre, comp);
			barpng.comprSize += temp.comprSize;
			barpng.orgSize += temp.orgSize;

			//CompressRes barsec = checkCompression(frame, outSec, comp);
			//barpng.comprSize += entropy(outSec);

			//show("bar", barPre, 1);
			result.push_back(barPre);
			//break;
		}

		Mat outresul;
		cv::merge(result, outresul);
		show("splitted", outresul, 1);

		barpng.printResult("bar: ");

		//barpng.printResult("(loseless): ");
		//cv::imwrite("D:\\len_compr.png", frame);
	}
	CN::closesocket(s);
	cv::waitKey(1);
}

//void compressCompire()
//{
//	cv::VideoCapture cap("chaplin.mp4");
//	// Check if camera opened successfully
//	if (!cap.isOpened()) {
//		cout << "Error opening video stream or file" << endl;
//		return;
//	}
//
//	while (1)
//	{
//		Mat frame;
//		// Capture frame-by-frame
//		cap >> frame;
//
//
//		// If the frame is empty, break immediately
//		if (frame.empty())
//			break;
//
//		processImage(frame, ComprType::png);
//		processImage(frame, ComprType::haff);
//
//		// Display the resulting frame
//		//imshow("Frame", frame);
//
//		// Press  ESC on keyboard to exit
//		char c = (char)cv::waitKey(25);
//		if (c == 27)
//			break;
//	}
//
//	// When everything done, release the video capture object
//	cap.release();
//
//	// Closes all the frames
//	//destroyAllWindows()
//
//	//VideoWriter video("outcpp.avi", CV_FOURCC('M', 'J', 'P', 'G'), 10, Size(frame_width, frame_height));;
//}

void compressMain()
{
	string ds;
	ds = "D:/Learning/BAR/220px-Lenna.png";
	ds = "D:/Learning/papers/CO4/test.png";
	ds = "D:/Learning/papers/CO4/coptic2.jpg";

	vector<string> paths{ "2.png", "3.jpg", "CAMERA.BMP", "car.png", "Coptic.jpg", "coptic2.jpg" };
	//vector<string> paths{ "coptic2.jpg" };
	//vector<string> paths{ "city.jpg", "car.png", "test.png" };
	//	"coptic3.jpg", "test.png", "test2.png", "test2s.png", "test3.png", "test4.png", "test5.png", "test6.png" };

	string basestr = "D:/Learning/papers/CO4/";
	for (size_t i = 0; i < paths.size(); i++)
	{
		string imgpath = basestr + paths[i];
		std::cout << imgpath << std::endl;

		std::cout << "---------" << paths[i] << "---------" << endl;
		Mat img = imread(imgpath, cv::IMREAD_GRAYSCALE);
		//processImage(img, ComprType::haff);
		//processImage(img, ComprType::png);
		processImage(img, ComprType::entropy);

		//break;
	}

}

// https://github.com/Cyan4973/FiniteStateEntropy
// https://github.com/wangziqi2013/BwTree

// https://github.com/drichardson/huffman

// https://github.com/isovic/wavelet-image-compression

// https://github.com/tumpji/WTlib

// https://github.com/iskay/Wavelet // bad
//https://github.com/ircam-ismm/wavelet