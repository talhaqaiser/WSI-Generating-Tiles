// testKakadu.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <fstream>
#include <jp2.h>
#include "testKakadu.h"
#include <vector>
#include <math.h>
#include <numeric>
#include <sstream>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <openslide.h>
#include <math.h>


using namespace kdu_supp;
using namespace std;
using namespace cv;

int saveImgLev=0, saveImgRow = 0, saveImgCol = 0, saveImgCounter=1;
int currLev = 0; int tileLim; int tileRew; int rewardCount =0;
vector<int> limit; vector<int> tiles; vector<int> rewards;
std::vector<float> tilesLevel;
std::vector<Point> levDimen;
vector<int> tempCount; int tempC;

bool flag_generate_missing_tiles = false;

openslide_t * osr;

Mat BGR2RGB(Mat input)
{
	Mat output; 
	vector<Mat> rgbChannels(3);
	split(input, rgbChannels);
	vector<Mat> channels;
	channels.push_back(rgbChannels[2]);
	channels.push_back(rgbChannels[1]);
	channels.push_back(rgbChannels[0]);
	merge(channels, output);
	return output;
}

Mat openSlide_ROI(string path, int levelDim, double roiX, double roiY, double roiW, double roiH);

void assignLabel(int levTiles, vector<int> tilesDir, int sImg, int RLim, int CLim)
{
	levTiles =  288; tilesDir.push_back(0); tilesDir.push_back(1);
	sImg =  99; RLim = 24; CLim = 12;
	vector<Point> tilesIndex; vector<int> labels;

	int diff =  256 - (sImg%256);
	int tRlim  = diff / CLim;
	int rew =  tRlim * CLim  +  sImg;
	int k =  tiles[0];

	while(levTiles >0)
	{
		for (int i=0; i< CLim; i++){
			for (int j=0; j<tRlim; j++)
			{
				tilesIndex.push_back(Point(i, j));
				labels.push_back(tiles[k]);
				sImg = sImg +1;
			}
		}
		
	}


}

bool generatingTiles(string imgPath,string dirPath, int level, int64_t w, int64_t h)
{
	int x, y; 
	double ds;
	// Generating Tile  
	//level = level - 1;
	double tileX = 0.0f, tileY = 0.0f; int dirIndex = 0;
	for (int i = 0; i < tilesLevel.size(); i++)
	{
		saveImgLev = i + 1;
		openslide_get_level_dimensions(osr, level, &w, &h);
		x = (int)w;		y = (int)h;

		for (int j = 0; j < (int)ceil((double)y / 256.0f); j++)
		{
			saveImgRow = j;
			for (int k = 0; k < (int)ceil((double)x / 256.0f); k++)
			{
				saveImgCol = k;
				ds = openslide_get_level_downsample(osr, level);
				Mat roiImg = openSlide_ROI(imgPath.c_str(), level, tileX, tileY, 256.0f, 256.0f);

				tileX = 1.0f - (((double)x - (256.0f*((double)k + 1.0f))) / (double)x);
				tileX = (tileX *  (double)x)*ds;

				if (tileX >= x*ds)
					tileX = 0;
				//saveImage(dirPath, roiImg);

				/*imshow("1", roiImg);
				waitKey();*/


				std::stringstream ss;
				ss << dirPath << "\\TileGroup" << dirIndex << "\\" << saveImgLev << "-" << saveImgCol << "-" << saveImgRow << ".jpg";
				imwrite(ss.str(), roiImg);
				saveImgCounter = saveImgCounter + 1;
				if (saveImgCounter % 256 == 0)
					dirIndex = dirIndex + 1;
			}

			tileY = 1.0f - (((double)y - (256.0f*((double)j + 1.0f))) / (double)y);
			tileY = (tileY *  (double)y)*ds;
			tileX = 0.0f;
		}

		level = level - 1;
		tileX = 0.0f; tileY = 0.0f;

	}
	return true; // add a scheck on save tile status 
}

bool generatingTilesForMissingLevels(string imgPath, string dirPath, int level, int64_t w, int64_t h)
{
	double tileW = 512.0f;
	double tileH = 512.0f;
	int x, y;
	double ds;
	// Generating Tile  
	level = level - 1;
	double tileX = 0.0f, tileY = 0.0f; int dirIndex = 0;
	for (int i = 0; i < tilesLevel.size(); i++)
	{
		saveImgLev = i + 1;
		openslide_get_level_dimensions(osr, level, &w, &h);
		x = (int)w;		y = (int)h;

		for (int j = 0; j < (int)ceil((double)y / tileW); j++)
		{
			saveImgRow = j;
			for (int k = 0; k < (int)ceil((double)x / tileH); k++)
			{
				saveImgCol = k;
				ds = openslide_get_level_downsample(osr, level);
				Mat roiImg = openSlide_ROI(imgPath.c_str(), level, tileX, tileY, tileW, tileH);

				tileX = 1.0f - (((double)x - (tileW*((double)k + 1.0f))) / (double)x);
				tileX = (tileX *  (double)x)*ds;

				if (tileX >= x*ds)
					tileX = 0;
				//saveImage(dirPath, roiImg);

				/*imshow("1", roiImg);
				waitKey();*/


				std::stringstream ss;
				ss << dirPath << "\\TileGroup" << dirIndex << "\\" << saveImgLev << "-" << saveImgCol << "-" << saveImgRow << ".jpg";
				imwrite(ss.str(), roiImg);
				saveImgCounter = saveImgCounter + 1;
				if (saveImgCounter % 256 == 0)
					dirIndex = dirIndex + 1;
			}

			tileY = 1.0f - (((double)y - (tileH*((double)j + 1.0f))) / (double)y);
			tileY = (tileY *  (double)y)*ds;
			tileX = 0.0f;
		}

		level = level - 1;
		tileX = 0.0f; tileY = 0.0f;
	}
	return true;
}

bool writeOverviewImage(string imgPath, string dirPath, int totalLevels)
{
	Mat lev0_img = openSlide_ROI(imgPath.c_str(), (double)totalLevels - 1, 0, 0, levDimen[0].x, levDimen[0].y);
	Mat dst;
	resize(lev0_img, dst, Size(lev0_img.cols / 4, lev0_img.rows / 4), 0.0, 0.0, 1);
	imshow("1", dst);
	waitKey();
	std::stringstream ss;
	ss << dirPath << "\\TileGroup0" << "\\0-0-0" << ".jpg";
	imwrite(ss.str(), dst);
	return true;
} 

bool writeXMLFile(string dirPath, int xmlWidth, int xmlHeight, int totalTiles)
{	
	ofstream myfile;
	myfile.open(dirPath + "//ImageProperties.xml");
	stringstream xmlText;
	xmlText << "<IMAGE_PROPERTIES WIDTH=\" " << xmlWidth << "\" " << "HEIGHT=\"" << xmlHeight << "\" " << "NUMTILES=\"" << totalTiles << "\" " << "NUMIMAGES=\"1\" VERSION=\"1.8\" TILESIZE=\"256\" />";
	myfile << xmlText.str();
	myfile.close();
	return true;
}

string createTilesDirectories(string imgPath, int totalTiles)
{
	std::size_t pos = imgPath.find(".");
	std::string dirPath = imgPath.substr(0, pos);
	CreateDirectory(dirPath.c_str(), NULL);

	std::vector<string> dirNames;
	int totalDir = (int)(ceil((double)totalTiles / 256.0f));
	for (int i = 0; i<totalDir; i++)
	{
		std::stringstream ss;
		ss << dirPath << "//TileGroup" << i;
		CreateDirectory((ss.str()).c_str(), NULL);
	}

	return dirPath;
}

bool countTilesAtEachResolution(int level, int &xmlWidth, int &xmlHeight, int64_t &w, int64_t &h)
{
	double ds;
	openslide_get_level_dimensions(osr, level, &w, &h);
	int x, y;
	x = (int)w;		y = (int)h;
	xmlWidth = x;	xmlHeight = y;
	double ds1, ds2;
	ds1 = round(openslide_get_level_downsample(osr, level));
	//levDimen.push_back(Point(x, y));

	// Count Tiles on Each Level
	int tempX, tempY;
	//level = 0;

	while (x > 256 || y >256)
	{
		openslide_get_level_dimensions(osr, level, &w, &h);
		x = (int)w;		y = (int)h;
		double c = ceil((double)x / 256.0f);
		tempX = (int)ceil((double)x / 256.0f);
		tempY = (int)ceil((double)y / 256.0f);
		if (x >= 0 && y >= 0)
		{
			tilesLevel.push_back(tempX*tempY);
			levDimen.push_back(Point(x, y));
			level = level + 1;
			ds2 = round(openslide_get_level_downsample(osr, level));
			

			if (flag_generate_missing_tiles ==  true &&  (ds2/ds1 != 2))
			{
				x = (int)w/2;		y = (int)h/2;
				tempX = (int)ceil((double)x / 256.0f);
				tempY = (int)ceil((double)y / 256.0f);
				tilesLevel.push_back((tempX*tempY));
				levDimen.push_back(Point(x, y));
			}
			ds1 = ds2;
		}
	}

	reverse(tilesLevel.begin(), tilesLevel.end());
	reverse(levDimen.begin(), levDimen.end());
	return true;
}




int main(int argc)
{		
	string imgPath;
	std::cout << "Drag Input Image" << endl;
	cin >> imgPath;

	osr = openslide_open(imgPath.c_str());
	int32_t lev = openslide_get_level_count(osr);
	int totalLevels = (int)lev, highest_res =  40, level;
	double ds;

	if (totalLevels <= 5)
	{
		std::cout << "Some resolution levels are missing from provided WSI"<<endl;
		std::cout << "\nThe available resolution levels are..." << endl;
		for (int i = 0; i < totalLevels; i++)
		{
			ds = openslide_get_level_downsample(osr, i);
			std::cout << highest_res / round(ds) << "X with down sampling factor of " << round(ds) << endl;
		}
		
		std::cout << "\nBy Assuming the highest avialable resolution is 40X" << endl;

		std::cout << "Would you like to generate the missing tiles? (Y/N)?" <<endl;
		char response = '\0';
		cin >> response;
		cout << endl;

		if (response == 'Y' || response == 'y')
		{
			flag_generate_missing_tiles = true;
		}
		else if (response == 'N' || response == 'n')
		{
			flag_generate_missing_tiles = false;
		}
		else
		{
			std::cout << "Invalid Response!"<<endl;
			system("pause");
			return 0;

		}

	}
	else
	{
		flag_generate_missing_tiles = false;
	}


	//-----------------------------------------------------------------------------------------------------------------------------------------
	switch (flag_generate_missing_tiles)
	{
	case 0:
	{
		std::string levelStr;
		std::cout << "Select Resolution Level [0-" << totalLevels - 1 << "]" << endl;
		cin >> levelStr;
		level = atoi(levelStr.c_str());
		if (level<0 && level>totalLevels)
		{
			cout << "Enter Valid Level Index" << endl;
			waitKey();
			return -1;
		}

		// Count Tiles on Each Resolution Level
		int xmlWidth; int xmlHeight;
		int64_t w, h;
		bool flag_count_tiles = countTilesAtEachResolution(level, xmlWidth, xmlHeight, w, h);

		// Count Total Tiles
		int totalTiles = 0;
		totalTiles = std::accumulate(tilesLevel.begin(), tilesLevel.end(), 0) + 1; // +1 for self generated overview image

		// Create Folder with Image Name and Create Tiles Directories
		string dirPath = createTilesDirectories(imgPath, totalTiles);

		// Write XML file
		bool flag_write_xml = writeXMLFile(dirPath, xmlWidth, xmlHeight, totalTiles);

		// Low Level Overview image 
		bool flag_write_ovImg = writeOverviewImage(imgPath, dirPath, totalLevels);

		// Generate Tiles for Specified Resolution Level 
		bool flag_tiles_gen;
		flag_tiles_gen = generatingTiles(imgPath, dirPath, level, w, h);
	}
	case 1:
	{
		std::string levelStr;
		std::cout << "Select Resolution Level [0-8]" << endl;
		cin >> levelStr;
		level = atoi(levelStr.c_str());
		if (level<0 && level>totalLevels)
		{
			cout << "Enter Valid Level Index" << endl;
			waitKey();
			return -1;
		}

		// Count Tiles on Each Resolution Level
		int xmlWidth; int xmlHeight;
		int64_t w, h;
		bool flag_count_tiles = countTilesAtEachResolution(level, xmlWidth, xmlHeight, w, h);

		// add remaining tiles

		// Count Total Tiles
		int totalTiles = 0;
		totalTiles = std::accumulate(tilesLevel.begin(), tilesLevel.end(), 0) + 1; // +1 for self generated overview image

		// Create Folder with Image Name and Create Tiles Directories
		string dirPath = createTilesDirectories(imgPath, totalTiles);

		// Low Level Overview image 
		bool flag_write_ovImg = writeOverviewImage(imgPath, dirPath, totalLevels);

	}
	default:
		break;
	}
	
	//flag_tiles_gen = generatingTilesForMissingLevels(imgPath, dirPath, level, w, h);

	
	system("pause");

	return 0;
}

 
Mat openSlide_ROI(string path, int levelDim, double roiX, double roiY, double roiW, double roiH)
{
	Mat roiImg;
	roiImg.create(roiH, roiW, CV_8UC3);

	uint32_t *dest = (uint32_t *)malloc(roiW * 4);
	int32_t lines_to_draw = roiH;
	double ds = openslide_get_level_downsample(osr, levelDim);
	int32_t yy = roiY / ds;
	int colInd = 0, rowInd = 0;
	while (lines_to_draw) {
		openslide_read_region(osr, dest,
			roiX, yy * ds, levelDim, roiW, 1);

		const char *err = openslide_get_error(osr);
		if (err) {
			//fail("%s", err);
		}

		// un-premultiply alpha and pack into expected format
		for (int i = 0; i < roiW; i++) {
			colInd = colInd + 1;
			uint32_t p = dest[i];
			uint8_t *p8 = (uint8_t *)(dest + i);

			uint8_t a = (p >> 24) & 0xFF;
			uint8_t r = (p >> 16) & 0xFF;
			uint8_t g = (p >> 8) & 0xFF;
			uint8_t b = p & 0xFF;

			switch (a) {
			case 0:
				r = 0;
				b = 0;
				g = 0;
				break;

			case 255:
				// no action
				break;

			default:
				r = (r * 255 + a / 2) / a;
				g = (g * 255 + a / 2) / a;
				b = (b * 255 + a / 2) / a;
				break;
			}

			// write back


			roiImg.data[roiImg.channels()*(roiImg.cols*rowInd + colInd) + 0] = b;
			roiImg.data[roiImg.channels()*(roiImg.cols*rowInd + colInd) + 1] = g;
			roiImg.data[roiImg.channels()*(roiImg.cols*rowInd + colInd) + 2] = r;

		}
		colInd = 0;
		rowInd = rowInd + 1;
		//png_write_row(png_ptr, (png_bytep) dest);
		yy++;
		//y++;
		lines_to_draw--;
		//if (lines_to_draw%10==0)
		//std::cout<<lines_to_draw<<endl;
	}

	// end
	free(dest);
	//resize(outImg, downSample, cv::Size(1280, 960));
	//imshow("win", roiImg);
	//waitKey();
	return roiImg;
}















