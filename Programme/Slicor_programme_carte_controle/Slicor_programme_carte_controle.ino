
//Utilitaire pour l'écran LCD
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#define len_lcd 20 //Nombre de caractère en longueur de l'écran
#define high_lcd 4 //Nombre de lignes de l'écran
#define lcd_adress 0x27 //Adresse de l'écran
LiquidCrystal_I2C lcd(lcd_adress,len_lcd,high_lcd);  //Configurer l'écran 
int screen_type = 0; //Permet de connaitre le mode d'affichage actuel
bool screen_chg = 0; //Permet de savoir si il y a eu changement sur l'écran

//Utilitaire du clavier
//#include <MultitapKeypad.h>
#define COL0 7 //Définition des broches du clavier
#define COL1 8 
#define COL2 9
#define COL3 10 
#define ROW3 A3 
#define ROW2 A2 
#define ROW1 A1 
#define ROW0 A0 
char clv [4][4] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
char key = 0;
char key_last = 0;

//MultitapKeypad kpd( ROW0, ROW1, ROW2, ROW3, COL0, COL1, COL2, COL3 ); //Création de l'objet du clavier
//Key key; //Création de l'objet touche étant un sous ensemble du clavier
int max_value = 100; //La valeur maximale qui peut être tapper à la main
bool preset = 0; //Permet de faire la distinction entre une valeur prédéfinie et une valeur tapper au clavier.

//Utilitaire voyant
int state = 0; //-1 en cours de homing
               // 0 pour le voyant rouge signifiant que la machine ne connait pas sa position, qu'elle est en fin de course ou qu'il y a une erreur.
               // 1 pour le voyant orange signifiant que la machine est en mouvement.
               // 2 pour le voyant vert signifiant que la machine est à l'arrêt et connait sa position (et qu'il n'y a pas d'erreur).        
#define V1 4 //pin voyant vert
#define V2 3 //pin voyant orange
#define V3 2 //pin voyant rouge
bool vchg = 0; //Permet de savoir si il y a eu un changement au niveau des voyants

//Utilitaire bouton
#define B1 5 //pin bouton avancé
#define B2 6 //pin bouton reculé
const int buttonPin1 = 2; // Broche du bouton 1
const int buttonPin2 = 3; // Broche du bouton 2
int buttonState1 = 0; // État du bouton 1
int buttonState2 = 0; // État du bouton 2
unsigned long lastButtonPress = 0; // Moment du dernier appui sur un bouton
int delay_btn = 300;
unsigned long antispam = 0; // Moment du dernier appui sur un bouton
int delay_antispam = 1500;

//Variables communiquées entre les deux cartes
int pos = -1; //Contient la position du vérin à un instant donné en mm
int mouvement = 0; //Longueur du prochain mouvement en mm
char inByte = 0;

//Variables de delay
unsigned long timer1 = 0; //Permet de limiter la fréquence d'affichage écran et voyants
int delay_timer1 = 1000;

void setup(){
  //Démarrer le port série
  Serial.begin(9600);

  //Démarrer l'écran LCD
  lcd.init();  //initialize the lcd
  lcd.backlight();  //open the backlight 
   
  //Affichage au démarrage
  screen(screen_type);

  //Configuration du clavier
  pinMode(COL0,INPUT_PULLUP);
  pinMode(COL1,INPUT_PULLUP);
  pinMode(COL2,INPUT_PULLUP);
  pinMode(COL3,INPUT_PULLUP);
  pinMode(ROW0,OUTPUT);
  pinMode(ROW1,OUTPUT);
  pinMode(ROW2,OUTPUT);
  pinMode(ROW3,OUTPUT);

  //Configuration des voyants
  pinMode(V1,OUTPUT); //Mettre pins des voyants en sortie
  pinMode(V2,OUTPUT);
  pinMode(V3,OUTPUT);
  
  //Configuration des boutons
  pinMode(B1,INPUT);
  pinMode(B2,INPUT);

  //Päuse pour l'affichage et laisser le temps aux composants de démarrer
  delay(3000);

  screen_type = 1;
  vchg = 1;
  screen_chg = 1;

  delay(1);
  delay(1);
  delay(1);
  delay(1);
  delay(1);
  delay(1);
  
}

void loop(){
  if(screen_chg){
    screen(screen_type); //Mise à jour de l'écran
    screen_chg = 0;
  }
  if(vchg){
    voyants(); //Mise à jour des voyants
    vchg = 0;
  }
  mes_puiss_read();
  if(pos != -1){
    clavier(); //Mise à jour du clavier
  }
  bouton(); //Mise à jour des boutons
}

char keyget(){
  int row = 0;

  digitalWrite(ROW0,LOW);
  digitalWrite(ROW1,HIGH);
  digitalWrite(ROW2,HIGH);
  digitalWrite(ROW3,HIGH);
  row = 0;
  if(!digitalRead(COL0)) return clv[row][0];
  if(!digitalRead(COL1)) return clv[row][1];
  if(!digitalRead(COL2)) return clv[row][2];
  if(!digitalRead(COL3)) return clv[row][3];

  digitalWrite(ROW0,HIGH);
  digitalWrite(ROW1,LOW);
  digitalWrite(ROW2,HIGH);
  digitalWrite(ROW3,HIGH);
  row = 1;
  if(!digitalRead(COL0)) return clv[row][0];
  if(!digitalRead(COL1)) return clv[row][1];
  if(!digitalRead(COL2)) return clv[row][2];
  if(!digitalRead(COL3)) return clv[row][3];

  digitalWrite(ROW0,HIGH);
  digitalWrite(ROW1,HIGH);
  digitalWrite(ROW2,LOW);
  digitalWrite(ROW3,HIGH);
  row = 2;
  if(!digitalRead(COL0)) return clv[row][0];
  if(!digitalRead(COL1)) return clv[row][1];
  if(!digitalRead(COL2)) return clv[row][2];
  if(!digitalRead(COL3)) return clv[row][3];

  digitalWrite(ROW0,HIGH);
  digitalWrite(ROW1,HIGH);
  digitalWrite(ROW2,HIGH);
  digitalWrite(ROW3,LOW);
  row = 3;
  if(!digitalRead(COL0)) return clv[row][0];
  if(!digitalRead(COL1)) return clv[row][1];
  if(!digitalRead(COL2)) return clv[row][2];
  if(!digitalRead(COL3)) return clv[row][3];

  return 0;

}

void clavier(){ //Gère le clavier
  key = keyget();
  if((key != key_last) && (key != 0)){  
    if(key == 'A'){ //Mettre le voyant en orange  //Preselection A
      mouvement = 1;
      state = 4; 
    }
    else if(key == 'B'){//Preselection B
      mouvement = 3;
      state = 4;
    }  
    else if(key == 'C'){//Preselection C
      mouvement = 5;
      state = 4;
    }  
    else if(key == 'D'){//Preselection D
      mouvement = 10;
      state = 4;
    }  
    else if(key == '*'){//Effacer
      mouvement = 0;
      state = 4;
    }
    else if(key == '#'){//Valider
      state = 2;
    }
    else if((key <= '9') && (key >= '0')){ //Entrer une valeur précise
      state = 4;
      mouvement = mouvement*10+(key-'0');
      if(mouvement > 500){
        mouvement = 500;
      }
    }
    screen_chg = 1;
    vchg = 1;
  }
  key_last = key;
}

void bouton(){ //Gère les boutons
  unsigned long currentMillis = millis(); // Obtenir le temps écoulé depuis le démarrage de la carte Arduino
  buttonState1 = digitalRead(B1); // Lire l'état du bouton 1
  buttonState2 = digitalRead(B2); // Lire l'état du bouton 2

  if((currentMillis - antispam) > delay_antispam){ //Comme le nom l'indique cela évite que la commande soit spammé
    if (buttonState1 == LOW && buttonState2 == HIGH) { // Si le bouton 1 est appuyé et le bouton 2 est relâché
      if (currentMillis - lastButtonPress > delay_btn) { // Si le dernier appui sur un bouton remonte à plus de 50 millisecondes
        //Serial.println("Bouton 1 appuyé"); // Afficher un message de débogage
        antispam = millis();
        mes_puiss_send(1);
        lastButtonPress = currentMillis; // Mettre à jour le moment du dernier appui sur un bouton
      }
    } 
    else if (buttonState1 == HIGH && buttonState2 == LOW) { // Si le bouton 1 est relâché et le bouton 2 est appuyé
      if (currentMillis - lastButtonPress > delay_btn) { // Si le dernier appui sur un bouton remonte à plus de 50 millisecondes
        //Serial.println("Bouton 2 appuyé"); // Afficher un message de débogage
        antispam = millis();
        mes_puiss_send(2);
        lastButtonPress = currentMillis; // Mettre à jour le moment du dernier appui sur un bouton
      }
    }
    else if (buttonState1 == LOW && buttonState2 == LOW) { // Si les deux boutons sont appuyés
      if (currentMillis - lastButtonPress > delay_btn) { // Si le dernier appui sur un bouton remonte à plus de 50 millisecondes
        //Serial.println("Les deux boutons sont appuyés"); // Afficher un message de débogage
        antispam = millis();
        vchg = 1; //Précise qu'il y a peut etre eu un changement au niveau des voyants
        screen_type = 2; //Change ou confirme le mode de l'écran
        screen_chg = 1;//Précisé qu'il y a peut etre eu un changement au niveau de l'écran
        mes_puiss_send(3);
        lastButtonPress = currentMillis; // Mettre à jour le moment du dernier appui sur un bouton
        state = -1; //Position inconnu
      }
    }
    else{
      lastButtonPress = millis();
    }
  }
  return 0;
}

void voyants(){ //Gère les voyants
  if(state == 0){ //Allumage du voyant 1
    digitalWrite(V1,HIGH);
    digitalWrite(V2,LOW);
    digitalWrite(V3,LOW);
  }
  else if(state == 1 || state == -1 || state == 4){ //Allumage du voyant 2
    digitalWrite(V1,LOW);
    digitalWrite(V2,HIGH);
    digitalWrite(V3,LOW);    
  }
  else if(state == 2 || state == 3){ //Allumage du voyant 3
    digitalWrite(V1,LOW);
    digitalWrite(V2,LOW);
    digitalWrite(V3,HIGH);    
  }
  return;
}

void mes_puiss_send(int mes){ //Gère les messages avec la carte de puissance
  switch(mes){
    case 0: 
      break; //Envoi rien du tout
    case 1:
      if(state == 2){
        Serial.print("m1"+String(mouvement));//Envoi le message de mouvement dans le sens 1
      }
      break;
    case 2:
      if(state == 2){
        Serial.print("m2"+String(mouvement));//Envoi le message de mouvement dans le sens 1
      }
      break;        
    case 3:
      if(state == 0 || state == 2){
        Serial.print("h");//Envoi le message de homing
        break;
      }
  }
  return;
}

void mes_puiss_read(){
  if(Serial.available() > 0) {
    inByte = Serial.read();//On lit le premier caractère pour connaitre le type du message
    if(inByte == 'p'){//Alors il s'agit d'un envoi de position
      if(Serial.available() && (char)Serial.peek() != '\n'){
        pos = 0;
        screen_chg = 1;
        vchg = 1;
      }
      while(Serial.available() && (char)Serial.peek() != '\n'){
        pos = pos * 10 + (Serial.read() - '0');
      } 
    }
    else if(inByte == 'r'){//Alors la carte de puissance indique qu'elle est prête à recevoir une nouvelle commande
      screen_chg = 1;
      vchg = 1;
      state = 2;
    }
    else if(inByte == 'm'){//Alors la carte de puissance indique qu'elle est en mouvement
      screen_chg = 1;
      vchg = 1;
      state = 1;      
    }
    else if(inByte == 's'){//Alors la carte de puissance indique qu'elle a atteind le fin de course
      screen_chg = 1;
      vchg = 1;
      state = 0;
    }
    else if(inByte == 'e'){//Alors la carte de puissance indique qu'il y a eu une erreur
      screen_chg = 1;
      vchg = 1;
      state = 3;
    }
  }
}

void screen(int mode){
  if(mode == 0){ //Ecran de démarrage
    lcd.setCursor(0,0);//On remet le curseur en haut à gauche
    lcd.clear();//On efface l'écran avant d'afficher de nouvelles informations
    affichage(0,"Slicor en cours",1);
    affichage(1,"de demarrage.",1);
    affichage(2,"Patientez...",1);
    affichage(3,"Fait par TH0L3F",1);
  }
  else if(mode == 1){ //Ecran avant le homing
    lcd.setCursor(0,0);//On remet le curseur en haut à gauche
    lcd.clear();//On efface l'écran avant d'afficher de nouvelles informations
    affichage(0,"En attente de",1);
    affichage(1,"Homing. Appuyez",1);
    affichage(2,"simultanement sur",1);
    affichage(3,"le bouton <- et ->",1);
  }
  else if(mode == 2){ //Ecran d'affichage en cours de fonctionnement
    lcd.clear();//On efface l'écran avant d'afficher de nouvelles informations
    affichage(0, "Position : ",2);
    if(pos == -1){//Position non connue
      affichage(0,"xxx",3); //Affichage d'un point d'interrogation
    }
    else{//Position connue
      affichage(0,String(pos),3);//Affichage de la position
    }
    affichage(1, "Longueur : ",2);
    affichage(1, String(mouvement)+" mm",3);
    if (state == -1){
      affichage(3, "  En cours de homing",2);
    }
    else if(state == 0){
      affichage(3, "   En bout de course",2);
    }
    else if(state == 1){
      affichage(3, "        En mouvement",2);
    }
    else if(state == 2){
      affichage(3, "         En position",2);
    }
    else if(state == 3){
      affichage(3, "  Erreur, pb machine",2);
    }
    else if(state == 4){
      affichage(3, "   Validez la valeur",2);
    }
  }
}

void affichage(int ligne, String text, int type){
  if((ligne > high_lcd) || (ligne < 0)){
    //Serial.println("Tentative d'écriture en dehors de l'écran (hauteur)");
    return;
  }
  else if((text.length() > len_lcd) || (text.length() < 0)){
    //Serial.println("Tentative d'écriture en dehors de l'écran (longueur)");
    return;    
  }
  else if(type == 1){ //Affichage du texte au centre de la ligne
    lcd.setCursor ( int(len_lcd/2)-int(text.length()/2), ligne ); 
    lcd.print(text); 
  }
  else if(type == 2){//Affichage aligner à gauche
    lcd.setCursor ( 0, ligne);
    lcd.print(text);
  }
  else if(type == 3){//Affichage aligner à droite
    lcd.setCursor ((len_lcd - text.length()), ligne);
    lcd.print(text);
  }
  return;
}
