#include <iostream>
#include <fstream>
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#ifndef M_PI
# define M_PI 3.14159265358979323846
#endif
#define PAS (M_PI/36)
#define DEBUT (M_PI/6)
#define FIN (2*M_PI - DEBUT)

#define BLOCK_SIZE 3.0f
#define NUM_BLOCKS 16
#define LEVEL_SIZE BLOCK_SIZE * NUM_BLOCKS
#define LEVEL_SIZE_HALF LEVEL_SIZE * 0.5f

//VARIABLE

//les flèches du clavier:
bool isLeftArrowPressed = false;
bool isRightArrowPressed = false;
bool isUpArrowPressed = false;
bool isDownArrowPressed = false;

//le temps pour les animations
unsigned Counter = 0;

unsigned pacmanPositionBlockX = 0, pacmanPositionBlockZ = 0;         //Position actuel sur le quadrillage
unsigned pacmanLastPositionBlockX = 0, pacmanLastPositionBlockZ = 0; //Position precedente sur le quadrillage

//la position final de pacman (camera qui suit le pacman)
float pacmanDrawPositionX = 0.0f;
float pacmanDrawPositionZ = 0.0f;

//differente direction
unsigned pacmanDirection = 0; //0 - est, 1 - nord, 2 - ouest, 3 - sud

bool isPacmanMoving = false;
const float pacmanMovementSpeed = 0.1f;
float pacmanMovementInterpolation = 0.0f; // Controler l'interpolation entre la derniere et l'actuelle position

//lumiere est materiel
GLfloat lightPos [] = {-10.0f, 17.0f, 14.0f, 0.0f};
GLfloat materialYellow[] = {0.83f, 0.70f, 0.0f, 1.0f};
GLfloat materialPureBlack[] = {0.0f, 0.0f, 0.0f, 0.0f};
GLfloat materialBlack[] = {0.05f, 0.05f, 0.06f, 1.0f};
GLfloat materialWhite[] = {1.00f, 1.00f, 1.00f, 1.0f};
GLfloat materialBlue[] = {0.2f, 0.3f, 0.7f, 1.0f};
GLfloat materialGreen[] = {0.2f, 0.7f, 0.3f, 1.0f};

//Fonctions
LRESULT CALLBACK WindowProc (HWND   fenetrePrincipale, UINT   message,
                             WPARAM wParam,            LPARAM lParam);

void activerOpenGL    (HWND hwnd, HDC* hDC, HGLRC* hRC);
void desactiverOpenGL (HWND hwnd, HDC  hDC, HGLRC  hRC);

void dessiner (HDC* hDC);

 void drawSphere( GLint slices, GLint stacks )
{
    float x, y, z ;
    for( float lat = 0 ; lat < M_PI ; lat = lat + M_PI/stacks )
    {
        glBegin( GL_QUAD_STRIP ) ;
        for( float lon = 0 ; lon < 2.1*M_PI ; lon = lon + 2*M_PI/slices )
        {
            x = cosf( lat ) * sinf( lon );
            y = sinf( lat ) * sinf( lon );
            z = cosf( lon ) ;

            glNormal3f( x, y, z );
            glVertex3f( x, y, z );

            x = cosf( lat + M_PI/stacks ) * sinf( lon );
            y = sinf( lat + M_PI/stacks ) * sinf( lon );
            z = cosf( lon ) ;

            glNormal3f( x, y, z );
            glVertex3f( x, y, z );
        }
        glEnd() ;
    }
}

// Definit un vertex de la sphère pour les angles a et b (en radians)
void coord(double a, double b) {
    glNormal3d (sin(a)*cos(b), cos(a), sin(a)*sin(b));
    glVertex3d(sin(a)*cos(b), cos(a), sin(a)*sin(b));

}

void drawPacman() {
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  materialYellow);

    glPushMatrix();

    float swayAmount = sin(float(Counter) * 0.2f); //variable qui controle l'animation de pacman

    glTranslatef(pacmanDrawPositionX, 1.0f + swayAmount * 0.05f, pacmanDrawPositionZ);
    glRotatef(90.0f * pacmanDirection, 0.0f, 1.0f, 0.0f);
    glRotatef(-10.0f + swayAmount * 10.0f, 0.0f, 0.0f, 1.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);

    for (double a = 0; a < M_PI; a+=PAS)
    {
        double b1;
        double a1 = a + PAS;

        glBegin(GL_QUADS);
        // Intérieur de la bouche 1ere partie
        coord(a, DEBUT);
        coord(a1, DEBUT);

        glNormal3d (0.0, 0.0, -1.0);
        glVertex3d(0, cos(a1), 0);

        glNormal3d (0.0, 0.0, -1.0);
        glVertex3d(0, cos(a), 0);

        // Arc qui va du haut de la bouche au bas de la bouche
        for (double b = DEBUT; b < FIN; b+=PAS) {
            b1 = b + PAS;

            float mouthAnimation = sin(float(Counter) * 0.2f) * 0.75f; // animation pour animer la bouche de pacman

            if (b1 > FIN) b1 = FIN - mouthAnimation;
            coord(a,b);
            coord(a1, b);
            coord(a1, b1);
            coord(a, b1);
        }
        // Intérieur de la bouche 2eme partie
        coord(a, FIN);
        coord(a1,FIN);

        glNormal3d (0.0, 0.0, -1.0);
        glVertex3d(0, cos(a1), 0);

        glNormal3d (0.0, 0.0, -1.0);
        glVertex3d(0, cos(a), 0);
        glEnd();

    }

    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  materialBlack);

    //l'oeil gauche
    glPushMatrix();
    glTranslatef(0.43f, 0.5f, 0.66f);
    glScalef(0.16f, 0.16f, 0.16f);

    drawSphere(32, 32);

    glPopMatrix();

    //l'oeil droit
    glPushMatrix();
    glTranslatef(0.43f, -0.5f, 0.66f);
    glScalef(0.16f, 0.16f, 0.16f);

    drawSphere(32, 32);

    glPopMatrix();


    glPopMatrix();
}

void drawBase() {
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  materialBlue);

    // Dessine le sol avec un quad
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-LEVEL_SIZE_HALF, 0.0f, -LEVEL_SIZE_HALF);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(LEVEL_SIZE_HALF, 0.0f, -LEVEL_SIZE_HALF);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(LEVEL_SIZE_HALF, 0.0f, LEVEL_SIZE_HALF);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-LEVEL_SIZE_HALF, 0.0f, LEVEL_SIZE_HALF);
    glEnd();

    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  materialBlack);

    glBegin(GL_LINES);

    //axe ligne x
    for(unsigned f = 0; f <= NUM_BLOCKS; f++) {
        glVertex3f(-LEVEL_SIZE_HALF + BLOCK_SIZE * f, 0.05f, -LEVEL_SIZE_HALF);
        glVertex3f(-LEVEL_SIZE_HALF + BLOCK_SIZE * f, 0.05f, LEVEL_SIZE_HALF);
    }

    //axe ligne z
    for(unsigned f = 0; f <= NUM_BLOCKS; f++) {
        glVertex3f(-LEVEL_SIZE_HALF, 0.05f, -LEVEL_SIZE_HALF + BLOCK_SIZE * f);
        glVertex3f(LEVEL_SIZE_HALF, 0.05f, -LEVEL_SIZE_HALF + BLOCK_SIZE * f);
    }
    glEnd();
}

 class LabyBlock {
public:
    bool isMur;
    bool pastille;

    LabyBlock() {
        isMur = false;
        pastille = false;
    }

    void Draw(const unsigned inColumn, const unsigned inRow) {
        glPushMatrix();
        glTranslatef(BLOCK_SIZE * inColumn - LEVEL_SIZE_HALF, 0.0f, BLOCK_SIZE * inRow - LEVEL_SIZE_HALF);

        //dessine les blocks
        if(isMur) {
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  materialGreen);

            glBegin(GL_QUADS);

            //au dessus
            glNormal3f(0.0f, 1.0f, 0.0f);
            glVertex3f(0.0f, 1.5f, 0.0f);
            glVertex3f(BLOCK_SIZE, 1.5f, 0.0f);
            glVertex3f(BLOCK_SIZE, 1.5f, BLOCK_SIZE);
            glVertex3f(0.0f, 1.5f, BLOCK_SIZE);

            //gauche
            glNormal3f(-1.0f, 0.0f, 0.0f);
            glVertex3f(0.0f, 0.0f, 0.0f);
            glVertex3f(0.0f, 1.5f, 0.0f);
            glVertex3f(0.0f, 1.5f, BLOCK_SIZE);
            glVertex3f(0.0f, 0.0f, BLOCK_SIZE);

            //droite
            glNormal3f(1.0f, 0.0f, 0.0f);
            glVertex3f(BLOCK_SIZE, 0.0f, 0.0f);
            glVertex3f(BLOCK_SIZE, 1.5f, 0.0f);
            glVertex3f(BLOCK_SIZE, 1.5f, BLOCK_SIZE);
            glVertex3f(BLOCK_SIZE, 0.0f, BLOCK_SIZE);

            //devant
            glNormal3f(0.0f, 0.0f, 1.0f);
            glVertex3f(0.0f, 1.5f, BLOCK_SIZE);
            glVertex3f(BLOCK_SIZE, 1.5f, BLOCK_SIZE);
            glVertex3f(BLOCK_SIZE, 0.0f, BLOCK_SIZE);
            glVertex3f(0.0f, 0.0f, BLOCK_SIZE);

            //derriere
            glNormal3f(0.0f, 0.0f, -1.0f);
            glVertex3f(0.0f, 1.5f, 0.0f);
            glVertex3f(BLOCK_SIZE, 1.5f, 0.0f);
            glVertex3f(BLOCK_SIZE, 0.0f, 0.0f);
            glVertex3f(0.0f, 0.0f, 0.0f);

            glEnd();

            //dessine les lignes
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  materialBlack);
            glBegin(GL_LINES);

            glVertex3f(0.0f, 1.5f, 0.0f);
            glVertex3f(BLOCK_SIZE, 1.5f, 0.0f);

            glVertex3f(0.0f, 1.5f, BLOCK_SIZE);
            glVertex3f(0.0f, 1.5f, 0.0f);

            glVertex3f(BLOCK_SIZE, 1.51f, BLOCK_SIZE);
            glVertex3f(0.0f, 1.5f, BLOCK_SIZE);

            glVertex3f(BLOCK_SIZE, 1.5f, BLOCK_SIZE);
            glVertex3f(BLOCK_SIZE, 1.5f, 0.0f);

            glVertex3f(0.0f, 1.5f, 0.0f);
            glVertex3f(0.0f, 0.0f, 0.0f);

            glVertex3f(BLOCK_SIZE, 1.5f, 0.0f);
            glVertex3f(BLOCK_SIZE, 0.0f, 0.0f);

            glVertex3f(0.0f, 1.5f, BLOCK_SIZE);
            glVertex3f(0.0f, 0.0f, BLOCK_SIZE);

            glVertex3f(BLOCK_SIZE, 1.5f, BLOCK_SIZE);
            glVertex3f(BLOCK_SIZE, 0.0f, BLOCK_SIZE);

            glEnd();
        }

        //dessine les spheres lumineux (pastille)
        if(pastille) {
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  materialWhite);
            glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,  materialYellow); //Pour le rendre lumineux

            glPushMatrix();

            glTranslatef(BLOCK_SIZE * 0.5f, 1.0f, BLOCK_SIZE * 0.5f);
            glScalef(0.2f, 0.2f, 0.2f);

            drawSphere(6, 6);
            glPopMatrix();

            glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,  materialPureBlack); //retourne la valeur d'emission par défaut
        }

        glPopMatrix();
    }

    ~LabyBlock() {}
};

LabyBlock Blocks[NUM_BLOCKS][NUM_BLOCKS];

int main (int argc, char** argv)
 {
      HINSTANCE  hInstance = (HINSTANCE) GetModuleHandle (NULL);
      WNDCLASSEX wcex;

      wcex.cbSize        = sizeof(WNDCLASSEX);
      wcex.style         = CS_OWNDC;
      wcex.lpfnWndProc   = WindowProc;
      wcex.cbClsExtra    = 0;
      wcex.cbWndExtra    = 0;
      wcex.hInstance     = hInstance;
      wcex.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
      wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
      wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
      wcex.lpszMenuName  = NULL;
      wcex.lpszClassName = "GameFusion";
      wcex.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

      RegisterClassEx(&wcex);

      HWND hwnd = CreateWindowEx (0,
                                  "GameFusion",
                                  "Open GL",
                                  WS_OVERLAPPEDWINDOW,
                                  CW_USEDEFAULT,
                                  CW_USEDEFAULT,
                                  700,
                                  700,
                                  NULL,
                                  NULL,
                                  hInstance,
                                  NULL);

      ShowWindow (hwnd, SW_SHOWDEFAULT);

      HDC   hDC;
      HGLRC hRC;

      activerOpenGL (hwnd, &hDC, &hRC);

      RECT windowRectangle;
      GetWindowRect(hwnd, &windowRectangle);
      SetCursorPos(windowRectangle.left + 350, windowRectangle.top + 350);

      MSG        msg;
      while (1)
       {
            dessiner (&hDC);
            if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            {
                if(msg.message == WM_QUIT || msg.message == WM_CLOSE) {
                    break;
                }
                else
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
            }

            //met a jour le compteur, utilisé pour l'animation
            Counter++;
            if(Counter >= 60) {
                Counter = 0;
            }

            Sleep(14);
       }

      desactiverOpenGL (hwnd, hDC, hRC);
      DestroyWindow(hwnd);

      return 0;
 }

LRESULT CALLBACK WindowProc (HWND   fenetrePrincipale, UINT   message,
                             WPARAM wParam,            LPARAM lParam)
 {
  switch (message)
   {
       case WM_KEYDOWN:
           switch(wParam) {
                case VK_LEFT:
                isLeftArrowPressed = true;
                break;

                case VK_RIGHT:
                isRightArrowPressed = true;
                break;

                case VK_UP:
                isUpArrowPressed = true;
                break;

                case VK_DOWN:
                isDownArrowPressed = true;
                break;
           }
           break;

        case WM_KEYUP:
           switch(wParam) {
                case VK_LEFT:
                isLeftArrowPressed = false;
                break;

                case VK_RIGHT:
                isRightArrowPressed = false;
                break;

                case VK_UP:
                isUpArrowPressed = false;
                break;

                case VK_DOWN:
                isDownArrowPressed = false;
                break;
           }
           break;

        case WM_CREATE  : return 0;
        break;

        case WM_CLOSE:
        case WM_DESTROY : {
            PostQuitMessage (0);
            return 0;
        }
        break;

        default         : return DefWindowProc(fenetrePrincipale, message,
                                               wParam,            lParam);
                                               break;
   }
 }

 void loadLevel() {
     std::ifstream levelFile("laby.txt", std::ios::in | std::ios::binary);
     if(!levelFile.fail()) {
         char tempCharacter;
         unsigned currentColumn = 0, currentRow = 0;
         while(levelFile.good()) {
             levelFile.read(&tempCharacter, sizeof(char));

             //Pour ignorer des charactere est des lignes en trop
             if((currentRow >= NUM_BLOCKS || currentColumn >=NUM_BLOCKS) && tempCharacter != 10)
                continue;

             switch(tempCharacter) {
                //space
                case ' ':
                    Blocks[currentRow][currentColumn].isMur = false;
                    currentRow++;
                    break;

                //mur
                case '#':
                    Blocks[currentRow][currentColumn].isMur = true;
                    currentRow++;
                    break;

                //position de depart
                case 'P':
                    pacmanPositionBlockX = currentRow;
                    pacmanPositionBlockZ = currentColumn;

                    pacmanLastPositionBlockX = pacmanPositionBlockX;
                    pacmanLastPositionBlockZ = pacmanPositionBlockZ;
                    currentRow++;
                    break;

                case 10:
                    currentColumn++;
                    currentRow = 0;
                    break;

                //ignore le reste
                default:
                    break;
             }
         }
         std::cout << " Le jeu est en cours de chargement..." << std::endl;
     } else {
         std::cout << " Erreur de lecture du fichier laby.txt" << std::endl;
     }

    //rempli les espaces vides avec des pastilles
     for(unsigned j = 0; j < NUM_BLOCKS; j++) {
        for(unsigned k = 0; k < NUM_BLOCKS; k++) {
            Blocks[j][k].pastille = !Blocks[j][k].isMur;
        }
     }
 }

void desactiverOpenGL (HWND hwnd, HDC hDC, HGLRC hRC)
 {
  wglMakeCurrent   (NULL, NULL);
  wglDeleteContext (hRC);
  ReleaseDC        (hwnd, hDC);
 }

void activerOpenGL (HWND hwnd, HDC* hDC, HGLRC* hRC)
 {
    *hDC = GetDC (hwnd);

    PIXELFORMATDESCRIPTOR pfd;

    ZeroMemory (&pfd, sizeof(pfd));

    pfd.nSize      = sizeof(pfd);
    pfd.nVersion   = 1;
    pfd.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |
                     PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    int iFormat = ChoosePixelFormat (*hDC, &pfd);

    SetPixelFormat (*hDC, iFormat, &pfd);

    *hRC = wglCreateContext(*hDC);

    wglMakeCurrent (*hDC, *hRC);

    glClearColor(0.25f, 0.25f, 0.30f, 1.0f);
	glClearDepth(1.0f);

    glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glLineWidth(3.0f);

    glEnable(GL_LIGHT1);
    glLightfv(GL_LIGHT1, GL_SPECULAR, materialWhite);

    GLfloat LightAmbient[]=		{ 0.5f, 0.5f, 0.5f, 1.0f };
    glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);

    GLfloat LightDiffuse[]=		{ 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);

    GLfloat materialSpecular[] = {0.53f, 0.53f, 0.52f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  materialSpecular);

    GLfloat materialShininess[] = {80.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS,  materialShininess);

    loadLevel();
 }

void updateMovement() {
    //active le mouvement
    if(isLeftArrowPressed) {
       if(pacmanPositionBlockX > 0 && !isPacmanMoving) { //si on peut allez a gauche
            if(Blocks[pacmanPositionBlockX - 1][pacmanPositionBlockZ].isMur == false) { //verifie si il y a un mur
               pacmanPositionBlockX--; //mouvement
               pacmanMovementInterpolation = 0.0f; // reinitilaise le mouvement
               isPacmanMoving = true;
            }
           pacmanDirection = 2; //ouest
       }
    }

    if(isRightArrowPressed) {
        if((pacmanPositionBlockX < NUM_BLOCKS - 1) && !isPacmanMoving) {
            if(Blocks[pacmanPositionBlockX + 1][pacmanPositionBlockZ].isMur == false) {
                pacmanPositionBlockX++;
                pacmanMovementInterpolation = 0.0f;
                isPacmanMoving = true;
            }
            pacmanDirection = 0; // est
        }
    }

    if(isUpArrowPressed) {
        if(pacmanPositionBlockZ > 0 && !isPacmanMoving) {
            if(Blocks[pacmanPositionBlockX][pacmanPositionBlockZ - 1].isMur == false) {
                pacmanPositionBlockZ--;
                pacmanMovementInterpolation = 0.0f;
                isPacmanMoving = true;
            }
            pacmanDirection = 1; // nord
        }
    }

    if(isDownArrowPressed) {
        if((pacmanPositionBlockZ < NUM_BLOCKS - 1) && !isPacmanMoving) {
            if(Blocks[pacmanPositionBlockX][pacmanPositionBlockZ + 1].isMur == false) {
                pacmanPositionBlockZ++;
                pacmanMovementInterpolation = 0.0f;
                isPacmanMoving = true;
            }
            pacmanDirection = 3; // sud
        }
    }

    //Met a jour la valeur du mouvement
    if(isPacmanMoving)
        pacmanMovementInterpolation += pacmanMovementSpeed;

    if(pacmanMovementInterpolation >= 1.0f) { //si l'interpolation est finit
        pacmanLastPositionBlockX = pacmanPositionBlockX; //met a jour la derniere position
        pacmanLastPositionBlockZ = pacmanPositionBlockZ;

        pacmanMovementInterpolation = 0.0f;   //reinitialise
        isPacmanMoving = false;               //mouvement terminé
    }

    //melange précédent et position actuelle, basée sur la valeur d'interpolation pour un mouvement continu entre les blocs
    float interpolatedPositionX = pacmanLastPositionBlockX * (1.0f - pacmanMovementInterpolation) + pacmanPositionBlockX * pacmanMovementInterpolation;
    float interpolatedPositionZ = pacmanLastPositionBlockZ * (1.0f - pacmanMovementInterpolation) + pacmanPositionBlockZ * pacmanMovementInterpolation;

    //Met a jour le dessin pour pacman
    pacmanDrawPositionX = -LEVEL_SIZE_HALF + BLOCK_SIZE * interpolatedPositionX + BLOCK_SIZE * 0.5f;
    pacmanDrawPositionZ = -LEVEL_SIZE_HALF + BLOCK_SIZE * interpolatedPositionZ + BLOCK_SIZE * 0.5f;

    Blocks[pacmanLastPositionBlockX][pacmanLastPositionBlockZ].pastille = false; //Supprime la pastille lorsque le pacman va dessus
}

 void dessiner (HDC* hDC)
 {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity();
    gluPerspective (70, 1, 1, 95);

    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(pacmanDrawPositionX, 8.0f, pacmanDrawPositionZ + 10.0f, pacmanDrawPositionX, 1.0f, pacmanDrawPositionZ, 0, 1, 0);

    glLightfv(GL_LIGHT1, GL_POSITION, lightPos);

    updateMovement();

    drawBase(); //dessine le sol

    //dessine tout les blocks
    for(unsigned column = 0; column < NUM_BLOCKS; column++) {
        for(unsigned row = 0; row < NUM_BLOCKS; row++) {
            Blocks[column][row].Draw(column, row);
        }
    }

    drawPacman();

    SwapBuffers(*hDC);
 }

