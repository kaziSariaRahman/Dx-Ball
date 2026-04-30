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


// ── OpenGL init ───────────────────────────────
void openGLInit(){
    glClearColor(0,0,0,1);
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    gluOrtho2D(0,WIN_W,0,WIN_H);
    glMatrixMode(GL_MODELVIEW);  glLoadIdentity();
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
