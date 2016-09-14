
/*********************/
/* CLASSE CONTROLLER */
/*********************/
class Controller {
public:
  enum { LEFT=0, RIGHT=1, ACC=2, DEC=3, NKEYS=4 };
  bool key[NKEYS];
  
  void Init();
  void EatKey(int keycode, int* keymap, bool pressed_or_released);
  void Joy(int keymap, bool pressed_or_released);
  Controller(){ Init(); } // costruttore
};

/****************/
/* CLASSE Titanic */
/****************/
class Titanic {

  void RenderAllParts(bool usecolor, float mozzo) const; 
                         // disegna tutte le parti dell'BARCA
                         // invocato due volte: per Titanic e la sua ombra

public:
  // Metodi
  void Init(); // inizializza variabili
  void Render(float mozzo) const; // disegna a schermo
  void DoStep(); // computa un passo del motore fisico
  void setAccMax(); // computa un passo del motore fisico
  Titanic(){ Init(); } // costruttore
 
  Controller controller;  
  
  // STATO DELLA BARCA
  // (DoStep fa evolvere queste variabili nel tempo)
  float px,py,pz,facing; // posizione e orientamento
  float mozzoA, mozzoP, sterzo; // stato interno
  float vx,vy,vz; // velocita' attuale
  //float volante;
  
  // STATS DELL'BARCA
  // (di solito rimangono costanti)
  float velSterzo, velRitornoSterzo, accMax, attrito,
        raggioRuotaA, raggioRuotaP, grip,
        attritoX, attritoY, attritoZ; // attriti
  float velVolante; 
};
