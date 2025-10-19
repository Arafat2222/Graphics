#ifdef __APPLE__
  #include <GLUT/glut.h>
#else
  #include <GL/glut.h>
#endif

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// ---------- constants / helpers ----------
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#define false 0
#define true  1

const int BOARD_X = 31;   // rows
const int BOARD_Y = 28;   // cols

// ---------- GAME STATE ----------
enum GameState { STATE_MENU, STATE_PLAY, STATE_PAUSE, STATE_GAMEOVER, STATE_WIN };
GameState gState = STATE_MENU;
int menuIndex = 0; // 0: Start/Resume, 1: Help, 2: Exit

// ---------- MAP (walls/tiles) ----------
int board_array[BOARD_X][BOARD_Y] = {
    {8,5,5,5,5,5,5,5,5,5,5,5,5,1,1,5,5,5,5,5,5,5,5,5,5,5,5,7},
    {6,0,0,0,0,0,0,0,0,0,0,0,0,2,4,0,0,0,0,0,0,0,0,0,0,0,0,6},
    {6,0,8,1,1,7,0,8,1,1,1,7,0,2,4,0,8,1,1,1,7,0,8,1,1,7,0,6},
    {6,0,2,11,11,4,0,2,11,11,11,4,0,2,4,0,2,11,11,11,4,0,2,11,11,4,0,6},
    {6,0,9,3,3,10, 0,9,3,3,3,10,0,9,10,0,9,3,3,3,10,0,9,3,3,10,0,6},
    {6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
    {6,0,8,1,1,7,0,8,7,0,8,1,1,1,1,1,1,7,0,8,7,0,8,1,1,7,0,6},
    {6,0,9,3,3,10,0,2,4,0,9,3,3,11,11,3,3,10,0,2,4,0,9,3,3,10,0,6},
    {6,0,0,0,0,0,0,2,4,0,0,0,0,2,4,0,0,0,0,2,4,0,0,0,0,0,0,6},
    {9,5,5,5,5,7,0,2,11,1,1,7,0,2,4,0,8,1,1,11,4,0,8,5,5,5,5,10},
    {0,0,0,0,0,6,0,2,11,3,3,10,0,9,10,0,9,3,3,11,4,0,6,0,0,0,0,0},
    {0,0,0,0,0,6,0,2,4,0,0,0,0,0,0,0,0,0,0,2,4,0,6,0,0,0,0,0},
    {0,0,0,0,0,6,0,2,4,0,8,5,5,1,1,5,5,7,0,2,4,0,6,0,0,0,0,0},
    {5,5,5,5,5,10,0,9,10,0,6,0,0,0,0,0,0,6,0,9,10,0,9,5,5,5,5,5},
    {0,0,0,0,0,0,0,0,0,0,6,0,0,0,0,0,0,6,0,0,0,0,0,0,0,0,0,0},
    {5,5,5,5,5,7,0,8,7,0,6,0,0,0,0,0,0,6,0,8,7,0,8,5,5,5,5,5},
    {0,0,0,0,0,6,0,2,4,0,9,5,5,5,5,5,5,10,0,2,4,0,6,0,0,0,0,0},
    {0,0,0,0,0,6,0,2,4,0,0,0,0,0,0,0,0,0,0,2,4,0,6,0,0,0,0,0},
    {0,0,0,0,0,6,0,2,4,0,8,1,1,1,1,1,1,7,0,2,4,0,6,0,0,0,0,0},
    {8,5,5,5,5,10,0,9,10,0,9,3,3,11,11,3,3,10,0,9,10,0,9,5,5,5,5,7},
    {6,0,0,0,0,0,0,0,0,0,0,0,0,2,4,0,0,0,0,0,0,0,0,0,0,0,0,6},
    {6,0,8,1,1,7,0,8,1,1,1,7,0,2,4,0,8,1,1,1,7,0,8,1,1,7,0,6},
    {6,0,9,3,11,4,0,9,3,3,3,10,0,9,10,0,9,3,3,3,10,0,2,11,3,10,0,6},
    {6,0,0,0,2,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,4,0,0,0,6},
    {2,1,7,0,2,4,0,8,7,0,8,1,1,1,1,1,1,7,0,8,7,0,2,4,0,8,1,4},
    {2,3,10,0,9,10,0,2,4,0,9,3,3,11,11,3,3,10,0,2,4,0,9,10,0,9,3,4},
    {6,0,0,0,0,0,0,2,4,0,0,0,0,2,4,0,0,0,0,2,4,0,0,0,0,0,0,6},
    {6,0,8,1,1,1,1,11,11,1,1,7,0,2,4,0,8,1,1,11,11,1,1,1,1,7,0,6},
    {6,0,9,3,3,3,3,3,3,3,3,10,0,9,10,0,9,3,3,3,3,3,3,3,3,10,0,6},
    {6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
    {9,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,10}
};

// ---------- PELLETS (1 = small, 3 = power) ----------
int pebble_array[BOARD_X][BOARD_Y] = {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0},
    {0,1,0,0,0,0,1,0,0,0,0,0,1,0,0,1,0,0,0,0,0,1,0,0,0,0,1,0},
    {0,3,0,0,0,0,1,0,0,0,0,0,1,0,0,1,0,0,0,0,0,1,0,0,0,0,3,0},
    {0,1,0,0,0,0,1,0,0,0,0,0,1,0,0,1,0,0,0,0,0,1,0,0,0,0,1,0},
    {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
    {0,1,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,1,0},
    {0,1,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,1,0},
    {0,1,1,1,1,1,1,0,0,1,1,1,1,0,0,1,1,1,1,0,0,1,1,1,1,1,1,0},
    {0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0},
    {0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0},
    {0,1,0,0,0,0,1,0,0,0,0,0,1,0,0,1,0,0,0,0,0,1,0,0,0,0,1,0},
    {0,1,0,0,0,0,1,0,0,0,0,0,1,0,0,1,0,0,0,0,0,1,0,0,0,0,1,0},
    {0,3,1,1,0,0,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,0,0,1,1,3,0},
    {0,0,0,1,0,0,1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1,0,0,1,0,0,0},
    {0,0,0,1,0,0,1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1,0,0,1,0,0,0},
    {0,1,1,1,1,1,1,0,0,1,1,1,1,0,0,1,1,1,1,0,0,1,1,1,1,1,1,0},
    {0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0},
    {0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0},
    {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

// ---------- globals ----------
GLubyte listDisp[5];
int tp_array[BOARD_X][BOARD_Y];
int respawn_timer[BOARD_X][BOARD_Y];           // kept for compatibility; no longer used
int pebbles_left = 0;

// respawn disabled: constants removed
// const int RESPAWN_SMALL_FRAMES = 900;
// const int RESPAWN_POWER_FRAMES = 1500;

double speed1 = 0.025;     // Pac-Man speed
double angle1 = 90;
double a = 13.5, b = 23;   // Pac-Man grid position
bool animate = false;

int lives = 3;
int points = 0;
unsigned char ckey = 'w';

// --- timing ---
int gameStartMs = 0;       // set on startGame()
int winElapsedMs = 0;      // captured on win
int gameOverElapsedMs = 0; // captured on game over

bool Open(int a,int b);
void Move();
void Pac(void);
void mykey(unsigned char key,int x,int y);
void P_Reinit();
void G_Reinit(void);
void tp_restore(void);
void RenderScene();
void specialDown(int key,int x,int y);
void specialUp(int key,int y,int z);
void create_list_lib();

// ---------- Fruits (UPDATED: stay until eaten; spawn every 30s) ----------
enum FruitType { FRUIT_NONE=0, FRUIT_APPLE=1, FRUIT_ORANGE=2 };
struct Fruit {
    bool active;
    FruitType type;
    int r, c;   // grid row/col
};

const int FRUIT_MAX = 12;              // allow multiple fruits to accumulate
Fruit fruits[FRUIT_MAX];
int nextFruitSpawnMs = 0;               // GLUT_ELAPSED_TIME target (ms)
const int FRUIT_SPAWN_INTERVAL_MS = 30000;  // 30 seconds

static  int rndRange(int lo,int hi){ return lo + (rand() % (hi-lo+1)); }

void fruitsInit(){
    for (int i=0;i<FRUIT_MAX;i++){ fruits[i].active=false; fruits[i].type=FRUIT_NONE; fruits[i].r=fruits[i].c=0; }
    nextFruitSpawnMs = glutGet(GLUT_ELAPSED_TIME) + FRUIT_SPAWN_INTERVAL_MS;
}

bool fruitCellOccupied(int r,int c){
    for (int i=0;i<FRUIT_MAX;i++){
        if (fruits[i].active && fruits[i].r==r && fruits[i].c==c) return true;
    }
    return false;
}
bool isCellEmptyForFruit(int r,int c){
    if (r<0||r>=BOARD_X||c<0||c>=BOARD_Y) return false;
    if (board_array[r][c] > 0) return false;          // wall
    if (r>=10 && r<=14 && c>=12 && c<=15) return false; // avoid jail
    if (fruitCellOccupied(r,c)) return false;
    return true;
}
void spawnOneFruit(){
    int idx=-1;
    for (int i=0;i<FRUIT_MAX;i++) if (!fruits[i].active){ idx=i; break; }
    if (idx<0) return; // pool full

    for (int tries=0; tries<400; ++tries){
        int r = rndRange(0, BOARD_X-1);
        int c = rndRange(0, BOARD_Y-1);
        if (!isCellEmptyForFruit(r,c)) continue;
        fruits[idx].r=r; fruits[idx].c=c;
        fruits[idx].type = (rand()%2==0)? FRUIT_APPLE : FRUIT_ORANGE;
        fruits[idx].active = true;
        return;
    }
}
void updateFruitSpawner(){
    if (gState==STATE_MENU) return;
    int now = glutGet(GLUT_ELAPSED_TIME);
    if (now >= nextFruitSpawnMs){
        spawnOneFruit();
        nextFruitSpawnMs = now + FRUIT_SPAWN_INTERVAL_MS;
    }
}
void drawAllFruits(){
    for (int i=0;i<FRUIT_MAX;i++){
        if (!fruits[i].active) continue;
        glPushMatrix();
        glTranslatef(-(float)BOARD_X/2.0f, -(float)BOARD_Y/2.0f, 0);
        glTranslatef(fruits[i].c, BOARD_Y - fruits[i].r, 0);
        glTranslatef(0.5f, 0.5f, 0.7f); // above pellets
        if (fruits[i].type == FRUIT_APPLE){
            glColor3f(1.0f, 0.0f, 0.0f);           // apple
            glutSolidSphere(0.18, 18, 18);
            glColor3f(0.0f, 0.8f, 0.0f);
            glBegin(GL_TRIANGLES);
              glVertex3f(0.0f, 0.22f, 0.0f);
              glVertex3f(0.10f, 0.28f, 0.0f);
              glVertex3f(0.02f, 0.10f, 0.0f);
            glEnd();
        } else {
            glColor3f(1.0f, 0.5f, 0.0f);           // orange
            glutSolidSphere(0.20, 18, 18);
        }
        glPopMatrix();
    }
}
void checkFruitsCollision(){
    int rr = (int)(b+0.5);
    int cc = (int)(a+0.5);
    for (int i=0;i<FRUIT_MAX;i++){
        if (!fruits[i].active) continue;
        if (rr==fruits[i].r && cc==fruits[i].c){
            if (fruits[i].type == FRUIT_APPLE) lives += 2;
            else                                lives += 3;
            fruits[i].active = false; // vanish only when eaten
        }
    }
}

// ---------- Ghosts ----------
bool gameover = false;
int num_ghosts = 3;    // 3 ghosts only
int start_timer = 3;

class Ghost {
  public:
    bool edible; int edible_max_time; int edible_timer;
    bool eaten;  bool transporting;
    float color[3];
    double speed, max_speed;
    bool in_jail; int jail_timer;
    double angle; double x,y;

    Ghost(double, double);
    ~Ghost(void);
    void Move();
    void Update(void);
    void Chase(double, double, bool*);
    bool Catch(double, double);
    void Reinit(void);
    void Vulnerable(void);
    void Draw(void);
    void game_over(void);
};

Ghost *ghost[4];

Ghost::~Ghost(void){}
Ghost::Ghost(double tx,double ty){
    x=tx; y=ty;
    angle=90; speed=max_speed=0.02; // slower base
    color[0]=1; color[1]=0; color[2]=0;
    eaten=false; edible_max_time=300; edible=false; edible_timer=0;
    in_jail=true; jail_timer=30; transporting=false;
}
void Ghost::Reinit(void){ edible=false; in_jail=true; angle=90; transporting=false; edible_timer=0; eaten=false; }
void Ghost::Move(){ x+=speed*cos(M_PI/180*angle); y+=speed*sin(M_PI/180*angle); }
void Ghost::game_over(){}

void Ghost::Update(void){
    // tunnel wrapping
    if ((int)x==0 && (int)y==14 && !transporting) angle=180;
    if (x<0.1 && (int)y==14){ x=26.9; transporting=true; }
    if ((int)x==27 && (int)y==14 && !transporting) angle=0;
    if (x>26.9 && (int)y==14){ x=0.1; transporting=true; }
    if ((int)x==2 || (int)x==25) transporting=false;

    // edible timer
    if (edible_timer==0 && edible && !eaten){ edible=false; speed=max_speed; }
    if (edible && edible_timer>0) edible_timer--;

    // jail logic
    if (in_jail && (int)(y+0.9)==11){ in_jail=false; angle=180; }
    if (in_jail && ((int)x==13 || (int)x==14)) angle=270;

    if (jail_timer==0 && in_jail){
        if (x<13) angle=0;
        if (x>14) angle=180;
    }
    if (jail_timer>0) jail_timer--;

    // return to jail when eaten
    if (eaten && ((int)x==13 || (int)(x+0.9)==14) && ((int)y>10 && (int)y<15)){
        in_jail=true; angle=90;
        if ((int)y==14){ eaten=false; speed=max_speed; jail_timer=66; x=11; }
    }
}
bool Ghost::Catch(double px,double py){ return (fabs(px-x)<0.2 && fabs(py-y)<0.2); }
void Ghost::Vulnerable(void){
    if (!edible){ angle=((int)angle+180)%360; speed=max_speed; }
    edible=true; edible_timer=edible_max_time;
}

static void drawGhost2D(float r,float g,float b,double dirDeg){
    glColor3f(r,g,b);
    // body
    glBegin(GL_QUADS);
      glVertex3f(-0.4f,-0.2f,0.0f); glVertex3f(0.4f,-0.2f,0.0f);
      glVertex3f( 0.4f, 0.3f,0.0f); glVertex3f(-0.4f, 0.3f,0.0f);
    glEnd();
    // head (semi-circle)
    glBegin(GL_TRIANGLE_FAN);
      glVertex3f(0.0f,0.3f,0.0f);
      for(int i=0;i<=28;i++){
        float a=(float)i/28.0f*M_PI;
        glVertex3f(0.4f*cosf(a), 0.3f+0.4f*sinf(a), 0.0f);
      }
    glEnd();
    // skirt
    glBegin(GL_TRIANGLE_STRIP);
      for(int i=0;i<=6;i++){
        float x=-0.4f+i*(0.8f/6.0f);
        float y=-0.2f-((i%2)?0.12f:0.0f);
        glVertex3f(x,y,0.0f); glVertex3f(x,-0.2f,0.0f);
      }
    glEnd();
    // eyes
    glColor3f(1,1,1);
    glBegin(GL_TRIANGLE_FAN);
      glVertex3f(-0.15f,0.18f,0.01f);
      for(int i=0;i<=16;i++){
        float a=2*M_PI*i/16.0f;
        glVertex3f(-0.15f+0.08f*cosf(a), 0.18f+0.10f*sinf(a), 0.01f);
      }
    glEnd();
    glBegin(GL_TRIANGLE_FAN);
      glVertex3f(0.15f,0.18f,0.01f);
      for(int i=0;i<=16;i++){
        float a=2*M_PI*i/16.0f;
        glVertex3f(0.15f+0.08f*cosf(a), 0.18f+0.10f*sinf(a), 0.01f);
      }
    glEnd();
    // pupils toward dirDeg
    float dx=0.05f*cosf((float)(dirDeg*M_PI/180.0));
    float dy=0.05f*sinf((float)(dirDeg*M_PI/180.0));
    glColor3f(0,0,1);
    glBegin(GL_TRIANGLE_FAN);
      glVertex3f(-0.15f+dx,0.18f+dy,0.02f);
      for(int i=0;i<=12;i++){
        float a=2*M_PI*i/12.0f;
        glVertex3f(-0.15f+dx+0.04f*cosf(a), 0.18f+dy+0.05f*sinf(a), 0.02f);
      }
    glEnd();
    glBegin(GL_TRIANGLE_FAN);
      glVertex3f(0.15f+dx,0.18f+dy,0.02f);
      for(int i=0;i<=12;i++){
        float a=2*M_PI*i/12.0f;
        glVertex3f(0.15f+dx+0.04f*cosf(a), 0.18f+dy+0.05f*sinf(a), 0.02f);
      }
    glEnd();
}
void Ghost::Draw(void)
{
    float r=color[0], g=color[1], b=color[2];
    if (edible){
        if (edible_timer<150){ int blink=(edible_timer/10)%2; r=g=blink; b=1.0f; }
        else { r=0; g=0; b=1; }
    }
    if (eaten){ r=1; g=1; b=0; }

    glPushMatrix();
    glTranslatef(x,-y,0);
    glTranslatef(0.5f,0.6f,0);
    glTranslatef((float)BOARD_X/-2.0f,(float)BOARD_Y/2.0f,1.5f); // above tiles
    drawGhost2D(r,g,b,angle);
    glPopMatrix();
}
void Ghost::Chase(double px,double py,bool *open_move){
    int c = edible ? -1 : 1;
    if ((int)angle==0 || (int)angle==180){
        if ((int)c*py > (int)c*y && open_move[1]) angle=90;
        else if ((int)c*py < (int)c*y && open_move[3]) angle=270;
    } else {
        if ((int)c*px > (int)c*x && open_move[0]) angle=0;
        else if ((int)c*px < (int)c*x && open_move[2]) angle=180;
    }
    if ((int)angle==0   && !open_move[0]) angle=90;
    if ((int)angle==90  && !open_move[1]) angle=180;
    if ((int)angle==180 && !open_move[2]) angle=270;
    if ((int)angle==270 && !open_move[3]) angle=0;
}

// ---------- pellets / tiles ----------
void tp_restore(void){
    pebbles_left = 0;
    for (int r=0; r<BOARD_X; r++){
        for (int c=0; c<BOARD_Y; c++){
            tp_array[r][c] = pebble_array[r][c];
            respawn_timer[r][c] = -1; // respawn disabled
            if (tp_array[r][c]==1 || tp_array[r][c]==3) pebbles_left++;
        }
    }
}

void renderBitmapString(float x,float y, void *font, const char *s){
    glRasterPos2f(x,y);
    for (const char* p=s; *p; ++p) glutBitmapCharacter(font, *p);
}
void Write(char *s){ while(*s) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*s++); }
void print(char *s){ while(*s) glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,*s++); }

// bounds-safe wall check
bool Open(int ca,int rb){
    if (rb<0 || rb>=BOARD_X || ca<0 || ca>=BOARD_Y) return false;
    return !(board_array[rb][ca] > 0);
}

// ---------- Pac-Man movement ----------
void Move()
{
    if (gState!=STATE_PLAY) return; // move only in play
    a += speed1*cos(M_PI/180*angle1);
    b += speed1*sin(M_PI/180*angle1);

    if(animate&&ckey==GLUT_KEY_UP   && (int)a-a>-0.1 && angle1!=270){
        if(Open((int)a,(int)b-1)){ animate=true; angle1=270; }
    }else if(animate&&ckey==GLUT_KEY_DOWN && (int)a-a>-0.1 && angle1!=90){
        if(Open((int)a,(int)b+1)){ animate=true; angle1=90; }
    }else if(animate&&ckey==GLUT_KEY_LEFT && (int)b-b>-0.1 && angle1!=180){
        if(Open((int)a-1,(int)b)){ animate=true; angle1=180; }
    }else if(animate&&ckey==GLUT_KEY_RIGHT&& (int)b-b>-0.1 && angle1!=0){
        if(Open((int)a+1,(int)b)){ animate=true; angle1=0; }
    }
}

// ---------- Pac-Man drawing ----------
static void drawPac2D(float mouthDeg,double dirDeg)
{
    const int SEG=64; float r=0.5f;
    float start = (float)((dirDeg - mouthDeg/2.0)*M_PI/180.0);
    float end   = (float)((dirDeg + mouthDeg/2.0)*M_PI/180.0);

    glColor3f(1.0f,1.0f,0.0f);
    glBegin(GL_TRIANGLE_FAN);
      glVertex3f(0,0,0.02f);
      for(int i=0;i<=SEG;i++){
        float a = (float)i/SEG * (2.0f*M_PI);
        if (a>start && a<end) continue; // mouth wedge
        glVertex3f(r*cosf(a), r*sinf(a), 0.02f);
      }
    glEnd();
    // eye
    glColor3f(0,0,0);
    float eyeA=(float)(dirDeg*M_PI/180.0);
    float ex=0.18f*cosf(eyeA)-0.18f*sinf(eyeA);
    float ey=0.18f*sinf(eyeA)+0.18f*cosf(eyeA);
    glBegin(GL_TRIANGLE_FAN);
      glVertex3f(ex,ey,0.03f);
      for(int i=0;i<=16;i++){
        float a=2*M_PI*i/16.0f; glVertex3f(ex+0.05f*cosf(a), ey+0.05f*sinf(a), 0.03f);
      }
    glEnd();
}
void Pac(void)
{
    glPushMatrix();
    glTranslatef(a,-b,0);
    glTranslatef(0.5f,0.6f,0);
    glTranslatef((float)BOARD_X/-2.0f,(float)BOARD_Y/2.0f,1.5f); // above walls
    drawPac2D(60.0f, angle1);
    glPopMatrix();
}

// ---------- Display lists for walls ----------
void create_list_lib()
{
    listDisp[1]=glGenLists(1); glNewList(listDisp[1],GL_COMPILE);
    glBegin(GL_QUADS); glColor3f(0,0,1);
      glVertex3f(1,1,1); glVertex3f(1,1,0); glVertex3f(0,1,0); glVertex3f(0,1,1);
    glEnd(); glEndList();

    listDisp[2]=glGenLists(1); glNewList(listDisp[2],GL_COMPILE);
    glBegin(GL_QUADS);
    glColor3f(0,0,1);
      glVertex3f(1,1,1); glVertex3f(1,1,0); glVertex3f(0,1,0); glVertex3f(0,1,1);
    glColor3f(0,0,1);
      glVertex3f(1,0,0); glVertex3f(1,0,1); glVertex3f(0,0,1); glVertex3f(0,0,0);
    glEnd(); glEndList();

    listDisp[3]=glGenLists(1); glNewList(listDisp[3],GL_COMPILE);
    glBegin(GL_QUADS);
    glColor3f(0,0,1);
      glVertex3f(1,1,1); glVertex3f(1,1,0); glVertex3f(0,1,0); glVertex3f(0,1,1);
    glColor3f(0,0,1);
      glVertex3f(1,1,0); glVertex3f(1,1,1); glVertex3f(1,0,1); glVertex3f(1,0,0);
    glEnd(); glEndList();

    listDisp[4]=glGenLists(1); glNewList(listDisp[4],GL_COMPILE);
    glBegin(GL_QUADS);
    glColor3f(0,0.3f,0.7f);
      glVertex3f(1,1,1); glVertex3f(0,1,1); glVertex3f(0,0,1); glVertex3f(1,0,1);
    glEnd(); glEndList();
}

// ---------- Draw board + pellets + Pac + Fruits ----------
void Draw(void)
{
    for (int r=0; r<BOARD_X; r++){
        for (int c=0; c<BOARD_Y; c++){
            glColor3f(0,0,1);
            int call_this=0;
            glPushMatrix();
            glTranslatef(-(float)BOARD_X/2.0f, -(float)BOARD_Y/2.0f, 0);
            glTranslatef(c, BOARD_Y-r, 0);
            glPushMatrix(); glTranslatef(0.5,0.5,0);

            switch (board_array[r][c]){
                case 4: glRotatef(90,0,0,1);
                case 3: glRotatef(90,0,0,1);
                case 2: glRotatef(90,0,0,1);
                case 1: call_this=1; break;
                case 6: glRotatef(90,0,0,1);
                case 5: call_this=2; break;
                case 10:glRotatef(90,0,0,1);
                case 9: glRotatef(90,0,0,1);
                case 8: glRotatef(90,0,0,1);
                case 7: call_this=3; break;
            }
            glScalef(1,1,0.5); glTranslatef(-0.5,-0.5,0); glCallList(listDisp[call_this]);
            glPopMatrix();
            if (call_this!=0 || board_array[r][c]==11){
                glTranslatef(0,0,-0.5); glCallList(listDisp[4]);
            }
            glPopMatrix();

            if (tp_array[r][c] > 0){
                glColor3f(1,1,0.2f);
                glPushMatrix();
                glTranslatef(-(float)BOARD_X/2.0f, -(float)BOARD_Y/2.0f, 0);
                glTranslatef(c, BOARD_Y-r, 0);
                glTranslatef(0.5,0.5,0.5);
                float rad = 0.08f * (tp_array[r][c]==3 ? 1.8f : 1.0f);
                glutSolidSphere(rad, 10, 10);
                glPopMatrix();
            }
        }
    }
    drawAllFruits();
    Pac();
}

// ---------- Overlay helpers ----------
static void beginOverlay(){
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
static void endOverlay(){
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}
static void drawCentered(float y, const char* s, void* font=GLUT_BITMAP_HELVETICA_18){
    glRasterPos2f(-4.5f, y);
    for (const char* p=s; *p; ++p) glutBitmapCharacter(font, *p);
}
static void renderMenuScreen(){
    beginOverlay();
    glColor3f(1,1,1);
    drawCentered(8.0f, "PAC-MAN (GLUT/OpenGL)", GLUT_BITMAP_HELVETICA_18);
    const char* items[3] = {"Start / Resume", "Help (H)", "Exit (Esc)"};
    for (int i=0;i<3;i++){
        if (i==menuIndex) glColor3f(1,1,0);
        else              glColor3f(0.8f,0.8f,0.8f);
        drawCentered(11.0f + i*1.4f, items[i], GLUT_BITMAP_HELVETICA_18);
    }
    glColor3f(0.7f,0.7f,0.7f);
    drawCentered(16.0f, "Use UP/DOWN + ENTER  |  S = Start", GLUT_BITMAP_9_BY_15);
    endOverlay();
}

// ---------- MAIN RENDER ----------
void RenderScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // MENU screen only overlay (no board)
    if (gState == STATE_MENU){
        renderMenuScreen();
        glutSwapBuffers();
        glutPostRedisplay();
        return;
    }

    // fruit spawner (every 30s regardless of existing fruits)
    updateFruitSpawner();

    // Pac tunnel wrap
    if ((int)a==27 && (int)b==14 && angle1==0){ a=0;  animate=true; }
    else if ((int)(a+0.9)==0 && (int)b==14 && angle1==180){ a=27; animate=true; }

    // Update logic during PLAY
    if (gState == STATE_PLAY){
        if (animate) Move();

        if (!Open((int)(a + cos(M_PI/180*angle1)), (int)(b + sin(M_PI/180*angle1))) &&
            a - (int)a < 0.1 && b - (int)b < 0.1){
            animate = false;
        }

        // Eat pellets
        int rr = (int)(b+0.5);
        int cc = (int)(a+0.5);
        if (rr>=0 && rr<BOARD_X && cc>=0 && cc<BOARD_Y){
            if (tp_array[rr][cc]==1){
                tp_array[rr][cc]=0; pebbles_left--; points+=1;
            } else if (tp_array[rr][cc]==3){
                tp_array[rr][cc]=0; pebbles_left--; points+=5;
                for (int i=0;i<num_ghosts;i++)
                    if (!ghost[i]->eaten) ghost[i]->Vulnerable();
            }
        }

        // fruit eat check
        checkFruitsCollision();

        // WIN condition: all pellets eaten
        if (pebbles_left == 0){
            gState = STATE_WIN;
            winElapsedMs = glutGet(GLUT_ELAPSED_TIME) - gameStartMs;
            animate = false;
        }
    }

    // Draw board + Pac (+ Fruits drawn inside Draw)
    Draw();

    // Ghost updates + draw + collisions during PLAY
    if (gState == STATE_PLAY){
        for (int d=0; d<num_ghosts; d++){
            if (!gameover && start_timer==0) ghost[d]->Update();

            if (!ghost[d]->in_jail && fmod(ghost[d]->x,1.0)<0.1 && fmod(ghost[d]->y,1.0)<0.1){
                bool open_move[4];
                for (int ang=0; ang<4; ++ang)
                    open_move[ang]=Open((int)(ghost[d]->x + cos(M_PI/180*ang*90)),
                                        (int)(ghost[d]->y + sin(M_PI/180*ang*90)));
                if (!ghost[d]->eaten) ghost[d]->Chase(a,b,open_move);
                else                  ghost[d]->Chase(13,11,open_move);
            }

            if (ghost[d]->in_jail &&
                !Open((int)(ghost[d]->x + cos(M_PI/180*ghost[d]->angle)),
                      (int)(ghost[d]->y + sin(M_PI/180*ghost[d]->angle))) &&
                ghost[d]->jail_timer > 0 &&
                fmod(ghost[d]->x,1.0)<0.1 && fmod(ghost[d]->y,1.0)<0.1)
            {
                ghost[d]->angle = fmod(ghost[d]->angle + 180.0, 360.0);
            }

            if (!gameover && start_timer==0) ghost[d]->Move();
            ghost[d]->Draw();

            if (!ghost[d]->eaten){
                bool collide = ghost[d]->Catch(a,b);
                if (collide && !ghost[d]->edible){
                    lives--;
                    if (lives==0){
                        gameover=true; gState=STATE_GAMEOVER;
                        gameOverElapsedMs = glutGet(GLUT_ELAPSED_TIME) - gameStartMs; // freeze time on game over
                    }
                    P_Reinit();
                    break;
                } else if (collide && ghost[d]->edible){
                    ghost[d]->edible=false; ghost[d]->eaten=true; ghost[d]->speed=1.0;
                }
            }
        }
    }

    // --------- Overlays: Score, Time, Lives; Pause; GameOver; Win ---------
    beginOverlay();

    // Score / Time / Lives top bar
    {
        char tmp[64];
        int showMs;
        if (gState==STATE_WIN)           showMs = winElapsedMs;
        else if (gState==STATE_GAMEOVER) showMs = gameOverElapsedMs; // freeze on game over
        else                             showMs = (glutGet(GLUT_ELAPSED_TIME) - gameStartMs);
        if (showMs < 0) showMs = 0;
        int ms = showMs % 1000;
        int total_s = showMs / 1000;
        int mm = total_s / 60;
        int ss = total_s % 60;

        glColor3f(1,1,0);
        glRasterPos2f(-12, 18);
        sprintf(tmp, "Score: %d", points);
        Write(tmp);

        glRasterPos2f(-2, 18);
        sprintf(tmp, "Time: %02d:%02d.%03d", mm, ss, ms);
        Write(tmp);

        glRasterPos2f(8, 18);
        sprintf(tmp, "Lives: %d", lives);
        Write(tmp);
    }

    // Pause overlay
    if (gState == STATE_PAUSE){
        glColor4f(0,0,0,0.45f);
        glBegin(GL_QUADS);
            glVertex2f(-14, 6); glVertex2f(14, 6);
            glVertex2f(14, 12); glVertex2f(-14, 12);
        glEnd();
        glColor3f(1,1,0.6f);
        drawCentered(9.0f, "PAUSED", GLUT_BITMAP_HELVETICA_18);
        glColor3f(1,1,1);
        drawCentered(10.5f, "Press P to Resume, M for Menu", GLUT_BITMAP_9_BY_15);
    }

    // Game Over overlay
    if (gState == STATE_GAMEOVER){
        glColor4f(0,0,0,0.55f);
        glBegin(GL_QUADS);
            glVertex2f(-14, 6); glVertex2f(14, 6);
            glVertex2f(14, 12); glVertex2f(-14, 12);
        glEnd();
        glColor3f(1,0,0);
        drawCentered(8.8f, "GAME OVER", GLUT_BITMAP_HELVETICA_18);
        glColor3f(1,1,1);
        drawCentered(10.4f, "Press R to Restart or M for Menu", GLUT_BITMAP_9_BY_15);
    }

    // Win overlay
    if (gState == STATE_WIN){
        glColor4f(0,0,0,0.55f);
        glBegin(GL_QUADS);
            glVertex2f(-14, 6); glVertex2f(14, 6);
            glVertex2f(14, 12); glVertex2f(-14, 12);
        glEnd();
        glColor3f(0,1,0);
        drawCentered(8.8f, "YOU WIN!", GLUT_BITMAP_HELVETICA_18);
        // Show total time taken
        char tbuf[64];
        int ms = winElapsedMs % 1000;
        int total_s = winElapsedMs / 1000;
        int mm = total_s / 60;
        int ss = total_s % 60;
        glColor3f(1,1,1);
        sprintf(tbuf, "Time: %02d:%02d.%03d", mm, ss, ms);
        drawCentered(10.0f, tbuf, GLUT_BITMAP_9_BY_15);
        drawCentered(11.4f, "Press R to Restart or M for Menu", GLUT_BITMAP_9_BY_15);
    }

    endOverlay();

    glutSwapBuffers();
    glutPostRedisplay();
}

// ---------- Input (keyboard) ----------
void startGame(){
    gameover=false; lives=3; points=0;
    P_Reinit(); G_Reinit(); tp_restore(); fruitsInit();
    gState=STATE_PLAY; start_timer=0; animate=false;
    gameStartMs = glutGet(GLUT_ELAPSED_TIME);
    winElapsedMs = 0;
    gameOverElapsedMs = 0;
}

void mykey(unsigned char key,int,int)
{
    if (key==27){ // ESC
        if (gState==STATE_MENU) exit(0);
        else gState=STATE_MENU;
        return;
    }

    if (gState==STATE_MENU){
        if (key=='\r' || key=='\n' || key==' ' || key=='s' || key=='S'){
            if (menuIndex==0)      startGame();
            else if (menuIndex==1) { /* help text sits on menu */ }
            else if (menuIndex==2) exit(0);
            return;
        }
        if (key=='h' || key=='H'){ return; }
    }

    if (key=='p' || key=='P'){
        if (gState==STATE_PLAY)  gState=STATE_PAUSE;
        else if (gState==STATE_PAUSE) gState=STATE_PLAY;
        return;
    }
    if (key=='m' || key=='M'){ gState=STATE_MENU; return; }
    if ((key=='r'||key=='R') && (gState==STATE_GAMEOVER)){ startGame(); return; }
    if ((key=='r'||key=='R') && (gState==STATE_WIN)){ startGame(); return; }

    if (start_timer>0) start_timer--;
}

void specialDown(int key,int,int)
{
    if (gState==STATE_MENU){
        if (key==GLUT_KEY_UP)   menuIndex = (menuIndex+2)%3;
        if (key==GLUT_KEY_DOWN) menuIndex = (menuIndex+1)%3;
        return;
    }
    if (gState!=STATE_PLAY) return;

    if (start_timer>0) start_timer--;
    ckey=key;
    if(key==GLUT_KEY_UP   && (int)a-a>-0.1 && angle1!=270){ if(Open((int)a,(int)b-1)){animate=true; angle1=270;} }
    else if(key==GLUT_KEY_DOWN && (int)a-a>-0.1 && angle1!=90 ){ if(Open((int)a,(int)b+1)){animate=true; angle1=90; } }
    else if(key==GLUT_KEY_LEFT && (int)b-b>-0.1 && angle1!=180){ if(Open((int)a-1,(int)b)){animate=true; angle1=180;} }
    else if(key==GLUT_KEY_RIGHT&& (int)b-b>-0.1 && angle1!=0  ){ if(Open((int)a+1,(int)b)){animate=true; angle1=0;  } }
}
void specialUp(int,int,int){}

// ---------- Re-inits ----------
void P_Reinit(){ a=13.5; b=23; angle1=90; animate=false; }
void G_Reinit(void)
{
    start_timer=3;

    // 3 ghosts only
    int start_x[3] = {11, 14, 16};
    float ghost_colors[3][3] = {
        {255,   0,   0},   // red
        {120, 240, 120},   // mint
        {255, 125,   0}    // orange
    };

    for (int i=0;i<num_ghosts;i++){
        if (!ghost[i]) continue;
        ghost[i]->Reinit();
        ghost[i]->x = start_x[i];
        ghost[i]->y = 14;
        ghost[i]->eaten = false;

        // slower ghosts
        ghost[i]->max_speed = 0.020 - 0.002*(float)i;
        if (ghost[i]->max_speed < 0.012) ghost[i]->max_speed = 0.012;
        ghost[i]->speed = ghost[i]->max_speed;

        ghost[i]->jail_timer = i*33 + 66;

        for (int j=0;j<3;j++)
            ghost[i]->color[j] = ghost_colors[i][j]/255.0f;
    }
}

// ---------- init / main ----------
void init()
{
    float color[4];
    glEnable(GL_LIGHT0); glEnable(GL_LIGHTING);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE); glEnable(GL_COLOR_MATERIAL);
    color[0]=1; color[1]=1; color[2]=0; color[3]=0; glLightfv(GL_LIGHT0,GL_DIFFUSE,color);
    color[0]=1; color[1]=0; color[2]=1; color[3]=1; glLightfv(GL_LIGHT0,GL_AMBIENT,color);
    glEnable(GL_NORMALIZE);

    glMatrixMode(GL_PROJECTION); glLoadIdentity(); gluPerspective(60,1.33,0.005,100);
    glMatrixMode(GL_MODELVIEW);  glLoadIdentity(); gluLookAt(-1.5,0,40,-1.5,0,0,0,1,0);
}

int main(int argc,char **argv)
{
    srand((unsigned)time(NULL)); // random for fruits

    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_RGBA | GL_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize(1200,780);
    glutInitWindowPosition(0,0);
    glutCreateWindow("PAC-MAN GL 3D");

    init();
    create_list_lib();

    glutDisplayFunc(RenderScene);
    glutKeyboardFunc(mykey);
    glutSpecialFunc(specialDown);
    glutSpecialUpFunc(specialUp);

    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);

    // create 3 ghosts
    for (int i=0;i<num_ghosts;i++) ghost[i] = new Ghost(0,0);
    G_Reinit();         // set positions/speeds/colors

    // pellets + fruits
    tp_restore();
    fruitsInit();

    gState = STATE_MENU;
    glutMainLoop();
    return 0;
}
