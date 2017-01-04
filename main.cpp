/************************************************************************************/
/* The 2 stereo images should have parallel principal axis.							*/
/* I cannot guarantee the reconstruct effect, especially when the images are shot	*/
/*	with inexact parallel principal axis.											*/
/* The 3D reconstruct algorithm still needs improvements, for there are so many		*/
/*	glitches.																		*/
/* Yan Ke, THUEE, xjed09@gmail.com, 201106											*/
/************************************************************************************/

#include "header.h"

namespace reconstruction
{
	enum Algorithm {FEATURE_POINT, BLOCK_MATCHING};
	Algorithm g_algo = FEATURE_POINT;//BLOCK_MATCHING; // 2 algorithms to select corresponding points and reconstruct 3D scene
};

using namespace reconstruction;

int main(int argc, char* argv[])
{
	Mat imgL = imread("view1.jpg"); 
	Mat	imgR = imread("view5.jpg");
	imshow("Imagen Izquierda", imgL);
	imshow("Imagen Derecha", imgL);
	waitKey(0);

	if (!(imgL.data) || !(imgR.data))
	{
		cerr<<"No se pudo cargar imagen!"<<endl;
		exit(1);
	}

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	float stdWidth = 800, resizeScale = 1;//stdWidth can change
	if (imgL.cols > stdWidth * 1.2)
	{
		resizeScale = stdWidth / imgL.cols;
		Mat imgL1, imgR1;
		resize(imgL, imgL1, Size(), resizeScale, resizeScale);
		resize(imgR, imgR1, Size(), resizeScale, resizeScale);
		imgL = imgL1.clone();
		imgR = imgR1.clone();
	}

	/************************************************************************/
	/* decide which points in the left image should be chosen               */
	/* and calculate their corresponding points in the right image          */
	/************************************************************************/
	cout << "Calculando puntos de características ..." << endl;
	vector<Point2f> ptsL, ptsR;
	vector<int> ptNum;
	if (g_algo == FEATURE_POINT)
		GetPair(imgL, imgR, ptsL, ptsR);
	else if (g_algo == BLOCK_MATCHING)
		GetPairBM(imgL, imgR, ptsL, ptsR);

	/************************************************************************/
	/* calculate 3D coordinates                                             */
	/************************************************************************/
	vector<Point3f> pts3D;
	float focalLenInPixel = 3740 * resizeScale, baselineInMM = 160;
	Point3f center3D;
	Vec3f size3D;
	float scale = .2; // scale the z coordinate so that it won't be too large spreaded
	//float imgHinMM = 400, // approximate real height of the scene in picture, useless
	//float MMperPixel = imgHinMM / imgL.rows;
	//float focalLenInMM = focalLenInPixel * MMperPixel;
	focalLenInPixel *= scale;

	cout << "Calculando coordenadas 3D ..." << endl;
	StereoTo3D(ptsL, ptsR, pts3D,  focalLenInPixel, baselineInMM,  imgL,
				center3D, size3D);

	/************************************************************************/
	/* Delaunay triangulation                                               */
	/************************************************************************/
	cout << "Haciendo triangulación ..." << endl;
	size_t pairNum = ptsL.size();
	vector<Vec3i> tri;
	TriSubDiv(ptsL, imgL, tri);

	/************************************************************************/
	/* Draw 3D scene using OpenGL                                           */
	/************************************************************************/
	glutInit(&argc, argv); // must be called first in a glut program
	InitGl(); // must be called first in a glut program

	cout << "Creando textura 3D ..." << endl;
	GLuint tex = Create3DTexture(imgL, tri, ptsL, pts3D, center3D, size3D);
	Show(tex, center3D, size3D);

	return 0;
}
