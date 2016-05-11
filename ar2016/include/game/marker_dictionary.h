#pragma once
#include "const.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>

using namespace cv;
using namespace std;

class MarkerDictionary
{
public:
	MarkerDictionary() {
		dictionary.markerSize = 7;
		// maximum number of bit corrections
		dictionary.maxCorrectionBits = 2;

		for (int i = 0; i < MARKER_NUM; i++) {
			// assume generateMarkerBits() generate a new marker in binary format, so that
			// markerBits is a 5x5 matrix of CV_8UC1 type, only containing 0s and 1s

			for (int x = 0, y = 6; y <= 48; x += 7, y += 7) {
				reverse(markers[i], x, y);
			}

			cv::Mat markerBits(7, 7, CV_8UC1, markers[i]);//generateMarkerBits();

			cv::Mat markerCompressed = aruco::Dictionary::getByteListFromBits(markerBits);
			// add the marker as a new row
			dictionary.bytesList.push_back(markerCompressed);
		}
	}

	void reverse(unsigned char array[], int start_index, int end_index)
	{
		int i, j;
		int temp;

		for (i = start_index, j = end_index; i < j; i++, j--)
		{
			temp = array[i];
			array[i] = array[j];
			array[j] = temp;
		}
	}

	aruco::Dictionary dictionary;
private:
	unsigned char markers[MARKER_NUM][49] = {
		{
			0,1,0,0,1,1,1,
			1,0,1,0,1,0,1,
			1,0,0,0,1,0,1,
			0,1,0,0,1,1,1,
			0,0,1,0,1,0,0,
			1,0,1,0,1,0,0,
			0,1,0,0,1,0,0
		},
		{
			0,0,1,1,1,0,0,
			0,1,0,0,0,1,0,
			0,1,0,0,0,1,0,
			0,0,0,1,1,0,0,
			0,0,0,1,0,0,0,
			0,0,0,0,0,0,0,
			0,0,0,1,0,0,0
		},
		{
			1,1,1,1,1,1,1,
			1,0,0,1,0,0,1,
			1,0,0,0,1,0,1,
			1,0,0,0,0,1,1,
			1,0,0,0,0,0,1,
			0,1,0,0,0,1,0,
			0,0,1,1,1,0,0
		},
		{
			0,0,0,0,1,1,1,
			0,0,0,1,0,0,1,
			1,0,1,0,0,1,1,
			0,1,0,0,0,1,0,
			0,0,1,0,1,0,0,
			0,1,0,1,0,0,0,
			1,0,0,0,1,0,0
		},
		{
			0,0,0,0,0,0,0,
			0,1,1,0,1,1,0,
			1,0,0,1,0,0,1,
			1,0,0,0,0,0,1,
			0,1,0,0,0,1,0,
			0,0,1,0,1,0,0,
			0,0,0,1,0,0,0
		}
	};
};