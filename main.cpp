#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

// ─── WINDOW ───────────────────────────────────
int WIN_W = 800, WIN_H = 600;

// ─── GAME STATE ───────────────────────────────
// 0=MENU 1=PLAYING 2=PAUSED 3=GAMEOVER 4=WIN 5=HELP 6=HIGHSCORE
int gameState = 0;

// ─── PADDLE ───────────────────────────────────
float paddleX=350, paddleY=30, paddleW=100, paddleH=14;
int   shootMode=0, shootTimer=0;

// ─── BALL ─────────────────────────────────────
float ballX=400, ballY=60, ballR=10;
float ballSX=3.5f, ballSY=4.5f;
int   ballMoving=0;
int   fireMode=0, fireTimer=0;
int   thruMode=0, thruTimer=0;

// ─── BRICKS ───────────────────────────────────
#define ROWS 6
#define COLS 10
#define BW   68
#define BH   22
#define BX0  26
#define BY0  430
int bType[ROWS][COLS];
int bHP[ROWS][COLS];
int totalNormal=0, remaining=0;

// ─── DROPS ────────────────────────────────────
#define MAX_DROPS 5
float dX[MAX_DROPS], dY[MAX_DROPS];
int   dType[MAX_DROPS], dAlive[MAX_DROPS];

// ─── BULLETS ──────────────────────────────────
#define MAX_BULLETS 8
float bltX[MAX_BULLETS], bltY[MAX_BULLETS];
int   bltAlive[MAX_BULLETS];

// ─── SCORE ────────────────────────────────────
int score=0, lives=3, highScore=0;

// ─── NOTIFICATION ─────────────────────────────
char notifMsg[64]="";
int  notifTime=0;

// ══════════════════════════════════════════════
// ██  Ishtiak
// ══════════════════════════════════════════════

// ── Bresenham Line Drawing Algorithm ──────────
void drawLineBres(int x1,int y1,int x2,int y2){
    int dx=abs(x2-x1),dy=abs(y2-y1);
    int sx=(x1<x2)?1:-1,sy=(y1<y2)?1:-1,err=dx-dy;
    glBegin(GL_POINTS);
    while(1){
        glVertex2i(x1,y1);
        if(x1==x2&&y1==y2) break;
        int e2=2*err;
        if(e2>-dy){err-=dy;x1+=sx;}
        if(e2< dx){err+=dx;y1+=sy;}
    }
    glEnd();
}

// ── Midpoint Circle Algorithm ─────────────────
void drawCircleMidpoint(int cx,int cy,int r){
    int x=0,y=r;
    float d=1.25f-r;
    glBegin(GL_POINTS);
    while(x<=y){
        glVertex2i(cx+x,cy+y); glVertex2i(cx-x,cy+y);
        glVertex2i(cx+x,cy-y); glVertex2i(cx-x,cy-y);
        glVertex2i(cx+y,cy+x); glVertex2i(cx-y,cy+x);
        glVertex2i(cx+y,cy-x); glVertex2i(cx-y,cy-x);
        if(d<0) d+=2*x+3;
        else   {d+=2*(x-y)+5;y--;}
        x++;
    }
    glEnd();
}

// ── Filled Circle (GL_TRIANGLE_FAN) ───────────
void drawFilledCircle(float cx,float cy,float r){
    // GL built-in GL_TRIANGLE_FAN
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx,cy);
    for(int i=0;i<=360;i++){
        float a=i*3.14159f/180.0f;
        glVertex2f(cx+r*cos(a),cy+r*sin(a));
    }
    glEnd();
}

// ── Helper: filled rect (GL_QUADS) ────────────
void fillRect(float x,float y,float w,float h){
    // GL built-in
    glBegin(GL_QUADS);
    glVertex2f(x,y);     glVertex2f(x+w,y);
    glVertex2f(x+w,y+h); glVertex2f(x,y+h);
    glEnd();
}

// ── Helper: outline rect (GL_LINE_LOOP) ───────
void outlineRect(float x,float y,float w,float h){
    // GL built-in
    glBegin(GL_LINE_LOOP);
    glVertex2f(x,y);     glVertex2f(x+w,y);
    glVertex2f(x+w,y+h); glVertex2f(x,y+h);
    glEnd();
}

// ── Helper: draw text (GLUT built-in) ─────────
void drawText(float x,float y,const char*s,void*font){
    // GLUT built-in
    glRasterPos2f(x,y);
    for(int i=0;s[i];i++) glutBitmapCharacter(font,s[i]);
}

// ── DRAW: background ──────────────────────────
void drawBackground(){
    // GL built-in GL_QUADS gradient
    glBegin(GL_QUADS);
    glColor3f(0.05f,0.05f,0.15f); glVertex2f(0,0);
    glColor3f(0.05f,0.05f,0.15f); glVertex2f(WIN_W,0);
    glColor3f(0.1f,0.1f,0.3f);    glVertex2f(WIN_W,WIN_H);
    glColor3f(0.1f,0.1f,0.3f);    glVertex2f(0,WIN_H);
    glEnd();
    // Bresenham Line for borders
    glColor3f(0.5f,0.5f,0.9f); glLineWidth(2);
    drawLineBres(0,0,0,WIN_H);
    drawLineBres(WIN_W-1,0,WIN_W-1,WIN_H);
    drawLineBres(0,WIN_H-1,WIN_W,WIN_H-1);
    glLineWidth(1);
}

// ── DRAW: ball ────────────────────────────────
void drawBall(){
    if(fireMode){
        // Midpoint Circle outline + filled for fireball
        glColor3f(1.0f,0.3f,0.0f); drawFilledCircle(ballX,ballY,ballR);
        glColor3f(1.0f,1.0f,0.0f); drawCircleMidpoint((int)ballX,(int)ballY,(int)ballR);
        glColor3f(1.0f,0.9f,0.0f); drawFilledCircle(ballX,ballY,ballR*0.5f);
    } else {
        // Midpoint Circle Algorithm for outline
        glColor3f(1.0f,0.9f,0.0f); drawFilledCircle(ballX,ballY,ballR);
        glColor3f(1.0f,1.0f,1.0f); drawCircleMidpoint((int)ballX,(int)ballY,(int)ballR);
        glColor3f(1.0f,1.0f,1.0f); drawFilledCircle(ballX,ballY,ballR*0.28f);
    }
}

// ── DRAW: bricks ──────────────────────────────
void drawBricks(){
    // GL built-in GL_QUADS for each brick
    float colors[ROWS][3]={
        {0.75f,0.75f,0.75f},
        {1.0f,0.2f,0.2f},
        {1.0f,0.6f,0.0f},
        {1.0f,1.0f,0.0f},
        {0.2f,0.8f,0.2f},
        {0.2f,0.6f,1.0f}
    };
    for(int r=0;r<ROWS;r++){
        for(int c=0;c<COLS;c++){
            if(!bType[r][c]) continue;
            float bx=BX0+c*(BW+4),by=BY0-r*(BH+4);
            float cr=colors[r][0],cg=colors[r][1],cb=colors[r][2];
            if(bType[r][c]==2){
                float f=bHP[r][c]/3.0f;
                cr=0.3f+0.5f*f; cg=0.3f+0.5f*f; cb=0.3f+0.5f*f;
            }
            glColor3f(cr,cg,cb); fillRect(bx,by,BW,BH);
            glColor3f(0,0,0);    outlineRect(bx,by,BW,BH);
            if(bType[r][c]==2){
                char hp[4]; sprintf(hp,"%d",bHP[r][c]);
                glColor3f(1,1,1);
                drawText(bx+BW/2-4,by+5,hp,GLUT_BITMAP_HELVETICA_12);
            }
        }
    }
}

// ── UPDATE: ball-brick collision ──────────────
void checkBallBrickCollision(){
    for(int r=0;r<ROWS;r++){
        for(int c=0;c<COLS;c++){
            if(!bType[r][c]) continue;
            float bx=BX0+c*(BW+4),by=BY0-r*(BH+4);
            float cx=ballX<bx?bx:(ballX>bx+BW?bx+BW:ballX);
            float cy=ballY<by?by:(ballY>by+BH?by+BH:ballY);
            float ddx=ballX-cx,ddy=ballY-cy;
            if(ddx*ddx+ddy*ddy>ballR*ballR) continue;
            bHP[r][c]--;
            if(bHP[r][c]<=0){
                if(bType[r][c]==1){remaining--;score+=(5-r)*10;}
                else score+=5;
                if(rand()%10<4){
                    for(int i=0;i<MAX_DROPS;i++){
                        if(!dAlive[i]){dX[i]=bx+BW/2;dY[i]=by;dType[i]=1+rand()%7;dAlive[i]=1;break;}
                    }
                }
                bType[r][c]=0;
                if(score>highScore) highScore=score;
                if(remaining==0){gameState=4;return;}
            }
            if(fireMode||thruMode) return;
            // bounce
            if(fabs(ddx)>fabs(ddy)) ballSX=-ballSX;
            else                     ballSY=-ballSY;
            // 2D Transformation: scale velocity vector on every 5 bricks
            int destroyed=totalNormal-remaining;
            if(destroyed>0&&destroyed%5==0){
                float spd=sqrt(ballSX*ballSX+ballSY*ballSY);
                if(spd<10){ballSX*=1.08f;ballSY*=1.08f;}
            }
            return;
        }
    }
}

// ── UPDATE: main game logic ───────────────────
void updateGameLogic(){
    // perk timers
    if(fireTimer>0){fireTimer--;if(!fireTimer)fireMode=0;}
    if(thruTimer>0){thruTimer--;if(!thruTimer)thruMode=0;}
    if(shootTimer>0){shootTimer--;if(!shootTimer)shootMode=0;}
    if(notifTime>0) notifTime--;
    // drops fall
    for(int i=0;i<MAX_DROPS;i++){
        if(!dAlive[i]) continue;
        dY[i]-=2.0f;
        if(dY[i]>=paddleY&&dY[i]<=paddleY+paddleH&&dX[i]>=paddleX&&dX[i]<=paddleX+paddleW){
            int t=dType[i];
            if(t==1){fireMode=1;fireTimer=300;strncpy(notifMsg,"FIREBALL!",63);notifTime=120;}
            if(t==2){thruMode=1;thruTimer=300;strncpy(notifMsg,"THROUGH BRICK!",63);notifTime=120;}
            if(t==3){lives--;strncpy(notifMsg,"DEATH! -1 Life",63);notifTime=120;if(lives<=0)gameState=3;}
            if(t==4){if(paddleW>40)paddleW-=30;strncpy(notifMsg,"SHRUNK!",63);notifTime=120;}
            if(t==5){shootMode=1;shootTimer=400;strncpy(notifMsg,"SHOOT! Press Z",63);notifTime=120;}
            if(t==6){lives++;strncpy(notifMsg,"+1 LIFE!",63);notifTime=120;}
            if(t==7){if(paddleW<180)paddleW+=30;strncpy(notifMsg,"EXPANDED!",63);notifTime=120;}
            dAlive[i]=0;
        }
        if(dY[i]<0) dAlive[i]=0;
    }
    // bullets move up
    for(int i=0;i<MAX_BULLETS;i++){
        if(!bltAlive[i]) continue;
        bltY[i]+=8.0f;
        if(bltY[i]>WIN_H){bltAlive[i]=0;continue;}
        for(int r=0;r<ROWS;r++){
            for(int c=0;c<COLS;c++){
                if(!bType[r][c]) continue;
                float bx=BX0+c*(BW+4),by=BY0-r*(BH+4);
                if(bltX[i]>=bx&&bltX[i]<=bx+BW&&bltY[i]>=by&&bltY[i]<=by+BH){
                    bHP[r][c]--;
                    if(bHP[r][c]<=0){
                        if(bType[r][c]==1){remaining--;score+=10;}
                        else score+=5;
                        bType[r][c]=0;
                        if(score>highScore)highScore=score;
                        if(remaining==0){gameState=4;return;}
                    }
                    bltAlive[i]=0; goto skip;
                }
            }
        }
        skip:;
    }
    // ball movement
    if(!ballMoving){ballX=paddleX+paddleW/2;ballY=paddleY+paddleH+ballR+1;return;}
    ballX+=ballSX; ballY+=ballSY;
    if(ballX-ballR<=0)      {ballX=ballR;      ballSX= fabs(ballSX);}
    if(ballX+ballR>=WIN_W)  {ballX=WIN_W-ballR;ballSX=-fabs(ballSX);}
    if(ballY+ballR>=WIN_H)  {ballY=WIN_H-ballR;ballSY=-fabs(ballSY);}
    if(ballY-ballR<=0){
        lives--; if(lives<=0){gameState=3;return;}
        ballX=paddleX+paddleW/2;ballY=paddleY+paddleH+ballR+1;
        ballSX=3.5f;ballSY=4.5f;ballMoving=0;
        fireMode=0;fireTimer=0;thruMode=0;thruTimer=0;
        return;
    }
    // paddle bounce — 2D Transformation: translate hit position to angle
    if(ballY-ballR<=paddleY+paddleH&&ballY+ballR>=paddleY&&
       ballX>=paddleX&&ballX<=paddleX+paddleW&&ballSY<0){
        ballSY=fabs(ballSY);
        float hit=(ballX-paddleX)/paddleW;
        ballSX=(hit-0.5f)*9.0f;
    }
    checkBallBrickCollision();
}

// ── OpenGL init ───────────────────────────────
void openGLInit(){
    glClearColor(0,0,0,1);
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    gluOrtho2D(0,WIN_W,0,WIN_H);
    glMatrixMode(GL_MODELVIEW);  glLoadIdentity();
}
// ── INIT: bricks ──────────────────────────────
void setupBricks(){
    totalNormal=0;
    for(int r=0;r<ROWS;r++){
        for(int c=0;c<COLS;c++){
            if(r==0){bType[r][c]=2;bHP[r][c]=3;}
            else    {bType[r][c]=1;bHP[r][c]=1;totalNormal++;}
        }
    }
    remaining=totalNormal;
}

// ── INIT: full new game ────────────────────────
void startNewGame(){
    paddleX=(WIN_W-paddleW)/2.0f; paddleW=100;
    score=0; lives=3; shootMode=0; shootTimer=0;
    for(int i=0;i<MAX_DROPS;i++)   dAlive[i]=0;
    for(int i=0;i<MAX_BULLETS;i++) bltAlive[i]=0;
    setupBricks();
    ballX=paddleX+paddleW/2; ballY=paddleY+paddleH+ballR+1;
    ballSX=3.5f; ballSY=4.5f; ballMoving=0;
    fireMode=0; fireTimer=0; thruMode=0; thruTimer=0;
}

// ── DRAW: paddle ──────────────────────────────
void drawPaddle(){
    // GL built-in GL_QUADS
    if(shootMode) glColor3f(0.0f,1.0f,1.0f);
    else          glColor3f(0.3f,0.5f,1.0f);
    fillRect(paddleX,paddleY,paddleW,paddleH);
    glColor3f(1,1,1); outlineRect(paddleX,paddleY,paddleW,paddleH);
    // Bresenham Line for shine effect on paddle
    glColor3f(0.8f,0.9f,1.0f);
    drawLineBres((int)paddleX+4,(int)(paddleY+paddleH-3),
                 (int)(paddleX+paddleW-4),(int)(paddleY+paddleH-3));
}

// ── DRAW: HUD ─────────────────────────────────
void drawHUD(){
    // GLUT built-in bitmap fonts
    char buf[64];
    glColor3f(1,1,1);
    sprintf(buf,"Score: %d",score);
    drawText(10,WIN_H-22,buf,GLUT_BITMAP_HELVETICA_18);
    sprintf(buf,"Lives: %d",lives);
    drawText(WIN_W-130,WIN_H-22,buf,GLUT_BITMAP_HELVETICA_18);
    sprintf(buf,"Best: %d",highScore);
    drawText(WIN_W/2-35,WIN_H-22,buf,GLUT_BITMAP_HELVETICA_18);
    int ty=WIN_H-45;
    if(fireMode) {glColor3f(1,.5f,0);sprintf(buf,"FIREBALL:%ds",fireTimer/60);drawText(10,ty,buf,GLUT_BITMAP_HELVETICA_12);ty-=15;}
    if(thruMode) {glColor3f(.4f,.4f,1);sprintf(buf,"THROUGH:%ds",thruTimer/60);drawText(10,ty,buf,GLUT_BITMAP_HELVETICA_12);ty-=15;}
    if(shootMode){glColor3f(0,1,1);sprintf(buf,"SHOOT:%ds",shootTimer/60);drawText(10,ty,buf,GLUT_BITMAP_HELVETICA_12);}
    if(notifTime>0){glColor3f(1,1,0);drawText(WIN_W/2-(int)strlen(notifMsg)*4,110,notifMsg,GLUT_BITMAP_HELVETICA_18);}
}

// ── DRAW: drop items ──────────────────────────
void drawDropItems(){
    // GL built-in GL_QUADS
    const char*names[]={"","FIRE","THRU","DIE!","SHRK","SHOT","+LIF","EXP"};
    float dc[8][3]={{0,0,0},{1,.4f,0},{.4f,.4f,1},{1,0,0},{.8f,0,.8f},{0,1,1},{0,1,0},{1,1,0}};
    for(int i=0;i<MAX_DROPS;i++){
        if(!dAlive[i]) continue;
        int t=dType[i];
        glColor3f(dc[t][0],dc[t][1],dc[t][2]); fillRect(dX[i]-20,dY[i]-8,40,16);
        glColor3f(0,0,0); outlineRect(dX[i]-20,dY[i]-8,40,16);
        glColor3f(0,0,0); drawText(dX[i]-16,dY[i]-4,names[t],GLUT_BITMAP_HELVETICA_10);
    }
}

// ── DRAW: bullets ─────────────────────────────
void drawBullets(){
    // GL built-in GL_QUADS
    glColor3f(1,1,0);
    for(int i=0;i<MAX_BULLETS;i++){
        if(!bltAlive[i]) continue;
        fillRect(bltX[i]-2,bltY[i],4,10);
    }
}

// ── DRAW: menu screen ─────────────────────────
void drawMenuScreen(){
    // GL built-in background + GLUT fonts
    glClearColor(0.05f,0.05f,0.2f,1); glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1,.8f,0);
    drawText(320,500,"DX  BALL",GLUT_BITMAP_TIMES_ROMAN_24);
    // Bresenham Line decorative separator
    glColor3f(0.5f,0.5f,1.0f); drawLineBres(280,488,520,488);
    glColor3f(.3f,1,.3f);  drawText(340,430,"1.  START GAME",GLUT_BITMAP_HELVETICA_18);
    glColor3f(.3f,.7f,1);  drawText(340,390,"2.  HIGH SCORE",GLUT_BITMAP_HELVETICA_18);
    glColor3f(1,.9f,.3f);  drawText(340,350,"3.  HELP",GLUT_BITMAP_HELVETICA_18);
    glColor3f(1,.4f,.4f);  drawText(340,310,"4.  EXIT",GLUT_BITMAP_HELVETICA_18);
    glColor3f(.6f,.6f,.6f);
    drawText(210,200,"Mouse / Arrow Keys  =  Move Paddle",GLUT_BITMAP_HELVETICA_12);
    drawText(210,180,"SPACE or Click      =  Launch Ball",GLUT_BITMAP_HELVETICA_12);
    drawText(210,160,"Z=Shoot  P=Pause  ESC=Exit",GLUT_BITMAP_HELVETICA_12);
    // Midpoint Circle Algorithm decorative ball
    glColor3f(1.0f,0.9f,0.0f); drawFilledCircle(400,560,15);
    glColor3f(1,1,1); drawCircleMidpoint(400,560,15);
}

// ── DRAW: help screen ─────────────────────────
void drawHelpScreen(){
    // GL built-in background + GLUT fonts
    glClearColor(0,0.05f,0.15f,1); glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1,.8f,0); drawText(330,560,"HOW TO PLAY",GLUT_BITMAP_TIMES_ROMAN_24);
    // Bresenham Line separator
    glColor3f(0.5f,0.8f,0.5f); drawLineBres(40,550,760,550);
    glColor3f(.5f,1,1); drawText(40,525,"--- CONTROLS ---",GLUT_BITMAP_HELVETICA_18);
    glColor3f(1,1,1);
    drawText(40,500,"Left/Right Arrow  OR  A/D   :  Move Paddle",GLUT_BITMAP_HELVETICA_12);
    drawText(40,482,"Mouse Move                  :  Move Paddle",GLUT_BITMAP_HELVETICA_12);
    drawText(40,464,"SPACE  or  Left Click       :  Launch Ball",GLUT_BITMAP_HELVETICA_12);
    drawText(40,446,"Z                           :  Shoot Bullet (SHOT perk needed)",GLUT_BITMAP_HELVETICA_12);
    drawText(40,428,"P                           :  Pause / Resume",GLUT_BITMAP_HELVETICA_12);
    drawText(40,410,"ESC                         :  Exit",GLUT_BITMAP_HELVETICA_12);
    glColor3f(.5f,1,.5f); drawText(40,382,"--- BRICK TYPES ---",GLUT_BITMAP_HELVETICA_18);
    glColor3f(.8f,.8f,.8f); drawText(40,360,"GREY (WALL)  :  Needs 3 hits. Shows HP number.",GLUT_BITMAP_HELVETICA_12);
    glColor3f(1,1,.3f);     drawText(40,342,"COLORED      :  1 hit to break. May drop a perk.",GLUT_BITMAP_HELVETICA_12);
    glColor3f(1,.5f,1); drawText(40,312,"--- DROP ITEMS ---",GLUT_BITMAP_HELVETICA_18);
    float dc[7][3]={{1,.4f,0},{.4f,.4f,1},{1,0,0},{.8f,0,.8f},{0,1,1},{0,1,0},{1,1,0}};
    const char*di[]={"FIRE  :  Ball destroys all bricks, no bounce!",
                     "THRU  :  Ball passes through bricks!",
                     "DIE!  :  AVOID - instant life lost!",
                     "SHRK  :  AVOID - paddle shrinks!",
                     "SHOT  :  Shoot bullets with Z key!",
                     "+LIF  :  Gain an extra life!",
                     "EXP   :  Paddle gets bigger!"};
    for(int i=0;i<7;i++){glColor3f(dc[i][0],dc[i][1],dc[i][2]);drawText(40,288-i*19,di[i],GLUT_BITMAP_HELVETICA_12);}
    glColor3f(1,.8f,0); drawText(40,155,"--- SCORING ---",GLUT_BITMAP_HELVETICA_18);
    glColor3f(1,1,1);
    drawText(40,133,"Top rows give more points. Ball speeds up every 5 bricks.",GLUT_BITMAP_HELVETICA_12);
    drawText(40,115,"Break ALL colored bricks to WIN!",GLUT_BITMAP_HELVETICA_12);
    glColor3f(.6f,.6f,.6f); drawText(300,35,"Press M to go back to Menu",GLUT_BITMAP_HELVETICA_18);
}

// ── DRAW: high score screen ───────────────────
void drawHighScoreScreen(){
    // GL built-in background + GLUT fonts
    glClearColor(0.05f,0.05f,0.2f,1); glClear(GL_COLOR_BUFFER_BIT);
    char buf[32];
    glColor3f(1,.8f,0); drawText(295,410,"HIGH  SCORE",GLUT_BITMAP_TIMES_ROMAN_24);
    // Bresenham Line separator
    glColor3f(0.5f,0.5f,1.0f); drawLineBres(250,398,550,398);
    glColor3f(.3f,1,.3f); sprintf(buf,"%d",highScore);
    drawText(370,330,buf,GLUT_BITMAP_TIMES_ROMAN_24);
    // Midpoint Circle Algorithm decoration
    glColor3f(1,.9f,0); drawFilledCircle(400,240,30);
    glColor3f(1,1,1);   drawCircleMidpoint(400,240,30);
    glColor3f(.6f,.6f,.6f); drawText(300,150,"Press M to go back to Menu",GLUT_BITMAP_HELVETICA_18);
}

// ── DRAW: pause overlay ───────────────────────
void drawPauseOverlay(){
    // GL built-in GL_QUADS
    glColor3f(0,0,0); fillRect(215,260,370,130);
    glColor3f(.5f,.5f,.5f); outlineRect(215,260,370,130);
    glColor3f(1,1,0); drawText(345,360,"PAUSED",GLUT_BITMAP_TIMES_ROMAN_24);
    glColor3f(1,1,1);
    drawText(240,320,"P=Resume  R=Restart  M=Menu",GLUT_BITMAP_HELVETICA_18);
    drawText(315,288,"ESC = Exit",GLUT_BITMAP_HELVETICA_18);
}

// ── DRAW: game over screen ────────────────────
void drawGameOverScreen(){
    // GL built-in background + GLUT fonts
    glClearColor(0.1f,0,0,1); glClear(GL_COLOR_BUFFER_BIT);
    char buf[64];
    glColor3f(1.0f,0.2f,0.2f); drawText(295,370,"GAME  OVER",GLUT_BITMAP_TIMES_ROMAN_24);
    glColor3f(1,1,1);
    sprintf(buf,"Final Score :  %d",score); drawText(295,310,buf,GLUT_BITMAP_HELVETICA_18);
    sprintf(buf,"High Score  :  %d",highScore); drawText(295,280,buf,GLUT_BITMAP_HELVETICA_18);
    glColor3f(0.5f,1.0f,0.5f);
    drawText(235,220,"R = Restart    M = Menu    ESC = Exit",GLUT_BITMAP_HELVETICA_18);
}

// ── DRAW: win screen ──────────────────────────
void drawWinScreen(){
    // GL built-in background + GLUT fonts
    glClearColor(0,0.1f,0,1); glClear(GL_COLOR_BUFFER_BIT);
    char buf[32];
    glColor3f(0.2f,1.0f,0.4f); drawText(275,370,"YOU  WIN!  :D",GLUT_BITMAP_TIMES_ROMAN_24);
    glColor3f(1,1,1); sprintf(buf,"Score:  %d",score); drawText(340,310,buf,GLUT_BITMAP_HELVETICA_18);
    glColor3f(1.0f,0.9f,0.0f);
    drawText(225,250,"R = Play Again    M = Menu    ESC = Exit",GLUT_BITMAP_HELVETICA_18);
}

// ── GLUT display callback ─────────────────────
void display(){
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    if     (gameState==0) drawMenuScreen();
    else if(gameState==5) drawHelpScreen();
    else if(gameState==6) drawHighScoreScreen();
    else if(gameState==1||gameState==2){
        drawBackground(); drawBricks(); drawDropItems();
        drawBullets(); drawPaddle(); drawBall(); drawHUD();
        if(!ballMoving){glColor3f(.8f,.8f,.8f);drawText(265,75,"Press SPACE to launch ball!",GLUT_BITMAP_HELVETICA_12);}
        if(gameState==2) drawPauseOverlay();
    }
    else if(gameState==3) drawGameOverScreen();
    else if(gameState==4) drawWinScreen();
    glutSwapBuffers();
}

// ── GLUT timer (~60 fps) ──────────────────────
void gameTimer(int v){
    if(gameState==1) updateGameLogic();
    glutPostRedisplay();
    glutTimerFunc(16,gameTimer,0);
}

// ── GLUT keyboard input ───────────────────────
void keyboardInput(unsigned char key,int x,int y){
    if(key==27) exit(0);
    if(gameState==0){
        if(key=='1'){startNewGame();gameState=1;}
        if(key=='2') gameState=6;
        if(key=='3') gameState=5;
        if(key=='4') exit(0);
    }
    else if(gameState==5||gameState==6){if(key=='m'||key=='M')gameState=0;}
    else if(gameState==1){
        if(key==' ')           ballMoving=1;
        if(key=='p'||key=='P') gameState=2;
        if(key=='a'||key=='A'){paddleX-=20;if(paddleX<0)paddleX=0;}
        if(key=='d'||key=='D'){paddleX+=20;if(paddleX+paddleW>WIN_W)paddleX=WIN_W-paddleW;}
        if(key=='z'||key=='Z'){
            // shoot bullet
            for(int i=0;i<MAX_BULLETS;i++){
                if(!bltAlive[i]){bltX[i]=paddleX+paddleW/2;bltY[i]=paddleY+paddleH+5;bltAlive[i]=1;break;}
            }
        }
    }
    else if(gameState==2){
        if(key=='p'||key=='P') gameState=1;
        if(key=='r'||key=='R'){startNewGame();gameState=1;}
        if(key=='m'||key=='M') gameState=0;
    }
    else if(gameState==3||gameState==4){
        if(key=='r'||key=='R'){startNewGame();gameState=1;}
        if(key=='m'||key=='M') gameState=0;
    }
    glutPostRedisplay();
}

// ── GLUT arrow key input ──────────────────────
void arrowKeyInput(int key,int x,int y){
    if(gameState==1){
        if(key==GLUT_KEY_LEFT) {paddleX-=20;if(paddleX<0)paddleX=0;}
        if(key==GLUT_KEY_RIGHT){paddleX+=20;if(paddleX+paddleW>WIN_W)paddleX=WIN_W-paddleW;}
    }
    glutPostRedisplay();
}

// ── GLUT mouse motion ─────────────────────────
void mouseMoveInput(int x,int y){
    if(gameState==1||gameState==2){
        paddleX=x-paddleW/2;
        if(paddleX<0) paddleX=0;
        if(paddleX+paddleW>WIN_W) paddleX=WIN_W-paddleW;
    }
    glutPostRedisplay();
}

// ── GLUT mouse click ──────────────────────────
void mouseClickInput(int btn,int state,int x,int y){
    if(gameState==1&&btn==GLUT_LEFT_BUTTON&&state==GLUT_DOWN)
        ballMoving=1;
}

// ── MAIN ──────────────────────────────────────
int main(int argc,char**argv){
    srand((unsigned)time(NULL));
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    glutInitWindowSize(WIN_W,WIN_H);
    glutInitWindowPosition(100,50);
    glutCreateWindow("DX Ball - CSE 426");
    openGLInit();
    startNewGame();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboardInput);
    glutSpecialFunc(arrowKeyInput);
    glutPassiveMotionFunc(mouseMoveInput);
    glutMotionFunc(mouseMoveInput);
    glutMouseFunc(mouseClickInput);
    glutTimerFunc(16,gameTimer,0);
    glutMainLoop();
    return 0;
}
