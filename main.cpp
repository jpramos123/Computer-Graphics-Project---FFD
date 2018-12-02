#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "GL/glut.h"

#define myPI 3.14159265

#define MAIORX 200.0f
#define MENORX -200.0f
#define MAIORY 200.0f
#define MENORY -200.0f

// largua e altura das dimen��es da Janela do Windows
#define LARGURA_WINDOW 400
#define ALTURA_WINDOW 400

#define MAIORX_BEZIER 400.0f
#define MENORX_BEZIER 0.0f
#define MAIORY_BEZIER 400.0f
#define MENORY_BEZIER 0.0f

// posi��o inicial (canto esquerdo superior) da janela Windoes
#define POSWX 250
#define POSWY 150

struct Ponto
{

	GLfloat x;
	GLfloat y;
	GLfloat z;
};

Ponto B3D[4][4];

void Inicializa(void);
void AlteraTamanhoJanela(GLsizei w, GLsizei h);
void Teclado(unsigned char key, int x, int y);
void myMouseFunc(int button, int state, int x, int y);
void Desenha3D(void);
void especificaParametrosVisualizacao();
void DesenhaBezierGrau3_v3();
GLfloat multiplyMatrix(GLfloat T[], GLfloat H[][4], GLfloat M[]);
void DesenhaBule();
void DesenhaBezierPlano();
void DesenhaPontos();
GLfloat myBezier3DMult(GLfloat S[4], GLfloat B[4][4], GLfloat T[4]);
void myMotionFunc(int x, int y);
void desenhaPlano();
// Variaveis usadas no gluLookAt
GLdouble
	userX = 0,
	userY = 0,
	userZ = 300,
	focoX = 0,
	focoY = 0,
	focoZ = 0,
	sentidoX = 0,
	sentidoY = 1,
	sentidoZ = 0;

// Variaveis de Perspectiva
GLfloat angulo_aberto, fAspect, minEnvView, maxEnvView;

// Variaveis para aplicar transformacoes

GLfloat bx, by; // pontos inseridos para as curvas

GLboolean GET_POINTS = 0;
GLint TOTAL_POINTS = 0;

//GLfloat B[16][2]; // para Bezier

int linha = 0;
int coluna = 0;
int oldLinha = 0;
int oldColuna = 0;
int raio = 300;
GLfloat meuAngulo = 270;

int main(int argc, char *argv[])
{

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

	glutInitWindowPosition(POSWX, POSWY);

	glutInitWindowSize(LARGURA_WINDOW, ALTURA_WINDOW);

	glutCreateWindow("Desenho em 3D");

	glutDisplayFunc(Desenha3D);

	glutReshapeFunc(AlteraTamanhoJanela);

	glutKeyboardFunc(Teclado);

	glutMouseFunc(myMouseFunc);

	glutMotionFunc(myMotionFunc);

	Inicializa();

	glutMainLoop();

	return 0;
}

// Funcao callback chamada quando o tamanho da janela é alterado
void AlteraTamanhoJanela(GLsizei w, GLsizei h)
{

	GLfloat largura, altura;

	// Evita divisão por zero
	if (h == 0)
		h = 1;

	largura = w;
	altura = h;

	fAspect = largura / altura;

	glViewport(0, 0, largura, altura);

	// Inicializa o sistema de coordenadas
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	especificaParametrosVisualizacao();
}

void especificaParametrosVisualizacao()
{

	printf("PARAMETROS DE VISUALIZAO\n");
	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);

	angulo_aberto = 60;
	minEnvView = 0.5;
	maxEnvView = 500;

	gluPerspective(angulo_aberto, fAspect, minEnvView, maxEnvView);

	gluLookAt(userX, userY, userZ, focoX, focoY, focoZ, sentidoX, sentidoY, sentidoZ);
}

void Desenha3D(void)
{

	desenhaPlano();
}

void desenhaPlano()
{

	glClear(GL_COLOR_BUFFER_BIT);

	if ((TOTAL_POINTS == 0) && (GET_POINTS))
	{

		B3D[0][0].x = bx;
		B3D[0][0].y = by;
		B3D[0][0].z = -50.0f;
		coluna++;
		TOTAL_POINTS++;
		GET_POINTS = 0;
		printf("\n\nLINHA: %d\nCOLUNA: %d\nTOTAL_POINTS: %d \n\n", linha, coluna, TOTAL_POINTS);
		printf("ENTREI PRIMEIRA VEZ! \n");
	}

	if ((TOTAL_POINTS > 0) && (GET_POINTS) && (TOTAL_POINTS < 16))
	{
		if (coluna < 4)
		{

			printf("\n\nLINHA: %d\nCOLUNA: %d\nTOTAL_POINTS: %d \n\n", linha, coluna, TOTAL_POINTS);

			B3D[linha][coluna].x = bx;
			B3D[linha][coluna].y = by;
			B3D[linha][coluna].z = -50.0f;
			coluna++;
			TOTAL_POINTS++;
			GET_POINTS = 0;
		}
		else if (linha < 3)
		{
			linha++;
			coluna = 0;
		}
	}

	for (int i = 0; i <= linha && TOTAL_POINTS; i++)
	{
		for (int j = 0; j <= coluna - 1; j++)
		{
			glPointSize(5);
			glColor3f(0, 0, 0);
			glBegin(GL_POINTS);
			glVertex3f(B3D[i][j].x, B3D[i][j].y, B3D[i][j].z);
			glEnd();
		}
	}

	if (TOTAL_POINTS == 16)
	{
		GLfloat aux[4][4];

		GLfloat passoBezier = 0.002;
		GLfloat T[4];
		GLfloat M[4];
		GLfloat S[4];

		GLfloat xfin, yfin, xt = 0, yt = 0, zt = 0;
		GLfloat xini, yini, zini;

		for (GLfloat t = 0; t <= 1; t += passoBezier)
		{
			for (GLfloat s = 0; s <= 1; s += passoBezier)
			{
				S[0] = pow(s, 3);
				S[1] = pow(s, 2);
				S[2] = s;
				S[3] = 1;

				T[0] = pow(t, 3);
				T[1] = pow(t, 2);
				T[2] = t;
				T[3] = 1;

				for (int i = 0; i < 4; i++)
				{
					for (int j = 0; j < 4; j++)
					{
						aux[i][j] = B3D[i][j].x;
					}
				}
				// Para a coordenada X
				xt = myBezier3DMult(S, aux, T);

				for (int i = 0; i < 4; i++)
				{
					for (int j = 0; j < 4; j++)
					{
						aux[i][j] = B3D[i][j].y;
					}
				}
				// Para a coordenada Y
				yt = myBezier3DMult(S, aux, T);

				for (int i = 0; i < 4; i++)
				{
					for (int j = 0; j < 4; j++)
					{
						aux[i][j] = B3D[i][j].z;
					}
				}
				// Para a coordenada Z
				zt = myBezier3DMult(S, aux, T);
				glColor3f(s, s + t - 1, t);
				glBegin(GL_POINTS);
				glVertex3f(xt, yt, zt);
			}
		}
		glEnd();
	}
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			glPointSize(5);
			glColor3f(0, 0, 0);
			glBegin(GL_POINTS);
			glVertex3f(B3D[i][j].x, B3D[i][j].y, B3D[i][j].z);
			glEnd();
		}
	}

	glutSwapBuffers();
}

GLfloat myBezier3DMult(GLfloat S[4], GLfloat B[4][4], GLfloat T[4])
{

	GLfloat Bz[4][4] = {
		{-1.0, 3.0, -3.0, 1.0},
		{3.0, -6.0, 3.0, 0.0},
		{-3.0, 3.0, 0.0, 0.0},
		{1.0, 0.0, 0.0, 0.0}};

	GLfloat T0[4], T1[4], T2[4], result;

	// Multiplica S por Bz e armazena em T0
	for (int i = 0; i < 4; i++)
	{
		T0[i] = 0;
		for (int j = 0; j < 4; j++)
		{
			T0[i] += S[j] * Bz[j][i];
		}
	}

	// Multiplica T0 por B e armazena em T1
	for (int i = 0; i < 4; i++)
	{
		T1[i] = 0;
		for (int j = 0; j < 4; j++)
		{
			T1[i] += T0[j] * B[j][i];
		}
	}

	// Multiplica T1 por Bz e armazena em T2
	for (int i = 0; i < 4; i++)
	{
		T2[i] = 0;
		for (int j = 0; j < 4; j++)
		{
			T2[i] += T1[j] * Bz[j][i];
		}
	}

	result = 0;
	// Multiplica T2 por T e armazena em result
	for (int i = 0; i < 4; i++)
	{
		result += T2[i] * T[i];
	}

	return result;
}

void DesenhaBule(void)
{

	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(1.0f, 0.0f, 0.0f);

	//glTranslatef(0,0,0);
	glutWireTeapot(30);

	glutSwapBuffers();
}

void Teclado(unsigned char key, int x, int y)
{

	if (key == 'd')
	{
		if (userZ <= 300 && userZ >= 0 && userX >= 0 && userX <= 300)
		{
			printf("ENTROU TECLADO\n");
			printf("%f , %f\n", userZ, userX);
			userZ -= 20;
			userX += 20;
			printf("%f , %f\n", userZ, userX);
		}
		if (userZ <= 0 && userX <= 300 && userX >= 0)
		{
			printf("ENTROU TECLADO 2\n");
			userZ -= 20;
			userX -= 20;
			printf("%f , %f\n", userZ, userX);
		}
		if (userZ <= 0 && userX <= 0)
		{
			printf("ENTROU TECLADO 3\n");
			userZ += 20;
			userX -= 20;
			printf("%f , %f\n", userZ, userX);
		}
		if (userZ >= 0 && userZ <= 300 && userX >= -300 && userX <= 0)
		{
			printf("ENTROU TECLADO 4\n");
			userZ += 20;
			userX += 20;
			printf("%f , %f\n", userZ, userX);
		}
	}
	if (key == 'w')
	{
		int passo = 5;
		if (userZ <= 300 && userZ >= 0 && userY >= 0 && userY <= 300)
		{
			printf("ENTROU TECLADO\n");
			printf("%f , %f\n", userZ, userY);
			userZ -= passo;
			userY += passo;
			printf("%f , %f\n", userZ, userY);
		}
		if (userZ <= 0 && userY <= 300 && userY >= 0)
		{
			printf("ENTROU TECLADO 2\n");
			userZ -= passo;
			userY -= passo;
			sentidoY = -1;
			printf("%f , %f\n", userZ, userY);
		}
		if (userZ <= 0 && userY <= 0)
		{
			printf("ENTROU TECLADO 3\n");
			userZ += passo;
			userY -= passo;
			printf("%f , %f\n", userZ, userY);
		}
		if (userZ >= 0 && userZ <= 300 && userY >= -300 && userY <= 0)
		{
			printf("ENTROU TECLADO 4\n");
			userZ += passo;
			userY += passo;
			sentidoY = 1;
			printf("%f , %f\n", userZ, userY);
		}
	}
	especificaParametrosVisualizacao();
	glutPostRedisplay();
}

void Inicializa(void)
{

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glLineWidth(2.0);
}

int oldx;
int oldy;
int valida;
void myMouseFunc(int button, int state, int x, int y)
{
	oldx = x;
	oldy = y;
	valida = state == GLUT_DOWN;

	if (button == GLUT_LEFT_BUTTON && (state == GLUT_DOWN))
	{
		//printf("\n\nBUTTON: %d \n STATE: %d \n", button, state);
		bx = x - 200;
		by = 200 - y;
		GET_POINTS = 1;

		//printf("\n %f %f\n", bx, by);
		if (TOTAL_POINTS == 16)
		{

			//TOTAL_POINTS = 0;
			GET_POINTS = 0;
		}
	}
	if (button == GLUT_RIGHT_BUTTON && (state == GLUT_DOWN))
	{

		// GET_POINTS = 0;
		// TOTAL_POINTS = 0;
		// linha = 0;
		// coluna = 0;
	}

	glutPostRedisplay();
}

void myMotionFunc(int x, int y)
{

for(int i = 0; i < 4; i++){
	for(int j = 0; j < 4; j++){
		if (x == B3D[i][j].x && y == B3D[i][j].y)
		{
			
			Desenha3D();
		}
		glutPostRedisplay();
	}
}

}

GLfloat multiplyMatrix(GLfloat T[], GLfloat H[][4], GLfloat M[])
{

	GLfloat HM[4];

	// m ultiplica primeiro H por M
	for (int i = 0; i < 4; i++)
	{
		HM[i] = 0;
		for (int j = 0; j < 4; j++)
		{
			HM[i] = HM[i] + H[i][j] * M[j];
		}
	}

	// multiplica T * HM
	GLfloat R = 0;
	for (int i = 0; i < 4; i++)
	{
		R = R + T[i] * HM[i];
	}

	return R;
}