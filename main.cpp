#include <math.h>
#include <time.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include "titanic.h"

#define CAMERA_BACK_titanic 0
#define CAMERA_LATERAL_titanic 1
#define CAMERA_TOP_titanic 2
#define CAMERA_MOUSE 3
#define CAMERA_TYPE_MAX 4

float viewAlpha   = 20;
float viewBeta    = 40; // angoli che definiscono la vista
float eyeDist     = 5.0; // distanza dell'occhio dall'origine
bool useWireframe = false;
bool useEnvmap    = true;
bool useShadow    = true;
bool disegna      = false;
int cameraType    = 0;
float mozzo       = 0; // var che permette alle eliche di girare
float matrix[8][4]; //matrice contenente le posizioni x,y,z dei cerchi 
int contatore;
bool up = true;
double start; // variabili per il calcolo del tempo
double start1;
double end;
double end1;
double tempo_attuale;
char ris[3]; //array contenente i millisecondi passati
Titanic titanic; // la barca
int nstep = 0; // numero di passi di FISICA fatti fin'ora
const int PHYS_SAMPLING_STEP = 10; // numero di millisec che un passo di fisica simula

// Frames Per Seconds
const int fpsSampling = 3000; // lunghezza intervallo di calcolo fps
float fps = 0; // valore di fps dell'intervallo precedente
int fpsNow = 0; // quanti fotogrammi ho disegnato fin'ora nell'intervallo attuale
Uint32 timeLastInterval = 0; // quando e' cominciato l'ultimo intervallo
bool onde = false;
bool metallic=false;

/* functions definite altrove */
extern void drawBonus(float mozzo, Titanic& titanic, bool& up);
extern void drawFloor();
extern void drawElicottero(float mozzo);
extern void drawIce();
extern void drawIce1();
extern void drawCircle(int contatore);
//extern void drawDrago();

/* posizioni del cubo da ricalcolare per disegno su minimappa */
extern int pos_x;
extern int pos_z;

/* il terreno è colorato o ha una texture applicata */
extern bool colori;

/* qualita del testo scritto e id della realtiva texture */
enum textquality {solid, shaded, blended};
uint font_id = -1;

void initMatrix(){
	contatore=0;
	matrix[0][0]=-40.5; matrix[0][1]= -24.66; matrix[0][2]=-26.7 ; matrix[0][3]=-24.4;
  matrix[1][0]=0.14; matrix[1][1]=0.46 ;matrix[1][2]= -60.92; matrix[1][3]=-43.81;
  matrix[2][0]=44.51; matrix[2][1]=69.41;matrix[2][2]= -40.04; matrix[2][3]=-34.45;
  matrix[3][0]=52.36; matrix[3][1]=64.68;matrix[3][2]= 3.53; matrix[3][3]=12.53;
  matrix[4][0]=37.07; matrix[4][1]=42.29; matrix[4][2]=43.58 ; matrix[4][3]=59.46;
  matrix[5][0]=-16; matrix[5][1]=-12.38;matrix[5][2]=43.59; matrix[5][3]= 60.50;
  matrix[6][0]=-60.85; matrix[6][1]=-44;matrix[6][2]= 54.58; matrix[6][3]=54.90;
  matrix[7][0]=-38.24; matrix[7][1]=-21.75;matrix[7][2]= 16.83 ; matrix[7][3]= 17.20;
}
/**************************************
 * Function per la scrittura di testo *
 **************************************/
void SDL_GL_DrawText(TTF_Font *font, // font
                    char fgR, char fgG, char fgB, char fgA, // colore testo
                    char bgR, char bgG, char bgB, char bgA, // colore background
                    char *text, int x, int y, // testo e posizione
                    enum textquality quality) { // qualità del testo
  SDL_Color tmpfontcolor = {fgR,fgG,fgB,fgA};
  SDL_Color tmpfontbgcolor = {bgR, bgG, bgB, bgA};
  SDL_Surface *initial;
  SDL_Surface *intermediary;
  SDL_Rect location;
  int w,h;

  /* Use SDL_TTF to render our text */
  initial=NULL;
  if (quality == solid)
    initial = TTF_RenderText_Solid(font, text, tmpfontcolor);
  else if (quality == shaded)
    initial = TTF_RenderText_Shaded(font, text, tmpfontcolor, tmpfontbgcolor);
  else if (quality == blended)
    initial = TTF_RenderText_Blended(font, text, tmpfontcolor);

  /* Convert the rendered text to a known format */
  w = initial->w;
  h = initial->h;

  /* Allochiamo una nuova surface RGB */
  intermediary = SDL_CreateRGBSurface(0, w, h, 32,
    0x000000ff,0x0000ff00, 0x00ff0000,0xff000000);

  /* copiamo il contenuto dalla prima alla seconda surface */
  SDL_BlitSurface(initial, 0, intermediary, 0);

  /* Tell GL about our new texture */
  glBindTexture(GL_TEXTURE_2D, font_id);
  glTexImage2D(GL_TEXTURE_2D, 0, 4, w, h, 0, GL_RGBA,
      GL_UNSIGNED_BYTE, intermediary->pixels );


  /* GL_NEAREST looks horrible, if scaled... */
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  if ( initial != NULL ){
    location.x = x;
    location.y = y;
  }

  glLineWidth(2);
  glColor3f(0,0,0);
  glBegin(GL_LINE_LOOP);
    glVertex2f(location.x-2    , location.y-2);
    glVertex2f(location.x + w+2, location.y-2);
    glVertex2f(location.x + w+2, location.y + h+2);
    glVertex2f(location.x   -2 , location.y + h+2);
  glEnd();

  /* prepare to render our texture */
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, font_id);
  glColor3f(1.0f, 1.0f, 1.0f);

/* Draw a quad at location */
  glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f);
      glVertex2f(location.x    , location.y);
    glTexCoord2f(1.0f, 1.0f);
      glVertex2f(location.x + w, location.y);
    glTexCoord2f(1.0f, 0.0f);
      glVertex2f(location.x + w, location.y + h);
    glTexCoord2f(0.0f, 0.0f);
      glVertex2f(location.x    , location.y + h);
  glEnd();

/*Draw a boundary button */
/*  glColor3f(0.0f, 0.0f, 0.0f);
  glBegin(GL_LINE_STRIP);
  glVertex2f((GLfloat)location.x-1, (GLfloat)location.y-1);
  glVertex2f((GLfloat)location.x + w +1, (GLfloat)location.y-1);
  glVertex2f((GLfloat)location.x + w +1, (GLfloat)location.y + h +1);
  glVertex2f((GLfloat)location.x-1    , (GLfloat)location.y + h +1);
  glVertex2f((GLfloat)location.x-1, (GLfloat)location.y-1);
  glEnd();
*/
/* Bad things happen if we delete the texture before it finishes */
  glFinish();

/* return the deltas in the unused w,h part of the rect */
  location.w = initial->w;
  location.h = initial->h;

/* Clean up */
  glDisable(GL_TEXTURE_2D);
  SDL_FreeSurface(initial);
  SDL_FreeSurface(intermediary);
//  glColor3f(0.0f, 0.0f, 0.0f);

}



/********************************************************
 * setta le matrici di trasformazione in modo
 * che le coordinate in spazio oggetto siano le coord 
 * del pixel sullo schemo (per disegno in 2D sopra la scena)
 ********************************************************/
void  SetCoordToPixel(int scrH, int scrW){
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(-1,-1,0);
  glScalef(2.0/scrW, 2.0/scrH, 1);
}
/********************************************************
 * function che permette il caricamento di una texture
 * da essere applicata ad un oggetto
 ********************************************************/
bool LoadTexture(int textbind,char *filename) {
  SDL_Surface *s = IMG_Load(filename);
  if (!s) return false;
  
  glBindTexture(GL_TEXTURE_2D, textbind);
  gluBuild2DMipmaps(
    GL_TEXTURE_2D, 
    GL_RGB,
    s->w, s->h, 
    GL_RGB,
    GL_UNSIGNED_BYTE,
    s->pixels
  );
  glTexParameteri(
    GL_TEXTURE_2D, 
    GL_TEXTURE_MAG_FILTER,
    GL_LINEAR ); 
  glTexParameteri(
    GL_TEXTURE_2D, 
    GL_TEXTURE_MIN_FILTER,
    GL_LINEAR_MIPMAP_LINEAR ); 
  return true;
}
/********************************************************
 * function per il disegno di una sfera che conterrà
 * la nostra scena e il nostro mondo
 ********************************************************/
void drawSphere(double r, int lats, int longs) {
  int i, j;
  for(i = 0; i <= lats; i++) {
     double lat0 = M_PI * (-0.5 + (double) (i - 1) / lats);
     double z0  = sin(lat0);
     double zr0 =  cos(lat0);
   
     double lat1 = M_PI * (-0.5 + (double) i / lats);
     double z1 = sin(lat1);
     double zr1 = cos(lat1);
    
     glBegin(GL_QUAD_STRIP);
     for(j = 0; j <= longs; j++) {
        double lng = 2 * M_PI * (double) (j - 1) / longs; // circonferenza
        double x = cos(lng);
        double y = sin(lng);
    
        // le normali servono per l'EnvMap
        glNormal3f(x * zr0, y * zr0, z0);
        glVertex3f(r * x * zr0, r * y * zr0, r * z0);
        glNormal3f(x * zr1, y * zr1, z1);
        glVertex3f(r * x * zr1, r * y * zr1, r * z1);
     }
     glEnd();
  }
}
/**************************************
 * setta la posizione della telecamera
 **************************************/
void setCamera(){

  double px = titanic.px;
  double py = titanic.py;
  double pz = titanic.pz;
  double angle = titanic.facing;
  double cosf = cos(angle*M_PI/180.0);
  double sinf = sin(angle*M_PI/180.0);
  double camd, camh, ex, ey, ez, cx, cy, cz;
  double cosff, sinff;

  // controllo la posizione della camera a seconda dell'opzione selezionata
  switch (cameraType) {
    case CAMERA_BACK_titanic:
      camd = 15.5;
      camh = 7.0;
      ex = px + camd*sinf;
      ey = py + camh;
      ez = pz + camd*cosf;
      cx = px - camd*sinf;
      cy = py + camh;
      cz = pz - camd*cosf;
      gluLookAt(ex,ey,ez,cx,cy,cz,0.0,1.0,0.0);
    break;
    case CAMERA_LATERAL_titanic:
      camd = 5.0;
      camh = 2.15;
      angle = titanic.facing + 40.0;
      cosff = cos(angle*M_PI/180.0);
      sinff = sin(angle*M_PI/180.0);
      ex = px + camd*sinff;
      ey = py + camh;
      ez = pz + camd*cosff;
      cx = px - camd*sinf;
      cy = py + camh;
      cz = pz - camd*cosf;
      gluLookAt(ex,ey,ez,cx,cy,cz,0.0,1.0,0.0);
      break;
    case CAMERA_TOP_titanic:
      camd = 5.5;
      camh = 1.0;
      ex = px + camd*sinf;
      ey = py + camh;
      ez = pz + camd*cosf;
      cx = px - camd*sinf;
      cy = py + camh;
      cz = pz - camd*cosf;
      gluLookAt(ex,ey+5,ez,cx,cy,cz,0.0,1.0,0.0);
      break;
   case CAMERA_MOUSE:
      glTranslatef(0,0,-eyeDist);
      glRotatef(viewBeta,  1,0,0);
      glRotatef(viewAlpha, 0,1,0);
      break;
    }
}


/*******************************************
 * function per il disegno dello sfondo
 *******************************************/
void drawSky() {
int H = 100;

  if (useWireframe) {
    glDisable(GL_TEXTURE_2D);
    glColor3f(0,0,0);
    glDisable(GL_LIGHTING);
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    drawSphere(100.0, 20, 20);
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glColor3f(1,1,1);
    glEnable(GL_LIGHTING);
  }
  else {
    glBindTexture(GL_TEXTURE_2D,2);
    
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE , GL_SPHERE_MAP); // Env map
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE , GL_SPHERE_MAP);
    glColor3f(1,1,1);
    glDisable(GL_LIGHTING);

    drawSphere(100.0, 20, 20);

    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
  }

}

/****************************************/
/* function per disegnare una minimappa */
/****************************************/
void drawMinimap(int scrH, int scrW) {
  /* calcolo delle coordinate reali dell'oggetto su minimappa */
  float minimap_posx;
  float minimap_posz;
  minimap_posx = ((50*titanic.px)/97) + 50 + 20;
  minimap_posz = ((50*titanic.pz)/97) + 50 + scrH-20-100;

  float minimap_cerchiox;
  float minimap_cerchioz;
  minimap_cerchiox = ((50*matrix[contatore][1])/97) + 50 + 20;
  minimap_cerchioz = ((50*matrix[contatore][3])/97) + 50 + scrH-20-100;

  float minimap_cubex;
  float minimap_cubez;
  minimap_cubex = ((50*pos_x)/97) + 50 + 20;
  minimap_cubez = ((50*pos_z)/97) + 50 + scrH-20-100;
  
  /* disegno del cursore */
  glColor3f(1 ,1 ,1);
  glBegin(GL_QUADS);
    glVertex2d(minimap_posx, minimap_posz + 3);
    glVertex2d(minimap_posx + 3, minimap_posz);
    glVertex2d(minimap_posx, minimap_posz - 3);
    glVertex2d(minimap_posx - 3, minimap_posz);
  glEnd();

  /* disegno del cerchio */
  glColor3f(1,1,0);
  glBegin(GL_QUADS);
    glVertex2d(minimap_cerchiox, minimap_cerchioz + 3);
    glVertex2d(minimap_cerchiox + 3, minimap_cerchioz);
    glVertex2d(minimap_cerchiox, minimap_cerchioz - 3);
    glVertex2d(minimap_cerchiox - 3, minimap_cerchioz);
  glEnd();

  if(!disegna){
    /* disegno del target */
    glColor3ub(255,0,0);
    glBegin(GL_QUADS);
      glVertex2d(minimap_cubex, minimap_cubez + 3);
      glVertex2d(minimap_cubex + 3, minimap_cubez);
      glVertex2d(minimap_cubex, minimap_cubez - 3);
      glVertex2d(minimap_cubex - 3, minimap_cubez);
    glEnd();
  }

  /* disegno minimappa */
  glColor3f(1,0,0);
  glBegin(GL_LINE_LOOP);
    glVertex2d(20,scrH -20 -100);
    glVertex2d(20,scrH -20);
    glVertex2d(120,scrH-20);
    glVertex2d(120,scrH-20-100);
  glEnd();

  glColor3ub(0,0,0);
  glBegin(GL_POLYGON);
    glVertex2d(20,scrH -20 -100);
    glVertex2d(20,scrH -20);
    glVertex2d(120,scrH -20);
    glVertex2d(120,scrH-20-100);
   glEnd();

}

/*********************************************
 * esegue il rendering dell'intera scena
 *********************************************/
void rendering(SDL_Window *win, double tempo_attuale, TTF_Font *font, int scrH, int scrW){
  
  printf("x: %f y: %f z: %f \n",titanic.px,titanic.py,titanic.pz);

  // un frame in piu'!!!
  fpsNow+=40;
  
  // impostiamo spessore linee
  glLineWidth(3);
     
  // settiamo il viewport
  glViewport(0,0, scrW, scrH);
  
  // colore di sfondo (fuori dal mondo)
  glClearColor(1,1,1,1);

  // settiamo la matrice di proiezione (imposta la piramide di vista)
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  gluPerspective( 70, //  angolo di apertura della piramide in gradi,
		((float)scrW) / scrH, // aspect Y/X,
		0.2, // distanza del NEAR CLIPPING PLANE in coordinate vista
		1000 // distanza del FAR CLIPPING PLANE in coordinate vista
  );

  /* passiamo a lavorare sui modelli */
  glMatrixMode( GL_MODELVIEW ); 
  glLoadIdentity();
  
  // riempe tutto lo screen buffer di pixel color sfondo
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  
  /* imposto diversi parametri di illuminazione per una luce */
  float tmpv[4] = {3,8,5,  1}; // ultima comp=0 => luce direzionale
                               // ultima comp=1 => luce puntiforme: i raggi escono in tutte le direzioni
  glLightfv(GL_LIGHT0, GL_POSITION, tmpv );
  
  // the ambient RGBA intensity of the light
  float tmpp[4] = {0.3,0.3,0.3,  1}; 
  glLightfv(GL_LIGHT0, GL_AMBIENT, tmpp);
  //glShadeModel(GL_SMOOTH);
  // GL_FLAT è il default: ogni poligono ha un colore
  // GL_SMOOTH: ogni punto della superficie del poligono ha un'ombreggiatura derivata
  //            dall'interpolazione delle normali ai vertici

  // settiamo la telecamera  
  setCamera();

  static float tmpcol[4] = {1,1,1,  1};
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, tmpcol);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 127);
  
  glEnable(GL_LIGHTING);
  
  // disegna il cielo come sfondo
  drawSky();
  
  // disegna il suolo
  drawFloor();

  // disegna gli icebergs
  drawIce();
  drawIce1();
 
//controllo se la barca ha passato uno dei cerchi
  if(titanic.px > matrix[contatore][0] && titanic.px < matrix[contatore][1] &&
   titanic.pz > matrix[contatore][2] && titanic.pz < matrix[contatore][3])
       contatore++;
   
	drawCircle(contatore);
  
  drawElicottero(mozzo);

  // disegna la barca
  titanic.Render(mozzo);

  //Gestione bonus velocità aggiuntiva
  if(titanic.accMax < 0.0030){
    
    // Se non sono già all'interno di un bonus disegno il cubo
    
    drawBonus(mozzo, titanic, up);
    start1=clock();
  }
  else{
     
     //cubo preso
     end1=clock();
     disegna=true;
     glDisable(GL_TEXTURE_2D);
     if((end1-start1)/CLOCKS_PER_SEC >= 10){
        disegna=false;
        titanic.setAccMax();
      }
  }
  glDisable(GL_LIGHTING);
	
  // attendiamo la fine della rasterizzazione di 
  // tutte le primitive mandate 
  
  SetCoordToPixel(scrH, scrW);

  if(disegna){
    
    glBegin(GL_QUADS);
    float y = 10*70 - 70*((end1 - start1)/CLOCKS_PER_SEC);
    //float ramp = fps/100;
    glColor3f(1,0,0);
    glVertex2d(5,0);
    glVertex2d(5,y);
    glVertex2d(0,y);
    glVertex2d(0,0);
    glEnd();
  
  }
  
  glLineWidth(2);

  // disegna la minimappa in alto a sinistra
  drawMinimap(scrH, scrW);
  // disegna un helper di fianco alla minimappa
  //drawHelper(tempo_attuale, scrH, scrW);
  glDisable(GL_TEXTURE_GEN_S);
  glDisable(GL_TEXTURE_GEN_T);
  char myword[] = "Tempo: ";
  end = clock();
  tempo_attuale = end - start;
  char str1[100];
  sprintf(str1, "%d", (int)(tempo_attuale/1000));
  int dim=strlen(str1);
  if(dim < 100){
    if(dim >= 3){
      ris[0]= str1[dim-3];
      ris[1]= str1[dim-2];
      ris[2]= str1[dim-1];
    }
    else if(dim == 2){
      ris[0]=str1[dim-2];
      ris[1]=str1[dim-1];
    }
    else if(dim == 1){
      ris[0]=str1[dim-1];
    }
  }
  char output[50];
  snprintf(output, 50, "%d:%d:%s", (int) (tempo_attuale/CLOCKS_PER_SEC / 60), (int) (tempo_attuale/CLOCKS_PER_SEC) 
    - ((int)(tempo_attuale/CLOCKS_PER_SEC / 60) * 60),ris  
  );
  SDL_GL_DrawText(font, 0, 0, 0, 0, 255, 255, 255, 255, strcat(myword, output), 25, scrH-180, shaded);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  
  glFinish();
  
  // ho finito: buffer di lavoro diventa visibile
  SDL_GL_SwapWindow(win);
  
}

/***************************
 * function di redraw
 ***************************/
void redraw(){
  // ci automandiamo un messaggio che (s.o. permettendo)
  // ci fara' ridisegnare la finestra
  SDL_Event e;
  e.type = SDL_WINDOWEVENT;
  e.window.event = SDL_WINDOWEVENT_EXPOSED;
  SDL_PushEvent(&e);
}


/*************************************
 * disegna la schermata di game over 
 *************************************/
void gameOver(SDL_Window *win, TTF_Font *font1,TTF_Font *font, int scrH, int scrW) {
  // settiamo il viewport
  glViewport(0,0, scrW, scrH);
  
  // colore di sfondo
  glClearColor(0,0,0,0);
  
  // riempe tutto lo screen buffer di pixel color sfondo
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
 
  SetCoordToPixel(scrH, scrW);

  glLineWidth(2);
  
  char tempo[6];
   snprintf(tempo, 50, "%d:%d:%s", (int) (tempo_attuale/CLOCKS_PER_SEC / 60), (int) (tempo_attuale/CLOCKS_PER_SEC) 
    - ((int)(tempo_attuale/CLOCKS_PER_SEC / 60) * 60),ris
  );
  char stringa_tempo[] = "Tempo impiegato: ";
  char g_over[] = "GAME OVER";
  char continuare[] = "Premi invio per continuare";
  int distanzaMargine;
  distanzaMargine = 138;
  SDL_GL_DrawText(font, 0, 0, 0, 0, 210, 210, 210, 255, strcat(stringa_tempo, tempo), scrW/2-distanzaMargine, scrH/2+100, shaded);
  SDL_GL_DrawText(font1, 0, 34, 234, 234, 210, 210, 210, 255, g_over, scrW/2-100, scrH/3+20, shaded);
  SDL_GL_DrawText(font1, 0, 0, 0, 0, 210, 210, 210, 255, continuare, scrW/2-240, scrH/4+20, shaded);
  glFinish();
  
  // ho finito: buffer di lavoro diventa visibile
  SDL_GL_SwapWindow(win);
}


/*************************************/
/******* PROGRAMMA PRINCIPALE ********/
/*************************************/
int main(int argc, char* argv[]) {
  SDL_Window *win;
  SDL_GLContext mainContext;
  Uint32 windowID;
  SDL_Joystick *joystick;
  static int keymap[Controller::NKEYS] = {SDLK_a, SDLK_d, SDLK_w, SDLK_s};

  int scrH          = 750;
  int scrW          = 750; // altezza e larghezza viewport (in pixels)

  // inizializzazione di SDL
  SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);

  if(TTF_Init() < 0) { 
    fprintf(stderr, "Impossibile inizializzare TTF: %s\n",SDL_GetError());
    SDL_Quit();
    return(2);
  }

  TTF_Font *font;
  font = TTF_OpenFont ("LatoRegular.ttf", 22);
  if (font == NULL) {
    fprintf (stderr, "Impossibile caricare il font.\n");
  }
  TTF_Font *font1;
  font1 = TTF_OpenFont ("PacFont.ttf", 22);
  if (font1 == NULL) {
    fprintf (stderr, "Impossibile caricare il font.\n");
  }

  // abilitazione joystick
  SDL_JoystickEventState(SDL_ENABLE);
  joystick = SDL_JoystickOpen(0);

  // settaggio dei buffer
  SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 ); 
  SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

  // creazione di una finestra di scrW x scrH pixels
  win = SDL_CreateWindow("Titanic", 0, 0, scrW, scrH, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
  
  SDL_Surface *image = IMG_Load ( "nave.png" );
  SDL_SetWindowIcon(win, image);

  // creiamo il nostro contesto OpenGL e lo colleghiamo alla nostra window
  mainContext = SDL_GL_CreateContext(win);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_NORMALIZE); // opengl, per favore, rinormalizza le normali prima di usarle
  //glEnable(GL_CULL_FACE);
  glFrontFace(GL_CW); // consideriamo Front Facing le facce ClockWise
  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_POLYGON_OFFSET_FILL); // sposta i frammenti generati dalla
  glPolygonOffset(1,1);             // rasterizzazione poligoni
                                    // indietro di 1
  
  /* caricamento di tutte le textures */
  //if (!LoadTexture(0,(char *)"logo.jpg")) return -1;
 
  if (!LoadTexture(1,(char *)"Metallic.jpg")) return -1;
  if (!LoadTexture(2,(char *)"sky_ok.jpg")) return -1;
  if (!LoadTexture(3,(char *)"simone.jpg")) return -1;
  if (!LoadTexture(4,(char *)"ice.jpg")) return -1;
  if (!LoadTexture(5,(char *)"sea.jpg")) return -1;
  if (!LoadTexture(6,(char *)"wood.jpg")) return -1;
  if (!LoadTexture(7,(char *)"nero.jpg")) return -1;
  if (!LoadTexture(8,(char *)"vetro.jpg")) return -1;
  
  start = clock();
  initMatrix();
  /* CICLO DEGLI EVENTI */
  bool done = 0;
  while (!done) {
    
    SDL_Event e;
    
    // guardo se c'e' un evento:
    if (SDL_PollEvent(&e)) {
    // se si: processa evento
     switch (e.type) {
      case SDL_KEYDOWN: // tasto abbassato
        titanic.controller.EatKey(e.key.keysym.sym, keymap , true);
        if (e.key.keysym.sym==SDLK_F1) cameraType=(cameraType+1)%CAMERA_TYPE_MAX;
        if (e.key.keysym.sym==SDLK_F2) useWireframe=!useWireframe;
        if (e.key.keysym.sym==SDLK_F3) onde=!onde;
        if (e.key.keysym.sym==SDLK_F4) colori = !colori;
        if (e.key.keysym.sym==SDLK_F5) metallic = !metallic;
        if (e.key.keysym.sym==SDLK_F6) useShadow = !useShadow;
      break;
      case SDL_KEYUP: // tasto sollevato
        titanic.controller.EatKey(e.key.keysym.sym, keymap , false);
        break;
      case SDL_QUIT: // quit
          done=1; 
          break;
      case SDL_WINDOWEVENT: // dobbiamo ridisegnare la finestra
        if (e.window.event == SDL_WINDOWEVENT_EXPOSED)
          rendering(win, tempo_attuale, font, scrH, scrW);
        else {
          windowID = SDL_GetWindowID(win);
          if (e.window.windowID == windowID) {
            switch (e.window.event)  {
              case SDL_WINDOWEVENT_SIZE_CHANGED: {
                scrW = e.window.data1;
                scrH = e.window.data2;
                glViewport(0,0,scrW,scrH);
                rendering(win, tempo_attuale, font, scrH, scrW);
                break;
              }
            }
          }
        }
        break;
        
      case SDL_MOUSEMOTION: /* movimento del mouse */
        if (e.motion.state & SDL_BUTTON(1) & cameraType == CAMERA_MOUSE) {
          viewAlpha+=e.motion.xrel;
          viewBeta +=e.motion.yrel;
          if (viewBeta<+5) viewBeta =+ 5;
          if (viewBeta>+90) viewBeta =+ 90;
        }
        break; 
       
      case SDL_MOUSEWHEEL: /* movimento della rotellina del mouse */
        if (e.wheel.y < 0 ) {
        // avvicino il punto di vista (zoom in)
          eyeDist = eyeDist*0.9;
          if (eyeDist<1) 
            eyeDist = 1;
        }
        if (e.wheel.y > 0 ) {
         // allontano il punto di vista (zoom out)
         eyeDist = eyeDist/0.9;
        }
        if(eyeDist > 96) // impedisco all'utente di uscire con la visuale dal mondo per vedere
                         // cosa c'è al di fuori (lo sfondo bianco) 
          eyeDist = 96;
        break;

      case SDL_JOYAXISMOTION: /* gestione analogico sinistro joystick */
        
        if( e.jaxis.axis == 0) {
          if ( e.jaxis.value < -3200 ) {
            titanic.controller.Joy(0 , true);
            titanic.controller.Joy(1 , false);                 
          }
          if ( e.jaxis.value > 3200 ) {
            titanic.controller.Joy(0 , false); 
            titanic.controller.Joy(1 , true);
          }
          if ( e.jaxis.value >= -3200 && e.jaxis.value <= 3200 ) {
            titanic.controller.Joy(0 , false);
            titanic.controller.Joy(1 , false);                 
          }
        }
        else if( e.jaxis.axis == 1) {
          if ( e.jaxis.value < -3200 ) {
            titanic.controller.Joy(2 , true);
            titanic.controller.Joy(3 , false);                 
          }
          if ( e.jaxis.value > 3200 ) {
            titanic.controller.Joy(2 , false); 
            titanic.controller.Joy(3 , true);
          }
          if ( e.jaxis.value >= -3200 && e.jaxis.value <= 3200 ) {
            titanic.controller.Joy(2 , false);
            titanic.controller.Joy(3 , false);                 
          }
        }
          
        rendering(win, tempo_attuale, font, scrH, scrW);
        break;
      
      case SDL_JOYBUTTONDOWN: /* gestione pressione di un tasto del joystick */
        
        printf("%d\n", e.jbutton.button);
        
        if ( e.jbutton.button == 0 ) // Tasto 9: cambiamento telecamera
          cameraType=(cameraType+1)%CAMERA_TYPE_MAX;
          if(cameraType == CAMERA_MOUSE)
            cameraType=(cameraType+1)%CAMERA_TYPE_MAX;

        if ( e.jbutton.button == 1 ) // TASTO 1: onde
          onde=!onde;

        if ( e.jbutton.button == 2 ) // TASTO 2: accelerazione
          titanic.controller.Joy(2 , true);

        if ( e.jbutton.button == 3 ) // TASTO 3: decelerazione
          titanic.controller.Joy(3 , true);

        if ( e.jbutton.button == 4 ) // TASTO 4: texture del mondo
          colori=!colori;

        if ( e.jbutton.button == 5 ) // TASTO 5: texture barca
          metallic=!metallic;

        if ( e.jbutton.button == 6 ) // TASTO 6: useWireframe
          useWireframe=!useWireframe;

        if ( e.jbutton.button == 7 ) // Tasto 7: ombra
          useShadow=!useShadow;

        break;
      
      case SDL_JOYBUTTONUP: /* Gestione rilascio di un tasto del joystick */

        titanic.controller.Joy(2 , false);
        titanic.controller.Joy(3 , false);
        
        break; 
      }
    } else {
        mozzo--;
        // nessun evento: siamo IDLE
      
        Uint32 timeNow = SDL_GetTicks(); // che ore sono?
        // ritorniamo il nuemro di millisecondi da quando la libreria SDL è stata inizializzata
      
        if (timeLastInterval + fpsSampling < timeNow) {
          fps = 1000.0*((float)fpsNow) / (timeNow-timeLastInterval);
          fpsNow = 0;
          timeLastInterval = timeNow;
        }
      
        bool doneSomething = false;
        int guardia = 0; // sicurezza da loop infinito
      
        // finche' il tempo simulato e' rimasto indietro rispetto
        // al tempo reale facciamo i passi di fisica che ci mancano
        // e poi ridisegniamo la scena
        while (nstep*PHYS_SAMPLING_STEP < timeNow ) {
          titanic.DoStep();
          nstep++;
          doneSomething = true;
          timeNow = SDL_GetTicks();
          guardia++;
          if (guardia > 1000) {
            done = true; 
            break;
          } // siamo troppo lenti!
        }
      
        if (doneSomething) 
          rendering(win, tempo_attuale, font, scrH, scrW);

        // dopo due minuti il gioco termina 
        end = clock();
        tempo_attuale = end - start;

        if(/*(tempo_attuale/CLOCKS_PER_SEC >= 180) ||*/ (contatore == 8)){
          int i = 0;
         
          done = 1;
          int done1 = 0;

          // schermata di game over
          while(!done1) { 
            if (SDL_PollEvent(&e)) {
              switch (e.type) {
                case SDL_KEYDOWN: 
                  if (e.key.keysym.sym==SDLK_RETURN)
                    done1 = 1; 
                  break;
                case SDL_WINDOWEVENT:
                  windowID = SDL_GetWindowID(win);
                  if (e.window.windowID == windowID) {
                    switch (e.window.event)  {
                      case SDL_WINDOWEVENT_SIZE_CHANGED: {
                        scrW = e.window.data1;
                        scrH = e.window.data2;
                        glViewport(0,0,scrW,scrH);
                        break;
                      }
                    }
                  }
              }
            } else { // disegno la schermata di game over
                gameOver(win, font1, font,scrH, scrW);
            } // fine else
          } // fine while
        } // fine if del tempo attuale

      else {
        // tempo libero!!!
      }
    }
  }

  SDL_GL_DeleteContext(mainContext);
  SDL_DestroyWindow(win);
  SDL_Quit ();
  return (0);
}

