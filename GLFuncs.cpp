#include "header.h"

#if !defined(GLUT_WHEEL_UP)
#  define GLUT_WHEEL_UP   3
#  define GLUT_WHEEL_DOWN 4
#endif

// Dibuja una textura de un triangulo
void MapTexTri(Mat & texImg, Point2f pt2D[3], Point3f pt3D[3]) 
{
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexImage2D(GL_TEXTURE_2D, 0, 3,texImg.cols, texImg.rows, 0,
					GL_RGB, GL_UNSIGNED_BYTE, texImg.data);

	glBegin(GL_TRIANGLES);

	glTexCoord2f(pt2D[0].x, pt2D[0].y); glVertex3f(pt3D[0].x, pt3D[0].y, pt3D[0].z);
	glTexCoord2f(pt2D[1].x, pt2D[1].y); glVertex3f(pt3D[1].x, pt3D[1].y, pt3D[1].z);
	glTexCoord2f(pt2D[2].x, pt2D[2].y); glVertex3f(pt3D[2].x, pt3D[2].y, pt3D[2].z);

	glEnd();
}

GLuint Create3DTexture(Mat &img, vector<Vec3i> &tri, 
						vector<Point2f> pts2DTex, vector<Point3f> &pts3D, 
						Point3f center3D, Vec3f size3D) 
{
	GLuint tex = glGenLists(1);
	int error = glGetError();
	if (error != GL_NO_ERROR) 
		cout << "Ha ocurrido un error de OpenGL: " << gluErrorString(error) << endl;
	if (tex == 0) return 0;

	Mat texImg;
	cvtColor(img, img, CV_BGR2RGB);
	resize(img, texImg, Size(512, 512));

	glNewList(tex, GL_COMPILE);

	vector<Vec3i>::iterator iterTri = tri.begin();
	Point2f pt2D[3];
	Point3f pt3D[3];

	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	for (; iterTri != tri.end(); iterTri++)
	{
		Vec3i &vertices = *iterTri;
		int ptIdx;
		for (int i = 0; i < 3; i++)
		{
			ptIdx = vertices[i];
			if (ptIdx == -1) break;
			pt2D[i].x = pts2DTex[ptIdx].x / img.cols;
			pt2D[i].y = pts2DTex[ptIdx].y / img.rows;
			pt3D[i] = (pts3D[ptIdx] - center3D) * (1.f / max(size3D[0], size3D[1]));
		}

		if (ptIdx != -1)
			MapTexTri(texImg, pt2D, pt3D);
	}
	glDisable(GL_TEXTURE_2D);
	glEndList();
	return tex;
}

#define PI_180			(CV_PI/180)
#define ROTATE_STEP		5
#define TRANSLATE_STEP	.3

static float	g_rx, g_ry;
static float	g_tz;
static GLuint	g_tex;

void InitGl() 
{
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Reconstruccion 3D");
	glClearColor(0,0,0,1);
	glutDisplayFunc(displayGl);
	glutReshapeFunc(resizeGl);
	glutKeyboardFunc(keyboard_control_Gl);
	glutSpecialFunc(special_control_Gl);
	glutMouseFunc(mouseGl);
	glutMotionFunc(mouse_move_Gl);
	glEnable(GL_DEPTH_TEST);
}

void Init_lightGl() 
{
	GLfloat light_position1[] = {0.0, -3.0, -3.0, 0.0};
	GLfloat light_ambient1[] = {1, 1, 1, 1};
	GLfloat light_diffuse1[] = {0.80, 0.8, 0.8, 1};
	GLfloat light_specular1[] = {0.75, 0.75, 0.75, 1};
	GLfloat light_shine1[] = {50};

	glLightfv(GL_LIGHT0, GL_AMBIENT , light_ambient1);
	glLightfv(GL_LIGHT0, GL_DIFFUSE , light_diffuse1 );
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular1);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position1);
	glLightfv(GL_LIGHT0, GL_SHININESS, light_shine1);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);
}

void Show(GLuint tex, Point3f center3D, Vec3i size3D) 
{
	g_tz = 2; // Ajustar posicion de la camara
	g_rx = 90;
	g_ry = 0;
	g_tex = tex;
	glutMainLoop();
}

void displayGl()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	float eyey = g_tz * sin(g_ry * PI_180),
			eyex = g_tz * cos(g_ry * PI_180) * cos(g_rx * PI_180),
			eyez = g_tz * cos(g_ry  *PI_180) * sin(g_rx * PI_180);
	gluLookAt(eyex,eyey,eyez, 0,0,0, 0,1,0);
	glColor3f(1, 1, 1);
	glCallList(g_tex);

	glPopMatrix();
	glPushMatrix();
	glColor3f(0, 1, 0);
	glTranslatef(-0.08, .08, -0.2); 
	glListBase(1000);
	glRasterPos3f(0, 0, 0);
	string help = "Use las teclas de direcci�n para rotar, la rueda del raton para zoom";
	glCallLists(help.size(), GL_UNSIGNED_BYTE, help.c_str());
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
}

void resizeGl(int w, int h) 
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, GLdouble(w) / GLdouble(h), 0.01, 10000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void mouseGl(int button, int state, int x, int y) 
{
	switch(button)
	{
		case GLUT_WHEEL_UP:
			g_tz -= TRANSLATE_STEP;
			break;

		case GLUT_WHEEL_DOWN:
			g_tz += TRANSLATE_STEP;
			break;

		default:
			break;
	}
	if (g_tz < 0)
		g_tz = 0;
	glutPostRedisplay();
}

void mouse_move_Gl(int x, int y) 
{
	glutPostRedisplay();
}

void keyboard_control_Gl(unsigned char key, int a, int b) 
{
	if (key == 0x1B)
		exit(1);
	glutPostRedisplay();
}

void special_control_Gl(int key, int x, int y) 
{
	if (key == GLUT_KEY_LEFT)
	{
		g_rx -= ROTATE_STEP;
		if (g_rx<1) g_rx = 1;
	}
	else if (key == GLUT_KEY_RIGHT)
	{
		g_rx += ROTATE_STEP;
		if (g_rx>=179) g_rx = 179;
	}
	else if (key == GLUT_KEY_UP)
	{
		g_ry -= ROTATE_STEP;
		if (g_ry<-89) g_ry = -89;
	}
	else if (key == GLUT_KEY_DOWN)
	{
		g_ry += ROTATE_STEP;
		if (g_ry>=89) g_ry = 89;
	}
	glutPostRedisplay();
}
