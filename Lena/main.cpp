//Brighter the image.
//To make a image brighter.
//Author:ShadowK
//email:zhu.shadowk@gmail.com
//2015.10.13
//Use Ctrl+M,Ctrl+O to fold the code.

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <cxcore.h>
#include <cv.h>
#include <highgui.h>
#include <vector>
#include <conio.h>
#include <Windows.h>
using namespace std;
using namespace cv;


struct Dthread
{
	IplImage *p;
	bool stop;
	string name;
};

//Compare two strings.(Case insensitive)
bool compare(const string& x, const string& y)
{
	string::const_iterator p = x.begin();
	string::const_iterator q = y.begin();
	while (p != x.end() && q != y.end() && toupper(*p) == toupper(*q))
	{
		++p;
		++q;
	}
	if (p == x.end())
	{
		return (q == y.end());
	}
	if (q == y.end())
	{
		return false;
	}
	return false;
}

//For split string.
std::vector<std::string> split(std::string str, std::string pattern)
{
	int pos;
	std::vector<std::string> result;
	str += pattern;
	int size = str.size();

	for (int i = 0; i<size; i++)
	{
		pos = str.find(pattern, i);
		if (pos<size)
		{
			std::string s = str.substr(i, pos - i);
			result.push_back(s);
			i = pos + pattern.size() - 1;
		}
	}
	return result;
}

//For displaying the image.
DWORD WINAPI Displayimg(LPVOID lpParamter)
{
	Dthread *dp = (Dthread *) lpParamter;
	IplImage *pth = dp->p;
	while (dp->stop == FALSE)
	{
		cvShowImage(dp->name.c_str(), pth);
		cvWaitKey(1000);
	}
	cvDestroyAllWindows();
	return 0;
}

//Add brightness and change the cotrast of the image.
void Change_br_and_co(int br, double co, IplImage *p)
{
	for (int y = 0; y < p->height; y++)
	{
		for (int x = 0; x < p->width; x++)
		{
			for (int k = 0; k < p->nChannels; k++)
				((uchar *)(p->imageData + x*p->widthStep))[y * p->nChannels + k] = saturate_cast<uchar>(co * ((uchar *)(p->imageData + x*p->widthStep))[y * p->nChannels + k] + br);
		}
	}
}

//Do the EqualizeHist algorithm to the image.
//Only support Channels 1
void EqualizeHist(IplImage *p)
{
	int L[256] = { 0 };
	for (int y = 0; y < p->height; y++)
	{
		for (int x = 0; x < p->width; x++)
		{
			L[((uchar *)(p->imageData + x*p->widthStep))[y * p->nChannels + 0]]++;
		}
	}
	for (int i = 1; i < 256; i++)
	{
		L[i] += L[i - 1];
	}
	int N = p->width * p->height;
	for (int y = 0; y < p->height; y++)
	{
		for (int x = 0; x < p->width; x++)
		{
			((uchar *)(p->imageData + x*p->widthStep))[y * p->nChannels + 0] = int(255 * (double)L[((uchar *)(p->imageData + x*p->widthStep))[y * p->nChannels + 0]] / N + 0.5);
		}
	}
	return;
}
int main()
{
	IplImage *image = NULL;
	Dthread d;
	HANDLE hThread;
	d.name = "Your image";
	d.p = image;
	d.stop = FALSE;
	bool Show = FALSE;
	while (1)
	{
		printf("Input command.Input \"help\" for command help.\n$");
		string str;
		getline(cin, str);
		vector<string> splitstr = split(str, " ");
		try
		{
			if (compare(splitstr[0], "help"))
			{
				printf("\"load <FileName>\" Load or reload a image with full location.\n");
				printf("Sample: load C:\\Users\\WIN_20150404_144352.JPG\n");
				printf("\"bright\" Use my algorithm to change the brightness of the picture.Only image with 1 channel supported.\n");
				printf("Sample:bright\n");
				printf("\"bright_and_contrast <int> <double>\" Normally both change the brightness and the contrast of the picture.\n");
				printf("Sample:bright_and_contrast 5 1.2\n");
				printf("\"display\"Display the image.\n");
				printf("Sameple: display\n");
				printf("\"hide\"Stop displaying the image.\n");
				printf("Sameple: hide\n");
				printf("\"save <FileName>\"Save the image.Input the Name without the path to save it at the location of the program.\n");
				printf("Sameple: save C:\\Users\\WIN_20150404_144352.JPG\n");
				printf("\"exit\"Exit the program\n");
				printf("Sameple: exit\n");
				printf("WARNING:Do not use image which has spaces in its FileName.\n");
			}
			else if (compare(splitstr[0], "load"))
			{
				if (splitstr.size() < 2)
				{
					printf("Input your FileName!\n");
					continue;
				}
				image = cvLoadImage(splitstr[1].c_str(),CV_LOAD_IMAGE_UNCHANGED);
				if (image != NULL)
				{
					printf("Load successful.\n");
				}
				else
				{
					printf("Wrong FileName.\n");
				}
			}
			else if (compare(splitstr[0], "bright"))
			{
				if (image == NULL)
				{
					printf("You haven't load an image yet!\n");
					continue;
				}
				if (image->nChannels != 1)
				{
					printf("We can not handle your image!\n");
					continue;
				}
				EqualizeHist(image);
			}
			else if (compare(splitstr[0], "bright_and_contrast"))
			{
				if (image == NULL)
				{
					printf("You haven't load an image yet!\n");
					continue;
				}
				if (splitstr.size() < 3)
				{
					printf("Not engough number!\n");
					continue;
				}
				int br = atoi(splitstr[1].c_str());
				double co = atof(splitstr[2].c_str());
				Change_br_and_co(br, co, image);
			}
			else if (compare(splitstr[0], "display"))
			{
				if (image == NULL)
				{
					printf("You haven't load an image yet!\n");
					continue;
				}
				if (Show == TRUE)
				{
					printf("Already displaying the image!\n");
					continue;
				}
				Show = TRUE;
				d.stop = FALSE;
				d.p = image;
				
				//OpenCv can not show the image without a waitkey, 
				//to make it run separately, using thread for help.
				hThread = CreateThread(NULL, 0, Displayimg, &d, 0, NULL);
			}
			else if (compare(splitstr[0], "hide"))
			{
				if (Show = FALSE)
				{
					printf("You haven't show the image!\n");
					continue;
				}
				Show = FALSE;
				d.stop = TRUE;
			}
			else if (compare(splitstr[0], "save"))
			{
				if (image == NULL)
				{
					printf("You haven't load an image yet!\n");
					continue;
				}
				if (splitstr.size() < 2)
				{
					printf("Input your FileName!\n");
					continue;
				}
				cvSaveImage(splitstr[1].c_str(), image);
				printf("Save successful.\n");
			}
			else if (compare(splitstr[0], "exit"))
			{
				printf("See you next time!\n");
				break;
			}
			else
			{
				printf("Wrong command.\n");
			}
			printf("\n");
		}
		catch (...)
		{
			printf("Some error just happened. You may re-input your command if you wish.\n");
		}
	}
	if (image != NULL)
		cvReleaseImage(&image);
	return 0;
}