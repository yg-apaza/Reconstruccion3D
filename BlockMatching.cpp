#include "header.h"

// Suaviza las distorciones del mapa de disparidad
void FixDisparity(Mat_<float> & disp, int numberOfDisparities) 
{
	Mat_<float> disp1;
	float lastPixel = 10;
	float minDisparity = 30; // N�mero m�nimo de disparidades
	
	//Recorrido del mapa de disparidad rellenando los vacios
	for (int i = 0; i < disp.rows; i++)
	{
		for (int j = numberOfDisparities; j < disp.cols; j++)
		{
			//Se llena con un valor por defecto
			if (disp(i, j) <= minDisparity) 
				disp(i, j) = lastPixel;
			//Actualizacion del valor
			else 
				lastPixel = disp(i, j);
		}
	}
	int border = 4;
	//Ampliaci�n del mapa de disparidad
	copyMakeBorder(disp, disp1, border, border, border, border, BORDER_REPLICATE);
	Mat element = getStructuringElement(MORPH_ELLIPSE, Size(border * 2 + 1, border * 2 + 1));
	
	//Transformaciones de morfolog�a
	morphologyEx(disp1, disp1, CV_MOP_OPEN, element); //Eliminar peque�as regiones claras
	morphologyEx(disp1, disp1, CV_MOP_CLOSE, element); //Eliminar peque�as regiones oscuras
	
	//Recorte del mapa de disparidad
	disp = disp1(Range(border, disp.rows - border), Range(border, disp.cols - border)).clone();
}

// Calcula el mapa de disparidades mediante el algoritmo semi-global block matching
void CalcDisparity(Mat &imgL, Mat &imgR, Mat_<float> &disp, int nDisp) 
{
	StereoSGBM sgbm;
	int nChannels = imgR.channels();

	//Par�metros iniciales
	sgbm.SADWindowSize = 3;
	sgbm.numberOfDisparities = nDisp;
	sgbm.preFilterCap = 63;
	sgbm.P1 = 8 * nChannels * sgbm.SADWindowSize * sgbm.SADWindowSize;
	sgbm.P2 = 32 * nChannels * sgbm.SADWindowSize * sgbm.SADWindowSize;
	sgbm.minDisparity = 0;
	sgbm.uniquenessRatio = 10;
	sgbm.speckleWindowSize = 100;
	sgbm.speckleRange = 32;
	sgbm.disp12MaxDiff = 1;
	sgbm.fullDP = false;

	Mat dispTemp, disp8;
	sgbm(imgL, imgR, dispTemp);
	
	//Conversi�n a una matriz flotante de 32-bit de 1 canal
	dispTemp.convertTo(disp, CV_32FC1, 1.0 / 16);
	
	//Conversi�n a una matriz unsigned de 8-bits (escala de grises)
	disp.convertTo(disp8, CV_8U, 255.0 / nDisp);
	imshow("Disparidad original", disp8);

	//Suavizar las distorciones
	FixDisparity(disp, nDisp);
	disp.convertTo(disp8, CV_8U, 255.0/nDisp);
	imshow("Disparidad suavizada", disp8);
}

// Elecci�n de los puntos caracter�sticos con Block Matching
void ChooseKeyPointsBM(Mat_<float> &disp, int nDisp, int nEdgePt, int nFlatPt,
					   vector<Point2f> &ptsL, vector<Point2f> &ptsR) 
{
	Mat_<float>  dCopy, dx, dy, dEdge;
	dCopy = disp.colRange(Range(nDisp, disp.cols)).clone();
	normalize(dCopy, dCopy, 0, 1, NORM_MINMAX);
	//imshow("Disparidad", dCopy);
	
	Mat dShow(dCopy.size(), CV_32FC3);
	if (dCopy.channels() == 1)
		cvtColor(dCopy, dShow, CV_GRAY2RGB);

	int sobelWinSz = 7;
	Sobel(dCopy, dx, -1, 1, 0, sobelWinSz);
	Sobel(dCopy, dy, -1, 0, 1, sobelWinSz);
	magnitude(dx, dy, dEdge);
	normalize(dEdge, dEdge, 0, 10, NORM_MINMAX);
	//imshow("Borde de la disparidad", dEdge);

	int filterSz[] = {50, 30};
	float slope[] = {4, 8};
	int keepBorder = 5;
	int cnt = 0;
	double value;
	float minValue = .003;
	Point2f selPt1, selPt2;
	Mat_<float> dEdgeCopy1 = dEdge.clone();

	// B�squeda de los bordes significativos y asigna 1 o 2 puntos caracter�sticos cerca a ellos
	while (cnt < nEdgePt)
	{
		Point loc;
		minMaxLoc(dEdgeCopy1, NULL, &value, NULL, &loc);
		if (value < minValue) break;

		float dx1 = dx(loc), dy1 = dy(loc);
		if (abs(dx1) >= abs(dy1))
		{
			selPt1.y = selPt2.y = loc.y;
			selPt1.x = loc.x - (dx1 > 0 ? slope[1] : slope[0]) + nDisp;
			selPt2.x = loc.x + (dx1 > 0 ? slope[0] : slope[1]) + nDisp;
			if (selPt1.x > keepBorder+nDisp)
			{
				ptsL.push_back(selPt1);
				ptsR.push_back(selPt1 - Point2f(disp(selPt1), 0));
				circle(dShow, selPt1-Point2f(nDisp, 0), 2, CV_RGB(255, 0, 0), 2);
				cnt++;
			}
			if (selPt2.x < disp.cols - keepBorder)
			{
				ptsL.push_back(selPt2);
				ptsR.push_back(selPt2 - Point2f(disp(selPt2), 0));
				circle(dShow, selPt2-Point2f(nDisp, 0), 2, CV_RGB(0, 255, 0), 2);
				cnt++;
			}
			
			int left = min(filterSz[1], loc.x),
				top = min(filterSz[0], loc.y),
				right = min(filterSz[1], dCopy.cols - loc.x - 1),
				bot = min(filterSz[0], dCopy.rows - loc.y - 1);
			Mat sub = dEdgeCopy1(Range(loc.y - top, loc.y + bot + 1), Range(loc.x - left, loc.x + right + 1));
			sub.setTo(Scalar(0));
		}
		else
		{
			selPt1.x = selPt2.x = loc.x+nDisp;
			selPt1.y = loc.y - (dy1 > 0 ? slope[1] : slope[0]);
			selPt2.y = loc.y + (dy1 > 0 ? slope[0] : slope[1]);
			if (selPt1.y > keepBorder)
			{
				ptsL.push_back(selPt1);
				ptsR.push_back(selPt1 - Point2f(disp(selPt1), 0));
				circle(dShow, selPt1-Point2f(nDisp, 0), 2, CV_RGB(255, 255, 0), 2);
				cnt++;
			}
			if (selPt2.y < disp.rows-keepBorder)
			{
				ptsL.push_back(selPt2);
				ptsR.push_back(selPt2 - Point2f(disp(selPt2), 0));
				circle(dShow, selPt2-Point2f(nDisp, 0), 2, CV_RGB(0, 255, 255), 2);
				cnt++;
			}

			int left = min(filterSz[0], loc.x),
				top = min(filterSz[1], loc.y),
				right = min(filterSz[0], dCopy.cols-loc.x - 1),
				bot = min(filterSz[1], dCopy.rows-loc.y - 1);
			Mat sub = dEdgeCopy1(Range(loc.y - top, loc.y + bot + 1), Range(loc.x - left, loc.x + right + 1));
			sub.setTo(Scalar(0));
		}
	}
	imshow("Puntos caracter�sticos en bordes",dShow);
	int filterSz0 = 6;
	keepBorder = 3;
	cnt = 0;
	Mat_<float> dEdgeCopy2;
	GaussianBlur(dEdge, dEdgeCopy2, Size(0, 0), 5);
	char str[10];

	// B�squeda de las �reas planas, asignaci�n de 1 punto caracter�stico cerca a ellas
	while (cnt < nFlatPt)
	{
		Point loc;
		minMaxLoc(dEdgeCopy2, &value, NULL, &loc, NULL);
		if (value == 10) break;

		loc.x += nDisp;
		if (loc.x > keepBorder + nDisp && loc.y > keepBorder &&
			loc.x < disp.cols && loc.y < disp.rows)
		{
			ptsL.push_back(loc);
			ptsR.push_back(Point2f(loc.x, loc.y) - Point2f(disp(loc), 0));
			circle(dShow, Point2f(loc.x, loc.y) - Point2f(nDisp, 0), 2, CV_RGB(255, 0, 255), 2);
			cnt++;
			snprintf(str, 10, "%.1f", disp(loc));
			putText(dShow, str, Point(loc.x - nDisp + 3, loc.y), FONT_HERSHEY_SIMPLEX, .3, CV_RGB(255, 0, 255));
		}

		loc.x -= nDisp;
		int filterSz1 = (10 - value * 3) * filterSz0;
		int left = min(filterSz1, loc.x),
			top = min(filterSz1, loc.y),
			right = min(filterSz1, dCopy.cols - loc.x - 1),
			bot = min(filterSz1, dCopy.rows-loc.y - 1);
		Mat sub = dEdgeCopy2(Range(loc.y - top, loc.y + bot + 1), Range(loc.x - left, loc.x + right + 1));
		sub.setTo(Scalar(10));
	}
	imshow("Puntos Caracter�sticos en planos",dShow);
}

void SaveDisparity(Mat_<float> &disp){
	Mat dispSave;
	
	//Normalizaci�n entre 0 y 1
	normalize(disp, dispSave, 0, 1, NORM_MINMAX);
	
	//Conversi�n a escala de grises
	dispSave.convertTo(dispSave, CV_8U, 255);
	imwrite("disp.jpg", dispSave);
}

void GetPairBM(Mat &imgL, Mat &imgR, vector<Point2f> &ptsL, vector<Point2f> &ptsR) 
{
	Mat_<float> disp;

	int numOfDisp = 80; // N�mero de disparidades, divisible entre 16
	CalcDisparity(imgL, imgR, disp, numOfDisp);
	SaveDisparity(disp);

	int numOfEgdePt = 80, numOfFlatPt = 50;
	ChooseKeyPointsBM(disp, numOfDisp, numOfEgdePt, numOfFlatPt, ptsL, ptsR);
	waitKey();
}
