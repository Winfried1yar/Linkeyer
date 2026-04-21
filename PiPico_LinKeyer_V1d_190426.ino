
// ============================================================================
//  Arduino IDE settings:
//  board: Raspberry Pi Pico (Arduino-Pico)
//  Verzögerungsstufen geändert 50mS/100mS/200ms  50mS~200mS 
//  PTT-Nachlauf  abhängig vom Vorlauf 100mS/200mS 
//  Schaltung PiPico_Keyer V4.pdf
//  CW-Verzögerung =  2mS *6*32Bit = ca 390mS
//  PTT 
//  https://www.arduino.cc/reference/en/language/functions/usb/keyboard/keyboardmodifiers/
//  programmer: none
//  PiPico_LinKeyer_V1_110426
// (C) DL1YAR keine kommerziele Nutzung. NUR zum Amateurgebrauch !!!
// ============================================================================

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"


#include <Arduino.h>

#include "Texte_Om.h"

#include "PinConfig.h"

#define an  HIGH  // LED leuchtet
#define aus LOW   // LED leuchtet nicht


char versi[] = " pipico linkeyer v1d 190426 (c) dl1yar         ";
char dummy[] = "v";

uint16_t Hz        = 850 ; // frequenz Mithörton                             

int8_t z =0;
int8_t z_ab   = 2 ;   // Taktzeit Delaybetrieb
int8_t z_ab_1 = 2 ;   // Taktzeit Delaybetrieb
int8_t z_ab_2 = 5 ;   // Taktzeit ohne delay der PTT-CW


//gesamt 9*32 Bit zum schieben 288Bit*2mS ~~ 596mS Delay max
//uint32_t ptt_delay9 =0   ;// Zur PTT-CW verschiebung
//uint32_t ptt_delay8 =0   ;// Zur PTT-CW verschiebung
uint32_t ptt_delay7 =0   ;// Zur PTT-CW verschiebung
uint32_t ptt_delay6 =0   ;// Zur PTT-CW verschiebung
uint32_t ptt_delay5 =0   ;// Zur PTT-CW verschiebung
uint32_t ptt_delay4 =0   ;// Zur PTT-CW verschiebung
uint32_t ptt_delay3 =0   ;// Zur PTT-CW verschiebung
uint32_t ptt_delay2 =0   ;// Zur PTT-CW verschiebung
uint32_t ptt_delay1 =0   ;// Zur PTT-CW verschiebung
//                       FFFFFFFF
uint32_t ptt_del_max = 0x10000000 ; //Grenzwert 32bit

//-----Zähler--------------------------------------------------
int zeit = 0;      // für die Pausenzeit
int i,ii,iii,txt_loop,pausen_z = 0;  // allgemeine Zähler
//---------------------------------

int8_t Merker = 0;  //Merkerbyte
int PKT_m     = 0;  // Merker Punkt 1= nicht gedrückt
int STR_m     = 1;  // Merker Strich
int PTT_m     = 2;  // Merker für Zeichenzähler  4
int CW_del    = 4;  // Merker für CW-Zeichen
 
int8_t Z_z    = 0;  //Zeichenzaehler
int8_t Cod    = 0;  //Codemuster
byte BuCoZ    = 0;  // zur schnelle Ausgabe 170524
int8_t Cw_z   = 0;  // zeichen zum 2.Core
bool cw_out ;


//---------------------------------
  
 
void paddle(){
  if (digitalRead(PadRev)==HIGH){
    if (digitalRead(PKT)==LOW) bitSet(Merker,PKT_m);
    if (digitalRead(STR)==LOW) bitSet(Merker,STR_m);
  }
  if (digitalRead(PadRev)==LOW){
    if (digitalRead(PKT)==LOW) bitSet(Merker,STR_m);
    if (digitalRead(STR)==LOW) bitSet(Merker,PKT_m);
  }
}
//--------------------------------------------------------------------
void Zeichenstart(){
   if(digitalRead(TonOff)== HIGH){tone(M_Ton,Hz);}
   rp2040.fifo.push(1);// push hier für den PTT-Nachlauf  und Cw
}
void Zeichenende(){   
  noTone(M_Ton);digitalWrite(M_Ton, aus);      //Mithörton aus  auch Selbstschwinger  
  rp2040.fifo.push(0);
   
}
//------------------------------------------------------------------------
void Pause(){
  zeit=(analogRead(SpeedPin));
  zeit = 1048-zeit;  //Poti drehrichtung umkehr
 //zeit =2444;// testzweck  ###############
     for(ii =0; ii < zeit;ii++){
//		    paddle();
      delayMicroseconds(33);  //   100/3
        }
         for(ii =0; ii < zeit;ii++){
//		    paddle();
      delayMicroseconds(33);
        }
         for(ii =0; ii < zeit;ii++){
		    paddle();
      delayMicroseconds(33);
        }
} 
//-------------------------------------------------------------------------
void Strich(){
   Zeichenstart();
      Pause(); Pause(); Pause();                       //Strich         
   Zeichenende();
  Pause();                               
 bitClear(Merker,STR_m);                              // immer letze Aktion
 bitClear(Merker,PTT_m); 
}
//------------------------------------------------------------------
void Punkt(){
  Zeichenstart();
      Pause();                                // Punkt
  Zeichenende(); 
  Pause();
 bitClear(Merker,PKT_m);                          // immer letze Aktion Punktmerker löschen
 bitClear(Merker,PTT_m);
}
//-------------------------------------------------------------------

void Junker(){
  if (digitalRead(PKT) == LOW){
   Zeichenstart();}
  while(digitalRead(PKT) == LOW){delayMicroseconds(11);}
  Zeichenende();
   noTone(M_Ton);                                              
}
void tune(){
     Zeichenstart();
  while(digitalRead(Tune) == LOW){delayMicroseconds(11);}
  Zeichenende();
   noTone(M_Ton);                                              
   
}
//---------------------------------
void m_code(int z){
  switch((z)){
          // Zeichen
           case 32 :Pause();Pause();Pause();Pause();Pause();;break;                 //Wortabstand(Space)
           case 40 :Strich();Punkt();Strich(),Strich();Punkt();Pause();break;              // (
           case 41 :Strich();Punkt();Strich(),Strich();Punkt();Strich();Pause();break;     // )
           case 43 :Punkt();Strich();Punkt();Strich();Punkt();Pause();break;               // +
           case 44 :Strich();Strich();Punkt();Punkt();Strich();Strich();Pause();break;     // ,  
           case 45 :Strich();Punkt();Punkt();Punkt();Strich();Pause();break;               // -#######
           case 46 :Punkt();Strich();Punkt();Strich();Punkt();Strich();Pause();break;      // .
           case 47 :Strich();Punkt();Punkt();Strich();Punkt();Pause();break;//             // /
           case 95 :Punkt();Punkt();Strich();Strich();Punkt();Strich();break;       //'_'("..--.-");  break;
           case 34 :Punkt();Strich();Punkt();Punkt();Strich();Punkt();break;        //'"'(".-..-.");  break;
           case 36 :Punkt();Punkt();Punkt();Strich();Punkt();Punkt();Strich();break;//'$'("...-..-"); break;
           case 64 :Punkt();Strich();Strich();Punkt();Strich();Punkt();break;       //'@'(".--.-.");  break;
           case 60 :Punkt();Strich();Punkt();Strich();Punkt();break;                //'<'(".-.-.") // AR
           case 62 :Punkt();Punkt();Punkt();Strich();Punkt();Strich();break;        //'>'("...-.-") // SK
           case 59 :Strich();Punkt();Strich();Punkt();Strich();Punkt();Pause();break;//### // ;
           case 61 :Strich();Punkt();Punkt();Punkt();Strich();Pause();break;//###          // =
           case 63 :Punkt();Punkt();Strich();Strich();Punkt();Punkt();Pause();break;//###  // ?
           // Zahlen
           case 48 :Strich();Strich();Strich();Strich();Strich();Pause();break;            //0 
           case 49 :Punkt();Strich();Strich();Strich();Strich();Pause();break;             //1
           case 50 :Punkt();Punkt();Strich();Strich();Strich();Pause();break;//###         //2
           case 51 :Punkt();Punkt();Punkt();Strich();Strich();Pause();break;//###          //3
           case 52 :Punkt();Punkt();Punkt();Punkt();Strich();Pause();break;//###           //4
           case 53 :Punkt();Punkt();Punkt();Punkt();Punkt();Pause();break;//###            //5
           case 54 :Strich();Punkt();Punkt();Punkt();Punkt();Pause();break;//###           //6
           case 55 :Strich();Strich();Punkt();Punkt();Punkt();Pause();break;//###          //7
           case 56 :Strich();Strich();Strich();Punkt();Punkt();Pause();break;//###         //8
           case 57 :Strich();Strich();Strich();Strich();Punkt();Pause();break;//###        //9
           //Kleinbuchstaben
           case 97  :Punkt();Strich();Pause();break;//###                          //a 
           case 98  :Strich();Punkt();Punkt();Punkt();Pause();break;//###          //b 
           case 99  :Strich();Punkt();Strich();Punkt();Pause();break;//###         //c 
           case 100 :Strich();Punkt();Punkt();Pause();break;//###                  //d 
           case 101 :Punkt();Pause();break;//###                                   //e 
           case 102 :Punkt();Punkt();Strich();Punkt();Pause();break;//###          //f 
           case 103 :Strich();Strich();Punkt();Pause();break;//###                 //g
           case 104 :Punkt();Punkt();Punkt();Punkt();Pause();break;//###           //h  
           case 105 :Punkt();Punkt();Pause();break;//###                           //i 
           case 106 :Punkt();Strich();Strich();Strich();Pause();break;//###        //j
           case 107 :Strich();Punkt();Strich();Pause();break;//###                 //k
           case 108 :Punkt();Strich();Punkt();Punkt();Pause();break;//###          //l 
           case 109 :Strich();Strich();Pause();break;//###                         //m
           case 110 :Strich();Punkt();Pause();break;                               //n  
           case 111 :Strich();Strich();Strich();Pause();break;                     //o 
           case 112 :Punkt();Strich();Strich();Punkt();Pause();break;              //p
           case 113 :Strich();Strich();Punkt();Strich();Pause();break;             //q
           case 114 :Punkt();Strich();Punkt();Pause();break;                       //r
           case 115 :Punkt();Punkt();Punkt();Pause();break;                        //s
           case 116 :Strich();Pause();break;                                       //t
           case 117 :Punkt();Punkt();Strich();Pause();break;                       //u 
           case 118 :Punkt();Punkt();Punkt();Strich();Pause();break;               //v 
           case 119 :Punkt();Strich();Strich();Pause();break;                      //w
           case 120 :Strich();Punkt();Punkt();Strich();Pause();break;              //x
           case 121 :Strich();Punkt();Strich();Strich();Pause();break;             //y
           case 122 :Strich();Strich();Punkt();Punkt();Pause();break;              //z
           //Großbuchstaben
           case 65  :Punkt();Strich();Pause();break;//###                          //A 
           case 66  :Strich();Punkt();Punkt();Punkt();Pause();break;//###          //b 
           case 67  :Strich();Punkt();Strich();Punkt();Pause();break;//###         //c 
           case 68  :Strich();Punkt();Punkt();Pause();break;//###                  //d 
           case 69  :Punkt();Pause();break;//###                                   //e 
           case 70  :Punkt();Punkt();Strich();Punkt();Pause();break;//###          //f 
           case 71  :Strich();Strich();Punkt();Pause();break;//###                 //g
           case 72  :Punkt();Punkt();Punkt();Punkt();Pause();break;//###           //h  
           case 73  :Punkt();Punkt();Pause();break;//###                           //i 
           case 74  :Punkt();Strich();Strich();Strich();Pause();break;//###        //j
           case 75  :Strich();Punkt();Strich();Pause();break;//###                 //k
           case 76  :Punkt();Strich();Punkt();Punkt();Pause();break;//###          //l 
           case 77  :Strich();Strich();Pause();break;//###                         //m
           case 78  :Strich();Punkt();Pause();break;                               //n  
           case 79  :Strich();Strich();Strich();Pause();break;                     //o 
           case 80  :Punkt();Strich();Strich();Punkt();Pause();break;              //p
           case 81  :Strich();Strich();Punkt();Strich();Pause();break;             //q
           case 82  :Punkt();Strich();Punkt();Pause();break;                       //r
           case 83  :Punkt();Punkt();Punkt();Pause();break;                        //s
           case 84  :Strich();Pause();break;                                       //t
           case 85  :Punkt();Punkt();Strich();Pause();break;                       //u 
           case 86  :Punkt();Punkt();Punkt();Strich();Pause();break;               //v 
           case 87  :Punkt();Strich();Strich();Pause();break;                      //w
           case 88  :Strich();Punkt();Punkt();Strich();Pause();break;              //x
           case 89  :Strich();Punkt();Strich();Strich();Pause();break;             //y
           case 90  :Strich();Strich();Punkt();Punkt();Pause();break;              //z
           
           case 35 :;break; //# Textschleife
           case 126 :;break; //~ Textschleife
         //default: Pause();Pause();Bu_a();break;
         default: Pause();Pause();;break;
          }         
}

//-----------------------------------------
void ausgabe(char* text){
  int i;
   txt_loop=0;
   do{
    //Serial.println(text);
    for (i =0;i <(strlen(text)+1);i++){ 
       if ((digitalRead(STR) == LOW)||(digitalRead(PKT) == LOW)) {i= strlen(text) ;txt_loop = 1;txt_loop = 1;}
        z=int(text[i]);
        if((z==35)&&(digitalRead(Cwd25) == HIGH) && (digitalRead(Cwd50) == HIGH) && (digitalRead(Cwd100) == HIGH)){digitalWrite(LedPTT,aus);digitalWrite(PTT,aus);} //#  PPT aus ######
        if (z==126){txt_loop = 1;}                                 //~  Abbruch Dauerschleife
        m_code(z);//Ausgabe
        
          }     
    }while(txt_loop == 0);  //Dauerschleife
    }
//-#######################################################################################
//-#######################################################################################
//-#######################################################################################
void setup() {
  // Core1 
  pinMode(PKT     ,  INPUT_PULLUP);
  pinMode(STR     ,  INPUT_PULLUP);
  pinMode(PadRev  ,  INPUT_PULLUP);
  pinMode(Straight,  INPUT_PULLUP);
  pinMode(TonOff  ,  INPUT_PULLUP);
  pinMode(Tune    ,  INPUT_PULLUP);

  pinMode(Cwd25,   INPUT_PULLUP);
  pinMode(Cwd50,   INPUT_PULLUP);
  pinMode(Cwd100,  INPUT_PULLUP);
  pinMode(Cwdna,   INPUT_PULLUP);

  pinMode(txt1_pin,  INPUT_PULLUP);
  pinMode(txt2_pin,  INPUT_PULLUP);
  pinMode(txt3_pin,  INPUT_PULLUP);
  pinMode(txt4_pin,  INPUT_PULLUP);

  pinMode(M_Ton,     OUTPUT);
  pinMode(Cw_Tx,     OUTPUT);
  pinMode(LedCw,     OUTPUT);
  pinMode(LedPTT,    OUTPUT);
  pinMode(PTT,       OUTPUT);

  pinMode(LED_BUILTIN, OUTPUT);

   Serial.begin(9600);   //########
  
   
   digitalWrite(LED_BUILTIN, HIGH); digitalWrite(LedPTT, an);digitalWrite(LedCw,an); delay(499); digitalWrite(LedCw,aus); digitalWrite(LedPTT, aus);digitalWrite(LED_BUILTIN, LOW);// Lebenszeichen 
   rp2040.fifo.push(0);   //Alle Delay auf 0
   
}
//-#######################################################################################
//-########################-Hauptschleife-################################################
//-#######################################################################################
void loop() {
   // Warten auf String...
   if((digitalRead(Cwd25)==HIGH)&&(digitalRead(Cwd50)==HIGH)&&(digitalRead(Cwd100)==HIGH)){ //für den PTT-Nachlauf
      z_ab = z_ab_2; } else{z_ab = z_ab_1;} // PTT-Nachlauf 
  if (Serial.available()>0) { int thisChar = Serial.read();   m_code(thisChar);  }
 
    if (bitRead(Merker,PKT_m)){ Punkt();}
    if (bitRead(Merker,STR_m)){Strich();}

    if (digitalRead(txt1_pin) == LOW){ausgabe( txt1);  }
    if (digitalRead(txt2_pin) == LOW){ausgabe( txt2);  }
    if (digitalRead(txt3_pin) == LOW){ausgabe( txt3);  }
    if (digitalRead(txt4_pin) == LOW){ausgabe( txt4);  }
     
    if(digitalRead(Straight)==HIGH){ paddle(); }  
    if(digitalRead(Straight)==LOW){ Junker(); }  
    if(digitalRead(Tune)==LOW){ tune(); }  
     
}


//#############################################################################
//                ab hier 2.Core
//################################# 20:März 26 ################################
void setup1() {
   delay(501);
}

void loop1() {

  if (rp2040.fifo.available()>0) { Cw_z = rp2040.fifo.pop();}
  
  if(Cw_z ==1){digitalWrite(LedPTT,an);digitalWrite(PTT,an);;}     //-- geht immer direkt -------------
  if((digitalRead(Cwd25)==HIGH)&&(digitalRead(Cwd50)==HIGH)&&(digitalRead(Cwd100)==HIGH)){
   
  // Ab Hier ohne Verzögerung ---PTT-Nachlauf 448mS ----------------------------
  if(Cw_z ==1){digitalWrite(Cw_Tx,an);digitalWrite(LedCw ,an);}    //--
  if(Cw_z ==0){digitalWrite(Cw_Tx,aus);digitalWrite(LedCw,aus);}   //--
  //-------------------------------------------------------------------
}

  delay(z_ab);//################ Zeichenabfrage  ###################
  // verschieben 7x32 Bit = 224Bit x2mS= 448mS  ????????
  ptt_delay7=(ptt_delay7 <<1);  if((ptt_delay6 >= 0x10000000)){ptt_delay7 =ptt_delay6+1;}
  ptt_delay6=(ptt_delay6 <<1);  if((ptt_delay5 >= 0x10000000)){ptt_delay6 =ptt_delay5+1;}
  ptt_delay5=(ptt_delay5 <<1);  if((ptt_delay4 >= 0x10000000)){ptt_delay5 =ptt_delay4+1;}
  ptt_delay4=(ptt_delay4 <<1);  if((ptt_delay3 >= 0x10000000)){ptt_delay4 =ptt_delay4+1;}
  ptt_delay3=(ptt_delay3 <<1);  if((ptt_delay2 >= 0x10000000)){ptt_delay3 =ptt_delay3+1;}
  ptt_delay2=(ptt_delay2 <<1);  if((ptt_delay1 >= 0x10000000)){ptt_delay2 =ptt_delay2+1;}
  ptt_delay1=(ptt_delay1 <<1);
  ptt_delay1 =ptt_delay1+Cw_z;

  //Cwd50(25)
 if((digitalRead(Cwd25)==LOW)&(digitalRead(Cwd50)==HIGH)&(digitalRead(Cwd100)==HIGH)){//50/2 = 25 Takte
       if (bitRead(ptt_delay1,25) == 1) {digitalWrite(Cw_Tx,HIGH);digitalWrite(LedCw,HIGH);}
       if (bitRead(ptt_delay1,25) == 0) {digitalWrite(Cw_Tx,LOW);digitalWrite(LedCw,LOW);}
    if(digitalRead(Cwdna)==HIGH){   // 100mS Haltezeit  100/2 + 25 =75 -2* 32 =11(ptt_delay3)PTT verzögerung
        ptt_delay3 &= ~(1<<11);}    // löscht das 7.Bit 6Taktr
    if(digitalRead(Cwdna)==LOW){    // 200mS (100 +25 Takte)-3*32  = 29(ptt_delay4) Takte   = 4294967295
       ptt_delay2 &= ~(1<<31);}     // löscht das 31.Bit 31Taktr
    }
  //Cwd100(50)
  if((digitalRead(Cwd25)==HIGH)&(digitalRead(Cwd50)==LOW)&(digitalRead(Cwd100)==HIGH)){//50 Takte 
   if (bitRead(ptt_delay2,18) == 1) {digitalWrite(Cw_Tx,HIGH);digitalWrite(LedCw,HIGH);}
   if (bitRead(ptt_delay2,18) == 0) {digitalWrite(Cw_Tx,aus);digitalWrite(LedCw,aus);}
     //PTT-Nachlauf
    if(digitalRead(Cwdna)==HIGH){   // 100(50)mS   50+50)-3+32  = 4(ptt_delay4) Takte 
          ptt_delay4 &= ~(1<<4);}   // löscht das 18.Bit 6Taktr
    if(digitalRead(Cwdna)==LOW){    //200mS 100 +50 - 4*32  = 22(ptt_delay5) Takte
      ptt_delay5 &= ~(1<<22);}      // löscht das 11.Bit 6Taktr
   }
  //Cwd200(75)
  if((digitalRead(Cwd25)==LOW)&(digitalRead(Cwd50)==LOW)&(digitalRead(Cwd100)==HIGH)){//100 Takte
        if (bitRead(ptt_delay3,4) == 1) {digitalWrite(Cw_Tx,HIGH);digitalWrite(LedCw,HIGH);}
        if (bitRead(ptt_delay3,4) == 0) {digitalWrite(Cw_Tx,aus);digitalWrite(LedCw,aus);}
    if(digitalRead(Cwdna)==HIGH){   // 100ms  50+100 -4*32  = 22(ptt_delay5) Takte 
      ptt_delay5 &= ~(1<<22);}      // löscht Bit
    if(digitalRead(Cwdna)==LOW){    // 200mS 100 +100 -6*32  = 14(ptt_delay7) Takte   = 4294967295
      ptt_delay7 &= ~(1<<14);}      // löscht das Bit 
    }
    //--------------------------------------------------------------
  //Cwd150 (50+100)   
  if((digitalRead(Cwd25)==HIGH)&(digitalRead(Cwd50)==HIGH)&(digitalRead(Cwd100)==LOW)){// 150mS  75 -2*32  =11 Takte  ptt_delay2 
     if (bitRead(ptt_delay3,11) == 1) {digitalWrite(Cw_Tx,HIGH);digitalWrite(LedCw,HIGH);}
     if (bitRead(ptt_delay3,11) == 0) {digitalWrite(Cw_Tx,aus);digitalWrite(LedCw,aus);}
    //PTT-Nachlauf
   if(digitalRead(Cwdna)==HIGH){      // 100mS   50+75-3*32  = 29(ptt_delay4) Takte 
      ptt_delay4  &= ~(1<<29);}       //
    if(digitalRead(Cwdna)==LOW){       //200mS   100+75-5*32 =15(ptt_delay6) Takte  
      ptt_delay6 &= ~(1<<15);} 
   }
 
   //Cwd250 ()   
  if((digitalRead(Cwd25)==LOW)&(digitalRead(Cwd50)==HIGH)&(digitalRead(Cwd100)==LOW)){// 250mS  125 -3*32  =29 Takte  ptt_delay4 
     if (bitRead(ptt_delay4,29) == 1) {digitalWrite(Cw_Tx,HIGH);digitalWrite(LedCw,HIGH);}
     if (bitRead(ptt_delay4,29) == 0) {digitalWrite(Cw_Tx,aus);digitalWrite(LedCw,aus);}
    //PTT-Nachlauf
        if(digitalRead(Cwdna)==HIGH){      // 100mS   50+125-5*32  = 10(ptt_delay6) Takte 
            ptt_delay6  &= ~(1<<10);}       //
        if(digitalRead(Cwdna)==LOW){       //200mS   100+125-6*32 =29(ptt_delay7) Takte  
            ptt_delay7 &= ~(1<<29);} 
   }
   
   // kein neues Zeichen ?? PTT fällt ab wenn keine 1
   if(ptt_delay1 ==0){if(ptt_delay2 ==0){if(ptt_delay3 ==0){
    if(ptt_delay4 ==0){if(ptt_delay5 ==0){if(ptt_delay6 ==0){
      if(ptt_delay7 ==0){
        digitalWrite(LedPTT,aus);digitalWrite(PTT,aus);};};};};};};}

  }




