/*
 * Jake White - Final Project - Shooter Tutor 
 *  An interactive program which recreates a puck being aimed and 
 *       shot by a stick controlled by the user
 * - June 30, 2015
 * 
 * 
 *  Shadow Map with Shaders by Willem A. (Vlakkies) Schreuder
 *    with Frame Buffer Depth Texture by Jay Kominek
 *
 *  
 *



Key bindings
  1/2        aim left/right
  3/4        aim up/dowm
  s/S        shoot the puck
  +/-        Change light elevation
  a          Toggle axes
  arrows     Change view angle
  PgDn/PgUp  Zoom in and out
  0          Reset view angle
  ESC        Exit




 */
#include "CSCIx229.h"
typedef struct {float x,y,z;} Point;
//  Global variables


int shoot = 0;


int hit1 = 1;
int hit2 = 1;
int hit3 = 1;
int hit4 = 1;
int hit5 = 1;

// variables for deciphering where the shot went
int top_right = 0;
int top_left = 0;
int bottom_left = 0;
int bottom_right = 0;
int five_hole = 0;
int wide_right = 0;
int wide_left = 0;
int missed_middle_top = 0;
int missed_middle_bottom = 0;
int missed_left_bottom = 0;
int missed_left_top = 0;
int missed_right_top = 0;
int missed_right_bottom = 0;

//variables for keeping track of past results
int missed = 0;
int goals = 0;
int total = 0;
int first_time = 0;

//variables used for starting and implementing a shot with the stick, puck, and targets
int start = 0;
int swing = 0;

//varibles used for creating the stick
double fz = 1.0;
double fybottom = -2.0;
double fytop = -1.6;
double bz = 0.8;

//variables for moving and placing the puck
double puck_z = 1.1;
double puck_x = -0.6;
double puck_y = -1.9;
double puck_ph = 90;
double puck_th = 0;

int          mode=0;    // Display mode
int          obj=15;    // Display objects (bitmap)
int          move=1;    // Light movement
int          axes=0;    // Display axes
int          box=0;     // Display enclosing box
int          n=8;       // Number of slices
int          th=-30;    // Azimuth of view angle
int          ph=+30;    // Elevation of view angle
int          tex2d[3];  // Textures (names)
int          dt=50;     // Timer period (ms)
double       asp=1;     // Aspect ratio
double       dim=3;     // Size of world
int          zh=0;      // Light azimuth
int          sendToStick=0;      // stick azimuth

//variables for rotating the hockey stick and deciphering where the puck is shot
double stick_th_horizontal  = 0;
double stick_ph_verticle    = 0;


float        Ylight=2;  // Elevation of light
float        Lpos[4];   // Light position
unsigned int framebuf=0;// Frame buffer id
double       Svec[4];   // Texture planes S
double       Tvec[4];   // Texture planes T
double       Rvec[4];   // Texture planes R
double       Qvec[4];   // Texture planes Q
int          Width;     // Window width
int          Height;    // Window height
int          shadowdim; // Size of shadow map textures
int          shader;    // Shader
char* text[]={"Shadows","Shadow Map"};

#define MAXN 64    // Maximum number of slices (n) and points in a polygon


//create hockey net
static void Net(float x,float y,float z , float th,float ph , float D)
{



   glPushMatrix();
   glTranslated(x,y,z);

   glRotated(ph,1,0,0);  
   glRotated(th,0,1,0);

   glScaled(D,D,D);

  //  Transform
   glPushMatrix();
   glTranslated(x,y,z);
   glRotated(ph,1,0,0);
   glRotated(th,0,1,0);
   glScaled(D,D,D);


   glColor3f(1,0,0);
   glNormal3f( 0, 0,-1);
   glBegin(GL_QUADS);
   glTexCoord2f(0,0); glVertex3f(+2,-1,-1);
   glTexCoord2f(1,0); glVertex3f(-2,-1,-1);
   glTexCoord2f(1,1); glVertex3f(-2,+1,-1);
   glTexCoord2f(0,1); glVertex3f(+2,+1,-1);
   glEnd();
   //  Right
   glColor3f(1,0,0);
   glNormal3f(+2, 0, 0);
   glBegin(GL_QUADS);
   glTexCoord2f(0,0); glVertex3f(+2,-1,+1);
   glTexCoord2f(1,0); glVertex3f(+2,-1,-1);
   glTexCoord2f(1,1); glVertex3f(+2,+1,-1);
   glTexCoord2f(0,1); glVertex3f(+2,+1,+1);
   glEnd();
   //  Left
   glColor3f(1,0,0);
   glNormal3f(-2, 0, 0);
   glBegin(GL_QUADS);
   glTexCoord2f(0,0); glVertex3f(-2,-1,-1);
   glTexCoord2f(1,0); glVertex3f(-2,-1,+1);
   glTexCoord2f(1,1); glVertex3f(-2,+1,+1);
   glTexCoord2f(0,1); glVertex3f(-2,+1,-1);
   glEnd();
   //  Top
   glColor3f(1,0,0);
   glNormal3f( 0,+1, 0);
   glBegin(GL_QUADS);
   glTexCoord2f(0,0); glVertex3f(-2,+1,+1);
   glTexCoord2f(1,0); glVertex3f(+2,+1,+1);
   glTexCoord2f(1,1); glVertex3f(+2,+1,-1);
   glTexCoord2f(0,1); glVertex3f(-2,+1,-1);
   glEnd();

   // Restore
   glPopMatrix(); 

   glPopMatrix();
   
}

static void Stick(float x,float y,float z , float th,float ph , float D)
{
   
   /************ front ********************/

      glPushMatrix();
      glTranslated(x,y,z);

      if (start)
      {
         glRotated(ph,0,0,0);
         glRotated(th,0,0,0);
      }
      else
      {
         glRotated(ph,1,0,0);  
         glRotated(th,0,1,0);
      }
      glScaled(D,D,D);

/************ front ********************/
      //shaft 
      glNormal3f( 0, 0, 1);
      glBegin(GL_QUADS);       //   x     y     z 
      glTexCoord2f(0,0); glVertex3f(0,    fybottom,   fz);// front left corner 2 
      glTexCoord2f(1,1); glVertex3f(0,    fytop, fz)  ; //  5
      glTexCoord2f(1,1); glVertex3f(-1.7, 2,    fz); //  6
      glTexCoord2f(0,1); glVertex3f(-2,   2,       fz); // top left 1 
      glEnd();

      // blade 
      
      glNormal3f( 0, 0, 1);
      glBegin(GL_QUADS);
      glTexCoord2f(0,0); glVertex3f(0,    fybottom,   fz);// front -- bottom corner 2 
      glTexCoord2f(1,1); glVertex3f(0,    fytop, fz); //  5 -- front -- top left blade
      glTexCoord2f(1,1); glVertex3f(1,    fytop, fz); //  6 -- front -- top right blade 
      glTexCoord2f(0,1); glVertex3f(1,    fybottom,   fz); // front -- bottom right 1 
      glEnd();
   
   /************ back ********************/


      // shaft 

      glNormal3f( 0, 0, 1);
      glBegin(GL_QUADS);
      glTexCoord2f(0,0); glVertex3f(0,fybottom, bz);// front left corner 2 
      glTexCoord2f(1,1); glVertex3f(0,-1.6, bz); //  5
      glTexCoord2f(1,1); glVertex3f(-1.7,2, bz); //  6
      glTexCoord2f(0,1); glVertex3f(-2,2, bz); // top left 1 
      glEnd();

      // blade 
      
      glNormal3f( 0, 0, 1);
      glBegin(GL_QUADS);
      glTexCoord2f(0,0); glVertex3f(0,fybottom, bz);// front left corner 2 
      glTexCoord2f(1,1); glVertex3f(0,fytop, bz); //  5 -- front -- top left blade
      glTexCoord2f(1,1); glVertex3f(1,fytop, bz); //  6 -- front -- top right blade 
      glTexCoord2f(0,1); glVertex3f(1,fybottom, bz); // front -- bottom right 1 
      glEnd();
   


   /************ blade end ********************/



      //  blade end
   
      glNormal3f(+1, 0, 0);
      glBegin(GL_QUADS);
      glTexCoord2f(0,0); glVertex3f(+1,fybottom,fz); // 1 bottom left 
      glTexCoord2f(1,0); glVertex3f(+1,fybottom,bz); // 2 bottom right
      glTexCoord2f(1,1); glVertex3f(+1,fytop,bz); // 3 top right 
      glTexCoord2f(0,1); glVertex3f(+1,fytop,fz); // 4 top left
      glEnd();
   

   /************ top shaft stick ********************/
   
      glNormal3f(1, 1, 0);
      glBegin(GL_QUADS);
      glTexCoord2f(1,1); glVertex3f(-1.7, 2,    fz); //  top left
      glTexCoord2f(1,1); glVertex3f(-1.7, 2,    bz); //  top right
      glTexCoord2f(1,1); glVertex3f(0,    fytop, bz); //  bottom right
      glTexCoord2f(1,1); glVertex3f(0,    fytop, fz); //  bottom left
      glEnd();
   

   /************ bottom shaft stick ********************/
   
      glNormal3f(-1, -1, 0);
      glBegin(GL_QUADS);
      
      glTexCoord2f(0,1); glVertex3f(-2,   2,       fz); // top right
      glTexCoord2f(0,1); glVertex3f(-2,   2,       bz); // top left
      glTexCoord2f(0,0); glVertex3f(0,    fybottom,   bz);// bottom left corner  
      glTexCoord2f(0,0); glVertex3f(0,    fybottom,   fz);// bottom right corner 

      glEnd();   
   

   /************ top blade ********************/
      //  Top

      glNormal3f( 0,+1, 0);
      glBegin(GL_QUADS);
      glTexCoord2f(0,0); glVertex3f(0,fytop,fz);
      glTexCoord2f(1,0); glVertex3f(+1,fytop,fz);
      glTexCoord2f(1,1); glVertex3f(+1,fytop,bz);
      glTexCoord2f(0,1); glVertex3f(0,fytop,bz);
      glEnd();


   /************ bottom blade ********************/
      //  Bottom
      
      glNormal3f( 0,-1, 0);
      glBegin(GL_QUADS);
      glTexCoord2f(0,0); glVertex3f(0,    fybottom,   fz);// top right corner 2 
      glTexCoord2f(0,0); glVertex3f(0,    fybottom,   bz);// top left corner 2 
      glTexCoord2f(0,0); glVertex3f(1,    fybottom,   bz);// bottom left corner 2 
      glTexCoord2f(0,0); glVertex3f(1,    fybottom,   fz);// bottom right corner 2 

      glEnd();


      glPopMatrix();
}





//function that takes in the aim of the stick 
// and deciphers what is going to happen with the puck

static void shot(double x, double y)
{
   if (x == 15.0)
   {
      wide_left = 1;
   }
   else if (x == 10.0 )
   {
      if(y == 0)
      {
         bottom_left = 1;
      }
      else if (y>=10)
      {
         missed_left_top = 1;
      }
      else
      {
         missed_left_bottom = 1;
      }
   }
   else if( x == 5.0)
   {
      if(y == 15.0)
      {
         top_left = 1;
      }
      else if (y == 10.0)
      {
         missed_left_top = 1;
      }
      else
      {
         missed_left_bottom = 1;
      }
   }
   else if ( x == 0.0)
   {
      if(y == 5)
      {
         five_hole = 1;
      }
      else if (y>5)
      {
         missed_middle_top = 1;
      }      
      else
      {
         missed_middle_bottom = 1;
      }
   }
   else if (x == -5.0)
   {
      if(y == 15.0)
      {
         top_right = 1;
      }
      else if (y >= 5.0 && y <=10 )
      {
         missed_right_top = 1;
      }
      else
      {
         missed_right_bottom = 1;
      }

   }
   else if (x == -10.0)
   {
      if (y == 0.0)
      {
         bottom_right = 1;
      }
      else 
      {
         missed_right_top = 1;
      }
   }
   else if (x == -15.0)
   {
      wide_right = 1;
   }
}


/*
 *  Draw a targets
 */
static void Target(float x,float y,float z , float th,float ph , float R,float H)
{
   int i,j;   // Counters
   int N=4*n; // Number of slices

   //  Transform
   glPushMatrix();
   glTranslated(x,y,z);
   glRotated(ph,1,0,0);
   glRotated(th,0,1,0);
   glScaled(R,R,H);
   glColor3f(1,1,0);

   //  Two end caps (fan of triangles)
   for (j=-1;j<=1;j+=2)
   {
      glNormal3d(0,0,j); 
      glBegin(GL_TRIANGLE_FAN);
      glTexCoord2d(0,0); glVertex3d(0,0,j);
      for (i=0;i<=N;i++)
      {
         float th = j*i*360.0/N;
         glTexCoord2d(Cos(th),Sin(th)); glVertex3d(Cos(th),Sin(th),j);
      }
      glEnd();
   }

   //  Cylinder Body (strip of quads)
   glBegin(GL_QUADS);
   for (i=0;i<N;i++)
   {
      float th0 =  i   *360.0/N;
      float th1 = (i+1)*360.0/N;
      glNormal3d(Cos(th0),Sin(th0),0); glTexCoord2d(0,th0/90.0); glVertex3d(Cos(th0),Sin(th0),+1);
      glNormal3d(Cos(th0),Sin(th0),0); glTexCoord2d(2,th0/90.0); glVertex3d(Cos(th0),Sin(th0),-1);
      glNormal3d(Cos(th1),Sin(th1),0); glTexCoord2d(2,th1/90.0); glVertex3d(Cos(th1),Sin(th1),-1);
      glNormal3d(Cos(th1),Sin(th1),0); glTexCoord2d(0,th1/90.0); glVertex3d(Cos(th1),Sin(th1),+1);
   }
   glEnd();

   //  Restore
   glPopMatrix();
}


/*
 *  Draw a puck
 */
static void Puck(float x,float y,float z , float th,float ph , float R,float H)
{
   int i,j;   // Counters
   int N=4*n; // Number of slices

   //  Transform
   glPushMatrix();
   glTranslated(x,y,z);
   glRotated(ph,1,0,0);
   glRotated(th,0,1,0);
   glScaled(R,R,H);
   glColor3f(0,0,0);

   //  Two end caps (fan of triangles)
   for (j=-1;j<=1;j+=2)
   {
      glNormal3d(0,0,j); 
      glBegin(GL_TRIANGLE_FAN);
      glTexCoord2d(0,0); glVertex3d(0,0,j);
      for (i=0;i<=N;i++)
      {
         float th = j*i*360.0/N;
         glTexCoord2d(Cos(th),Sin(th)); glVertex3d(Cos(th),Sin(th),j);
      }
      glEnd();
   }

   //  Cylinder Body (strip of quads)
   glBegin(GL_QUADS);
   for (i=0;i<N;i++)
   {
      float th0 =  i   *360.0/N;
      float th1 = (i+1)*360.0/N;
      glNormal3d(Cos(th0),Sin(th0),0); glTexCoord2d(0,th0/90.0); glVertex3d(Cos(th0),Sin(th0),+1);
      glNormal3d(Cos(th0),Sin(th0),0); glTexCoord2d(2,th0/90.0); glVertex3d(Cos(th0),Sin(th0),-1);
      glNormal3d(Cos(th1),Sin(th1),0); glTexCoord2d(2,th1/90.0); glVertex3d(Cos(th1),Sin(th1),-1);
      glNormal3d(Cos(th1),Sin(th1),0); glTexCoord2d(0,th1/90.0); glVertex3d(Cos(th1),Sin(th1),+1);
   }
   glEnd();

   //  Restore
   glPopMatrix();
}

/*
 *  Draw a wall
 */
static void Wall(float x,float y,float z, float th,float ph , float Sx,float Sy,float Sz , float St)
{
   int   i,j;
   float s=1.0/n;
   float t=0.5*St/n;

   //  Transform
   glPushMatrix();
   glTranslated(x,y,z);
   glRotated(ph,1,0,0);
   glRotated(th,0,1,0);
   glScaled(Sx,Sy,Sz);

   //  Draw walls
   glNormal3f(0,0,1);
   for (j=-n;j<n;j++)
   {
      glBegin(GL_QUAD_STRIP);
      for (i=-n;i<=n;i++)
      {
         glTexCoord2f((i+n)*t,(j  +n)*t); glVertex3f(i*s,    j*s,-1);
         glTexCoord2f((i+n)*t,(j+1+n)*t); glVertex3f(i*s,(j+1)*s,-1);
      }
      glEnd();
   }

   //  Restore
   glPopMatrix();
}

/*
 *  Set light
 *    light>0 bright
 *    light<0 dim
 *    light=0 off
 */
static void Light(int light)
{
   //  Set light position
   Lpos[0] = 2*Cos(zh);
   Lpos[1] = Ylight;
   Lpos[2] = 2*Sin(zh);
   Lpos[3] = 1;

   //  Enable lighting
   if (light)
   {
      float Med[]  = {0.3,0.3,0.3,1.0};
      float High[] = {1.0,1.0,1.0,1.0};
      //  Enable lighting with normalization
      glEnable(GL_LIGHTING);
      glEnable(GL_NORMALIZE);
      //  glColor sets ambient and diffuse color materials
      glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
      glEnable(GL_COLOR_MATERIAL);
      //  Enable light 0
      glEnable(GL_LIGHT0);
      glLightfv(GL_LIGHT0,GL_POSITION,Lpos);
      glLightfv(GL_LIGHT0,GL_AMBIENT,Med);
      glLightfv(GL_LIGHT0,GL_DIFFUSE,High);
   }
   else
   {
      glDisable(GL_LIGHTING);
      glDisable(GL_COLOR_MATERIAL);
      glDisable(GL_NORMALIZE);
   }
}

/*
 *  Draw scene
 *    light (true enables lighting)
 */
void Scene(int light)
{
   int k;  // Counters used to draw floor

   //  Set light position and properties
   Light(light);

   //  Draw objects         x       y  z      th,        ph    dims   
   if (obj&0x01)     Stick(-0.8, -1.3, 1,    stick_th_horizontal,  stick_ph_verticle, 0.3);     //stick
   if (obj&0x02)     Puck(puck_x,  puck_y, puck_z,   puck_th,    puck_ph  , 0.1,0.02);          //puck
   if (obj&0x03)     Net(-0.3, -0.75, -2.0,    0,  0, 0.75    );                                //net
   if (obj&0x04 && hit1)     Target(0.4, -0.85, -3.0,  0,    0  , 0.1,0.1); //top right target
   if (obj&0x04 && hit2)     Target(-1.4, -0.85, -3.0,  0,    0  , 0.1,0.1);
   if (obj&0x04 && hit3)     Target(-1.4, -1.75, -3.0,  0,    0  , 0.1,0.1);
   if (obj&0x04 && hit4)     Target(0.45, -1.75, -3.0,  0,    0  , 0.1,0.1); //bottom right target
   if (obj&0x04 && hit5)     Target(-0.47, -1.4, -3.0,  0,    0  , 0.1,0.1); //five hole target


   //  Enable textures for floor, ceiling and walls
   glEnable(GL_TEXTURE_2D);

   //  Water texture for floor and ceiling
   // bottom surface
   glBindTexture(GL_TEXTURE_2D,tex2d[0]);
   glColor3f(1.0,1.0,1.0);
   for (k=-1;k<=box;k+=2)
      Wall(0,0,0, 0,90*k , 8,8,2 , 4);
   //  Crate texture for walls
   glBindTexture(GL_TEXTURE_2D,tex2d[1]);
   for (k=0;k<4*box;k++)
      Wall(0,0,0, 90*k,0 , 8,2,8 , 1);

   //  Disable textures
   glDisable(GL_TEXTURE_2D);
}

/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
   int id;
   const double len=2.0;
   //  Eye position
   float Ex = -2*dim*Sin(th)*Cos(ph);
   float Ey = +2*dim        *Sin(ph);
   float Ez = +2*dim*Cos(th)*Cos(ph);

   //  Erase the window and the depth buffers
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Disable lighting
   glDisable(GL_LIGHTING);

   //
   //  Draw the scene with shadows
   //
   //  Set perspective view
   if (mode)
   {
      //  Half width for shadow map display
      Project(60,asp/2,dim);
      glViewport(0,0,Width/2,Height);
   }
   else
   {
      //  Full width
      Project(60,asp,dim);
      glViewport(0,0,Width,Height);
   }
   gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);

   //  Draw light position as sphere (still no lighting here)
   glColor3f(1,1,1);
   glPushMatrix();
   glTranslated(Lpos[0],Lpos[1],Lpos[2]);
   glutSolidSphere(0.03,10,10);
   glPopMatrix();

   //  Enable shader program
   glUseProgram(shader);
   id = glGetUniformLocation(shader,"tex");
   if (id>=0) glUniform1i(id,0);
   id = glGetUniformLocation(shader,"depth");
   if (id>=0) glUniform1i(id,1);

   // Set up the eye plane for projecting the shadow map on the scene
   glActiveTexture(GL_TEXTURE1);
   glTexGendv(GL_S,GL_EYE_PLANE,Svec);
   glTexGendv(GL_T,GL_EYE_PLANE,Tvec);
   glTexGendv(GL_R,GL_EYE_PLANE,Rvec);
   glTexGendv(GL_Q,GL_EYE_PLANE,Qvec);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_COMPARE_MODE,GL_COMPARE_R_TO_TEXTURE);
   glActiveTexture(GL_TEXTURE0);

   // Draw objects in the scene (including walls)

   Scene(1);

   //  Disable shader program
   glUseProgram(0);

   //  Draw axes (white)
   glColor3f(1,1,1);
   if (axes)
   {
      glBegin(GL_LINES);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(len,0.0,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,len,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,0.0,len);
      glEnd();
      //  Label axes
      glRasterPos3d(len,0.0,0.0);
      Print("X");
      glRasterPos3d(0.0,len,0.0);
      Print("Y");
      glRasterPos3d(0.0,0.0,len);
      Print("Z");
   }

   //  Display parameters
   glColor3f(0,0,0);
   glWindowPos2i(5,5);
   // Fatal("variable1 value: %d\n",variable1);

   Print("Hit Targets: %d, Missed Targets: %d, Total Shots: %d",
      goals, missed, total);
   //  Render the scene and make it visible
   ErrCheck("display");
   glFlush();
   glutSwapBuffers();
}

/*
 *  Build Shadow Map
 */

void ShadowMap(void)
{
   double Lmodel[16];  //  Light modelview matrix
   double Lproj[16];   //  Light projection matrix
   double Tproj[16];   //  Texture projection matrix
   double Dim=2.0;     //  Bounding radius of scene
   double Ldist;       //  Distance from light to scene center

   //  Save transforms and modes
   glPushMatrix();
   glPushAttrib(GL_TRANSFORM_BIT|GL_ENABLE_BIT);
   //  No write to color buffer and no smoothing
   glShadeModel(GL_FLAT);
   glColorMask(0,0,0,0);
   // Overcome imprecision
   glEnable(GL_POLYGON_OFFSET_FILL);

   //  Turn off lighting and set light position
   Light(0);

   //  Light distance
   Ldist = sqrt(Lpos[0]*Lpos[0] + Lpos[1]*Lpos[1] + Lpos[2]*Lpos[2]);
   if (Ldist<1.1*Dim) Ldist = 1.1*Dim;

   //  Set perspective view from light position
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(114.6*atan(Dim/Ldist),1,Ldist-Dim,Ldist+Dim);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   gluLookAt(Lpos[0],Lpos[1],Lpos[2] , 0,0,0 , 0,1,0);
   //  Size viewport to desired dimensions
   glViewport(0,0,shadowdim,shadowdim);

   // Redirect traffic to the frame buffer
   glBindFramebuffer(GL_FRAMEBUFFER,framebuf);

   // Clear the depth buffer
   glClear(GL_DEPTH_BUFFER_BIT);
   // Draw all objects that can cast a shadow

   Scene(0);


   //  Retrieve light projection and modelview matrices
   glGetDoublev(GL_PROJECTION_MATRIX,Lproj);
   glGetDoublev(GL_MODELVIEW_MATRIX,Lmodel);

   // Set up texture matrix for shadow map projection,
   // which will be rolled into the eye linear
   // texture coordinate generation plane equations
   glLoadIdentity();
   glTranslated(0.5,0.5,0.5);
   glScaled(0.5,0.5,0.5);
   glMultMatrixd(Lproj);
   glMultMatrixd(Lmodel);

   // Retrieve result and transpose to get the s, t, r, and q rows for plane equations
   glGetDoublev(GL_MODELVIEW_MATRIX,Tproj);
   Svec[0] = Tproj[0];    Tvec[0] = Tproj[1];    Rvec[0] = Tproj[2];    Qvec[0] = Tproj[3];
   Svec[1] = Tproj[4];    Tvec[1] = Tproj[5];    Rvec[1] = Tproj[6];    Qvec[1] = Tproj[7];
   Svec[2] = Tproj[8];    Tvec[2] = Tproj[9];    Rvec[2] = Tproj[10];   Qvec[2] = Tproj[11];
   Svec[3] = Tproj[12];   Tvec[3] = Tproj[13];   Rvec[3] = Tproj[14];   Qvec[3] = Tproj[15];

   // Restore normal drawing state
   glShadeModel(GL_SMOOTH);
   glColorMask(1,1,1,1);
   glDisable(GL_POLYGON_OFFSET_FILL);
   glPopAttrib();
   glPopMatrix();
   glBindFramebuffer(GL_FRAMEBUFFER,0);

   //  Check if something went wrong
   ErrCheck("ShadowMap");
}

/*
 *
 */
void InitMap()
{
   unsigned int shadowtex; //  Shadow buffer texture id
   int n;

   //  Make sure multi-textures are supported
   glGetIntegerv(GL_MAX_TEXTURE_UNITS,&n);
   if (n<2) Fatal("Multiple textures not supported\n");

   //  Get maximum texture buffer size
   glGetIntegerv(GL_MAX_TEXTURE_SIZE,&shadowdim);
   //  Limit texture size to maximum buffer size
   glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE,&n);
   if (shadowdim>n) shadowdim = n;
   //  Limit texture size to 2048 for performance
   if (shadowdim>2048) shadowdim = 2048;
   if (shadowdim<512) Fatal("Shadow map dimensions too small %d\n",shadowdim);

   //  Do Shadow textures in MultiTexture 1
   glActiveTexture(GL_TEXTURE1);

   //  Allocate and bind shadow texture
   glGenTextures(1,&shadowtex);
   glBindTexture(GL_TEXTURE_2D,shadowtex);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, shadowdim, shadowdim, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

   //  Map single depth value to RGBA (this is called intensity)
   glTexParameteri(GL_TEXTURE_2D,GL_DEPTH_TEXTURE_MODE,GL_INTENSITY);

   //  Set texture mapping to clamp and linear interpolation
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

   //  Set automatic texture generation mode to Eye Linear
   glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);
   glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);
   glTexGeni(GL_R,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);
   glTexGeni(GL_Q,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);

   // Switch back to default textures
   glActiveTexture(GL_TEXTURE0);

   // Attach shadow texture to frame buffer
   glGenFramebuffers(1,&framebuf);
   glBindFramebuffer(GL_FRAMEBUFFER,framebuf);
   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowtex, 0);
   //  Don't write or read to visible color buffer
   glDrawBuffer(GL_NONE);
   glReadBuffer(GL_NONE);
   //  Make sure this all worked
   if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) Fatal("Error setting up frame buffer\n");
   glBindFramebuffer(GL_FRAMEBUFFER,0);

   //  Check if something went wrong
   ErrCheck("InitMap");

   //  Create shadow map
   ShadowMap();
}

/*
 *  GLUT calls this routine when nothing else is going on
 */


void idle(int k)
{
   //  Elapsed time in seconds
   double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   zh = fmod(90*t,1440.0);

   //if the shot process has begun
   if (shoot) 
   {
      start = 1; //initalize the shot process
      shoot = 0;

      shot(stick_th_horizontal, stick_ph_verticle);

   };

   //begin the shot process 
   if(start) 
   {

      if ( swing <= 100) //back swing
      {
         stick_ph_verticle = -1*swing;
         swing = swing + 10;

         start++;
      }
      else if (swing > 100 && swing <= 200) //swing changes directions
      {
         stick_ph_verticle = -1*(100-(swing-100));
         swing = swing + 10 ;
         
      }
      else if (swing > 200 && swing <= 300) // make contact with puck and follow through with stick
      {
         stick_ph_verticle = swing-200;
         
         puck_ph = 20*zh;
         puck_th = 20*zh;
         
         puck_z = puck_z - 0.3; //puck location on z -- should remain consistent throughout

         if(top_right) //these variables are set within the shot() function 
         {
            puck_y += 0.04;
            puck_x += 0.05;
         }
         else if(top_left)
         {
            puck_y += 0.05;
            puck_x -= 0.05;

         }
         else if(bottom_left)
         {
            puck_x -= 0.06;  
         }
         else if(bottom_right)
         {
            puck_x += 0.06;  
         }
         else if(five_hole)
         {
            puck_y += 0.02;
         }
         else if(wide_right)
         {
            puck_x += 0.1;    
         }
         else if(wide_left)
         {
            puck_x -= 0.1;      
         }
         else if (missed_middle_bottom)
         {
            
         }
         else if (missed_middle_top) 
         {
            puck_y += 0.045;
         }      
         else if (missed_left_top)
         {
            puck_x -= 0.05;
            puck_y += 0.03;
         }
         else if (missed_left_bottom)
         {
            puck_x -= 0.05;
            // puck_y += 0.0;
         }
         else if (missed_right_top)
         {
            puck_x += 0.05;
            puck_y += 0.02;
         }
         else if (missed_right_bottom)
         {
            puck_x += 0.05;
         }

         swing = swing + 5;

      }
      else if (swing > 300 && swing <= 400) // bringing stick back down //puck lands on ice
      {
         puck_z = -3.7;
         stick_ph_verticle = 300-(swing-100);
         swing = swing + 10;
         puck_y = -1.9;

         first_time++;

         if(first_time == 1)
         {
            if(top_right || top_left || bottom_right || bottom_left || five_hole)
            {
               goals++;
               total++;
            }
            else
            {
               missed++;
               total++;

            };
         }

         if(top_right)
         {
            puck_x -= 0.06;
            hit1 = 0;
         }
         else if(top_left)
         {
            puck_x += 0.06;
            hit2 = 0;  
         }
         else if(bottom_left)
         {
            puck_x += 0.06;
            hit3 = 0;
         }
         else if(bottom_right)
         {
            puck_x -= 0.06;
            hit4 = 0;  
         }
         else if(five_hole)
         {
            hit5 = 0;
         }
      }; 

   };

   //  Update shadow map
   ShadowMap();
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
   //  Schedule update
   if (move) glutTimerFunc(dt,idle,0);
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
   //  Right arrow key - increase angle by 5 degrees
   if (key == GLUT_KEY_RIGHT)
      th += 1;
   //  Left arrow key - decrease angle by 5 degrees
   else if (key == GLUT_KEY_LEFT)
      th -= 1;
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP)
      ph += 1;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
      ph -= 1;
   //  PageUp key - increase dim
   else if (key == GLUT_KEY_PAGE_DOWN)
      dim += 0.1;
   //  PageDown key - decrease dim
   else if (key == GLUT_KEY_PAGE_UP && dim>1)
      dim -= 0.1;
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
   //  Exit on ESC
   if (ch == 27)
      exit(0);
   //  Reset view angle
   else if (ch == '0')
      th = ph = 0;
   //  Toggle axes
   else if (ch == 'a' || ch == 'A')
      axes = 1-axes;
   //  Toggle light movement
   else if (ch == 's' || ch == 'S')
      shoot = 1;
   //  Light elevation
   else if (ch=='-')
      Ylight -= 0.1;
   else if (ch=='+')
      Ylight += 0.1;

   else if(ch =='1' && stick_th_horizontal <= 10)
   {
      stick_th_horizontal += 5;
      puck_x -= 0.01;
      puck_z -= 0.01;
      // puck_z += 0.2;
   }
   else if(ch =='2' && stick_th_horizontal >= -10)
   {
      stick_th_horizontal -= 5;
      puck_x += 0.01;
      puck_z += 0.01;
   }
   else if(ch =='3' && stick_ph_verticle <= 10) //check2
   {
      stick_ph_verticle += 5;
      puck_z -= 0.02;
   }
   else if(ch =='4' && stick_ph_verticle >= 5)
   {
      stick_ph_verticle -= 5;
      puck_z += 0.02;
   }
   else if(ch =='r') //reset variables
   {

      top_right  = 0;
      top_left = 0;
      bottom_left = 0;
      bottom_right  = 0;
      five_hole = 0;
      first_time = 0;
      wide_left = 0;
      wide_right = 0;
      missed_middle_top = 0;
      missed_middle_bottom = 0;
      missed_right_top = 0;
      missed_right_bottom = 0;
      missed_left_top = 0;
      missed_left_bottom = 0;

      hit1 = 1;
      hit2 = 1;
      hit3 = 1;
      hit4 = 1;
      hit5 = 1;

      stick_ph_verticle = 0;
      puck_ph = 90;
      puck_th = 0;
      puck_z = 1.1;
      start = 0;
      swing = 0;
      puck_x = -0.6;

      stick_th_horizontal = 0;
      stick_ph_verticle = 0;


   }

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
   //  Store window dimensions
   Width  = width;
   Height = height;
}

//
//  Read text file
//
static char* ReadText(const char *file)
{
   int   n;
   char* buffer;
   //  Open file
   FILE* f = fopen(file,"rt");
   if (!f) Fatal("Cannot open text file %s\n",file);
   //  Seek to end to determine size, then rewind
   fseek(f,0,SEEK_END);
   n = ftell(f);
   rewind(f);
   //  Allocate memory for the whole file
   buffer = (char*)malloc(n+1);
   if (!buffer) Fatal("Cannot allocate %d bytes for text file %s\n",n+1,file);
   //  Snarf the file
   if (fread(buffer,n,1,f)!=1) Fatal("Cannot read %d bytes for text file %s\n",n,file);
   buffer[n] = 0;
   //  Close and return
   fclose(f);
   return buffer;
}

//
//  Print Shader Log
//
static void PrintShaderLog(int obj,const char* file)
{
   int len=0;
   glGetShaderiv(obj,GL_INFO_LOG_LENGTH,&len);
   if (len>1)
   {
      int n=0;
      char* buffer = (char *)malloc(len);
      if (!buffer) Fatal("Cannot allocate %d bytes of text for shader log\n",len);
      glGetShaderInfoLog(obj,len,&n,buffer);
      fprintf(stderr,"%s:\n%s\n",file,buffer);
      free(buffer);
   }
   glGetShaderiv(obj,GL_COMPILE_STATUS,&len);
   if (!len) Fatal("Error compiling %s\n",file);
}

//
//  Print Program Log
//
void PrintProgramLog(int obj)
{
   int len=0;
   glGetProgramiv(obj,GL_INFO_LOG_LENGTH,&len);
   if (len>1)
   {
      int n=0;
      char* buffer = (char *)malloc(len);
      if (!buffer) Fatal("Cannot allocate %d bytes of text for program log\n",len);
      glGetProgramInfoLog(obj,len,&n,buffer);
      fprintf(stderr,"%s\n",buffer);
   }
   glGetProgramiv(obj,GL_LINK_STATUS,&len);
   if (!len) Fatal("Error linking program\n");
}

//
//  Create Shader
//
void CreateShader(int prog,const GLenum type,const char* file)
{
   //  Create the shader
   int shader = glCreateShader(type);
   //  Load source code from file
   char* source = ReadText(file);
   glShaderSource(shader,1,(const char**)&source,NULL);
   free(source);
   //  Compile the shader
   glCompileShader(shader);
   //  Check for errors
   PrintShaderLog(shader,file);
   //  Attach to shader program
   glAttachShader(prog,shader);
}


//
//  Create Shader Program
//
int CreateShaderProg(const char* VertFile,const char* FragFile)
{
   //  Create program
   int prog = glCreateProgram();
   //  Create and compile vertex shader
   if (VertFile) CreateShader(prog,GL_VERTEX_SHADER,VertFile);
   //  Create and compile fragment shader
   if (FragFile) CreateShader(prog,GL_FRAGMENT_SHADER,FragFile);
   //  Link program
   glLinkProgram(prog);
   //  Check for errors
   PrintProgramLog(prog);
   //  Return name
   return prog;
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
   glutInitWindowSize(600,600);
   glutCreateWindow("Jake White - Final Project - Shooter Tutor");
#ifdef USEGLEW
   //  Initialize GLEW
   if (glewInit()!=GLEW_OK) Fatal("Error initializing GLEW\n");
   if (!GLEW_VERSION_2_0) Fatal("OpenGL 2.0 not supported\n");
#endif
   //  Set callbacks
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutSpecialFunc(special);
   glutKeyboardFunc(key);
   glutTimerFunc(dt,idle,0);
   //  Load textures
   // check2 -- textures for surface and walls
   tex2d[0] = LoadTexBMP("ice.bmp");
   tex2d[1] = LoadTexBMP("crate.bmp");
   tex2d[2] = LoadTexBMP("crate.bmp");
   // Enable Z-buffer
   glEnable(GL_DEPTH_TEST);
   glDepthFunc(GL_LEQUAL);
   glPolygonOffset(4,0);
   //  Initialize texture map
   shader = CreateShaderProg("shadow.vert","shadow.frag");
   //  Initialize texture map
   InitMap();

   //  Pass control to GLUT so it can interact with the user
   ErrCheck("init");
   glutMainLoop();
   return 0;
}
