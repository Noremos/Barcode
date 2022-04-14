#pragma once
#undef _HAS_STD_BYTE

#include "prep.h"

#undef max
// Client side C/C++ program to demonstrate Socket programming
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment (lib, "Ws2_32.lib")
#include <iostream>
//#include <string.h>
#include <sstream>
#include <WinSock2.h>
#include <WS2tcpip.h>
//using namespace std;
using std::cout;
using std::cin;

SOCKET _socket;

enum class PY_SIGN : uchar
{
	SEND_IMG = 0,

};

SOCKET connect()
{
	WSADATA WsaData;
	bool err =  WSAStartup(MAKEWORD(2, 2), &WsaData) == NO_ERROR;

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
	data[0] = (uchar)PY_SIGN::SEND_IMG;
 	memcpy(data.get() + 1, &imgFullSize, 4);
	memcpy(data.get() + 5, &img.rows, 4);
	memcpy(data.get() + 9, &img.cols, 4);
	memcpy(data.get() + 13, img.data, imgDataSize);

	send(_socket, (const char*)data.get(), totalSize, 0);
	data.release();

	char buffer[8192];
	memset(buffer, 0, 8192);
	int bytes_recv = 0;
	
	do
	{
		bytes_recv = recv(_socket, (char*)buffer, 9, 0);
	}
	while (bytes_recv <= 0);

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
		bytes_recv = recv(_socket, (char*)buffer, 8192, 0);
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

cv::Mat precompressBar(const Mat& img, int len, bool onlyOneSize = true)
{
	bc::BarConstructor<uchar> bcont;
	bcont.addStructure(bc::ProcType::f0t255, bc::ColorType::gray, bc::ComponentType::Component);
	//bcont.addStructure(bc::ProcType::f255t0, bc::ColorType::gray, bc::ComponentType::Component);
	bcont.createBinaryMasks = true;
	bcont.createGraph = false;
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
			if (line->len() <= len)
			{
				auto& matr = line->matr;
				for (int k = 0; k < matr.size(); ++k)
				{
					auto& p = matr[k];
					uchar& val = out.at<uchar>(p.getY(), p.getX());
					//assert(val >= p.value);
					val -= p.value;
				}
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
		std::cout << pred << getProc() << "%" << endl;
	}
};

enum class ComprType
{
	png,
	haff
};

/// PNG
size_t comprPng(const Mat& img)
{
	std::vector<uchar> buff;
	cv::imencode(".jpg", img, buff);
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

namespace huff
{
#include "../3d/Wavelet/Huffman.h"
	size_t huff(const Mat& img)
	{
		wlt_header_info out;
		std::vector<char> buff = getMatData(img);
		HuffEncode((UCHAR*)buff.data(), buff.size(), out);
		return buff.size();
	}
}

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
	default:
		break;
	}
	
	return sizes;
}

#include "../3d/Wavelet-Transform-2D/wt2d.h"

void processImage(const Mat& frame, ComprType comp)
{
	cv::namedWindow("source", cv::WINDOW_NORMAL);
	cv::imshow("source", frame);

	SOCKET s = connect();

	Mat wavPre = precompressWave3(frame);
	CompressRes wavpng = checkCompression(frame, wavPre,comp);
	//CompressRes wavpng = checkCompression(frame, wavPre, ".png");
	cv::namedWindow("wave", cv::WINDOW_NORMAL);
	cv::imshow("wave", wavPre);
	wavpng.printResult("wave:");

	vector<int> Ds{ 1, 2, 5, 10, 15, 30 };
	for (size_t j = 0; j < Ds.size(); j++)
	{
		Mat barPre = precompressBar(frame, Ds[j], false);

		CompressRes barpng = checkCompression(frame, barPre, comp);
		cv::namedWindow("bar", cv::WINDOW_NORMAL);
		cv::imshow("bar", barPre);
		cv::waitKey(1);
		barpng.printResult("bar:");
	}
	closesocket(s);
	cv::waitKey(0);
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
	//	"coptic3.jpg", "test.png", "test2.png", "test2s.png", "test3.png", "test4.png", "test5.png", "test6.png" };

	string basestr = "D:/Learning/papers/CO4/";
	for (size_t i = 0; i < paths.size(); i++)
	{
		string imgpath = basestr + paths[i];
		std::cout << imgpath << std::endl;

		std::cout << "---------" << paths[i] << "---------" << endl;
		Mat img = imread(imgpath, cv::IMREAD_GRAYSCALE);
		//processImage(img, ComprType::haff);
		processImage(img, ComprType::png);

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