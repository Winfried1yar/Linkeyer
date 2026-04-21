//------Pinbelegung Core1 Version 4  ----------------------
int PKT      = 14  ;// GPIO   Paddle
int STR      = 15  ;// GPIO   Paddle
int M_Ton   =  17  ;// GPIO    Mithörton Piezo
int Cw_Tx   =  16  ;// GPIO    Morsetaste zum TX
int PTT     =  18  ;// GPIO   PTT zum Tx

int LedCw   =  0   ;// 2mA max
int LedPTT  =  1   ;// 2mA max

int Cwd25    =  2  ;// CW-Delay  25 mS   13 Takte 8192
int Cwd50    =  3  ;// CW-Delay  50 mS   25 Takte 33554432
int Cwd100   =  4  ;// CW-Delay 100 mS   50 Takte (50-32)=262144
int Cwdna    =  5  ;// CW-Nachlauf 25 mS 13 Takte

int txt1_pin = 6   ;// Speichertexte
int txt2_pin = 7   ;// Speichertexte
int txt3_pin = 8   ;// Speichertexte
int txt4_pin = 9   ; // Speichertexte

int Tune      = 10  ; // Dummy
int TonOff    = 11  ; //nur Mithörton
int Straight  = 12  ; //Junkertaste
int PadRev    = 13  ; // Paddle drehen

int SpeedPin = A0;  // AD-Wandler eingang(Poti)
