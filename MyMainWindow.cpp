#include "MyMainWindow.h"

namespace reconstruction
{
	enum Algorithm {FEATURE_POINT, BLOCK_MATCHING};
};

using namespace reconstruction;

MyMainWindow::MyMainWindow(wxWindow *parent) : FBMainWindow(parent) {
  
}

void MyMainWindow::OnButtonClose( wxCommandEvent& event ) {
	Close();
}

void MyMainWindow::execute( wxCommandEvent& event )  {
	reconstruction::Algorithm g_algo;
	if (radioFP->GetValue() == true){
		g_algo = FEATURE_POINT;
	}
	if (radioBM->GetValue() == true){
		g_algo = BLOCK_MATCHING;
	}
	//Carga de las imágenes
	String filename = txtImg->GetValue().c_str();
	Mat imgL = imread("Images/left/"+filename); 
	Mat	imgR = imread("Images/right/"+filename);

	imshow("Imagen Izquierda", imgL);
	imshow("Imagen Derecha", imgR);
	waitKey(0);

	if (!(imgL.data) || !(imgR.data))
	{
		cerr<<"No se pudo cargar imagen!"<<endl;
		exit(1);
	}
	
	/************************************************************************/
	/* Redimensión de las imágenes                                          */
	/************************************************************************/
	float stdWidth = 800, resizeScale = 1;
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
	/* Elección de los puntos característicos en la imagen izquierda        */
	/* Cálculo de los puntos correspondientes en la imagen derecha           */
	/************************************************************************/
	cout << "Calculando puntos caracteristicos ..." << endl;
	vector<Point2f> ptsL, ptsR;
	vector<int> ptNum;
	if (g_algo == FEATURE_POINT)
		GetPair(imgL, imgR, ptsL, ptsR);
	else if (g_algo == BLOCK_MATCHING)
		GetPairBM(imgL, imgR, ptsL, ptsR);
	
	/************************************************************************/
	/* Cálculo de las coordenadas 3D                                        */
	/************************************************************************/
	vector<Point3f> pts3D;
	float focalLenInPixel = 3740 * resizeScale, baselineInMM = 160;
	Point3f center3D;
	Vec3f size3D;
	float scale = .2; // Escala de la coordenada z para concentrar el espacio
	focalLenInPixel *= scale;
	
	cout << "Calculando coordenadas 3D ..." << endl;
	StereoTo3D(ptsL, ptsR, pts3D, focalLenInPixel, baselineInMM, imgL, center3D, size3D);
	
	/************************************************************************/
	/* Triangulación de Delaunay                                            */
	/************************************************************************/
	cout << "Ejecutando triangulacion ..." << endl;
	size_t pairNum = ptsL.size();
	vector<Vec3i> tri;
	TriSubDiv(ptsL, imgL, tri);
	
	/************************************************************************/
	/* Dibujo de la escena 3D mediante OpenGL                               */
	/************************************************************************/
	char *argv [1];
	int argc = 1;
	argv[0] = strdup("Reconstruccion");
	glutInit(&argc, argv); // Inicializa la librería GLUT
	InitGl(); //Inicializa las funciones de openGL
	
	cout << "Creando textura 3D ..." << endl;
	GLuint tex = Create3DTexture(imgL, tri, ptsL, pts3D, center3D, size3D);
	ShowW(tex, center3D, size3D);

}
