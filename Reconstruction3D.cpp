#include "header.h"
#include <time.h>

bool isGoodTri(Vec3i &v, vector<Vec3i> & tri) 
{
	int a = v[0], b = v[1], c = v[2];
	v[0] = min(a, min(b,c));
	v[2] = max(a, max(b,c));
	v[1] = a + b + c - v[0] - v[2];
	if (v[0] == -1) return false;
	
	vector<Vec3i>::iterator iter = tri.begin();
	for(; iter != tri.end(); iter++)
	{
		Vec3i &check = *iter;
		if (check[0]==v[0] &&
			check[1]==v[1] &&
			check[2]==v[2])
		{
			break;
		}
	}
	if (iter == tri.end())
	{
		tri.push_back(v);
		return true;
	}
	return false;
}

// Triangulación de Delaunay
void TriSubDiv(vector<Point2f> &pts, Mat &img, vector<Vec3i> &tri) 
{
	CvSubdiv2D* subdiv; // Subdivision
	// Almacenamiento para los puntos obtenidos por la subdivision de Delaunay
	CvMemStorage* storage = cvCreateMemStorage(0);
	// Rectangulo que abarca toda el area de subdivision
	Rect rc = Rect(0, 0, img.cols, img.rows);
	
	subdiv = cvCreateSubdiv2D(CV_SEQ_KIND_SUBDIV2D, sizeof(*subdiv),
							  sizeof(CvSubdiv2DPoint),
							  sizeof(CvQuadEdge2D),
							  storage);
	
	cvInitSubdivDelaunay2D(subdiv, rc);
	
	for (size_t i = 0; i < pts.size(); i++)
	{
		CvSubdiv2DPoint *pt = cvSubdivDelaunay2DInsert(subdiv, pts[i]);
		pt->id = i;
	}
	
	CvSeqReader reader;
	int total = subdiv->edges->total;
	int elem_size = subdiv->edges->elem_size;
	
	cvStartReadSeq((CvSeq*)(subdiv->edges), &reader, 0);
	Point buf[3];
	const Point *pBuf = buf;
	Vec3i verticesIdx;
	Mat imgShow = img.clone();
	
	srand((unsigned)time(NULL));
	for(int i = 0; i < total; i++) 
	{   
		CvQuadEdge2D* edge = (CvQuadEdge2D*)(reader.ptr);   
		if(CV_IS_SET_ELEM(edge)) 
		{
			CvSubdiv2DEdge t = (CvSubdiv2DEdge)edge; 
			int iPointNum = 3;
			Scalar color = CV_RGB(rand()&255, rand()&255, rand()&255);
			
			int j;
			for(j = 0; j < iPointNum; j++ )
			{
				CvSubdiv2DPoint* pt = cvSubdiv2DEdgeOrg(t);
				if(!pt) break;
				buf[j] = pt->pt;
				verticesIdx[j] = pt->id;
				t = cvSubdiv2DGetEdge(t, CV_NEXT_AROUND_LEFT);
			}
			if (j != iPointNum)
				continue;
			if (isGoodTri(verticesIdx, tri))
			{
				polylines(imgShow, &pBuf, &iPointNum, 
						  1, true, color,
						  1, CV_AA, 0);
			}
			
			t = (CvSubdiv2DEdge)edge + 2;
			
			for(j = 0; j < iPointNum; j++)
			{
				CvSubdiv2DPoint* pt = cvSubdiv2DEdgeOrg(t);
				if(!pt) break;
				buf[j] = pt->pt;
				verticesIdx[j] = pt->id;
				t = cvSubdiv2DGetEdge(t, CV_NEXT_AROUND_LEFT);
			}   
			if (j != iPointNum)
				continue;
			if (isGoodTri(verticesIdx, tri))
			{
				polylines( imgShow, &pBuf, &iPointNum, 
						  1, true, color,
						  1, CV_AA, 0);
			}
		}
		CV_NEXT_SEQ_ELEM(elem_size, reader);
	}
	
	char title[100];
	snprintf(title, 100, "Delaunay: %d triangulos", tri.size());
	imshow(title, imgShow);
	waitKey();
}

// Calcula las coordenadas 3D
void StereoTo3D( vector<Point2f> ptsL, vector<Point2f> ptsR, vector<Point3f> &pts3D,
				float focalLenInPixel, float baselineInMM, Mat img,
				Point3f &center3D, Vec3f &size3D)
{
	vector<Point2f>::iterator iterL = ptsL.begin(), iterR = ptsR.begin();
	
	float xl, xr, ylr;
	float imgH = float(img.rows), imgW = float(img.cols);
	Point3f pt3D;
	float minX = 1e9, maxX = -1e9;
	float minY = 1e9, maxY = -1e9;
	float minZ = 1e9, maxZ = -1e9;
	
	Mat imgShow = img.clone();
	char str[100];
	int ptCnt = ptsL.size(), showPtNum = 30, cnt = 0;
	int showIntv = max(ptCnt / showPtNum, 1);
	
	for (; iterL != ptsL.end(); iterL++, iterR++)
	{
		xl = iterL->x;
		xr = iterR->x;
		ylr = (iterL->y + iterR->y) / 2;
		
		// Formula para obtener X, Y, Z
		pt3D.z = -focalLenInPixel * baselineInMM / (xl - xr);
		pt3D.y = -(-ylr + imgH / 2) * pt3D.z / focalLenInPixel;
		pt3D.x = (imgW / 2 - xl) * pt3D.z / focalLenInPixel;
		
		// Limites de la imagen tridimensional
		minX = min(minX, pt3D.x);
		maxX = max(maxX, pt3D.x);
		minY = min(minY, pt3D.y);
		maxY = max(maxY, pt3D.y);
		minZ = min(minZ, pt3D.z);
		maxZ = max(maxZ, pt3D.z);
		
		// Almacena el punto tridimensional obtenido		
		pts3D.push_back(pt3D);
		
		if ((cnt++) % showIntv == 0)
		{
			Scalar color = CV_RGB(rand()&64, rand()&64, rand()&64);
			snprintf(str, 100, "%.0f,%.0f,%.0f", pt3D.x, pt3D.y, pt3D.z);
			putText(imgShow, str, Point(xl-13,ylr-3), FONT_HERSHEY_SIMPLEX, .3, color);
			circle(imgShow, *iterL, 2, color, 3);
		}
	}
	
	imshow("Coordenadas 3D obtenidas", imgShow);
	waitKey();
	
	center3D.x = (minX + maxX) / 2;
	center3D.y = (minY + maxY) / 2;
	center3D.z = (minZ + maxZ) / 2;
	
	// Tamanio de la imagen tridimensional
	size3D[0] = maxX - minX;
	size3D[1] = maxY - minY;
	size3D[2] = maxZ - minZ;
}
