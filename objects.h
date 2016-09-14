/**************************************************/
/* header contenente la definizione della maggior */
/* parte degli oggetti mesh presenti nella scena  */
/* che non fanno parte della classe titanic         */
/**************************************************/

#include <math.h>

bool crea = true; // il cubo deve essere rigenerato
bool generatore = true; // devo inizializzare il seme
int pos_x, pos_y, pos_z; // traslazione del cubo
int pos_x2, pos_y2, pos_z2; // traslazione del cubo
bool colori = false; // il mare è colorato o ha una textura applicata


Mesh ice((char *)"ice.obj");
Mesh ice1((char *)"iceberg.obj");
Mesh circle((char *)"circle.obj");
Mesh elicottero((char *)"elicottero.obj");
Mesh elica((char *)"elica.obj");
Mesh finestre((char *)"finestre.obj");
Mesh elica_inferiore((char *)"elica_inferiore.obj");


//Mesh drago((char *)"drago.obj");
//Mesh aereo((char *)"aereo.obj");

extern int punteggio;
extern bool onde;
extern bool useShadow ;

/**************************/
/* disegna la mongolfiera */
/**************************/


void drawElicottero(float mozzo) {
  
  glPushMatrix();
  glScalef(0.3,0.3,0.3);
  glRotatef(-(0.2*mozzo),0,1,0); // l'oggetto ruota nel mondo
  glTranslatef(250,100,0); // trasliamo l'oggetto
  
  glPushMatrix();
  glTranslatef(0,0,-3);
  glRotatef(25*mozzo,0,1,0);
  glColor3f(0.74,0.71,0.23); 
  elica.RenderNxF();
  glPopMatrix();

  glPushMatrix();
  //glTranslatef(0,0,-3);
  glTranslatef(0, +elica_inferiore.Center().Y(), +elica_inferiore.Center().Z());
  glRotatef(25*mozzo,1,0,0);
  glColor3f(0.74,0.71,0.23); 
  glTranslatef(0, -elica_inferiore.Center().Y(), -elica_inferiore.Center().Z() );
  elica_inferiore.RenderNxF();
  glPopMatrix();
  

  glPushMatrix();
  //glRotatef(25*mozzo,0,1,0);
  //applico texture vetro alle finestre
  glBindTexture(GL_TEXTURE_2D, 8);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_TEXTURE_GEN_S); // abilito la generazione automatica delle coord texture S e T
  glEnable(GL_TEXTURE_GEN_T);
  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP); // Env map
  glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
  glColor3f(1,1,1); // metto il colore neutro (viene moltiplicato col colore texture, componente per componente)
  finestre.RenderNxF();
  glPopMatrix();
  glDisable(GL_TEXTURE_2D);
  
  glPushMatrix();
  glColor3f(0,0,0); // metto il colore neutro (viene moltiplicato col colore texture, componente per componente)
  //glDisable(GL_LIGHTING); // disabilito il lighting OpenGL standard (lo faccio con la texture)*/
  elicottero.RenderNxF();
  glPopMatrix();
  if(useShadow){
    //disegno ombra elicottero
    glPushMatrix();
    float py=98;
    glTranslatef(0,-py,0);
    //glScalef(1+py/50,1+py/50,1+py/50);
    glColor3f(0.4,0.4,0.4); // colore fisso
    glTranslatef(0,0.01,0); // alzo l'ombra di un epsilon per evitare z-fighting con il pavimento
    glScalef(1.01,0,1.01);  // appiattisco sulla Y, ingrandisco dell'1% sulla Z e sulla X 
    glDisable(GL_LIGHTING); // niente lighing per l'ombra
    elicottero.RenderNxF();// disegno l'elicottero appiattito
    glRotatef(25*mozzo,0,1,0);
    elica.RenderNxF();
    glPopMatrix();
    glEnable(GL_LIGHTING);
  }
  
  glPopMatrix();

}

void drawIce() {
  
    glPushMatrix();
    glScalef(2,2,2); // scalo l'oggetto
    glColor3f(1,1,1);
    glTranslatef(-25,0,20); // trasliamo l'oggetto
    ice.RenderNxF();
    glPopMatrix();
 
}

void drawIce1() {
  
  glPushMatrix();
  glTranslatef(-100,0,0); // trasliamo l'oggetto
	glRotatef(90,0,1,0);
  ice1.RenderNxF();
  glPopMatrix();
  
}

void drawCircle(int contatore) {
	
   glColor3f(0.8,0.5,0.1);
   if(contatore >= 7){
      glPushMatrix();
      glScalef(1.5,1.5,1.5); // scalo l'oggetto
      glTranslatef(-20,0,10); // trasliamo l'oggetto
      glRotatef(90,1,0,0);
      circle.RenderNxF();
      glPopMatrix();
      glColor3f(0,0.7,0);
    }
   if(contatore >= 6){
      glPushMatrix();
      glScalef(1.5,1.5,1.5); // scalo l'oggetto
      glTranslatef(-35,0,35); // trasliamo l'oggetto
      glRotatef(90,1,0,0);
      circle.RenderNxF();
      glPopMatrix();
      glColor3f(0,0.7,0);
    }
   if(contatore >= 5){
      glPushMatrix();
      glScalef(1.5,1.5,1.5); // scalo l'oggetto
      glTranslatef(-10,0,35); // trasliamo l'oggetto
      glRotatef(90,1,0,0);
      glRotatef(-100,0,0,1);
      circle.RenderNxF();
      glPopMatrix();
      glColor3f(0,0.7,0);
    }
   if(contatore >= 4){
      glPushMatrix();
      glScalef(1.5,1.5,1.5); // scalo l'oggetto
      glTranslatef(25,0,35); // trasliamo l'oggetto
      glRotatef(90,1,0,0);
      glRotatef(-110,0,0,1);
      circle.RenderNxF();
      glPopMatrix();
      glColor3f(0,0.7,0);
    }
   if(contatore >= 3){
       glPushMatrix();    
       glScalef(1.5,1.5,1.5); // scalo l'oggetto
       glTranslatef(40,0,5); // trasliamo l'oggetto
       glRotatef(90,1,0,0);
       glRotatef(30,0,0,1);
       circle.RenderNxF();
       glPopMatrix();
       glColor3f(0,0.7,0);
    }
   if(contatore >= 2){   
       glPushMatrix();
       glScalef(1.5,1.5,1.5); // scalo l'oggetto
       glTranslatef(35,0,-25); // trasliamo l'oggetto
       glRotatef(90,1,0,0);
       glRotatef(-20,0,0,1);
       circle.RenderNxF();
       glPopMatrix();
       glColor3f(0,0.7,0);
    }
   if(contatore >= 1){
       glPushMatrix();
       glScalef(1.5,1.5,1.5); // scalo l'oggetto
       glTranslatef(0,0,-35); // trasliamo l'oggetto	
       glRotatef(90,1,0,0);
       glRotatef(-90,0,0,1);
       circle.RenderNxF();
       glPopMatrix();
       glColor3f(0,0.7,0);
    }
	  glPushMatrix();
		glScalef(1.5,1.5,1.5); // scalo l'oggetto
		glTranslatef(-22,0,-18); // trasliamo l'oggetto
		glRotatef(90,1,0,0);
		circle.RenderNxF();
		glPopMatrix();

   
}
/********************************************************
 * function per il disengo del terreno su cui si troveranno
 * i diversi oggetti presenti nella scena
 ********************************************************/
void drawFloor() {
  const float S=100; // size
  const float H=0;   // altezza
  const int K=150; //disegna K x K quads

  // ulilizzo le coordinate OGGETTO
  // cioe' le coordnate originali, PRIMA della moltiplicazione per la ModelView
  // in modo che la texture sia "attaccata" all'oggetto, e non "proiettata" su esso
  
  if(!colori) {
    // disegno il terreno ripetendo una texture su di esso
    glBindTexture(GL_TEXTURE_2D, 5);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
  }

  // disegna KxK quads
   glBegin(GL_QUADS);
   glNormal3f(0,1,0); // normale verticale uguale x tutti
    float altezzaOnde = 1.0; 
    int step=0;
    for (int x=0; x<K; x++) 
      for (int z=0; z<K; z++) {
        // scelgo il colore per quel quad
         if(colori) {
          glColor3f(0.0, 0.0, 4.0);
         }
        float x0=-S + 2*(x+0)*S/K;
        float x1=-S + 2*(x+1)*S/K;
        float z0=-S + 2*(z+0)*S/K;
        float z1=-S + 2*(z+1)*S/K;
		
      if(onde){
    		if(!colori) glTexCoord2f(0.0, 0.0);
    		glVertex3d(x0,sin(0.34*x0), z0);
    		if(!colori) glTexCoord2f(1.0, 0.0);
    		glVertex3d(x1,sin(0.34*x1), z0);
    		if(!colori) glTexCoord2f(1.0, 1.0);
    		glVertex3d(x1,sin(0.34*x1), z1);
    		if(!colori) glTexCoord2f(0.0, 1.0);
    		glVertex3d(x0, sin(0.34*x0), z1);
  	 }
     else{

    	 if(!colori) glTexCoord2f(0.0, 0.0);
        glVertex3d(x0, H, z0);
        if(!colori) glTexCoord2f(1.0, 0.0);
        glVertex3d(x1, H, z0);
        if(!colori) glTexCoord2f(1.0, 1.0);
        glVertex3d(x1, H, z1);
        if(!colori) glTexCoord2f(0.0, 1.0);
        glVertex3d(x0, H, z1);
      }
     /* glTexCoord2f(0.0, 0.0);
      glVertex3d(x0, H, z0);
      glTexCoord2f(1.0, 0.0);
      glVertex3d(x1, H, z0);
      glTexCoord2f(1.0, 1.0);
      glVertex3d(x1, H, z1);
      glTexCoord2f(0.0, 1.0);
      glVertex3d(x0, H, z1);
	*/
        step++;
    }
  glEnd();
  glDisable(GL_TEXTURE_2D);

}

/*********************************************/
/* disegna un cubo con una texture personale */
/*********************************************/
void drawBonus(float mozzo, Titanic& titanic , bool& up) {

  // se devo inizializzare il seme
  if(generatore){
    srand(time(NULL));
    generatore = false;
  }

  /* disegno del primo cubo */

  // se devo rigenerare il cubo
  if(crea) {
    pos_x = (rand()%59+1)-30;
    //pos_y = (rand()%29+1);
    pos_y = 0;
    if(up)
      pos_z = (rand()%36+24);
    else
      pos_z = -(rand()%36+24);
    //se non è stato catturato non devo ridisegnare il cubo
    crea = false;
  }

  // disegno del cubo con una texture personale su tutti e sei i lati
  glPushMatrix();
  glBindTexture(GL_TEXTURE_2D, 3);
  glEnable(GL_TEXTURE_2D);
  glDisable(GL_TEXTURE_GEN_S);
  glDisable(GL_TEXTURE_GEN_T);
  glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
  glTranslatef(pos_x,pos_y,pos_z);
  glTranslatef(2, 2, 2);
  glRotatef(mozzo,1,1,0);
  glTranslatef(-2, -2, -2);

 glBegin(GL_QUADS);
      /* Front. */
      glTexCoord2f(0.0, 0.0);
      glVertex3f(1.0, 1.0, 3.0);
      glTexCoord2f(1.0, 0.0);
      glVertex3f(3.0, 1.0, 3.0);
      glTexCoord2f(1.0, 1.0);
      glVertex3f(3.0, 3.0, 3.0);
      glTexCoord2f(0.0, 1.0);
      glVertex3f(1.0, 3.0, 3.0);

      /* Down. */
      glTexCoord2f(0.0, 0.0);
      glVertex3f(1.0, 1.0, 1.0);
      glTexCoord2f(1.0, 0.0);
      glVertex3f(3.0, 1.0, 1.0);  
      glTexCoord2f(1.0, 1.0);
      glVertex3f(3.0, 1.0, 3.0);
      glTexCoord2f(0.0, 1.0);
      glVertex3f(1.0, 1.0, 3.0);

      /* Back. */
      glTexCoord2f(0.0, 0.0);
      glVertex3f(1.0, 3.0, 1.0);
      glTexCoord2f(1.0, 0.0);
      glVertex3f(3.0, 3.0, 1.0);  
      glTexCoord2f(1.0, 1.0);
      glVertex3f(3.0, 1.0, 1.0);
      glTexCoord2f(0.0, 1.0);
      glVertex3f(1.0, 1.0, 1.0);

      /* Up. */
      glTexCoord2f(0.0, 0.0);
      glVertex3f(1.0, 3.0, 3.0);
      glTexCoord2f(1.0, 0.0);
      glVertex3f(3.0, 3.0, 3.0);  
      glTexCoord2f(1.0, 1.0);
      glVertex3f(3.0, 3.0, 1.0);
      glTexCoord2f(0.0, 1.0);
      glVertex3f(1.0, 3.0, 1.0);
      
      /* SideLeft. */
      glTexCoord2f(0.0, 0.0);
      glVertex3f(1.0, 3.0, 1.0);
      glTexCoord2f(1.0, 0.0);
      glVertex3f(1.0, 3.0, 3.0);  
      glTexCoord2f(1.0, 1.0);
      glVertex3f(1.0, 1.0, 3.0);
      glTexCoord2f(0.0, 1.0);
      glVertex3f(1.0, 1.0, 1.0);

      /* SideRight. */
      glTexCoord2f(0.0, 0.0);
      glVertex3f(3.0, 3.0, 1.0);
      glTexCoord2f(1.0, 0.0);
      glVertex3f(3.0, 3.0, 3.0);  
      glTexCoord2f(1.0, 1.0);
      glVertex3f(3.0, 1.0, 3.0);
      glTexCoord2f(0.0, 1.0);
      glVertex3f(3.0, 1.0, 1.0);

    glEnd();
    glDisable(GL_TEXTURE_2D);

    glLineWidth(1);

    glColor3f(0,0,0);
    glBegin(GL_LINE_LOOP);
      glVertex3f(1.0, 1.0, 3.0);
      glVertex3f(3.0, 1.0, 3.0);
      glVertex3f(3.0, 3.0, 3.0);
      glVertex3f(1.0, 3.0, 3.0);
    glEnd();

    glBegin(GL_LINE_LOOP);
      glVertex3f(1.0, 3.0, 1.0);
      glVertex3f(3.0, 3.0, 1.0);  
      glVertex3f(3.0, 1.0, 1.0);
      glVertex3f(1.0, 1.0, 1.0);
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(1.0, 1.0, 3.0);
    glVertex3f(1.0, 1.0, 1.0);

    glVertex3f(3.0, 1.0, 3.0);
    glVertex3f(3.0, 1.0, 1.0);

    glVertex3f(1.0, 3.0, 3.0);
    glVertex3f(1.0, 3.0, 1.0);

    glVertex3f(3.0, 3.0, 3.0);
    glVertex3f(3.0, 3.0, 1.0);      
    glEnd();
    // se la barca ha catturato il cubo
    if (titanic.px >= pos_x - 4 && titanic.px <= pos_x + 4 &&
        titanic.py >= pos_y - 4 && titanic.py <= pos_y + 4 &&
        titanic.pz >= pos_z - 4 && titanic.pz <= pos_z + 4) {
       up=!up;
     //aumento la velocità
      if(titanic.accMax <= 0.0033)
           titanic.accMax = titanic.accMax + 0.0022;
  	       crea = true;
    }

glPopMatrix();


}
