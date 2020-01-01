/*
 *  FinalProject
 *
 *  m             Toggles light (sun) movement
 *  p             Toggles first person/perspective projection
 *  +/-           Change field of view of perspective
 *  x             Toggle axes display
 *  arrow keys    Change view angle
 *  PgDn/PgUp     Zoom in and out (in perspective view)
 *  0             Reset view angle
 *  ESC           Exit
 *
 */
#include "JTKPOLBAN.h"
#include "loadtexbmp.cpp"
#include "fatal.cpp"
#include "Errcheck.cpp"
#include "print.cpp"
#include "project.cpp"
#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif
int axes=0;       //  Display axes
int mode=1;       //  Projection mode
int move=1;       //  Move light
int th=-180;         //  Azimuth of view angle
int ph=35;         //  Elevation of view angle
int fov=55;       //  Field of view (for perspective)
int light=1;      //  Lighting
double asp=1.333;  //  Aspect ratio
double dim=35.0;   //  Size of world

// Light values
int one       =   1;  // Unit value
int distance  =   25;  // Light distance
int inc       =  10;  // Ball increment
int smooth    =   1;  // Smooth/Flat shading
int local     =   0;  // Local Viewer Model
int emission  =  30;  // Emission intensity (%)
int ambient   =  80;  // Ambient intensity (%)
int diffuse   = 100;  // Diffuse intensity (%)
int specular  =   0;  // Specular intensity (%)
int shininess =   0;  // Shininess (power of two)
float shiny   =   1;  // Shininess (value)
int zh        =  90;  // Light azimuth
float ylight  = 13;  // Elevation of light

int at0=100;      //  Constant  attenuation %
int at1=20;        //  Linear    attenuation %
int at2=20;        //  Quadratic attenuation %

double fpMoveInc = 0.02; //Multiplier for how much to move each keystroke in FP mode

//First person camera location
double fpX = 0;
double fpY = 0.7;
double fpZ = -0.3;

//x, y, z for refrence point in glLookAt() for FP mode
double refX = 5;
double refY = 0;
double refZ = 0;

//Texture Variables
int tMode = 0;
float texScale = 0.5;
GLuint
_textureSkyboxBack,
_textureSkyboxFront,
_textureSkyboxLeft,
_textureSkyboxRight,
_textureSkyboxTop,
_textureGlass,
_textureWoodFence,
_textureAsphalt,
_textureBasicMetal,
_textureBrownBrick,
_textureCarWheel,
_textureCinderBlock,
_textureGarageDoor,
_textureGrass,
_textureGreyBrick,
_textureHedge,
_textureSidewalk,
_textureWalkway,
_textureWhiteBrick,
_textureCarGrill,
_textureFrontDoor2,
_textureWindow1,
_textureMetalRoof,
_textureTireTread,
_textureWarehouseWindow,
_textureHeadLamp,
_textureCarbonFiber,
_textureWoodBeam;

//Light Vecotrs
float Ambient[]   = {0.01*80 ,0.01*80 ,0.01*80 ,1.0};
float Diffuse[]   = {1.0,1.0,1.0,1.0};
float Specular[]  = {0.01*0,0.01*0,0.01*0,1.0};

// track and car
float mTrackBlockSize = 4;
float mTrackStartPosX = 12;
float mTrackStartPosZ = 15;
float mCarInitAngle = 180;
int mCarMoveDelay = 5; // miliseconds per car steps

float mCar1StepsInc = 0.1;
float mCar1PosX = mTrackStartPosX;
float mCar1PosZ = mTrackStartPosZ;
float mCar1Angle = 0;
float mCar1Steps = 0;

float mCar2StepsInc = 0.1135;
float mCar2PosX = mTrackStartPosX;
float mCar2PosZ = mTrackStartPosZ + mTrackBlockSize;
float mCar2Angle = 0;
float mCar2Steps = 0;

/*
 *  Draw a cube
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 *     rotated th about the y axis
 */
static void cube(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th)
{
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);

   //Texture repitition values
   float texRepX = 1.0;
   float texRepY = 1.0;

   //  Cube
   glBegin(GL_QUADS);
   //  Front
   texRepX = dx/texScale;
   texRepY = dy/texScale;
   glNormal3f( 0, 0, 1);
   glTexCoord2f(0.0,0.0); glVertex3f(-1,-1, 1);
   glTexCoord2f(texRepX,0.0); glVertex3f(+1,-1, 1);
   glTexCoord2f(texRepX,texRepY); glVertex3f(+1,+1, 1);
   glTexCoord2f(0.0,texRepY); glVertex3f(-1,+1, 1);
   //  Back
   texRepX = dx/texScale;
   texRepY = dy/texScale;
   glNormal3f( 0, 0,-1);
   glTexCoord2f(0.0,0.0); glVertex3f(+1,-1,-1);
   glTexCoord2f(texRepX,0.0); glVertex3f(-1,-1,-1);
   glTexCoord2f(texRepX,texRepY); glVertex3f(-1,+1,-1);
   glTexCoord2f(0.0,texRepY); glVertex3f(+1,+1,-1);
   //  Right
   texRepX = dz/texScale;
   texRepY = dy/texScale;
   glNormal3f(+1, 0, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(+1,-1,+1);
   glTexCoord2f(texRepX,0.0); glVertex3f(+1,-1,-1);
   glTexCoord2f(texRepX,texRepY); glVertex3f(+1,+1,-1);
   glTexCoord2f(0.0,texRepY); glVertex3f(+1,+1,+1);
   //  Left
   texRepX = dz/texScale;
   texRepY = dy/texScale;
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(-1,-1,-1);
   glTexCoord2f(texRepX,0.0); glVertex3f(-1,-1,+1);
   glTexCoord2f(texRepX,texRepY); glVertex3f(-1,+1,+1);
   glTexCoord2f(0.0,texRepY); glVertex3f(-1,+1,-1);
   //  Top
   texRepX = dx/texScale;
   texRepY = dz/texScale;
   glNormal3f( 0,+1, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(-1,+1,+1);
   glTexCoord2f(texRepX,0.0); glVertex3f(+1,+1,+1);
   glTexCoord2f(texRepX,texRepY); glVertex3f(+1,+1,-1);
   glTexCoord2f(0.0,texRepY); glVertex3f(-1,+1,-1);
   //  Bottom
   texRepX = dx/texScale;
   texRepY = dz/texScale;
   glNormal3f( 0,-one, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(-1,-1,-1);
   glTexCoord2f(texRepX,0.0); glVertex3f(+1,-1,-1);
   glTexCoord2f(texRepX,texRepY); glVertex3f(+1,-1,+1);
   glTexCoord2f(0.0,texRepY); glVertex3f(-1,-1,+1);
   //  End
   glEnd();
   //  Undo transofrmations
   glPopMatrix();
}

/*
 *  Draw vertex in polar coordinates with normal
 */
static void Vertex(double th,double ph)
{
   double x = Sin(th)*Cos(ph);
   double y = Cos(th)*Cos(ph);
   double z =         Sin(ph);
   //  For a sphere at the origin, the position
   //  and normal vectors are the same
   glNormal3d(x,y,z);
   glVertex3d(x,y,z);
}

/*
 *  Draw a ball
 *     at (x,y,z)
 *     radius (r)
 */
static void ball(double x,double y,double z,double r)
{
   int th,ph;
   float yellow[] = {1.0,1.0,0.0,1.0};
   float Emission[]  = {1.0,1.0,1.0,1.0};
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glScaled(r,r,r);
   //  White ball
   glColor3f(1,1,0.9);
   glMaterialf(GL_FRONT,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
   glMaterialfv(GL_FRONT,GL_EMISSION,Emission);
   //  Bands of latitude
   for (ph=-90;ph<90;ph+=inc)
   {
      glBegin(GL_QUAD_STRIP);
      for (th=0;th<=360;th+=2*inc)
      {
         Vertex(th,ph);
         Vertex(th,ph+inc);
      }
      glEnd();
   }
   //  Undo transofrmations
   glPopMatrix();
}

/*
 *  Draw a section of the car body
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 *     rotated th about the y axis
 *     w (1 to color windows for car body, 0 otherwise)
 */
static void body(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th,
                 int w)
{
   //  Set specular color to white
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

   glBindTexture(GL_TEXTURE_2D,_textureBasicMetal);
   texScale = 0.1;

   glEnable(GL_POLYGON_OFFSET_FILL);

   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);

   //Texture repitition values
   float texRepX = 1.0;
   float texRepY = 1.0;

   //  Cube
   glBegin(GL_QUADS);

   //  Top
   texRepX = dx/texScale;
   texRepY = dz/texScale;
   glNormal3f( 0,+1, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(-1,+1,+1);
   glTexCoord2f(texRepX,0.0); glVertex3f(+1,+1,+1);
   glTexCoord2f(texRepX,texRepY); glVertex3f(+1,+1,-1);
   glTexCoord2f(0.0,texRepY); glVertex3f(-1,+1,-1);
   //  Bottom
   texRepX = dx/texScale;
   texRepY = dz/texScale;
   glNormal3f( 0,-one, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(-1,-1,-1);
   glTexCoord2f(texRepX,0.0); glVertex3f(+1,-1,-1);
   glTexCoord2f(texRepX,texRepY); glVertex3f(+1,-1,+1);
   glTexCoord2f(0.0,texRepY); glVertex3f(-1,-1,+1);

   //  Front
   texRepX = dx/texScale;
   texRepY = dy/texScale;
   glNormal3f( 0, 0, 1);
   glTexCoord2f(0.0,0.0); glVertex3f(-1,-1, 1);
   glTexCoord2f(texRepX,0.0); glVertex3f(+1,-1, 1);
   glTexCoord2f(texRepX,texRepY); glVertex3f(+1,+1, 1);
   glTexCoord2f(0.0,texRepY); glVertex3f(-1,+1, 1);
   //  Back
   texRepX = dx/texScale;
   texRepY = dy/texScale;
   glNormal3f( 0, 0,-1);
   glTexCoord2f(0.0,0.0); glVertex3f(+1,-1,-1);
   glTexCoord2f(texRepX,0.0); glVertex3f(-1,-1,-1);
   glTexCoord2f(texRepX,texRepY); glVertex3f(-1,+1,-1);
   glTexCoord2f(0.0,texRepY); glVertex3f(+1,+1,-1);

   //  End
   glEnd();

   glDisable(GL_POLYGON_OFFSET_FILL);
   //Color and texture to add windows
   if(w == 1) {
      glColor3f(0.8, 0.8, 1);
      glBindTexture(GL_TEXTURE_2D,_textureGlass);
      texRepX = 1.0;
      texRepY = 1.0;
      glBegin(GL_QUADS);
      //  Front
      glNormal3f(0, 0, 1);
      glTexCoord2f(0.0,0.0); glVertex3f(-0.8,-1, 1);
      glTexCoord2f(texRepX,0.0); glVertex3f(+0.8,-1, 1);
      glTexCoord2f(texRepX,texRepY); glVertex3f(+0.8,+1, 1);
      glTexCoord2f(0.0,texRepY); glVertex3f(-0.8,+1, 1);
      //  Back
      glNormal3f(0, 0,-1);
      glTexCoord2f(0.0,0.0); glVertex3f(+0.8,-1,-1);
      glTexCoord2f(texRepX,0.0); glVertex3f(-0.8,-1,-1);
      glTexCoord2f(texRepX,texRepY); glVertex3f(-0.8,+1,-1);
      glTexCoord2f(0.0,texRepY); glVertex3f(+0.8,+1,-1);
      glEnd();
   }
   glEnable(GL_POLYGON_OFFSET_FILL);
   
   //  Undo transformations
   glPopMatrix();
}

static void wheel(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th,
                 int d,
                 int s)
{
   //  Set specular color to white
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   //Turn off shininess for the rubber tire
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,0);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);

   glBindTexture(GL_TEXTURE_2D,_textureCarWheel);

   glColor3f(0.8,0.8,0.8);
   //Tire
   glBegin(GL_TRIANGLE_FAN);
   glNormal3f(0, 0, -1);
   glTexCoord2f(0.5,0.5);
   glVertex3f(0, 0, -0.05);
   for (th=0;th<=360;th+=s)
   {
      double ph = d-90;
      glTexCoord2f(0.5*Cos(th)+0.5,0.5*Sin(th)+0.5);
      glVertex3d(Sin(th)*Cos(ph), Cos(th)*Cos(ph), -0.05);
   }
   glEnd();

   glBegin(GL_TRIANGLE_FAN);
   glNormal3f(0, 0, 1);
   glTexCoord2f(0.5,0.5);
   glVertex3f(0, 0, 0.05);
   for (th=360;th>=0;th-=s)
   {
      double ph = d-90;
      glTexCoord2f(0.5*Cos(th)+0.5,0.5*Sin(th)+0.5);
      glVertex3d(Sin(th)*Cos(ph) , Cos(th)*Cos(ph), 0.05);
   }
   glEnd();

   glBindTexture(GL_TEXTURE_2D,_textureTireTread);

   glColor3f(0.5,0.5,0.55);
   glBegin(GL_QUAD_STRIP);
   for (th=0;th<=360;th+=s)
   {
      double ph = d-90;
      glNormal3f(Sin(th)*Cos(ph), Cos(th)*Cos(ph), 0);
      glTexCoord2f(0,0.1*th); glVertex3d(Sin(th)*Cos(ph), Cos(th)*Cos(ph), -0.05);
      glTexCoord2f(1,0.1*th); glVertex3d(Sin(th)*Cos(ph), Cos(th)*Cos(ph), 0.05);
   }
   // glNormal3f(Sin(0)*Cos(-90), Cos(0)*Cos(-90), 0);
   // glVertex3d(Sin(0)*Cos(-90), Cos(0)*Cos(-90), -0.05);
   // glVertex3d(Sin(0)*Cos(-90), Cos(0)*Cos(-90), 0.05);
   glEnd();

   //  Undo transformations
   glPopMatrix();
}

static void bumper(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th,
                 int m)
{
   //  Set specular color to white
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);

   //Texture repitition values
   float texRepX = 1.0;
   float texRepY = 1.0;

   glBindTexture(GL_TEXTURE_2D,_textureBasicMetal);
   texScale = 0.1;

   //Offset the bumper so that the lights and grill are drawn directly on the surface
   glEnable(GL_POLYGON_OFFSET_FILL);

   //Bumper

   //Base
   texRepX = dx/texScale;
   texRepY = dy/texScale;
   glBegin(GL_POLYGON);
   glNormal3f(0, -1, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(0,0,0.4);
   glTexCoord2f(texRepX,0.0); glVertex3f(0,0,-0.4);
   glTexCoord2f(texRepX,texRepY); glVertex3f(0.1,0,-0.35);
   glTexCoord2f(0.0,texRepY); glVertex3f(0.1,0,0.35);
   glEnd();

   //Front Panels
   glBegin(GL_QUADS);
   texRepX = dx/texScale;
   texRepY = dy/texScale;
   glNormal3f(0.447214, 0, 0.894427);
   glTexCoord2f(0.0,0.0); glVertex3f(0.1, 0, 0.35);
   glTexCoord2f(texRepX,0.0); glVertex3f(0.1, 0.2, 0.35);
   glTexCoord2f(texRepX,texRepY); glVertex3f(0, 0.2, 0.4);
   glTexCoord2f(0.0,texRepY); glVertex3f(0, 0, 0.4);

   texRepX = dx/texScale;
   texRepY = dy/texScale;
   glNormal3f(1, 0, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(0.1, 0, -0.35);
   glTexCoord2f(texRepX,0.0); glVertex3f(0.1, 0.2, -0.35);
   glTexCoord2f(texRepX,texRepY); glVertex3f(0.1, 0.2, 0.35);
   glTexCoord2f(0.0,texRepY); glVertex3f(0.1, 0, 0.35);

   texRepX = dx/texScale;
   texRepY = dy/texScale;
   glNormal3f(0.447214, 0, -0.894427);
   glTexCoord2f(0.0,0.0); glVertex3f(0.1, 0, -0.35);
   glTexCoord2f(texRepX,0.0); glVertex3f(0, 0, -0.4);
   glTexCoord2f(texRepX,texRepY); glVertex3f(0, 0.2, -0.4);
   glTexCoord2f(0.0,texRepY); glVertex3f(0.1, 0.2, -0.35);
   
   glEnd();

   //Upper Bumper
   glBegin(GL_QUADS);
   texRepX = dx/texScale;
   texRepY = dy/texScale;
   glNormal3f(0.447214, 0.894427, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(0, 0.25, 0.35);
   glTexCoord2f(texRepX,0.0); glVertex3f(0.1, 0.2, 0.35);
   glTexCoord2f(texRepX,texRepY); glVertex3f(0.1, 0.2, -0.35);
   glTexCoord2f(0.0,texRepY); glVertex3f(0, 0.25, -0.35);
   glEnd();

   glBegin(GL_TRIANGLES);
   texRepX = dx/texScale;
   texRepY = dy/texScale;
   glNormal3f(0.333333, 0.666667, 0.666667);
   glTexCoord2f(0.0, 0.0); glVertex3f(0, 0.2, 0.4);
   glTexCoord2f(texRepX/2, texRepY); glVertex3f(0.1, 0.2, 0.35);
   glTexCoord2f(texRepX, 0.0); glVertex3f(0, 0.25, 0.35);

   texRepX = dx/texScale;
   texRepY = dy/texScale;
   glNormal3f(0.333333, 0.666667, -0.666667);
   glTexCoord2f(0.0, 0.0); glVertex3f(0, 0.25, -0.35);
   glTexCoord2f(texRepX/2, texRepY); glVertex3f(0.1, 0.2, -0.35);
   glTexCoord2f(texRepX, 0.0); glVertex3f(0, 0.2, -0.4);
   glEnd();

   //  Disable polygon offset
   glDisable(GL_POLYGON_OFFSET_FILL);

   if (m == 1) {
      glColor3f(0.2,0.2,0.2);
      glBindTexture(GL_TEXTURE_2D,_textureCarGrill);

      //Grill
      glBegin(GL_QUADS);
      glNormal3f(1, 0, 0);
      glTexCoord2f(0.0,0.0); glVertex3f(0.1, 0.15, 0.18);
      glTexCoord2f(0.5,0.0); glVertex3f(0.1, 0.03, 0.18);
      glTexCoord2f(0.5,1.0); glVertex3f(0.1, 0.03, -0.18);
      glTexCoord2f(0.0,1.0); glVertex3f(0.1, 0.15, -0.18);
      glEnd();
   }

   //Lights (taillights or headlights)
   float emColor[4];
   if(m == 1) {
      emColor[0] = 1.0 * emission;
      emColor[1] = 1.0 * emission;
      emColor[2] = 0.8 * emission;
      emColor[3] = 1.0 * emission;
      glColor3f(1, 1, 0.8);
   } else {
      emColor[0] = 0.8 * emission;
      emColor[1] = 0.0 * emission;
      emColor[2] = 0.0 * emission;
      emColor[3] = 1.0 * emission;
      glColor3f(.8, 0, 0);
   }

   glMaterialf(GL_FRONT,GL_SHININESS,0);
   glMaterialfv(GL_FRONT,GL_SPECULAR,emColor);
   glMaterialfv(GL_FRONT,GL_EMISSION,emColor);

   glBindTexture(GL_TEXTURE_2D,_textureHeadLamp);

   glBegin(GL_TRIANGLE_FAN);
   glNormal3f(1, 0, 0);
   glTexCoord2f(0.5,0.5); glVertex3f(0.1, 0.13, -0.25);
   for (th=0;th<=360;th+=10)
   {
      double ph = 3-90;
      glTexCoord2f(0.5*Cos(th)+0.5,0.5*Sin(th)+0.5);
      glVertex3d(0.1, 0.13+(Cos(th)*Cos(ph)), -0.25+(Sin(th)*Cos(ph)));
   }
   glEnd();

   glBegin(GL_TRIANGLE_FAN);
   glNormal3f(1, 0, 0);
   glTexCoord2f(0.5,0.5); glVertex3f(0.1, 0.13, 0.25);
   for (th=0;th<=360;th+=10)
   {
      double ph = 3-90;
      glTexCoord2f(0.5*Cos(th)+0.5,0.5*Sin(th)+0.5);
      glVertex3d(0.1, 0.13+(Cos(th)*Cos(ph)), 0.25+(Sin(th)*Cos(ph)));
   }
   glEnd();

   glEnable(GL_POLYGON_OFFSET_FILL);

   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

   //Undo transformations
   glPopMatrix();  
}

static void policeCar(double x,double y,double z,
                     double dx,double dy,double dz,
                     double th)
{
   //  Set specular color to white
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

   float cr = 0.1;
   float cb = 0.1;
   float cg = 0.1;

   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);

   glPolygonOffset(1,1);

   wheel(0.6,0,0.4, 1,1,1, 0, 8, 10);
   wheel(-0.6,0,-0.4, 1,1,1, 0, 8, 10);
   wheel(0.6,0,-0.4, 1,1,1, 0, 8, 10);
   wheel(-0.6,0,0.4, 1,1,1, 0, 8, 10);

   glColor3f(cr, cb, cg);

   //Lower Body
   body(0,0.1,0, 0.8,0.1,0.4, 0, 0);
   //Cabin
   glColor3f(cr, cb, cg);
   body(0,0.3,0, 0.2,0.1,0.35, 0, 1);
   glColor3f(cr, cb, cg);
   body(-0.4,0.3,0, 0.2,0.1,0.35, 0, 1);

   texScale = 1.0;

   glColor3f(cr, cb, cg);

   //Front Bumper
   bumper(0.8,0,0, 1,1,1, 0, 1);

   glColor3f(cr, cb, cg);

   //Rear Bumper
   bumper(-0.8,0,0, 1,1,1, 180, 0);

   //  Set specular color to white
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
   
   glEnable(GL_POLYGON_OFFSET_FILL);

   glColor3f(cr, cb, cg);

   glBindTexture(GL_TEXTURE_2D,_textureBasicMetal);

   //Texture repitition values
   float texRepX = 1.0;
   float texRepY = 1.0;

   //Hood and upper side pannels
   texRepX = dx/texScale;
   texRepY = dz/texScale;
   glBegin(GL_QUADS);
   glNormal3f(0, 0.707107, 0.707107);
   glTexCoord2f(0.0,0.0); glVertex3f(-0.8, 0.25, 0.35);
   glTexCoord2f(texRepX,0.0); glVertex3f(-0.8, 0.2, 0.4);
   glTexCoord2f(texRepX,texRepY); glVertex3f(0.8, 0.2, 0.4);
   glTexCoord2f(0.0,texRepY); glVertex3f(0.8, 0.25, 0.35);

   glNormal3f(0, 1, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(0.4, 0.25, 0.35);
   glTexCoord2f(texRepX,0.0); glVertex3f(0.8, 0.25, 0.35);
   glTexCoord2f(texRepX,texRepY); glVertex3f(0.8, 0.25, -0.35);
   glTexCoord2f(0.0,texRepY); glVertex3f(0.4, 0.25, -0.35);
   
   glNormal3f(0, 0.707107, -0.707107);
   glTexCoord2f(0.0,0.0); glVertex3f(-0.8, 0.2, -0.4);
   glTexCoord2f(texRepX,0.0); glVertex3f(-0.8, 0.25, -0.35);
   glTexCoord2f(texRepX,texRepY); glVertex3f(0.8, 0.25, -0.35);
   glTexCoord2f(0.0,texRepY); glVertex3f(0.8, 0.2, -0.4);
   glEnd();

   glBindTexture(GL_TEXTURE_2D,_textureGlass);

   glColor3f(0.8, 0.8, 1);

   //Windshield
   texRepX = 1.0;
   texRepY = 1.0;
   glBegin(GL_QUADS);
   glNormal3f(0.6, 0.8, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(0.4,0.25,0.35);
   glTexCoord2f(texRepX,0.0); glVertex3f(0.4,0.25,-0.35);
   glTexCoord2f(texRepX,texRepY); glVertex3f(0.2,0.4,-0.35);
   glTexCoord2f(0.0,texRepY); glVertex3f(0.2,0.4,0.35);
   glEnd();

   glBegin(GL_TRIANGLES);
   glNormal3f(0,0,1);
   glTexCoord2f(0.0,0.0); glVertex3f(0.2,0.4,0.35);
   glTexCoord2f(texRepX, 0.0); glVertex3f(0.2,0.25,0.35);
   glTexCoord2f(texRepX, texRepY); glVertex3f(0.4,0.25,0.35);

   glNormal3f(0,0,-1);
   glTexCoord2f(0.0,0.0); glVertex3f(0.4,0.25,-0.35);
   glTexCoord2f(texRepX, 0.0); glVertex3f(0.2,0.25,-0.35);
   glTexCoord2f(texRepX, texRepY); glVertex3f(0.2,0.4,-0.35);
   glEnd();

   //Rear Window
   texRepX = 1.0;
   texRepY = 1.0;
   glBegin(GL_QUADS);
   glNormal3f(-0.6, 0.8, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(-0.8,0.25,-0.35);
   glTexCoord2f(texRepX,0.0); glVertex3f(-0.8,0.25,0.35);
   glTexCoord2f(texRepX,texRepY); glVertex3f(-0.6,0.4,0.35);
   glTexCoord2f(0.0,texRepY); glVertex3f(-0.6,0.4,-0.35);
   glEnd();

   glBegin(GL_TRIANGLES);
   glNormal3f(0,0,1);
   glTexCoord2f(0.0,0.0); glVertex3f(-0.8,0.25,0.35);
   glTexCoord2f(texRepX, 0.0); glVertex3f(-0.6,0.25,0.35);
   glTexCoord2f(texRepX, texRepY); glVertex3f(-0.6,0.4,0.35);
   
   glNormal3f(0,0,-1);
   glTexCoord2f(0.0,0.0); glVertex3f(-0.6,0.4,-0.35);
   glTexCoord2f(texRepX, 0.0); glVertex3f(-0.6,0.25,-0.35);
   glTexCoord2f(texRepX, texRepY); glVertex3f(-0.8,0.25,-0.35);
   glEnd();

   float t = glutGet(GLUT_ELAPSED_TIME)/1000.0;

   //Light bar

   glBindTexture(GL_TEXTURE_2D,_textureGlass);
   //Mid section
   glColor3f(0.1, 0.1, 0.1);
   cube(-0.2,0.42,0, 0.07,0.02,0.05, 0);

   //  Enable light 1 - Police Light
   glEnable(GL_LIGHT1);
   
   if ((int)(t/0.5) % 2 == 0) {
      float amb[4] = {0,0,0,0};
      float dif[4] = {0.8,0,0,1};
      float spec[4] = {0,0,0,1};
      float pos[4] = {-0.2,0.26,-0.15,1.0};
      //Red Light
      glLightfv(GL_LIGHT1,GL_AMBIENT ,amb);
      glLightfv(GL_LIGHT1,GL_DIFFUSE ,dif);
      glLightfv(GL_LIGHT1,GL_SPECULAR,spec);
      glLightfv(GL_LIGHT1,GL_POSITION,pos);

      glLightf(GL_LIGHT1,GL_CONSTANT_ATTENUATION ,at0/100.0);
      glLightf(GL_LIGHT1,GL_LINEAR_ATTENUATION   ,at1/100.0);
      glLightf(GL_LIGHT1,GL_QUADRATIC_ATTENUATION,at2/100.0);

      //Red Light
      float redEm[4] = {0.8, 0, 0, 1.0};
      glMaterialf(GL_FRONT,GL_SHININESS,0);
      glMaterialfv(GL_FRONT,GL_SPECULAR,redEm);
      glMaterialfv(GL_FRONT,GL_EMISSION,redEm);
      glColor3f(0.5, 0, 0);
      cube(-0.2,0.42,-0.15, 0.07,0.02,0.1, 0);
      //Blue Light
      glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
      glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
      glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
      glColor3f(0, 0, 0.5);
      cube(-0.2,0.42,0.15, 0.07,0.02,0.1, 0);
   } else {
      float amb[4] = {0,0,0,0};
      float dif[4] = {0,0,0.8,1};
      float spec[4] = {0,0,0,1};
      float pos[4] = {-0.2,0.26,0.15,1.0};
      //Blue Light
      glLightfv(GL_LIGHT1,GL_AMBIENT ,amb);
      glLightfv(GL_LIGHT1,GL_DIFFUSE ,dif);
      glLightfv(GL_LIGHT1,GL_SPECULAR,spec);
      glLightfv(GL_LIGHT1,GL_POSITION,pos);

      glLightf(GL_LIGHT1,GL_CONSTANT_ATTENUATION ,at0/100.0);
      glLightf(GL_LIGHT1,GL_LINEAR_ATTENUATION   ,at1/100.0);
      glLightf(GL_LIGHT1,GL_QUADRATIC_ATTENUATION,at2/100.0);

      //Red Light
      glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
      glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
      glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
      glColor3f(0.5, 0, 0);
      cube(-0.2,0.42,-0.15, 0.07,0.02,0.1, 0);
      //Blue Light
      float blueEm[4] = {0, 0, 0.8, 1.0};
      glMaterialf(GL_FRONT,GL_SHININESS,0);
      glMaterialfv(GL_FRONT,GL_SPECULAR,blueEm);
      glMaterialfv(GL_FRONT,GL_EMISSION,blueEm);
      glColor3f(0, 0, 0.5);
      cube(-0.2,0.42,0.15, 0.07,0.02,0.1, 0);
   }

   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

   //Undo transformations
   glPopMatrix();
}

static void car(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th,
                 float cr, float cb, float cg)
{
   //  Set specular color to white
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);

   glPolygonOffset(1,1);

   wheel(0.6,0,0.4, 1,1,1, 0, 8, 10);
   wheel(-0.6,0,-0.4, 1,1,1, 0, 8, 10);
   wheel(0.6,0,-0.4, 1,1,1, 0, 8, 10);
   wheel(-0.6,0,0.4, 1,1,1, 0, 8, 10);

   glColor3f(cr, cb, cg);

   //Lower Body
   body(0,0.1,0, 0.8,0.1,0.4, 0, 0);
   //Cabin
   body(-0.1,0.3,0, 0.3,0.1,0.35, 0, 1);

   texScale = 1.0;

   glColor3f(cr, cb, cg);

   //Front Bumper
   bumper(0.8,0,0, 1,1,1, 0, 1);

   glColor3f(cr, cb, cg);

   //Rear Bumper
   bumper(-0.8,0,0, 1,1,1, 180, 0);

   //  Set specular color to white
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
   
   glEnable(GL_POLYGON_OFFSET_FILL);

   glColor3f(cr, cb, cg);

   glBindTexture(GL_TEXTURE_2D,_textureBasicMetal);

   //Texture repitition values
   float texRepX = 1.0;
   float texRepY = 1.0;

   //Hood and upper side pannels
   texRepX = dx/texScale;
   texRepY = dz/texScale;
   glBegin(GL_QUADS);
   glNormal3f(0, 0.707107, 0.707107);
   glTexCoord2f(0.0,0.0); glVertex3f(-0.8, 0.25, 0.35);
   glTexCoord2f(texRepX,0.0); glVertex3f(-0.8, 0.2, 0.4);
   glTexCoord2f(texRepX,texRepY); glVertex3f(0.8, 0.2, 0.4);
   glTexCoord2f(0.0,texRepY); glVertex3f(0.8, 0.25, 0.35);

   glNormal3f(0, 1, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(0.4, 0.25, 0.35);
   glTexCoord2f(texRepX,0.0); glVertex3f(0.8, 0.25, 0.35);
   glTexCoord2f(texRepX,texRepY); glVertex3f(0.8, 0.25, -0.35);
   glTexCoord2f(0.0,texRepY); glVertex3f(0.4, 0.25, -0.35);
   
   glNormal3f(0, 0.707107, -0.707107);
   glTexCoord2f(0.0,0.0); glVertex3f(-0.8, 0.2, -0.4);
   glTexCoord2f(texRepX,0.0); glVertex3f(-0.8, 0.25, -0.35);
   glTexCoord2f(texRepX,texRepY); glVertex3f(0.8, 0.25, -0.35);
   glTexCoord2f(0.0,texRepY); glVertex3f(0.8, 0.2, -0.4);
   glEnd();

   //Trunk
   texRepX = dx/texScale;
   texRepY = dz/texScale;
   glBegin(GL_QUADS);
   glNormal3f(0,1,0);
   glTexCoord2f(0.0,0.0); glVertex3f(-0.8, 0.25, -0.35);
   glTexCoord2f(texRepX,0.0); glVertex3f(-0.8, 0.25, 0.35);
   glTexCoord2f(texRepX,texRepY); glVertex3f(-0.6, 0.25, 0.35);
   glTexCoord2f(0.0,texRepY); glVertex3f(-0.6, 0.25, -0.35);
   glEnd();

   glBindTexture(GL_TEXTURE_2D,_textureGlass);

   glColor3f(0.8, 0.8, 1);

   //Windshield
   texRepX = 1.0;
   texRepY = 1.0;
   glBegin(GL_QUADS);
   glNormal3f(0.6, 0.8, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(0.4,0.25,0.35);
   glTexCoord2f(texRepX,0.0); glVertex3f(0.4,0.25,-0.35);
   glTexCoord2f(texRepX,texRepY); glVertex3f(0.2,0.4,-0.35);
   glTexCoord2f(0.0,texRepY); glVertex3f(0.2,0.4,0.35);
   glEnd();

   glBegin(GL_TRIANGLES);
   glNormal3f(0,0,1);
   glTexCoord2f(0.0,0.0); glVertex3f(0.2,0.4,0.35);
   glTexCoord2f(texRepX, 0.0); glVertex3f(0.2,0.25,0.35);
   glTexCoord2f(texRepX, texRepY); glVertex3f(0.4,0.25,0.35);

   glNormal3f(0,0,-1);
   glTexCoord2f(0.0,0.0); glVertex3f(0.4,0.25,-0.35);
   glTexCoord2f(texRepX, 0.0); glVertex3f(0.2,0.25,-0.35);
   glTexCoord2f(texRepX, texRepY); glVertex3f(0.2,0.4,-0.35);
   glEnd();

   //Rear Window
   texRepX = 1.0;
   texRepY = 1.0;
   glBegin(GL_QUADS);
   glNormal3f(-0.6, 0.8, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(-0.6,0.25,-0.35);
   glTexCoord2f(texRepX,0.0); glVertex3f(-0.6,0.25,0.35);
   glTexCoord2f(texRepX,texRepY); glVertex3f(-0.4,0.4,0.35);
   glTexCoord2f(0.0,texRepY); glVertex3f(-0.4,0.4,-0.35);
   glEnd();

   glBegin(GL_TRIANGLES);
   glNormal3f(0,0,1);
   glTexCoord2f(0.0,0.0); glVertex3f(-0.6,0.25,0.35);
   glTexCoord2f(texRepX, 0.0); glVertex3f(-0.4,0.25,0.35);
   glTexCoord2f(texRepX, texRepY); glVertex3f(-0.4,0.4,0.35);
   
   glNormal3f(0,0,-1);
   glTexCoord2f(0.0,0.0); glVertex3f(-0.4,0.4,-0.35);
   glTexCoord2f(texRepX, 0.0); glVertex3f(-0.4,0.25,-0.35);
   glTexCoord2f(texRepX, texRepY); glVertex3f(-0.6,0.25,-0.35);
   glEnd();

   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

   glBindTexture(GL_TEXTURE_2D,_textureCarbonFiber);

   //Spoiler
   glColor3f(0.3,0.3,0.3);
   cube(-0.75,0.28,0.3, 0.02,0.03,0.02, 0);
   cube(-0.75,0.28,-0.3, 0.02,0.03,0.02, 0);

   texRepX = 5.0;
   texRepY = 1.0;

   glBegin(GL_QUADS);
   glNormal3f(0, -1, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(-0.7,0.31,-0.35);
   glTexCoord2f(texRepX,0.0); glVertex3f(-0.7,0.31,0.35);
   glTexCoord2f(texRepX,texRepY); glVertex3f(-0.8,0.31,0.35);
   glTexCoord2f(0.0,texRepY); glVertex3f(-0.8,0.31,-0.35);
   
   glNormal3f(0.196116, 0.980581, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(-0.8,0.33,-0.35);
   glTexCoord2f(texRepX,0.0); glVertex3f(-0.8,0.33,0.35);
   glTexCoord2f(texRepX,texRepY); glVertex3f(-0.7,0.31,0.35);
   glTexCoord2f(0.0,texRepY); glVertex3f(-0.7,0.31,-0.35);

   texRepX = 5.0;
   texRepY = 0.2;
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(-0.8,0.33,0.35);
   glTexCoord2f(texRepX,0.0); glVertex3f(-0.8,0.33,-0.35);
   glTexCoord2f(texRepX,texRepY); glVertex3f(-0.8,0.31,-0.35);
   glTexCoord2f(0.0,texRepY); glVertex3f(-0.8,0.31,0.35);
   
   glEnd();

   glBindTexture(GL_TEXTURE_2D,_textureBasicMetal);
   glColor3f(cr, cb, cg);

   //Spoiler Fins
   texRepX = dx/texScale;
   texRepY = dy/texScale;
   glBegin(GL_TRIANGLES);
   glNormal3f(0,0,-1);
   glTexCoord2f(0.0,0.0); glVertex3f(-0.68,0.31,-0.35);
   glTexCoord2f(texRepX, 0.0); glVertex3f(-0.82,0.31,-0.35);
   glTexCoord2f(texRepX, texRepY); glVertex3f(-0.82,0.35,-0.35);

   glNormal3f(0,0,1);
   glTexCoord2f(0.0,0.0); glVertex3f(-0.82,0.35,0.35);
   glTexCoord2f(texRepX, 0.0); glVertex3f(-0.82,0.31,0.35);
   glTexCoord2f(texRepX, texRepY); glVertex3f(-0.68,0.31,0.35);
   
   //Duplicate to draw both sides (with inverted normals) when face culling is on
   glNormal3f(0,0,1);
   glTexCoord2f(0.0,0.0); glVertex3f(-0.68,0.31,-0.35);
   glTexCoord2f(texRepX, 0.0); glVertex3f(-0.82,0.31,-0.35);
   glTexCoord2f(texRepX, texRepY); glVertex3f(-0.82,0.35,-0.35);

   glNormal3f(0,0,-1);
   glTexCoord2f(0.0,0.0); glVertex3f(-0.82,0.35,0.35);
   glTexCoord2f(texRepX, 0.0); glVertex3f(-0.82,0.31,0.35);
   glTexCoord2f(texRepX, texRepY); glVertex3f(-0.68,0.31,0.35);
   
   glEnd();

   //Undo transformations
   glPopMatrix();
}

static void workshop(double x, double z, double th)
{
   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,0,z);
   glRotated(th,0,1,0);

   //Building - Brown Workshop
   texScale = 0.5;
   cube(0,1.6,0, 2,0.4,1, 0); //Top
   cube(-1.75,0.65,0, 0.25,0.55,1, 0); //Left
   cube(1.75,0.65,0, 0.25,0.55,1, 0); //Right
   cube(0,0.65,0, 0.2,0.55,1, 0); //Middle

   //"Floor" for brown workshop
   glColor3f(0.7, 0.7, 0.7);
   glBindTexture(GL_TEXTURE_2D,_textureSidewalk);
   texScale = 1;
   cube(0,-0.05,0.9, 2,0.15,0.1, 0);
   
   //Garage Doors
   glColor3f(0.5, 0.5, 0.5);
   glBindTexture(GL_TEXTURE_2D,_textureGarageDoor);
   glBegin(GL_QUADS);
   float texRepX = 1.0;
   float texRepY = 1.0;
   //Left
   glNormal3f(0, 0, 1);
   glTexCoord2f(0.0,0.0); glVertex3f(-1.5, 0.1, 0.9);
   glTexCoord2f(texRepX,0.0); glVertex3f(-0.2, 0.1, 0.9);
   glTexCoord2f(texRepX,texRepY); glVertex3f(-0.2, 1.2, 0.9);
   glTexCoord2f(0.0,texRepY); glVertex3f(-1.5, 1.2, 0.9);
   //Right
   glNormal3f(0, 0, 1);
   glTexCoord2f(0.0,0.0); glVertex3f(0.2, 0.1, 0.9);
   glTexCoord2f(texRepX,0.0); glVertex3f(1.5, 0.1, 0.9);
   glTexCoord2f(texRepX,texRepY); glVertex3f(1.5, 1.2, 0.9);
   glTexCoord2f(0.0,texRepY); glVertex3f(0.2, 1.2, 0.9);
   glEnd();

   //Undo transformations
   glPopMatrix();
}

static void greyHouse(double x, double z, double th) {
   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,0,z);
   glRotated(th,0,1,0);

   glPolygonOffset(1,1);
   glEnable(GL_POLYGON_OFFSET_FILL);

   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

   //Walkway (to house)
   glColor3f(0.7, 0.7, 0.7);
   glBindTexture(GL_TEXTURE_2D,_textureWalkway);
   texScale = 0.4;
   cube(0,-0.05,-1.5, 0.5,0.15,0.4, 90); // Between hedges

   //Hedges
   glColor3f(0, 0.3, 0.1);
   glBindTexture(GL_TEXTURE_2D,_textureHedge);
   texScale = 0.25;
   cube(1.23,0.3,-1.2, 0.77,0.2,0.17, 0);
   cube(-1.23,0.3,-1.2, 0.77,0.2,0.17, 0);

   //Grass
   glColor3f(0.7, 0.7, 0.7);
   glBindTexture(GL_TEXTURE_2D,_textureGrass);
   texScale = 0.5;
   cube(-1.2,-0.05,-1.5, 0.8,0.15,0.5, 0);
   cube(1.2,-0.05,-1.5, 0.8,0.15,0.5, 0);

   //Building - Grey House
   glColor3f(0.7, 0.7, 0.7);
   glBindTexture(GL_TEXTURE_2D,_textureGreyBrick);
   texScale = 0.5;
   cube(0,0.9,-3, 2,0.8,1, 0);

   //Door Frame
   glColor3f(0.5, 0.5, 0.5);
   glBindTexture(GL_TEXTURE_2D,_textureWoodBeam);
   texScale = 0.5;
   cube(-0.4,0.65,-1.95, 0.1,0.55,0.05, 0);
   cube(0.4,0.65,-1.95, 0.1,0.55,0.05, 0);
   cube(0,1.25,-1.95, 0.5,0.05,0.05, 0);

   //Window Sills
   cube(-1.3,0.65,-1.95, 0.45,0.05,0.05, 0); //Left
   cube(1.3,0.65,-1.95, 0.45,0.05,0.05, 0); //Right

   glDisable(GL_POLYGON_OFFSET_FILL);

   //Door
   glColor3f(0.5, 0.5, 0.5);
   glBindTexture(GL_TEXTURE_2D,_textureFrontDoor2);
   glBegin(GL_QUADS);
   double texRepX = 1.0;
   double texRepY = 1.0;
   glNormal3f(0, 0, 1);
   glTexCoord2f(0.0,0.0); glVertex3f(-0.31, 0.1, -2);
   glTexCoord2f(texRepX,0.0); glVertex3f(0.31, 0.1, -2);
   glTexCoord2f(texRepX,texRepY); glVertex3f(0.31, 1.21, -2);
   glTexCoord2f(0.0,texRepY); glVertex3f(-0.31, 1.21, -2);
   glEnd();

   //Windows
   glColor3f(0.7, 0.7, 0.7);

   glBindTexture(GL_TEXTURE_2D,_textureWindow1);
   glBegin(GL_QUADS);
   texRepX = 1.0;
   texRepY = 2.0;

   //Left Window
   glNormal3f(0, 0, 1);
   glTexCoord2f(0.0,0.0); glVertex3f(-1.7, 0.7, -2);
   glTexCoord2f(texRepX,0.0); glVertex3f(-0.9, 0.7, -2);
   glTexCoord2f(texRepX,texRepY); glVertex3f(-0.9, 1.3, -2);
   glTexCoord2f(0.0,texRepY); glVertex3f(-1.7, 1.3, -2);

   //Right Window
   glNormal3f(0, 0, 1);
   glTexCoord2f(0.0,0.0); glVertex3f(0.9, 0.7, -2);
   glTexCoord2f(texRepX,0.0); glVertex3f(1.7, 0.7, -2);
   glTexCoord2f(texRepX,texRepY); glVertex3f(1.7, 1.3, -2);
   glTexCoord2f(0.0,texRepY); glVertex3f(0.9, 1.3, -2);

   glEnd();

   //Undo transformations
   glPopMatrix();
}

static void skybox(float dim) {
   glDisable(GL_POLYGON_OFFSET_FILL);

   //  Set specular color to white
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,0);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

   glColor3f(0.7, 0.7, 0.7);
   glBindTexture(GL_TEXTURE_2D,_textureSkyboxFront);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glBegin(GL_QUADS);
   glNormal3f(0, 0, -1);
   glTexCoord2f(1.0,0.0); glVertex3f(+dim,-dim, dim);
   glTexCoord2f(0.0,0.0); glVertex3f(-dim,-dim, dim);
   glTexCoord2f(0.0,1.0); glVertex3f(-dim,+dim, dim);
   glTexCoord2f(1.0,1.0); glVertex3f(+dim,+dim, dim);
   glEnd();

   glBindTexture(GL_TEXTURE_2D,_textureSkyboxBack);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glBegin(GL_QUADS);
   glNormal3f(0, 0, 1);
   glTexCoord2f(1.0,0.0); glVertex3f(-dim,-dim, -dim);
   glTexCoord2f(0.0,0.0); glVertex3f(+dim,-dim, -dim);
   glTexCoord2f(0.0,1.0); glVertex3f(+dim,+dim, -dim);
   glTexCoord2f(1.0,1.0); glVertex3f(-dim,+dim, -dim);
   glEnd();

   glBindTexture(GL_TEXTURE_2D,_textureSkyboxRight);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glBegin(GL_QUADS);
   glNormal3f(-1, 0, 0);
   glTexCoord2f(1.0,0.0); glVertex3f(dim,-dim, -dim);
   glTexCoord2f(0.0,0.0); glVertex3f(dim,-dim, +dim);
   glTexCoord2f(0.0,1.0); glVertex3f(dim,+dim, +dim);
   glTexCoord2f(1.0,1.0); glVertex3f(dim,+dim, -dim);
   glEnd();

   glBindTexture(GL_TEXTURE_2D,_textureSkyboxLeft);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glBegin(GL_QUADS);
   glNormal3f(1, 0, 0);
   glTexCoord2f(1.0,0.0); glVertex3f(-dim,-dim, +dim);
   glTexCoord2f(0.0,0.0); glVertex3f(-dim,-dim, -dim);
   glTexCoord2f(0.0,1.0); glVertex3f(-dim,+dim, -dim);
   glTexCoord2f(1.0,1.0); glVertex3f(-dim,+dim, +dim);
   glEnd();

   glBindTexture(GL_TEXTURE_2D,_textureSkyboxTop);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glBegin(GL_QUADS);
   glNormal3f(0, -1, 0);
   glTexCoord2f(1.0,0.0); glVertex3f(+dim,dim, +dim);
   glTexCoord2f(0.0,0.0); glVertex3f(-dim,dim, +dim);
   glTexCoord2f(0.0,1.0); glVertex3f(-dim,dim, -dim);
   glTexCoord2f(1.0,1.0); glVertex3f(+dim,dim, -dim);
   glEnd();
}

static void lampPost(double x,double y,double z,
                     double dx,double dy,double dz,
                     double th)
{
   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(th,0,1,0);

   //  Set specular color to white
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

   float radScale = 0.03;
   float baseRadScale = 0.05;
   float capRadScale = 0.01;
   float outerRadScale = 0.08;

   //Pole
   glColor3f(0.4, 0.4, 0.4);
   glBindTexture(GL_TEXTURE_2D,_textureBasicMetal);   
   glBegin(GL_QUADS);
   int i;
   for(i=45; i <= 360; i += 45) {
      //Base
      glNormal3f(Cos(i-22.5),0,Sin(i-22.5));
      glTexCoord2f(Cos(i), 0); glVertex3f(baseRadScale*Cos(i), 0, baseRadScale*Sin(i));
      glTexCoord2f(Cos(i), 0); glVertex3f(baseRadScale*Cos(i-45), 0, baseRadScale*Sin(i-45));
      glTexCoord2f(Cos(i), 1); glVertex3f(radScale*Cos(i-45), 0.1, radScale*Sin(i-45));
      glTexCoord2f(Cos(i), 1); glVertex3f(radScale*Cos(i), 0.1, radScale*Sin(i));
      //Pole Lower
      glNormal3f(Cos(i-22.5),0,Sin(i-22.5));
      glTexCoord2f(Cos(i), 0); glVertex3f(radScale*Cos(i), 0.1, radScale*Sin(i));
      glTexCoord2f(Cos(i), 0); glVertex3f(radScale*Cos(i-45), 0.1, radScale*Sin(i-45));
      glTexCoord2f(Cos(i), 2); glVertex3f(radScale*Cos(i-45), 0.5, radScale*Sin(i-45));
      glTexCoord2f(Cos(i), 2); glVertex3f(radScale*Cos(i), 0.5, radScale*Sin(i));
      //Pole Upper
      glNormal3f(Cos(i-22.5),0,Sin(i-22.5));
      glTexCoord2f(Cos(i), 0); glVertex3f(radScale*Cos(i), 0.5, radScale*Sin(i));
      glTexCoord2f(Cos(i), 0); glVertex3f(radScale*Cos(i-45), 0.5, radScale*Sin(i-45));
      glTexCoord2f(Cos(i), 2); glVertex3f(radScale*Cos(i-45), 1, radScale*Sin(i-45));
      glTexCoord2f(Cos(i), 2); glVertex3f(radScale*Cos(i), 1, radScale*Sin(i));
      //Light Cap
      glNormal3f(Cos(i-22.5),0,Sin(i-22.5));
      glTexCoord2f(Cos(i), 0); glVertex3f(radScale*Cos(i), 1.15, radScale*Sin(i));
      glTexCoord2f(Cos(i), 0); glVertex3f(radScale*Cos(i-45), 1.15, radScale*Sin(i-45));
      glTexCoord2f(Cos(i), 1); glVertex3f(capRadScale*Cos(i-45), 1.17, capRadScale*Sin(i-45));
      glTexCoord2f(Cos(i), 1); glVertex3f(capRadScale*Cos(i), 1.17, capRadScale*Sin(i));
   }
   glEnd();

   //Light
   float em[4] = {0.8, 0.8, 0.1, 1.0};
   glMaterialf(GL_FRONT,GL_SHININESS,0);
   glMaterialfv(GL_FRONT,GL_SPECULAR,em);
   glMaterialfv(GL_FRONT,GL_EMISSION,em);
   glColor3f(0.8, 0.8, 0.1);
   glBindTexture(GL_TEXTURE_2D,_textureGlass);   
   glBegin(GL_QUADS);
   i = 45;
   for(i = 45; i <= 360; i += 45) {
      //Lower
      glNormal3f(Cos(i-22.5),0,Sin(i-22.5));
      glTexCoord2f(Cos(i), 0); glVertex3f(radScale*Cos(i), 1, radScale*Sin(i));
      glTexCoord2f(Cos(i), 0); glVertex3f(radScale*Cos(i-45), 1, radScale*Sin(i-45));
      glTexCoord2f(Cos(i), 1); glVertex3f(outerRadScale*Cos(i-45), 1.1, outerRadScale*Sin(i-45));
      glTexCoord2f(Cos(i), 1); glVertex3f(outerRadScale*Cos(i), 1.1, outerRadScale*Sin(i));
      //Upper
      glNormal3f(Cos(i-22.5),0,Sin(i-22.5));
      glTexCoord2f(Cos(i), 0); glVertex3f(outerRadScale*Cos(i), 1.1, outerRadScale*Sin(i));
      glTexCoord2f(Cos(i), 0); glVertex3f(outerRadScale*Cos(i-45), 1.1, outerRadScale*Sin(i-45));
      glTexCoord2f(Cos(i), 1); glVertex3f(radScale*Cos(i-45), 1.15, radScale*Sin(i-45));
      glTexCoord2f(Cos(i), 1); glVertex3f(radScale*Cos(i), 1.15, radScale*Sin(i));
   }
   glEnd();

   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

   //Undo transformations
   glPopMatrix();
}

static void curve(double x, double y, double z,
                 double dx, double dy, double dz,
                 double th, double radius)
{
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);

   //Texture repitition values
   float texRepX = 1.0;
   float texRepY = 1.0;

   texRepX = dx/texScale;
   texRepY = dz/texScale;
	int i;
	float x_now; 
			float z_now;
	const unsigned int triangles = 20; // number of triangles
	const float twoPi = 2.0f * 3.14159f;
	float delta = twoPi / triangles;
	// Top
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(0.0,+1.0,0.0); // origin
	for(i = 0; i <= triangles/4; i++){
		x_now =  (radius * cos((i) *  delta));
		z_now =  (radius * sin((i) * delta));
		glTexCoord2f(x_now,z_now); glVertex3f(x_now,+1.0,z_now);
	}
	glEnd();
	
	// Bottom
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(0.0,-1.0,0.0); // origin
	for(i = 0; i <= triangles/4; i++){
		x_now =  (radius * cos((i) *  delta));
		z_now =  (radius * sin((i) * delta));
		glTexCoord2f(x_now,z_now); glVertex3f(x_now,-1.0,z_now);
	}
	glEnd();
	
	// Side Circular
	glBegin(GL_TRIANGLE_STRIP);
		for(i = 0; i <= triangles/4;i++) { 
			x_now =  (radius * cos((i) *  delta));
			z_now =  (radius * sin((i) * delta));
			glTexCoord2f(x_now,z_now); glVertex3f(x_now,+1.0,z_now);
			glTexCoord2f(x_now,z_now); glVertex3f(x_now,-1.0,z_now);
		}
	glEnd();
   glPopMatrix();
}

void track(void) {
  	float xPos = mTrackStartPosX;
	float zPos = mTrackStartPosZ;
	float yPos = 0.1;
	float carPosY = yPos + 0.4;
	float dx = 0.5 * mTrackBlockSize;
	float dy = 0.1;
	float dz = 0.5 * mTrackBlockSize;
	float radius = dx * 4 / (mTrackBlockSize/2);
	texScale = dx;
	
	// lane 1, step 0 - 12, direction -x
	for(int i = 0; i <= 12; i++) {
		cube(xPos, yPos, zPos, dx, dy, dz, 0);
		cube(xPos, yPos, zPos+mTrackBlockSize, dx, dy, dz, 0);
		xPos -= mTrackBlockSize;
	}
	
	// turn 1, step 13, quadran 3 (270 degree)
	curve(xPos+(mTrackBlockSize/2), yPos, zPos-(mTrackBlockSize/2), dx, dy, dz, 270, radius);

	// lane 2, step 14 - 16, direction -z
	zPos -= mTrackBlockSize;
	for(int i = 14; i <= 16; i++) {
		cube(xPos, yPos, zPos, dx, dy, dz, 0);
		cube(xPos-mTrackBlockSize, yPos, zPos, dx, dy, dz, 0);
		zPos -= mTrackBlockSize;
	}
	
	// turn 2, step 17, quadran 2 (180 degree)
	curve(xPos+(mTrackBlockSize/2), yPos, zPos+(mTrackBlockSize/2), dx, dy, dz, 180, radius);
	
	// lane 3, step 18 - 20, direction +x
	xPos += mTrackBlockSize;
	for(int i = 18; i <= 20; i++) {
		cube(xPos, yPos, zPos, dx, dy, dz, 0);
		cube(xPos, yPos, zPos-mTrackBlockSize, dx, dy, dz, 0);
		xPos += mTrackBlockSize;
	}
	
	// turn 3, step 21, quadran 4 (360 degree)
	zPos -= mTrackBlockSize;
	curve(xPos-(mTrackBlockSize/2), yPos, zPos-(mTrackBlockSize/2), dx, dy, dz, 360, radius);
	xPos += mTrackBlockSize;
	
	// lane 4, step 22 - 24, direction -z
	zPos -= mTrackBlockSize;
	for(int i = 22; i <= 24; i++) {
		cube(xPos, yPos, zPos, dx, dy, dz, 0);
		cube(xPos-mTrackBlockSize, yPos, zPos, dx, dy, dz, 0);
		zPos -= mTrackBlockSize;
	}
	
	// turn 4, step 25, quadran 2 (180 degree)
	curve(xPos+(mTrackBlockSize/2), yPos, zPos+(mTrackBlockSize/2), dx, dy, dz, 180, radius);
	
	// lane 5, step 26 - 38, direction +x
	xPos += mTrackBlockSize;
	for(int i = 26; i <= 38; i++) {
		cube(xPos, yPos, zPos, dx, dy, dz, 0);
		cube(xPos, yPos, zPos-mTrackBlockSize, dx, dy, dz, 0);
		xPos += mTrackBlockSize;
	}
	
	// turn 5, step 39, quadran 1 (90 degree)
	curve(xPos-(mTrackBlockSize/2), yPos, zPos+(mTrackBlockSize/2), dx, dy, dz, 90, radius);
	
	// lane 6, step 40 - 42, direction +z
	zPos += mTrackBlockSize;
	for(int i = 40; i <= 42; i++) {
		cube(xPos, yPos, zPos, dx, dy, dz, 0);
		cube(xPos+mTrackBlockSize, yPos, zPos, dx, dy, dz, 0);
		zPos += mTrackBlockSize;
	}
	
	// turn 6, step 43, quadran 4 (360 degree)
	curve(xPos-(mTrackBlockSize/2), yPos, zPos-(mTrackBlockSize/2), dx, dy, dz, 360, radius);
	
	// lane 7, step 44 - 46, direction -x
	xPos -= mTrackBlockSize;
	for(int i = 44; i <= 46; i++) {
		cube(xPos, yPos, zPos, dx, dy, dz, 0);
		cube(xPos, yPos, zPos+mTrackBlockSize, dx, dy, dz, 0);
		xPos -= mTrackBlockSize;
	}
	
	// turn 7, step 47, quadran 2 (180 degree)
	zPos += mTrackBlockSize;
	curve(xPos+(mTrackBlockSize/2), yPos, zPos+(mTrackBlockSize/2), dx, dy, dz, 180, radius);
	xPos -= mTrackBlockSize;
	
	// lane 8, step 48 - 50, direction +z
	zPos += mTrackBlockSize;
	for(int i = 48; i <= 50; i++) {
		cube(xPos, yPos, zPos, dx, dy, dz, 0);
		cube(xPos+mTrackBlockSize, yPos, zPos, dx, dy, dz, 0);
		zPos += mTrackBlockSize;
	}
	
	// turn 8, step 51, quadran 4 (360 degree)
	curve(xPos-(mTrackBlockSize/2), yPos, zPos-(mTrackBlockSize/2), dx, dy, dz, 360, radius);
	
	// car 1 blue
	car(mCar1PosX, carPosY, mCar1PosZ, 0.5*mTrackBlockSize, 0.4*mTrackBlockSize, 0.4*mTrackBlockSize, -(mCarInitAngle+mCar1Angle), 0.2, 0.2, 0.7);
	
	// car 2 red
	car(mCar2PosX, carPosY, mCar2PosZ, 0.5*mTrackBlockSize, 0.4*mTrackBlockSize, 0.4*mTrackBlockSize, -(mCarInitAngle+mCar2Angle), 0.7, 0.2, 0.2);
}

void grass(void) {
	float xPos = mTrackStartPosX + (mTrackBlockSize * 11);
	float zPos = mTrackStartPosZ + (mTrackBlockSize * 5);
	float yPos = 0.0;
	float dx = 0.5 * mTrackBlockSize;
	float dy = 0.1;
	float dz = 0.5 * mTrackBlockSize;
	texScale = dx;
	
	for(int i = 0; i < 30; i++) {
		float zTemp = zPos;
		for(int j = 0; j < 20; j++) {
			cube(xPos, yPos, zTemp, dx, dy, dz, 0);
			zTemp -= mTrackBlockSize;
		}
		xPos -= mTrackBlockSize;
	}
}


/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
   const double len=2.0;  //  Length of axes
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   //Enable Face Culling
   glEnable(GL_CULL_FACE);
   //Enable Textures
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , tMode?GL_REPLACE:GL_MODULATE);


	glEnable(GL_CULL_FACE);
	glDisable(GL_CULL_FACE);
   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective - set eye position
   if (mode)
   {
      double Ex = -2*dim*Sin(th)*Cos(ph);
      double Ey = +2*dim        *Sin(ph);
      double Ez = +2*dim*Cos(th)*Cos(ph);
      gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
   }
   //  First Person
   else
   {
      refX = (dim * Sin(th)) + fpX;
      refY = (dim * Sin(ph));
      refZ = (dim * -Cos(th)) + fpZ;
      gluLookAt(fpX,fpY,fpZ, refX,refY,refZ, 0,1,0);
   }

   //  Flat or smooth shading
   glShadeModel(smooth ? GL_SMOOTH : GL_FLAT);

   //  Light position
   float Position[]  = {distance*Cos(zh),distance*Sin(zh),0,1.0};
   // float MoonPosition[]  = {-(distance*Cos(zh)),-(distance*Sin(zh)),0,1.0};

   //  Draw light position as ball (still no lighting here)
//   glColor3f(1,1,1);
//   ball(Position[0],Position[1],Position[2] , 1); //Sun
//    ball(MoonPosition[0],MoonPosition[1],MoonPosition[2], 0.3); //Moon

   //  OpenGL should normalize normal vectors
   glEnable(GL_NORMALIZE);
   //  Enable lighting
   glEnable(GL_LIGHTING);
   //  Location of viewer for specular calculations
   glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,local);
   //  glColor sets ambient and diffuse color materials
   glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
   glEnable(GL_COLOR_MATERIAL);

   //  Enable light 0 - Sun
   glEnable(GL_LIGHT0);
   //  Set ambient, diffuse, specular components and position of light 0 (Sun)
   glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
   glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
   glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
   glLightfv(GL_LIGHT0,GL_POSITION,Position);
   

   //Inital values for texture repitition
   double texRepX = 1.0;
   double texRepY = 1.0;

   //Draw scene

   //Skybox
   skybox(128);

   //  Set specular color to white
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

   //Enable offset for windows, doors, and other decals
   glPolygonOffset(1,1);
   glEnable(GL_POLYGON_OFFSET_FILL);


//  . Street surface 
   glColor3f(0.4, 0.4, 0.4);
   glBindTexture(GL_TEXTURE_2D,_textureAsphalt);
//   texScale = 0.5;
   track();
   
   glColor3f(0.4, 0.6, 0.2);
   glBindTexture(GL_TEXTURE_2D, _textureGrass);
   grass();

// //  Street Surface - Side Streets
//   glColor3f(0.4, 0.4, 0.4);
//   glBindTexture(GL_TEXTURE_2D,_textureAsphalt);
//   texScale = 0.5;
//   float yPos = -3.5;
//
//   for(i = 0; i < 6; i++){
//      cube(-7.75,-0.1,yPos, 1,0.1,0.75, 90);
//      cube(-9.25,-0.1,yPos, 1,0.1,0.75, 90);
//      cube(7.75,-0.1,yPos, 1,0.1,0.75, 90);
//      cube(9.25,-0.1,yPos, 1,0.1,0.75, 90);
//      yPos += 2;
//   }

   //  Display parameters
  // glWindowPos2i(5,5);
//   printf("\nAngle=%d,%d  Dim=%.1f  FOV=%d  Projection=%s\n",th,ph,dim,fov,mode?"Perpective":"FP");

   //  Render the scene and make it visible
   ErrCheck("display");
   glFlush();
   glutSwapBuffers();
}

static void setEnvLighting()
{
   float ambScale = 0.8;

   if(Sin(zh) >= 0.2) {
      Ambient[0] = (255 / 255) * ambScale;
      Ambient[1] = (255 / 255) * ambScale;
      Ambient[2] = (255 / 255) * ambScale;

      Diffuse[0] = 1.0;
      Diffuse[1] = 1.0;
      Diffuse[2] = 1.0;
   } else if (Sin(zh) < 0.4 && Sin(zh) >= 0) {
      Ambient[0] = (255 / 255) * ambScale;
      Ambient[1] = ((60 + (195*Sin(zh)*2.5)) / 255) * ambScale;
      Ambient[2] = ((60 + (195*Sin(zh)*2.5)) / 255) * ambScale;

      Diffuse[0] = Sin(zh)*2.5;
      Diffuse[1] = Sin(zh)*2.5;
      Diffuse[2] = Sin(zh)*2.5;
   } else if (Sin(zh) < 0 && Sin(zh) >= -0.4){
      Ambient[0] = ((255 - (235*Sin(zh)*-2.5)) / 255) * ambScale;
      Ambient[1] = (60 / 255) * ambScale;
      Ambient[2] = ((60 + (120*Sin(zh)*-2.5)) / 255) * ambScale;

      Diffuse[0] = 0;
      Diffuse[1] = 0;
      Diffuse[2] = 0;
   } else {
      Ambient[0] = (20 / 255) * ambScale;
      Ambient[1] = (60 / 255) * ambScale;
      Ambient[2] = (180 / 255) * ambScale;

      Diffuse[0] = 0;
      Diffuse[1] = 0;
      Diffuse[2] = 0;
   }
}

/*
 *  GLUT calls this routine when the window is resized
 */
void idle()
{
   //  Elapsed time in seconds
   double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   zh = fmod(10*t,360.0);

//   setEnvLighting();

   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

void car1Timer(void) {
	
	float steps = mCar1Steps / mTrackBlockSize;
	
	if(mCar1Angle > 359) mCar1Angle = 0;
	else if(mCar1Angle < 0) mCar1Angle = 359;
	
	// lane 1, step 0 - 12, direction -x
	if(steps >= 0 && steps < 12) {
		if(mCar1Angle > 0 && mCar1Angle < 360) mCar1Angle++;
		mCar1PosX -= mCar1StepsInc;
		mCar1Steps += mCar1StepsInc;
	}
	
	// turn 1, step 12 - 14, turn right(+), from 0 to 90
	if(steps >= 12 && steps < 14) {
		if(mCar1Angle < 90) mCar1Angle++;
		mCar1PosX -= mCar1StepsInc / 2;
		mCar1PosZ -= mCar1StepsInc / 2;
		mCar1Steps += mCar1StepsInc;
	}
	
	// lane 2, step 14 - 16, direction -z
	if(steps >= 14 && steps < 16) {
		if(mCar1Angle < 90) mCar1Angle++;
		mCar1PosZ -= mCar1StepsInc;
		mCar1Steps += mCar1StepsInc;
	}
	
	// turn 2, step 16 - 18, turn right(+), from 90 to 180
	if(steps >= 16 && steps < 18) {
		if(mCar1Angle < 180) mCar1Angle++;
		mCar1PosX += mCar1StepsInc / 2;
		mCar1PosZ -= mCar1StepsInc / 2;
		mCar1Steps += mCar1StepsInc;
	}
	
	// lane 3, step 18 - 20, direction +x
	if(steps >= 18 && steps < 20) {
		if(mCar1Angle < 180) mCar1Angle++;
		mCar1PosX += mCar1StepsInc;
		mCar1Steps += mCar1StepsInc;
	}
	
	// turn 3, step 20 - 24, turn left(-), from 180 to 90
	if(steps >= 20 && steps < 24) {
		if(mCar1Angle > 90) mCar1Angle--;
		if(steps < 21) {
			mCar1PosX += mCar1StepsInc;
		}
		else if(steps >= 21 && steps < 23) {
			mCar1PosX += mCar1StepsInc / 2;
			mCar1PosZ -= mCar1StepsInc / 2;
		}
		else if(steps >= 23) {
			mCar1PosZ -= mCar1StepsInc;
		}
		mCar1Steps += mCar1StepsInc;
	}
	
	// lane 4, step 24 - 26, direction -z
	if(steps >= 24 && steps < 26) {
		if(mCar1Angle > 90) mCar1Angle--;
		mCar1PosZ -= mCar1StepsInc;
		mCar1Steps += mCar1StepsInc;
	}
	
	// turn 4, step 26 - 28, turn right(+), from 90 to 180
	if(steps >= 26 && steps < 28) {
		if(mCar1Angle < 180) mCar1Angle++;
		mCar1PosX += mCar1StepsInc / 2;
		mCar1PosZ -= mCar1StepsInc / 2;
		mCar1Steps += mCar1StepsInc;
	}
	
	// lane 5, step 28 - 40, direction +x
	if(steps >= 28 && steps < 40) {
		if(mCar1Angle < 180) mCar1Angle++;
		mCar1PosX += mCar1StepsInc;
		mCar1Steps += mCar1StepsInc;
	}
	
	// turn 5, step 40 - 42, turn right(+), from 180 to 270
	if(steps >= 40 && steps < 42) {
		if(mCar1Angle < 270) mCar1Angle++;
		mCar1PosX += mCar1StepsInc / 2;
		mCar1PosZ += mCar1StepsInc / 2;
		mCar1Steps += mCar1StepsInc;
	}
	
	// lane 6, step 42 - 44, direction +z
	if(steps >= 42 && steps < 44) {
		if(mCar1Angle < 270) mCar1Angle++;
		mCar1PosZ += mCar1StepsInc;
		mCar1Steps += mCar1StepsInc;
	}
	
	// turn 6, step 44 - 46, turn right(+), 270 to 360/0
	if(steps >= 44 && steps < 46) {
		if(mCar1Angle > 0 && mCar1Angle < 360) mCar1Angle++;
		mCar1PosX -= mCar1StepsInc / 2;
		mCar1PosZ += mCar1StepsInc / 2;
		mCar1Steps += mCar1StepsInc;
	}
	
	// lane 7, step 46 - 48, direction -x
	if(steps >= 46 && steps < 48) {
		if(mCar1Angle > 0 && mCar1Angle < 360) mCar1Angle++;
		mCar1PosX -= mCar1StepsInc;
		mCar1Steps += mCar1StepsInc;
	}
	
	// turn 7, step 48 - 52, turn left(-), from 0 to 270
	if(steps >= 48 && steps < 52) {
		if(mCar1Angle <= 0 || mCar1Angle > 270) mCar1Angle--;
		if(steps < 49) {
			mCar1PosX -= mCar1StepsInc;
		}
		else if(steps >= 49 && steps < 51) {
			mCar1PosX -= mCar1StepsInc / 2;
			mCar1PosZ += mCar1StepsInc / 2;
		}
		else if(steps >= 51) {
			mCar1PosZ += mCar1StepsInc;
		}
		mCar1Steps += mCar1StepsInc;
	}
	
	// lane 8, step 52 - 54, direction +z
	if(steps >= 52 && steps < 54) {
		if(mCar1Angle <= 0 || mCar1Angle > 270) mCar1Angle--;
		mCar1PosZ += mCar1StepsInc;
		mCar1Steps += mCar1StepsInc;
	}
	
	// turn 8, step 54 - 56, turn right(+) from 270 to 360/0
	if(steps >= 54 && steps < 56) {
		if(mCar1Angle > 0 && mCar1Angle < 360) mCar1Angle++;
		mCar1PosX -= mCar1StepsInc / 2;
		mCar1PosZ += mCar1StepsInc / 2;
		mCar1Steps += mCar1StepsInc;
	}
	
	// reset for loop
	if(steps >= 56) {
		mCar1PosX = mTrackStartPosX;
		mCar1PosZ = mTrackStartPosZ;
		mCar1Angle = 0;
		mCar1Steps = 0;
	}
}

void car2Timer(void) {
	
	float steps = mCar2Steps / mTrackBlockSize;
	
	if(mCar2Angle > 359) mCar2Angle = 0;
	else if(mCar2Angle < 0) mCar2Angle = 359;
	
	// lane 1, step 0 - 12, direction -x
	if(steps >= 0 && steps < 12) {
		if(mCar2Angle > 0 && mCar2Angle < 360) mCar2Angle++;
		mCar2PosX -= mCar2StepsInc;
		mCar2Steps += mCar2StepsInc;
	}
	
	// turn 1, step 12 - 16, turn right(+), from 0 to 90
	if(steps >= 12 && steps < 16) {
		if(mCar2Angle < 90) mCar2Angle++;
		if(steps < 13) {
			mCar2PosX -= mCar2StepsInc;
		}
		else if(steps >= 13 && steps < 15) {
			mCar2PosX -= mCar2StepsInc / 2;
			mCar2PosZ -= mCar2StepsInc / 2;
		}
		else if(steps >= 15) {
			mCar2PosZ -= mCar2StepsInc;
		}
		mCar2Steps += mCar2StepsInc;
	}
	
	// lane 2, step 16 - 18, direction -z
	if(steps >= 16 && steps < 18) {
		if(mCar2Angle < 90) mCar2Angle++;
		mCar2PosZ -= mCar2StepsInc;
		mCar2Steps += mCar2StepsInc;
	}
	
	// turn 2, step 18 - 22, turn right(+), from 90 to 180
	if(steps >= 18 && steps < 22) {
		if(mCar2Angle < 180) mCar2Angle++;
		if(steps < 19) {
			mCar2PosZ -= mCar2StepsInc;
		}
		else if(steps >= 19 && steps < 21) {
			mCar2PosX += mCar2StepsInc / 2;
			mCar2PosZ -= mCar2StepsInc / 2;
		}
		else if(steps >= 21) {
			mCar2PosX += mCar2StepsInc;
		}
		
		mCar2Steps += mCar2StepsInc;
	}
	
	// lane 3, step 22 - 24, direction +x
	if(steps >= 22 && steps < 24) {
		if(mCar2Angle < 180) mCar2Angle++;
		mCar2PosX += mCar2StepsInc;
		mCar2Steps += mCar2StepsInc;
	}
	
	// turn 3, step 24 - 26, turn left(-), from 180 to 90
	if(steps >= 24 && steps < 26) {
		if(mCar2Angle > 90) mCar2Angle--;
		mCar2PosX += mCar2StepsInc / 2;
		mCar2PosZ -= mCar2StepsInc / 2;
		mCar2Steps += mCar2StepsInc;
	}
	
	// lane 4, step 26 - 28, direction -z
	if(steps >= 26 && steps < 28) {
		if(mCar2Angle > 90) mCar2Angle--;
		mCar2PosZ -= mCar2StepsInc;
		mCar2Steps += mCar2StepsInc;
	}
	
	// turn 4, step 28 - 32, turn right(+), from 90 to 180
	if(steps >= 28 && steps < 32) {
		if(mCar2Angle < 180) mCar2Angle++;
		if(steps < 29) {
			mCar2PosZ -= mCar2StepsInc;
		}
		else if(steps >= 29 && steps < 31) {
			mCar2PosX += mCar2StepsInc / 2;
			mCar2PosZ -= mCar2StepsInc / 2;
		}
		else if(steps >= 31) {
			mCar2PosX += mCar2StepsInc;
		}
		mCar2Steps += mCar2StepsInc;
	}
	
	// lane 5, step 32 - 44, direction +x
	if(steps >= 32 && steps < 44) {
		if(mCar2Angle < 180) mCar2Angle++;
		mCar2PosX += mCar2StepsInc;
		mCar2Steps += mCar2StepsInc;
	}
	
	// turn 5, step 44 - 48, turn right(+), from 180 to 270
	if(steps >= 44 && steps < 48) {
		if(mCar2Angle < 270) mCar2Angle++;
		if(steps < 45) {
			mCar2PosX += mCar2StepsInc;
		}
		else if(steps >= 45 && steps < 47) {
			mCar2PosX += mCar2StepsInc / 2;
			mCar2PosZ += mCar2StepsInc / 2;
		}
		else if(steps >= 47) {
			mCar2PosZ += mCar2StepsInc;
		}
		mCar2Steps += mCar2StepsInc;
	}
	
	// lane 6, step 48 - 50, direction +z
	if(steps >= 48 && steps < 50) {
		if(mCar2Angle < 270) mCar2Angle++;
		mCar2PosZ += mCar2StepsInc;
		mCar2Steps += mCar2StepsInc;
	}
	
	// turn 6, step 50 - 54, turn right(+), 270 to 360/0
	if(steps >= 50 && steps < 54) {
		if(mCar2Angle > 0 && mCar2Angle < 360) mCar2Angle++;
		if(steps < 51) {
			mCar2PosZ += mCar2StepsInc;
		}
		else if(steps >= 51 && steps < 53) {
			mCar2PosX -= mCar2StepsInc / 2;
			mCar2PosZ += mCar2StepsInc / 2;
		}
		else if(steps >= 53) {
			mCar2PosX -= mCar2StepsInc;
		}
		mCar2Steps += mCar2StepsInc;
	}
	
	// lane 7, step 54 - 56, direction -x
	if(steps >= 54 && steps < 56) {
		if(mCar2Angle > 0 && mCar2Angle < 360) mCar2Angle++;
		mCar2PosX -= mCar2StepsInc;
		mCar2Steps += mCar2StepsInc;
	}
	
	// turn 7, step 56 - 58, turn left(-), from 0 to 270
	if(steps >= 56 && steps < 58) {
		if(mCar2Angle <= 0 || mCar2Angle > 270) mCar2Angle--;
		mCar2PosX -= mCar2StepsInc / 2;
		mCar2PosZ += mCar2StepsInc / 2;
		mCar2Steps += mCar2StepsInc;
	}
	
	// lane 8, step 58 - 60, direction +z
	if(steps >= 58 && steps < 60) {
		if(mCar2Angle <= 0 || mCar2Angle > 270) mCar2Angle--;
		mCar2PosZ += mCar2StepsInc;
		mCar2Steps += mCar2StepsInc;
	}
	
	// turn 8, step 60 - 64, turn right(+) from 270 to 360/0
	if(steps >= 60 && steps < 64) {
		if(mCar2Angle > 0 && mCar2Angle < 360) mCar2Angle++;
		if(steps < 61) {
			mCar2PosZ += mCar2StepsInc;
		}
		else if(steps >= 61 && steps < 63) {
			mCar2PosX -= mCar2StepsInc / 2;
			mCar2PosZ += mCar2StepsInc / 2;
		}
		else if(steps >= 63) {
			mCar2PosX -= mCar2StepsInc;
		}
		mCar2Steps += mCar2StepsInc;
	}
	
	// reset for loop
	if(steps >= 64) {
		mCar2PosX = mTrackStartPosX;
		mCar2PosZ = mTrackStartPosZ + mTrackBlockSize;
		mCar2Angle = 0;
		mCar2Steps = 0;
	}
}

void timer(int miliseconds) {
	
	car1Timer();
	car2Timer();
	
	glutPostRedisplay();
	glutTimerFunc(mCarMoveDelay, timer, 0);
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
   //  Right arrow key - increase angle by 5 degrees
   if (key == GLUT_KEY_RIGHT)
      th += 5;
   //  Left arrow key - decrease angle by 5 degrees
   else if (key == GLUT_KEY_LEFT)
      th -= 5;
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP)
      ph += 5;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
      ph -= 5;
 //  PageUp key - increase dim
   else if (key == GLUT_KEY_PAGE_DOWN)
      dim += 0.1;
   //  PageDown key - decrease dim
   else if (key == GLUT_KEY_PAGE_UP && dim>1)
      dim -= 0.1;
   th %= 360;
   ph %= 360;
   //  Update projection
   Project(fov,asp,dim);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when a key is pressed
 */void key(unsigned char ch,int x,int y)
{
   //  Exit on ESC
   if (ch == 27)
      exit(0);
   //  Reset view angle
   else if (ch == '0')
      th = ph = 0;
   //  Toggle axes
   else if (ch == 'x' || ch == 'X')
      axes = 1-axes;
   //  Switch projection mode
   else if (ch == 'p' || ch == 'P')
      mode = 1-mode;
   //  Toggle light movement
   else if (ch == 'm' || ch == 'M')
      move = 1-move;
   //  Change field of view angle
   else if (ch == '-' && ch>1)
      fov--;
   else if (ch == '+' && ch<179)
      fov++;
   //  Move
   else if (ch == 'w' || ch == 'W')
      fpX++;
    //  Move
   else if (ch == 's' || ch == 'S')
      fpX--;
   //  Move
   else if (ch == 'd' || ch == 'D')
      fpY++;
    //  Move
   else if (ch == 'a' || ch == 'A')
      fpY--;
    //  Move
   else if (ch == 'q' || ch == 'Q')
      fpZ++;
    //  Move
   else if (ch == 'e' || ch == 'E')
      fpZ--;
   //  Reproject
   Project(fov,asp,dim);
   //  Animate if requested
   glutIdleFunc(move?idle:NULL);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Set projection
   Project(fov,asp,dim);
}


/*
 * Init Texture BMP
 */
void init(){
	_textureSkyboxBack = LoadTexBMP("skybox-back.bmp");
	_textureSkyboxFront = LoadTexBMP("skybox-front.bmp");
	_textureSkyboxLeft = LoadTexBMP("skybox-left.bmp");
	_textureSkyboxRight = LoadTexBMP("skybox-right.bmp");
	_textureSkyboxTop = LoadTexBMP("skybox-top.bmp");
	_textureGlass = LoadTexBMP("glass.bmp");
	_textureWoodFence = LoadTexBMP("wood-fence.bmp");
	_textureAsphalt = LoadTexBMP("asphalt.bmp");
	_textureBasicMetal = LoadTexBMP("basic-metal.bmp"); 
	_textureBrownBrick = LoadTexBMP("brown-brick.bmp");
	_textureCarWheel = LoadTexBMP("car-wheel.bmp");
	_textureCinderBlock = LoadTexBMP("cinder-block.bmp");
	_textureGarageDoor = LoadTexBMP("garage-door.bmp");
	_textureGrass = LoadTexBMP("grass.bmp");
	_textureGreyBrick = LoadTexBMP("grey-brick.bmp");
	_textureHedge = LoadTexBMP("hedge.bmp");
	_textureSidewalk = LoadTexBMP("sidewalk.bmp");
	_textureWalkway = LoadTexBMP("walkway.bmp");
	_textureWhiteBrick = LoadTexBMP("white-brick.bmp");
	_textureCarGrill = LoadTexBMP("car-grill.bmp");
	_textureFrontDoor2 = LoadTexBMP("front-door-2.bmp");
	_textureWindow1 = LoadTexBMP("window-1.bmp");
	_textureMetalRoof = LoadTexBMP("metal-roof.bmp");
	_textureTireTread = LoadTexBMP("tire-tread.bmp");
	_textureWarehouseWindow = LoadTexBMP("warehouse-window.bmp");
	_textureHeadLamp = LoadTexBMP("headlamp.bmp");
	_textureCarbonFiber = LoadTexBMP("carbon-fiber.bmp");
	_textureWoodBeam = LoadTexBMP("wood-beam.bmp");
}
/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
   //  Initialize GLUT
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   glutInitWindowSize(800,600);
   glutCreateWindow("Final Project");
   //  Set callbacks
   init();
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutSpecialFunc(special);
   glutKeyboardFunc(key);
   glutIdleFunc(idle);
   glutTimerFunc(mCarMoveDelay, timer, 0);
   //  Pass control to GLUT so it can interact with the user
   ErrCheck("init");
   glutMainLoop();
   return 0;
}
