#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>


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


// ── OpenGL init ───────────────────────────────
void openGLInit(){
    glClearColor(0,0,0,1);
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    gluOrtho2D(0,WIN_W,0,WIN_H);
    glMatrixMode(GL_MODELVIEW);  glLoadIdentity();
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
 DRAW: high score screen 
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


int main(int argc,char**argv){
    srand((unsigned)time(NULL));
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    glutInitWindowSize(WIN_W,WIN_H);
    glutInitWindowPosition(100,50);
    glutCreateWindow("DX Ball - CSE 426");
    openGLInit();
     glutMainLoop();
    return 0;
}
