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
Mat jpegROI(string path, int levelDim, double roiX, double roiY, double roiW, double roiH);
Mat openSlide_ROI(string path, int levelDim, double roiX, double roiY, double roiW, double roiH);

void saveImage(string path, Mat outImg);

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
	level = level - 1;
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
		char response;
		cin >> response;
		cout << endl;

		if (response == 'Y' || response == 'y')
		{

		}
		else if (response == 'N' || response == 'n')
		{
			std::string levelStr;//"4";
			std::cout << "Enter Level Index [0-" << totalLevels - 1 << "]" << endl;
			cin >> levelStr;
			level = atoi(levelStr.c_str());
			if (level<0 && level>totalLevels)
			{
				cout << "Enter Valid Level Index" << endl;
				waitKey();
				return -1;
			}
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
		std::string levelStr;//"4";
		std::cout << "Enter Level Index [0-" << totalLevels - 1 << "]" << endl;
		cin >> levelStr;
		level = atoi(levelStr.c_str());
		if (level<0 && level>totalLevels)
		{
			cout << "Enter Valid Level Index" << endl;
			waitKey();
			return -1;
		}
	}

	

	

	int64_t w, h;
	openslide_get_level_dimensions(osr, level, &w, &h);
	int x, y;
	x = (int)w;		y = (int)h;
	int xmlWidth = x;  int xmlHeight = y;
	levDimen.push_back(Point(x, y));

	// Count Tiles on Each Level
	int tempX, tempY;
	//level = 0;
	
	while (x > 256 || y >256)
	{
		double c = ceil((double)x / 256.0f);
		tempX = (int)ceil((double)x / 256.0f);
		tempY = (int)ceil((double)y / 256.0f);
		if (x >= 0 && y >= 0)
		{
			tilesLevel.push_back(tempX*tempY);
		}
		
		level = level + 1;

		openslide_get_level_dimensions(osr, level, &w, &h);
		x = (int)w;		y = (int)h;

		if (x >= 0 && y >= 0)
		{
			levDimen.push_back(Point(x, y));
		}
		
	}
	reverse(tilesLevel.begin(), tilesLevel.end());
	reverse(levDimen.begin(), levDimen.end());


	// Create Folder with image name
	std::size_t pos = imgPath.find(".");
	std::string dirPath = imgPath.substr(0, pos);
	CreateDirectory(dirPath.c_str(), NULL);

	// Count Total Tiles
	int totalTiles = 0;
	totalTiles = std::accumulate(tilesLevel.begin(), tilesLevel.end(), 0) + 1; // +1 for self generated overview image

	// Create Tiles Directories
	std::vector<string> dirNames;
	int totalDir = (int)(ceil((double)totalTiles / 256.0f));
	for (int i = 0; i<totalDir; i++)
	{
		std::stringstream ss;
		ss << dirPath << "//TileGroup" << i;
		CreateDirectory((ss.str()).c_str(), NULL);
	}

	// Write XML file
	ofstream myfile;
	myfile.open(dirPath + "//ImageProperties.xml");
	stringstream xmlText;
	xmlText << "<IMAGE_PROPERTIES WIDTH=\" " << xmlWidth << "\" " << "HEIGHT=\"" << xmlHeight << "\" " << "NUMTILES=\"" << totalTiles << "\" " << "NUMIMAGES=\"1\" VERSION=\"1.8\" TILESIZE=\"256\" />";
	myfile << xmlText.str();
	myfile.close();


	// Level 0 image 
	Mat lev0_img = openSlide_ROI(imgPath.c_str(), (double)totalLevels-1, 0, 0, levDimen[0].x, levDimen[0].y);
	Mat dst;
	resize(lev0_img, dst, Size(lev0_img.cols / 4, lev0_img.rows / 4), 0.0, 0.0, 1);
	imshow("1", dst);
	waitKey();
	std::stringstream ss;
	ss << dirPath << "\\TileGroup0" << "\\0-0-0" << ".jpg";
	imwrite(ss.str(), dst);

	bool tiles_gen_flag;
	tiles_gen_flag  = generatingTiles(imgPath, dirPath, level, w, h);

	tiles_gen_flag = generatingTilesForMissingLevels(imgPath, dirPath, level, w, h);

	
	
	

		/*kdu_compressed_source *input = NULL;
		jp2_family_src jp2_ultimate_src;
		jp2_source jp2_in;

		input = &jp2_in;
		//jp2_ultimate_src.open(argv[1]);
		string imgPath;//"E://Research//Qatar_University//openSlide//images//10972IH-11_A1H&E_1.jp2";
		std::cout<<"Drag Input Image"<<endl;
		cin>> imgPath;
		jp2_ultimate_src.open(imgPath.c_str());
		if (!jp2_in.open(&jp2_ultimate_src))
			{     
				//kdu_error e; e << "Supplied input file, \"" << argv[1] << "\", does "
			  //"not appear to contain any boxes."; 
		}
		jp2_in.read_header();

		kdu_codestream codestream;
		codestream.create(input);

		///std::string levelStr =  argv[3];
		std::string levelStr;//"4";
		std::cout<<"Enter Level Index [0-9]"<<endl;
		cin>> levelStr;
		int level = atoi(levelStr.c_str());
		if (level<0 && level>9)
		{
			cout<<"Enter Valid Level Index"<<endl;
			waitKey();
			return -1;
		}

		codestream.apply_input_restrictions(0,0,
										  level,0,NULL,
										  KDU_WANT_OUTPUT_COMPONENTS);
		kdu_dims dims; codestream.get_dims(0,dims,true);
		int x =  dims.size.x, y=  dims.size.y;
		int xmlWidth =  x ;  int xmlHeight =  y;
		levDimen.push_back(Point(x, y));

		// Count Tiles on Each Level
		int tempX, tempY;
		while (x > 256 || y >256)
		{
			double c =  ceil((double)x/256.0f);
			tempX = (int)ceil((double)x/256.0f);
			tempY = (int)ceil((double)y/256.0f);
			tilesLevel.push_back(tempX*tempY);
			level = level + 1;
			codestream.apply_input_restrictions(0,0,
										  level,0,NULL,
										  KDU_WANT_OUTPUT_COMPONENTS);
			codestream.get_dims(0,dims,true);
			x = dims.size.x; y = dims.size.y;
			levDimen.push_back(Point(x, y));
		}
		tilesLevel.push_back(1);
		reverse(tilesLevel.begin(),tilesLevel.end()); 
		reverse(levDimen.begin(), levDimen.end());

		// Create Folder with image name
		std::size_t pos = imgPath.find(".jp2");
		std::string dirPath = imgPath.substr (0, pos);  
		CreateDirectory(dirPath.c_str(), NULL);

		// Count Total Tiles
		int totalTiles=0;
		totalTiles =std::accumulate(tilesLevel.begin(),tilesLevel.end(),0);

		// Create Tiles Directories
		std::vector<string> dirNames;
		int totalDir =  (int) (ceil((double)totalTiles/256.0f));
		for (int i=0; i<totalDir; i++)
		{
				 std::stringstream ss;
				 ss << dirPath << "//TileGroup" << i;
				 CreateDirectory((ss.str()).c_str(), NULL);
		}

		// Write XML file
		ofstream myfile;
		myfile.open (dirPath + "//ImageProperties.xml");
		stringstream xmlText;
		xmlText<<"<IMAGE_PROPERTIES WIDTH=\" " << xmlWidth <<"\" "<<"HEIGHT=\""<<xmlHeight<<"\" "<<"NUMTILES=\""<<totalTiles<<"\" "<<"NUMIMAGES=\"1\" VERSION=\"1.8\" TILESIZE=\"256\" />";
		myfile << xmlText.str();
		myfile.close();


		double tileX= 0.0f, tileY=0.0f; int dirIndex=0;
		for (int i=0; i<tilesLevel.size(); i++)
		{
			saveImgLev =  i;
			codestream.apply_input_restrictions(0,0,
										  level,0,NULL,
										  KDU_WANT_OUTPUT_COMPONENTS);
			codestream.get_dims(0,dims,true);
			x = dims.size.x; y = dims.size.y;

			for (int j =0; j<(int)ceil((double)y/256.0f); j++)
			{
				saveImgRow = j;
				for (int k=0; k<(int)ceil((double)x/256.0f); k++)
				{
					saveImgCol = k;
					Mat roiImg = jpegROI(imgPath, level , tileX, tileY, 256.0f, 256.0f);
					tileX = 1.0f - (((double)x - (256.0f*((double)k+1.0f)))/(double)x);	
					if (tileX >= 1)
						tileX =0;
					//saveImage(dirPath, roiImg);

					std::stringstream ss;
					ss << dirPath << "//TileGroup" << dirIndex << "//" << saveImgLev<<"-"<<saveImgCol<<"-"<<saveImgRow<<".jpg";
					imwrite(ss.str(),roiImg); 
					saveImgCounter = saveImgCounter +1;
					if (saveImgCounter%256 ==0)
						dirIndex = dirIndex +1;


				}
				
				tileY = 1.0f - (((double)y - (256.0f*((double)j+1.0f)))/(double)y);
				tileX=0.0f;
			}
			level =  level -1;
			tileX= 0.0f; tileY=0.0f;
		}
				
		
		*/

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

void saveImage(string path, Mat outImg)
{
	if (tilesLevel[saveImgLev] + saveImgCounter < 256)
	{
		std::stringstream ss;
		ss << path << "//TileGroup" << saveImgCounter/256 << "//" << saveImgLev<<"-"<<saveImgCol<<"-"<<saveImgRow<<".jpg";
		imwrite(ss.str(), outImg);
		saveImgCounter = saveImgCounter + 1;
		currLev = saveImgLev;

	}
	else
	{
		if (currLev ==  saveImgLev )
		{
			std::stringstream ss, ss1;
			for (int i=0; i<limit.size(); i++)
			{
				if (saveImgRow < limit[i])
				{
					if (saveImgLev == 7 &&  saveImgRow ==  78)
						int a=1;
					ss << path << "//TileGroup" << tiles[i] << "//" << saveImgLev<<"-"<<saveImgCol<<"-"<<saveImgRow<<".jpg";
					imwrite(ss.str(), outImg);
					saveImgCounter = saveImgCounter + 1;
					if (i>0)
					{
						ss1 << path << "//TileGroup" << tiles[i-1] << "//" << saveImgLev<<"-"<<saveImgCol<<"-"<<saveImgRow<<".jpg";
						imwrite(ss1.str(), outImg);
					}
					break;
				}
				/*else if (rewards[i] > 0 && tempCount[i] < rewards[i])
				{
					if (rewardCount == tempC-1)
						rewardCount = 0;
					else if (saveImgCol ==  rewardCount)
					{
						ss << path << "//TileGroup" << tiles[i] << "//" << saveImgLev<<"-"<<saveImgCol<<"-"<<saveImgRow<<".jpg";
						imwrite(ss.str(), outImg);
						saveImgCounter = saveImgCounter + 1;
						rewardCount =  rewardCount +1;
						tempCount[i] =  tempCount[i]+1;
						break;
					}
				}*/
			
			}



		}
		else
		{
			limit.clear();
			tiles.clear();
			rewards.clear();
			rewardCount = 0;tempCount.clear();
			currLev = saveImgLev;
			int diff =  256- (saveImgCounter%256);
			int levTiles = tilesLevel[saveImgLev] ;
			Point tempPt =  levDimen[saveImgLev];
			tempC =  ceil((double)tempPt.x/256.0f); int tempR =  ceil((double)tempPt.y/256.0f);
			tileLim  = diff/tempC; 
			tileRew = diff%tempC;
			int tileGr =saveImgCounter/256 ;
			int tileLimT=tileLim;

			while(levTiles > 0 )
			{
				tileGr = tileGr +1;
				rewards.push_back(tileRew);
				limit.push_back(tileLim);
				tiles.push_back(tileGr);
				tempCount.push_back(0);
				levTiles = levTiles-  tileLimT*tempC;
				//levTiles = levTiles-  256;
				if (levTiles > 256)
				{
					tileLim =  256 /tempC +  tileLim;
					tileLimT = 256 /tempC ;
				}
				else
				{tileLim =  levTiles / tempC + tileLim;tileLimT = 256 /tempC ;}
				tileRew =  256 - tileLimT*tempC;

			}

			int sumTiles = 0;
			tiles.clear();
			for (int i=0; i<currLev; i++)
			{
				sumTiles =  sumTiles + tilesLevel[i];
			}
			int maxL =  sumTiles +  tilesLevel[currLev];
			maxL =  ceil((double)maxL / 256.0f);
			int minL = sumTiles / 256;
			for (int i=minL; i<maxL; i++)
				tiles.push_back(i);

			rewards.at(rewards.size()-1) =0;
			std::stringstream ss;
			ss << path << "//TileGroup" << saveImgCounter/256 << "//" << saveImgLev<<"-"<<saveImgCol<<"-"<<saveImgRow<<".jpg";
			imwrite(ss.str(), outImg);
			saveImgCounter = saveImgCounter + 1;
		}

	}


}















