// Titanic.cpp
// implementazione dei metodi definiti in Titanic.h

#include <stdio.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <vector> // la classe vector di STL 
#include <stdlib.h>
#include <time.h>
#include "titanic.h"
#include "point3.h"
#include "mesh.h"
#include "objects.h"

// variabili globali di tipo mesh
Mesh barca((char *)"boat.obj");
Mesh esterno_barca((char *)"EsternoBarca.obj");
Mesh restante_barca((char *)"RestanteBarca.obj");
Mesh timone((char *)"timone.obj");

struct list{
  int val;
  list*next;
};

extern bool useShadow; // var globale esterna: per generare l'ombra
extern bool onde;
extern bool metallic;

/*****************************************************************************/
/* da invocare quando e' stato premuto/rilasciato il tasto numero "keycode"  */
/*****************************************************************************/
void Controller::EatKey(int keycode, int* keymap, bool pressed_or_released) {
  for (int i=0; i<NKEYS; i++) {
    if (keycode == keymap[i]) 
      key[i] = pressed_or_released;
  }
}

/*************************************************************/
/* da invocare quando e' stato premuto/rilasciato un jbutton */
/*************************************************************/
void Controller::Joy(int keymap, bool pressed_or_released) {
    key[keymap] = pressed_or_released;
}

/***************************************************/
/* funzione che prepara tutto per usare un env map */
/***************************************************/
void SetupTexture() {
  // facciamo binding con la texture 1
  if(metallic)
    glBindTexture(GL_TEXTURE_2D, 1);
  else
    glBindTexture(GL_TEXTURE_2D, 6);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_GEN_S); // abilito la generazione automatica delle coord texture S e T
    glEnable(GL_TEXTURE_GEN_T);
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP); // Env map
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    glColor3f(1,1,1); // metto il colore neutro (viene moltiplicato col colore texture, componente per componente)
    glDisable(GL_LIGHTING); // disabilito il lighting OpenGL stan<sdard (lo faccio con la texture)
}

/***************************************************************/
// DoStep: facciamo un passo di fisica (a delta_t costante)
// Indipendente dal rendering.
// Ricordiamoci che possiamo LEGGERE ma mai SCRIVERE
// la struttura controller da DoStep
/***************************************************************/
void Titanic::DoStep(){
  // computiamo l'evolversi della barca
  static int i=5;
  
  float vxm, vym, vzm; // velocita' in spazio barca
  
  // da vel frame mondo a vel frame barca
  float cosf = cos(facing*M_PI/180.0);
  float sinf = sin(facing*M_PI/180.0);
  vxm = +cosf*vx - sinf*vz;
  vym = vy;
  vzm = +sinf*vx + cosf*vz;
  
  // gestione dello sterzo
  if (controller.key[Controller::LEFT]) sterzo += velSterzo;
  if (controller.key[Controller::RIGHT]) sterzo -= velSterzo;
  sterzo*=velRitornoSterzo; // ritorno a timone dritto
  
  if (controller.key[Controller::ACC]) vzm-=accMax; // accelerazione in avanti 
  if (controller.key[Controller::DEC]) vzm+=accMax; // accelerazione indietro
  
  
  // attirti (semplificando)
  vxm*=attritoX;  
  vym*=attritoY;
  vzm*=attritoZ;
  
  // l'orientamento della barca' segue quello dello sterzo
  // (a seconda della velocita' sulla z)
  facing = facing - (vzm*grip)*sterzo;
  
  // ritorno a vel coord mondo
  vx = +cosf*vxm + sinf*vzm;
  vy = vym;
  vz = -sinf*vxm + cosf*vzm;
  
  // posizione = posizione + velocita * delta t (ma delta t e' costante)
  px+=vx;
  if(onde)
    py= sin(0.34*px);
  else
    py = -0.7;
  //py=sin(vx);
  //py=sin(0.34*px);
  pz+=vz;

  /* imposto i limiti del mondo di gioco */
  if(pz >= 65) pz = 65;
  if(pz <= -65) pz = -65;
  if(px >= 65) px = 65;
  if(px <= -65) px = -65;

  /*controlli compenetrazioni primo iceberg*/

 if((pz > -29) && (pz < 26) &&
     (px > -8 ) && (px < 40.66 )){ 
        
        pz-=vz;
        px-=vx;

  }
}

/***********************/
/* init del controller */
/***********************/
void Controller::Init(){
  for (int i=0; i<NKEYS; i++) 
    key[i]=false;
}


/***************************/
/* init della classe Titanic */
/***************************/
void Titanic::Init(){
  // inizializzo lo stato della barca
  
  // posizione e orientamento
  px = -17;
  py = -0.7;
  pz = 23;
  facing = 0;
  
  // stato
  sterzo = 0;

  // velocita' attuale
  vx = 0;
  vy = 0;
  vz = 0;
  
  // inizializzo la struttura di controllo
  controller.Init();
  
           // A
  velSterzo = 2.4;         // A
  velRitornoSterzo = 0.93; // B, sterzo massimo = A*B / (1-B)
  velVolante = 0.04;

  accMax = 0.0011;
  
  // attriti: percentuale di velocita' che viene mantenuta
  // 1 = no attrito
  // <<1 = attrito grande
  attritoZ = 0.991;  // piccolo attrito sulla Z (nel senso di rotolamento delle ruote)
  attritoX = 0.8;  // grande attrito sulla X (per evitare slittamento)
  attritoY = 1.0;  // attrito sulla y nullo
  
  // Nota: vel max = accMax*attritoZ / (1-attritoZ)
  
  //raggioRuotaA = 0.25;
  //raggioRuotaP = 0.35;
  
  grip = 0.45;
}

/***************************************************************/
// funzione che disegna tutti i pezzi della barca
// (da invocarsi due volte: per la barca, e per la sua ombra)
// (se usecolor e' falso, NON sovrascrive il colore corrente
//  e usa quello stabilito prima di chiamare la funzione)
/***************************************************************/
void Titanic::RenderAllParts(bool usecolor, float mozzo) const{
  
  // disegna la barca con una mesh
    glScalef(-0.005,0.005,-0.005); // patch: riscaliamo la mesh
    glRotatef(180, 0,1,0);
     
    glPushMatrix();
    glTranslate(  timone.Center() );
    glRotatef(14,0,1,0); //raddrizzo il timone
    glColor3f(0.6,0.40,0.12); // colore del timone
    glRotatef(-sterzo,0,1,0);
    //glRotatef(mozzoA,1,0,0);
    glTranslatef(-25,0,0); //porto il timone a sinistra
  	glTranslate( -timone.Center() );
  	timone.RenderNxF();
	  glPopMatrix();
    
    glPushMatrix();
    glColor3f(1.0 ,1.0 ,1.0);
    restante_barca.RenderNxV();
    glPopMatrix();
    
    SetupTexture();
    glPushMatrix();
    esterno_barca.RenderNxV();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
  
}

/*********************/
/* disegna a schermo */
/*********************/
void Titanic::Render(float mozzo) const{
  // sono nello spazio mondo
  glPushMatrix();
  glTranslatef(px,py,pz);
  glRotatef(facing, 0,1,0);
	// sono nello spazio barca
  RenderAllParts(true, mozzo); 
  glPopMatrix(); 
  glPopMatrix();
}

void Titanic::setAccMax(){
  accMax=0.0011;
}
