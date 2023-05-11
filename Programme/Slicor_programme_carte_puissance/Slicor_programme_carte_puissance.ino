//*****COMMUNICATION*******
//Variables communiquées entre les deux cartes
int n_error = 0;
int pos_vol = 0; //Position voulue
int delay_send_pos = 1000;
unsigned long last_time = 0;

//*********VERIN***********
//Broches vérin
#define pin_pwm 5 //Pin de contrôle de la vitesse
#define pin_sens 7 //Pin de contrôle dans le sens A
bool sens_abs = true;//Permet d'échanger tous les sens si besoin

//**********REL************
//Broches régle électronique linéaire (rel)
#define rel_A 2 //Pin A du rel
#define rel_B 3 //Pin B du rel

//Variables rel
float dpp = 0.005; // dpp (distance per pulsion) in micrometer
int pos_rel = 157; //Enregistrement de la position de la règle (en pulsion)
bool ax = false; //Ancien état de la broche A
bool bx = false; //Ancien état de la broche B
bool a = false; //Nouvel état de la broche A
bool b = false; //Nouvel état de la broche B

//*****FIN DE COURSE*******
#define fca A0 //Pin fin de course A (position minimal)
#define fcb A1 //Pin fin de course B (position maximale)
bool unk = false; //Si unk vaut false : la position n'est pas connu. Il faut faire un homing

void setup(){

  //Démarrer le port série
  Serial.begin(9600);

  //Configuration du vérin
  pinMode(pin_sens,OUTPUT); //Pin de sens vérin
  pinMode(pin_pwm,OUTPUT); //Pin de sens vérin
  
  //Configuration capteur position linéaire
  attachInterrupt(digitalPinToInterrupt(rel_A), cpl_pos, CHANGE); //Permet de créer des interruption pour la lecture du rel
  attachInterrupt(digitalPinToInterrupt(rel_B), cpl_pos, CHANGE); //Uniquement sur les passages à l'état haut

  pinMode(fca, INPUT_PULLUP);
  pinMode(fcb, INPUT_PULLUP);

  //Activation des interruptions
  interrupts();

  delay(1);
  delay(1);
  delay(1);
  delay(1);
  delay(1);
  delay(1);
  delay(1);
  delay(1);

}

void loop(){
  if (Serial.available()) { // Vérifie si des données sont disponibles sur le port série
    char message = Serial.read(); // Lit le premier caractère reçu sur le port série
    if (message == 'h') {
      Serial.print('h');
      homing(); // Exécute la fonction homming()
      Serial.print('r');
    }
    else if (message == 'm') {
      char direction = Serial.read(); // Lit le caractère suivant
      int length = 0;

      // Lit les caractères suivants jusqu'à ce qu'on atteigne la fin de ligne
      while (Serial.available() && (char)Serial.peek() != '\n') {
        length = length * 10 + (Serial.read() - '0');
      }

      Serial.read(); // Lit le caractère de fin de ligne

      if (direction == '1') {
        pos_vol = pos_vol + length;
        Serial.print("m");
        mouv(); // Exécute la fonction move() dans le sens opposé avec la longueur spécifiée
        Serial.print("r");
      }
      else if (direction == '2') {
        pos_vol = pos_vol - length;
        if(pos_vol < 0){
          pos_vol = 0;
        }
        Serial.print("m");
        mouv(); // Exécute la fonction move() dans le sens opposé avec la longueur spécifiée
        Serial.print("r");
      }
    }
  }
  if((millis()-last_time)>delay_send_pos){
    Serial.print("p"+String(pos_rel));
    last_time = millis();
  }
}

void cpl_pos(){
  a = digitalRead(rel_A);
  b = digitalRead(rel_B);
  if(ax == a && bx == b){return 0;}//Pas de mouvements
  else if(ax && bx && !a && b){pos_rel++;}//Mouvement dans le sens 1
  else if(!ax && bx && !a && !b){pos_rel++;}//Mouvement dans le sens 1
  else if(!ax && !bx && a && !b){pos_rel++;}//Mouvement dans le sens 1
  else if(ax && !bx && a && b){pos_rel++;}//Mouvement dans le sens 1
  else if(ax && bx && a && !b){pos_rel--;}//Mouvement dans le sens 2
  else if(ax && !bx && !a && !b){pos_rel--;}//Mouvement dans le sens 2
  else if(!ax && !bx && !a && b){pos_rel--;}//Mouvement dans le sens 2
  else if(!ax && bx && a && b){pos_rel--;}//Mouvement dans le sens 2
  else{}//Il y a eu un saut d'un état
  ax = a;//Sauvegarde des nouveaux paramètres
  bx = b;//Sauvegarde des nouveaux paramètres
}

void verin_stop(){
  digitalWrite(pin_pwm,LOW);
  return;
}

void homing(){ //Permet de faire le homing sur la demande de la carte de commande
  digitalWrite(pin_sens,sens_abs);
  for(int i = 0; i < 255; i++){
    analogWrite(pin_pwm,i);
    delay(10);
    if(digitalRead(fca)){//Accélération progressive
      return;
    }
  }
  while(!digitalRead(fca)){
    delay(1);
  }
  verin_stop();
  return;
}

void mouv(){//Se déplacer à la position voulue
  for(int i = 0; i < 255; i++){
    analogWrite(pin_pwm,i);
    delay(10);
    if(digitalRead(fca)){//Accélération progressive
      return;
    }
  }
  delay(500);
  verin_stop();
}
