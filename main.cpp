#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <GL/glut.h>

// =============================================
//  СОСТОЯНИЯ ИГРЫ
// =============================================
enum GameState { MENU, PLAYING, WON };
GameState gameState = MENU;
int selectedMaze = 0;  // 0,1,2

// =============================================
//  ЛАБИРИНТЫ
// =============================================
const int MAP_W = 15;
const int MAP_H = 15;

// 0=проход, 1=стена, 2=выход
int mazes[3][MAP_H][MAP_W] = {
    // Лабиринт 1 — лёгкий
    {
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,1,1,1,1,1,0,1,1,1,1,1,0,1},
        {1,0,1,0,0,0,1,0,1,0,0,0,1,0,1},
        {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1},
        {1,0,1,0,1,0,0,0,0,0,1,0,1,0,1},
        {1,0,1,0,1,1,1,1,1,1,1,0,1,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,1,1,1,0,1,1,1,0,1,1,1,0,1},
        {1,0,0,0,1,0,0,0,1,0,0,0,0,0,1},
        {1,1,1,0,1,1,1,0,1,1,1,0,1,1,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,1,1,1,1,1,0,1,1,1,1,1,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,2,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    },
    // Лабиринт 2 — средний
    {
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,0,0,0,0,0,1,0,0,0,0,0,0,0,1},
        {1,0,1,1,0,1,1,0,1,1,1,0,1,0,1},
        {1,0,1,0,0,0,0,0,0,0,1,0,1,0,1},
        {1,0,1,0,1,1,1,1,1,0,1,0,0,0,1},
        {1,0,0,0,1,0,0,0,1,0,0,0,1,0,1},
        {1,1,1,0,1,0,1,0,1,1,1,1,1,0,1},
        {1,0,0,0,0,0,1,0,0,0,0,0,0,0,1},
        {1,0,1,1,1,1,1,1,1,0,1,1,1,0,1},
        {1,0,0,0,0,0,0,0,1,0,0,0,1,0,1},
        {1,0,1,1,1,0,1,0,1,1,1,0,1,0,1},
        {1,0,1,0,0,0,1,0,0,0,0,0,0,0,1},
        {1,0,1,0,1,1,1,1,1,0,1,1,1,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,2,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    },
    // Лабиринт 3 — сложный
    {
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,0,1,0,1,0,0,0,1,0,0,0,1,0,1},
        {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1},
        {1,0,0,0,0,0,1,0,0,0,1,0,0,0,1},
        {1,1,1,1,1,0,1,1,1,0,1,1,1,0,1},
        {1,0,0,0,1,0,0,0,1,0,0,0,0,0,1},
        {1,0,1,0,1,1,1,0,1,1,1,1,1,0,1},
        {1,0,1,0,0,0,0,0,0,0,0,0,1,0,1},
        {1,0,1,1,1,1,1,1,1,0,1,0,1,0,1},
        {1,0,0,0,0,0,1,0,0,0,1,0,0,0,1},
        {1,1,1,0,1,0,1,0,1,1,1,1,1,0,1},
        {1,0,0,0,1,0,0,0,1,0,0,0,0,0,1},
        {1,0,1,1,1,1,1,0,1,0,1,1,1,1,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,2,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    }
};

// текущий активный лабиринт
int maze[MAP_H][MAP_W];

// =============================================
//  ИГРОК
// =============================================
float playerX     = 1.5f;
float playerZ     = 1.5f;
float playerAngle = 0.0f;
const float moveSpeed = 0.06f;
const float rotSpeed  = 2.5f;
const float MARGIN    = 0.25f;
const float WALL_H    = 1.0f;
const float EYE_H     = 0.5f;

bool keys[256] = {};
bool won = false;
float torchTime = 0.0f;

// =============================================
//  КОЛЛИЗИЯ
// =============================================
bool isWall(float x, float z) {
    int cx = (int)x, cz = (int)z;
    if (cx < 0 || cx >= MAP_W || cz < 0 || cz >= MAP_H) return true;
    return maze[cz][cx] == 1;
}
bool collidesAt(float x, float z) {
    return isWall(x-MARGIN,z-MARGIN)||isWall(x+MARGIN,z-MARGIN)
        || isWall(x-MARGIN,z+MARGIN)||isWall(x+MARGIN,z+MARGIN);
}
void checkFinish() {
    int cx=(int)playerX, cz=(int)playerZ;
    if (cx>=0&&cx<MAP_W&&cz>=0&&cz<MAP_H&&maze[cz][cx]==2) won=true;
}

// =============================================
//  РИСОВАНИЕ — стены с нормалями
// =============================================
void wallQuad(float x1,float y1,float z1,
              float x2,float y2,float z2,
              float x3,float y3,float z3,
              float x4,float y4,float z4,
              float nx,float ny,float nz) {
    glNormal3f(nx,ny,nz);
    glBegin(GL_QUADS);
        glVertex3f(x1,y1,z1); glVertex3f(x2,y2,z2);
        glVertex3f(x3,y3,z3); glVertex3f(x4,y4,z4);
    glEnd();
}

void drawWallCell(int mx, int mz) {
    float x=(float)mx, z=(float)mz, h=WALL_H;
    if (mz>0 && maze[mz-1][mx]!=1) {
        glColor3f(0.8f,0.35f,0.35f);
        wallQuad(x,0,z, x+1,0,z, x+1,h,z, x,h,z,  0,0,-1);
    }
    if (mz<MAP_H-1 && maze[mz+1][mx]!=1) {
        glColor3f(0.7f,0.28f,0.28f);
        wallQuad(x+1,0,z+1, x,0,z+1, x,h,z+1, x+1,h,z+1,  0,0,1);
    }
    if (mx>0 && maze[mz][mx-1]!=1) {
        glColor3f(0.5f,0.28f,0.65f);
        wallQuad(x,0,z+1, x,0,z, x,h,z, x,h,z+1,  -1,0,0);
    }
    if (mx<MAP_W-1 && maze[mz][mx+1]!=1) {
        glColor3f(0.45f,0.22f,0.60f);
        wallQuad(x+1,0,z, x+1,0,z+1, x+1,h,z+1, x+1,h,z,  1,0,0);
    }
}

void drawMaze() {
    for (int z=0;z<MAP_H;z++)
        for (int x=0;x<MAP_W;x++) {
            if (maze[z][x]==1) drawWallCell(x,z);
            if (maze[z][x]==2) {
                glColor3f(0.0f,0.7f,0.2f);
                glNormal3f(0,1,0);
                glBegin(GL_QUADS);
                    glVertex3f(x+0.05f,0.01f,z+0.05f);
                    glVertex3f(x+0.95f,0.01f,z+0.05f);
                    glVertex3f(x+0.95f,0.01f,z+0.95f);
                    glVertex3f(x+0.05f,0.01f,z+0.95f);
                glEnd();
            }
        }
}

void drawFloorCeiling() {
    float w=(float)MAP_W, d=(float)MAP_H;
    glColor3f(0.22f,0.22f,0.22f);
    glNormal3f(0,1,0);
    glBegin(GL_QUADS);
        glVertex3f(0,0,0); glVertex3f(w,0,0);
        glVertex3f(w,0,d); glVertex3f(0,0,d);
    glEnd();
    glColor3f(0.10f,0.10f,0.18f);
    glNormal3f(0,-1,0);
    glBegin(GL_QUADS);
        glVertex3f(0,WALL_H,0); glVertex3f(w,WALL_H,0);
        glVertex3f(w,WALL_H,d); glVertex3f(0,WALL_H,d);
    glEnd();
}

// =============================================
//  ФАКЕЛ
// =============================================
void setupTorch() {
    float f = 0.80f + 0.12f*sinf(torchTime*11.3f)
                    + 0.08f*sinf(torchTime*23.7f)
                    + 0.04f*sinf(torchTime*41.1f);

    GLfloat pos[]  = { playerX, EYE_H+0.1f, playerZ, 1.0f };
    GLfloat diff[] = { f*1.0f, f*0.6f, f*0.15f, 1.0f };
    GLfloat amb[]  = { 0.06f, 0.03f, 0.01f, 1.0f };
    GLfloat spec[] = { 0.2f, 0.1f, 0.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, pos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  diff);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  amb);
    glLightfv(GL_LIGHT0, GL_SPECULAR, spec);
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION,  0.3f);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION,    0.6f);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.2f);
}

// =============================================
//  ТУМАН
// =============================================
void setupFog() {
    GLfloat fogCol[] = {0.0f,0.0f,0.0f,1.0f};
    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_EXP2);
    glFogfv(GL_FOG_COLOR, fogCol);
    glFogf(GL_FOG_DENSITY, 0.30f);
    glHint(GL_FOG_HINT, GL_NICEST);
}

// =============================================
//  2D ВСПОМОГАЛКА
// =============================================
void begin2D() {
    int w=glutGet(GLUT_WINDOW_WIDTH), h=glutGet(GLUT_WINDOW_HEIGHT);
    glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
    glOrtho(0,w,0,h,-1,1);
    glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_FOG);
}
void end2D() {
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION); glPopMatrix();
    glMatrixMode(GL_MODELVIEW);  glPopMatrix();
}

void drawText(float x, float y, const char* s, void* font=GLUT_BITMAP_HELVETICA_18) {
    glRasterPos2f(x, y);
    for (int i=0; s[i]; i++) glutBitmapCharacter(font, s[i]);
}

// =============================================
//  МИНИКАРТА
// =============================================
void drawMinimap() {
    begin2D();
    int winH=glutGet(GLUT_WINDOW_HEIGHT);
    const int T=7, OX=10, OY=winH-MAP_H*T-10;
    for (int z=0;z<MAP_H;z++) for (int x=0;x<MAP_W;x++) {
        if      (maze[z][x]==1) glColor3f(0.5f,0.5f,0.5f);
        else if (maze[z][x]==2) glColor3f(0.0f,0.9f,0.3f);
        else                    glColor3f(0.08f,0.08f,0.12f);
        glBegin(GL_QUADS);
            glVertex2f(OX+x*T,    OY+z*T);
            glVertex2f(OX+(x+1)*T,OY+z*T);
            glVertex2f(OX+(x+1)*T,OY+(z+1)*T);
            glVertex2f(OX+x*T,    OY+(z+1)*T);
        glEnd();
    }
    float px=OX+playerX*T, pz=OY+playerZ*T;
    glColor3f(1.0f,0.3f,0.3f);
    glBegin(GL_QUADS);
        glVertex2f(px-2,pz-2); glVertex2f(px+2,pz-2);
        glVertex2f(px+2,pz+2); glVertex2f(px-2,pz+2);
    glEnd();
    end2D();
}

// =============================================
//  ПРИЦЕЛ
// =============================================
void drawCrosshair() {
    begin2D();
    int cx=glutGet(GLUT_WINDOW_WIDTH)/2;
    int cy=glutGet(GLUT_WINDOW_HEIGHT)/2;
    glColor3f(1,1,1); glLineWidth(1.5f);
    glBegin(GL_LINES);
        glVertex2f(cx-10,cy); glVertex2f(cx+10,cy);
        glVertex2f(cx,cy-10); glVertex2f(cx,cy+10);
    glEnd();
    end2D();
}

// =============================================
//  ЭКРАН ПОБЕДЫ
// =============================================
void drawWinScreen() {
    begin2D();
    int w=glutGet(GLUT_WINDOW_WIDTH), h=glutGet(GLUT_WINDOW_HEIGHT);
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0,0.15f,0,0.7f);
    glBegin(GL_QUADS);
        glVertex2f(0,0); glVertex2f(w,0);
        glVertex2f(w,h); glVertex2f(0,h);
    glEnd();
    glDisable(GL_BLEND);
    glColor3f(0.2f,1.0f,0.4f);
    drawText(w/2-70, h/2+15, "ВЫХОД НАЙДЕН!", GLUT_BITMAP_TIMES_ROMAN_24);
    glColor3f(0.7f,0.7f,0.7f);
    drawText(w/2-55, h/2-20, "ESC — выйти", GLUT_BITMAP_HELVETICA_18);
    end2D();
}

// =============================================
//  ГЛАВНОЕ МЕНЮ
// =============================================
void drawMenu() {
    int w=glutGet(GLUT_WINDOW_WIDTH), h=glutGet(GLUT_WINDOW_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    begin2D();

    // фон
    glColor3f(0.05f,0.05f,0.1f);
    glBegin(GL_QUADS);
        glVertex2f(0,0); glVertex2f(w,0);
        glVertex2f(w,h); glVertex2f(0,h);
    glEnd();

    // заголовок
    glColor3f(0.9f,0.6f,0.1f);
    drawText(w/2-90, h-120, "3D  ЛАБИРИНТ", GLUT_BITMAP_TIMES_ROMAN_24);

    // пункты
    const char* items[] = {
        "1.  Легкий",
        "2.  Средний",
        "3.  Сложный"
    };
    for (int i=0;i<3;i++) {
        if (i==selectedMaze)
            glColor3f(0.2f,1.0f,0.4f);   // выбранный — зелёный
        else
            glColor3f(0.7f,0.7f,0.7f);
        drawText(w/2-60, h/2+40 - i*40, items[i], GLUT_BITMAP_HELVETICA_18);
    }

    // подсказка
    glColor3f(0.4f,0.4f,0.4f);
    drawText(w/2-110, 60, "W/S — выбор   |   ENTER — старт", GLUT_BITMAP_HELVETICA_12);
    drawText(w/2-80,  40, "WASD — движение   QE — стрейф", GLUT_BITMAP_HELVETICA_12);

    end2D();
    glutSwapBuffers();
}

// =============================================
//  СТАРТ ИГРЫ
// =============================================
void startGame() {
    memcpy(maze, mazes[selectedMaze], sizeof(maze));
    playerX     = 1.5f;
    playerZ     = 1.5f;
    playerAngle = 0.0f;
    won         = false;
    torchTime   = 0.0f;
    gameState   = PLAYING;
}

// =============================================
//  ОТОБРАЖЕНИЕ
// =============================================
void display() {
    if (gameState==MENU) { drawMenu(); return; }

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    float rad=playerAngle*(float)M_PI/180.0f;
    gluLookAt(playerX,EYE_H,playerZ,
              playerX+cosf(rad),EYE_H,playerZ+sinf(rad),
              0,1,0);

    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
    setupTorch();
    setupFog();

    drawFloorCeiling();
    drawMaze();

    drawMinimap();
    drawCrosshair();
    if (won) drawWinScreen();

    glutSwapBuffers();
}

// =============================================
//  ОБНОВЛЕНИЕ
// =============================================
void update(int) {
    if (gameState==PLAYING && !won) {
        float rad=playerAngle*(float)M_PI/180.0f;
        float dx=cosf(rad)*moveSpeed, dz=sinf(rad)*moveSpeed;
        float sx=cosf(rad+(float)M_PI*0.5f)*moveSpeed;
        float sz=sinf(rad+(float)M_PI*0.5f)*moveSpeed;

        if (keys['w']||keys['W']) {
            if (!collidesAt(playerX+dx,playerZ)) playerX+=dx;
            if (!collidesAt(playerX,playerZ+dz)) playerZ+=dz;
        }
        if (keys['s']||keys['S']) {
            if (!collidesAt(playerX-dx,playerZ)) playerX-=dx;
            if (!collidesAt(playerX,playerZ-dz)) playerZ-=dz;
        }
        if (keys['q']||keys['Q']) {
            if (!collidesAt(playerX-sx,playerZ)) playerX-=sx;
            if (!collidesAt(playerX,playerZ-sz)) playerZ-=sz;
        }
        if (keys['e']||keys['E']) {
            if (!collidesAt(playerX+sx,playerZ)) playerX+=sx;
            if (!collidesAt(playerX,playerZ+sz)) playerZ+=sz;
        }
        if (keys['a']||keys['A']) playerAngle-=rotSpeed;
        if (keys['d']||keys['D']) playerAngle+=rotSpeed;
        checkFinish();
    }
    torchTime+=0.016f;
    glutPostRedisplay();
    glutTimerFunc(16,update,0);
}

// =============================================
//  ВВОД
// =============================================
void keyDown(unsigned char key, int, int) {
    keys[key]=true;
    if (key==27) exit(0);

    if (gameState==MENU) {
        if (key=='w'||key=='W') selectedMaze=(selectedMaze+2)%3;
        if (key=='s'||key=='S') selectedMaze=(selectedMaze+1)%3;
        if (key==13) startGame();  // ENTER
        if (key=='1') { selectedMaze=0; startGame(); }
        if (key=='2') { selectedMaze=1; startGame(); }
        if (key=='3') { selectedMaze=2; startGame(); }
    }
}
void keyUp(unsigned char key,int,int) { keys[key]=false; }

// =============================================
//  ОКНО
// =============================================
void reshape(int w,int h) {
    if (h==0) h=1;
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    gluPerspective(70.0,(double)w/h,0.05,100.0);
    glMatrixMode(GL_MODELVIEW);
}

void init() {
    glClearColor(0,0,0,1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);
}

int main(int argc,char** argv) {
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
    glutInitWindowSize(960,640);
    glutCreateWindow("3D Labirint");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyDown);
    glutKeyboardUpFunc(keyUp);
    glutTimerFunc(16,update,0);
    glutMainLoop();
    return 0;
}
