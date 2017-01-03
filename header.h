#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/legacy/legacy.hpp>

using namespace cv;

#include <GL/glut.h>

#include <iostream>
#include <fstream>
#include <string>
#include <time.h>

using namespace std;

/* Reconstruction 3D */
void StereoTo3D(vector<Point2f> ptsL, vector<Point2f> ptsR, vector<Point3f> &pts3D, 
				float focalLenInPixel, float baselineInMM, 
				Mat img, Point3f &center3D, Vec3f &size3D);
void TriSubDiv(vector<Point2f> &pts, Mat &img, vector<Vec3i> &tri);

/* Algorithms */
void GetPair(Mat &imgL, Mat &imgR, vector<Point2f> &ptsL, vector<Point2f> &ptsR); // FEATURE_POINT
void GetPairBM(Mat &imgL, Mat &imgR, vector<Point2f> &ptsL, vector<Point2f> &ptsR); // BLOCK_MATCHING

/* OpenGL functions*/
void InitGl();
void Init_lightGl();
void displayGl();
void resizeGl(int w, int h);
void mouseGl(int button, int state, int x, int y);
void mouse_move_Gl(int x,int y);
void keyboard_control_Gl(unsigned char key, int a, int b);
void special_control_Gl(int key, int x, int y);

void MapTexTri(Mat & texImg, Point2f pt2D, Point3f pt3D);
GLuint Create3DTexture(Mat &img, vector<Vec3i> &tri, 
					   vector<Point2f> pts2DTex, vector<Point3f> &pts3D, 
					   Point3f center3D, Vec3f size3D);
void Show(GLuint tex, Point3f center3D, Vec3i size3D);
