#define pin_pwm 5 //Pïn de contrôle de la vitesse dans le sens B sur le MOSFET N
#define pin_sens 7 //Pin de contrôle dans le sens A sur le MOSFET P
//#define pin_sens_b 7 //Pin de contrôle dans le sens B sur le MOSFET P

void setup() {
  pinMode(pin_sens,OUTPUT);
  //pinMode(pin_sens_b,OUTPUT);
  pinMode(pin_pwm,OUTPUT);
}

void loop() {
  digitalWrite(pin_pwm,LOW);  
  digitalWrite(pin_sens,HIGH);
  /*
  digitalWrite(pin_sens,HIGH);
  for(int i =0; i<255; i++){
    analogWrite(pin_pwm,i);
    delay(10);
  }
  delay(10000);
  for(int i =255; i>0; i--){
    analogWrite(pin_pwm,i);
    delay(10);
  }
  digitalWrite(pin_sens,LOW);
    for(int i =0; i<255; i++){
    analogWrite(pin_pwm,i);
    delay(10);
  }
  delay(10000);
  for(int i =255; i>0; i--){
    analogWrite(pin_pwm,i);
    delay(10);
  }*/

}

