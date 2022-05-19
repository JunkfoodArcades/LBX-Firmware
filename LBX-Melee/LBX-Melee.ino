// Update 2022-05-19: Updated SOCD behavior and angles

// This code uses Nicohood's Nintendo library  https://github.com/NicoHood/Nintendo
// This code is original but based off of the concepts in Simple's DIY repository (from Jan 6 2017) as well as Crane's GCCPCB repository. Special thanks to both of those peeps!
// https://github.com/SimpleControllers/SimpleControllersBuild-a-Box
// https://github.com/Crane1195/GCCPCB/tree/master/code/GCCPCB2-v1.209 (outdated, he has a new repo called CL-FW)
// Values in the code were observed by flashing Crane's code to an arduino PCB and observing the values in "Visual Controller Test".

// This code is open source and instuctions for routine flashing, remapping and using it's features (switching to a Brook board) as well as a pinout for our LBX will be posted online.
// Modifications to adapt this code for other game types and protocols will be posted in separate files.
// Code that combines all game types and protocols has not been written.
// An open source license in line with other projects in this space is provided.

#include "Nintendo.h"

CGamecubeConsole GamecubeConsole(7); //Sends data to the GC Console on pin 7
Gamecube_Data_t d = defaultGamecubeData;

// Define ints for holding bit vals
  unsigned long longBtnPress = 0; 
  unsigned long longPrevBtnPress = 0;
  unsigned int intLeftRight = 0;
  unsigned int prevIntLeftRight = 0;
  bool lockLeftRight = false;
  int lockoutLeft=0;
  int lockoutRight=0;
  unsigned int intUpDown = 0;
  unsigned int prevIntUpDown = 0;
  bool lockUpDown = false;
  unsigned int intCLeftRight = 0;
  unsigned int prevIntCLeftRight = 0;
  bool lockCLeftRight = false;
  unsigned int intCUpDown = 0;
  unsigned int prevIntCUpDown = 0;
  bool lockCUpDown = false;
  int intDirs = 0;
  int intMods = 0;
  int intCstick = 0;
  int axisX = 128;
  int axisY = 128;
  int cstickX = 128;
  int cstickY = 128;
  int intRLLSMSB = 0;
  int intR = 0;
  int intL = 0;
  bool switchActive = false;

  bool boolLeft = false;
  bool boolDown = false;
  bool boolRight = false;
  bool boolL = false;
  bool boolModX = false;
  bool boolJFAKey = false;
  bool boolModY = false;
  bool boolCDown = false;
  bool boolCLeft = false;
  bool boolA = false;
  bool boolCRight = false;
  bool boolCUp = false;
  bool boolGCUp = false;
  bool boolZ = false;
  bool boolX = false;
  bool boolB = false;
  bool boolR = false;
  bool boolY = false;
  bool boolLS = false;
  bool boolMS = false;
  bool boolStart = false;
  bool boolDPadUp =false;
  bool boolDPadDown =false;
  bool boolDPadLeft =false;
  bool boolDPadRight =false;

// Define ModX and ModY impact on single cardinal directions in 2 arrays. (see spreadsheet tabs 1 & 2)
int defaultCardinal[4] = {208,48,48,208}; // Cardinals
int modXArr[4] = {171,85,75,181}; // Mod X
int modYArr[4] = {187,69,101,155}; // Mod Y

// Define angle values in 4 arrays. (see spreadsheet tabs 1 & 2)
int angleArr1[31] = {103,69,99,72,89,65,87,72,86,67,73,98,60,73,98,60,73,98,69,73,98,69,69,92,58,86,60,85,64,85,71};                                // Down
int angleArr2[31] = {69,103,72,99,65,89,72,87,79,77,72,77,88,72,77,88,72,77,103,72,77,103,103,58,92,60,86,69,81,77,81};                             // Left
int angleArr3[31] = {153,187,157,184,167,191,169,184,170,189,183,158,196,183,158,196,183,158,187,183,158,187,187,164,198,170,196,171,192,171,185};  // UP
int angleArr4[31] = {187,153,184,157,191,167,184,169,177,179,184,179,168,184,179,168,184,179,153,184,179,153,153,198,164,196,170,187,175,179,175};  // Right

// Define special values in 5 arrays. (see spreadsheet tabs 1 & 2)  
int specialArr4[4] = {187,69,48,208};   // ModY+B
int specialArr6[2] = {228,28};          // Cstick array

void setup() {
// Define inputs & outputs
  pinMode(15, INPUT_PULLUP); 
  pinMode(16, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
  pinMode(11, INPUT_PULLUP);
  pinMode(03, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);
  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(13, INPUT_PULLUP);
  pinMode(A0, INPUT_PULLUP);
  pinMode(A1, INPUT_PULLUP);
  pinMode(A2, INPUT_PULLUP);
  pinMode(A3, INPUT_PULLUP);
  pinMode(A5, INPUT_PULLUP);
  pinMode(A4, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(17, OUTPUT);

// Enable Brook Mode by holding A and ModX. Sets MUX high or low - default is low
  if ((digitalRead(3) == LOW) && (digitalRead(12) == LOW)){
    switchActive = true; 
    digitalWrite(A4, HIGH); // Brook board enabled
    pinMode(17, OUTPUT);    //Send UP to Brook with pin 17
  }
}

void loop() {
//read pins & set longBtnPress (which is a binary map of all 21 inputs)
//Example: 00000000000010000001

    //reverse polarity here so that all bool operators are inverted. Original pin purpose is commented on the right. 
    //You can remap here by replacing bool(old purpose) with bool(new purpose). Be sure to keep the names correctly formatted. We will have a video if thi sis confusing.     
    boolLeft    = !((PINB & (1 << 1)) >> 1); // PINB1 Left
    boolDown    = !((PINB & (1 << 2)) >> 2); // PINB2 Down
    boolRight   = !((PINB & (1 << 3)) >> 3); // PINB3 Right
    boolL       = !((PINB & (1 << 7)) >> 7); // PINB7 L
    boolModX    = !((PIND & (1 << 0)) >> 0); // PIND0 ModX
    boolJFAKey  = !((PIND & (1 << 1)) >> 1); // PIND1 JFAKey
    boolModY    = !((PIND & (1 << 2)) >> 2); // PIND2 ModY
    boolCDown   = !((PIND & (1 << 3)) >> 3); // PIND3 CDown
    boolCLeft   = !((PIND & (1 << 4)) >> 4); // PIND4 CLeft
    boolA       = !((PIND & (1 << 6)) >> 6); // PIND6 A
    boolCRight  = !((PIND & (1 << 7)) >> 7); // PIND7 CRight
    boolCUp     = !((PINB & (1 << 4)) >> 4); // PINB4 CUp
    boolGCUp    = !((PINB & (1 << 5)) >> 5); // PINB5 GCUp
    boolZ       = !((PINB & (1 << 6)) >> 6); // PINB6 Z
    boolX       = !((PINC & (1 << 6)) >> 6); // PINC6 X
    boolB       = !((PINC & (1 << 7)) >> 7); // PINC7 B
    boolR       = !((PINF & (1 << 7)) >> 7); // PINF7 R
    boolY       = !((PINF & (1 << 6)) >> 6); // PINF6 Y
    boolLS      = !((PINF & (1 << 5)) >> 5); // PINF5 LS
    boolMS      = !((PINF & (1 << 4)) >> 4); // PINF4 MS
    boolStart   = !((PINF & (1 << 0)) >> 0); // PINF0 Start

    longBtnPress = 0;
    if (boolLeft)   {bitSet(longBtnPress, 0);}
    if (boolDown)   {bitSet(longBtnPress, 1);}
    if (boolRight)  {bitSet(longBtnPress, 2);}
    if (boolL)      {bitSet(longBtnPress, 3);}
    if (boolModX)   {bitSet(longBtnPress, 4);}
    if (boolJFAKey) {bitSet(longBtnPress, 5);}
    if (boolModY)   {bitSet(longBtnPress, 6);}
    if (boolCDown)  {bitSet(longBtnPress, 7);}
    if (boolCLeft)  {bitSet(longBtnPress, 8);}
    if (boolA)      {bitSet(longBtnPress, 9);}
    if (boolCRight) {bitSet(longBtnPress, 10);}
    if (boolCUp)    {bitSet(longBtnPress, 11);}
    if (boolGCUp)   {bitSet(longBtnPress, 12);}
    if (boolZ)      {bitSet(longBtnPress, 13);}
    if (boolX)      {bitSet(longBtnPress, 14);}
    if (boolB)      {bitSet(longBtnPress, 15);}
    if (boolR)      {bitSet(longBtnPress, 16);}
    if (boolY)      {bitSet(longBtnPress, 17);}
    if (boolLS)     {bitSet(longBtnPress, 18);}
    if (boolMS)     {bitSet(longBtnPress, 19);}
    if (boolStart)  {bitSet(longBtnPress, 20);}
    
    //If brook mode = yes then skip everything and call brook function
    if(switchActive==true){evalBrookMode();}
    else if (switchActive==false){

  // ********If intBtnPress = 0 then reset vars from prev loops start a new loop looking for inputs again
  if(longBtnPress == 0){
    longPrevBtnPress = 0;
    intLeftRight = 0;
    prevIntLeftRight = 0;
    lockLeftRight = false;
    intUpDown = 0;
    prevIntUpDown = 0;
    lockUpDown = false;
    intCLeftRight = 0;
    prevIntCLeftRight = 0;
    lockCLeftRight = false;
    intCUpDown = 0;
    prevIntCUpDown = 0;
    lockCUpDown = false;
    intDirs = 0;
    intMods = 0;
    intCstick = 0;
    axisX = 128;
    axisY = 128;
    cstickX = 128;
    cstickY = 128;
    intR = 0;
    intL = 0;
  }

      // button changes need to be processed. Set longPrevBtnPress and continue
      longPrevBtnPress = longBtnPress;
      
      //Zero out analog vals here?
        intR=0;
        intL=0;
        axisY=128;
        axisX=128;
        cstickY=128;
        cstickX=128;
        
      //Handle L, R, LS, MS
      if((boolL) || (boolR) || (boolLS) || (boolMS)){
        if(boolL){intL=170;}
        if(boolR){intR=170;}
        else if(boolMS){intR=125;}
        else if(boolLS){intR=80;}
        }
      
        scrubDirSOCD();
      
      scrubCStickSOCD();
        
      if (boolJFAKey){scrubDpad();}
      else{
        d.report.dup    = 0;
        d.report.ddown  = 0;
        d.report.dleft  = 0;
        d.report.dright = 0;
        }
        
      scrubAngles();
 
  d.report.a = boolA;
  d.report.b = boolB;
  d.report.x = boolX;
  d.report.y = boolY;
  d.report.z = boolZ;
  d.report.start = boolStart;
  d.report.r = boolR;
  d.report.l = boolL;
  d.report.xAxis  = axisX;
  d.report.yAxis  = axisY;
  d.report.cxAxis   = cstickX;
  d.report.cyAxis   = cstickY;
  d.report.right = intR;
  d.report.left = intL;
  GamecubeConsole.write(d);
  }

}

// Scrub directional SOCD
void scrubDirSOCD() {
  //if no common SOCD dirs are held, set dir input tracking vars to 0
  if ((boolLeft==0) && (boolRight==0)){intLeftRight = 0;prevIntLeftRight = 0;lockLeftRight=false;}
  
    intLeftRight=0;
    if(boolLeft){bitSet(intLeftRight,0);}
    if(boolRight){bitSet(intLeftRight,1);}
    
    //Set prevIntLeftRight
    if((prevIntLeftRight==0) && ((intLeftRight>0) && (intLeftRight<3))){prevIntLeftRight = intLeftRight;}
    
    //2ip no reactivate (melee)
    // Must check for cleaning having been performed at least once. Crane uses a Lockout var. That's the approach here as well.
    if((prevIntLeftRight==intLeftRight) && (lockLeftRight == true)){intLeftRight=0;}
    else if((prevIntLeftRight>0) && (intLeftRight<3) && (prevIntLeftRight!=intLeftRight)){prevIntLeftRight=0;lockLeftRight=false;} //If they hold the seccond button and release the first, update prevIntLeftRight to match.
    else if(prevIntLeftRight>0 && intLeftRight==3){intLeftRight = (intLeftRight ^ prevIntLeftRight); lockLeftRight=true;} // SOCD, 2ip no reactivate. Can't subtract a larger number from a smaller number and stil have a positive int. Do a bit comparison using XOR instead.
      
  if ((boolGCUp==0) && (boolDown==0)){intUpDown = 0; prevIntUpDown = 0;lockUpDown=false;}
  //Check up and down
    intUpDown=0;
    if(boolDown){bitSet(intUpDown,0);}
    if(boolGCUp){bitSet(intUpDown,1);}
    //Set prevIntUpDown    
    if((prevIntUpDown == 0) && ((intUpDown>0) && (intUpDown<3))){prevIntUpDown = intUpDown;}
    //2ip no reactivate (melee)
    // Must check for cleaning having been performed at least once. Crane uses a Lockout var. That's the approach here as well.
    if((prevIntUpDown==intUpDown) && (lockUpDown == true)){intUpDown=0;}
    else if((prevIntUpDown>0) && (intUpDown<3) && (prevIntUpDown!=intUpDown)){prevIntUpDown=0;lockUpDown=false;} // If they hold the seccond button and release the first, update prevIntLeftRight to match.   
    else if(prevIntUpDown>0 && intUpDown==3){intUpDown = (intUpDown ^ prevIntUpDown); lockUpDown=true;} // SOCD, 2ip no reactivate. Can't subtract a larger number from a smaller number and stil have a positive int. Do a bit comparison using XOR instead.
}

void scrubCStickSOCD() {
  //if no common SOCD dirs are held, set dir input tracking vars to 0
  if ((boolCLeft==0) && (boolCRight==0)){lockCLeftRight=false; intCLeftRight=0; prevIntCLeftRight=0; cstickX=128;}
  //Check Cleft and Cright
    intCLeftRight=0;
    if(boolCLeft){bitSet(intCLeftRight,0);}
    if(boolCRight){bitSet(intCLeftRight,1);}
    //Set prevIntCLeftRight
    if((prevIntCLeftRight==0) && ((intCLeftRight>0) && (intCLeftRight<3))){prevIntCLeftRight = intCLeftRight;}
    //2ip no reactivate (melee)
    // Must check for cleaning having been performed at least once. Crane uses a Lockout var. That's the approach here as well.
    if((prevIntCLeftRight==intCLeftRight) && (lockCLeftRight == true)){intCLeftRight=0;}
    else if((prevIntCLeftRight>0) && (intCLeftRight<3) && (prevIntCLeftRight!=intCLeftRight)){prevIntCLeftRight=0;lockCLeftRight=false;} //If they hold the seccond button and release the first, update prevIntLeftRight to match.
    else if(prevIntCLeftRight>0 && intCLeftRight==3){intCLeftRight = (intCLeftRight ^ prevIntCLeftRight); lockCLeftRight=true;} // SOCD, 2ip no reactivate. Can't subtract a larger number from a smaller number and stil have a positive int. Do a bit comparison using XOR instead.
    if (intCLeftRight==0){cstickX=128;}
    if (intCLeftRight==1){cstickX=28;}
    if (intCLeftRight==2){cstickX=228;}
  
  if ((boolCUp==0) && (boolCDown==0)){lockCUpDown=false; intCUpDown = 0; prevIntCUpDown = 0; cstickY=128;}
  //Check up and down  
    intCUpDown=0;
    if(boolCDown){bitSet(intCUpDown,0);}
    if(boolCUp){bitSet(intCUpDown,1);}
    //Set prevIntUpDown    
    if((prevIntCUpDown == 0) && ((intCUpDown>0) && (intCUpDown<3))){prevIntCUpDown = intCUpDown;}
    //2ip no reactivate (melee)    
    // Must check for cleaning having been performed at least once. Crane uses a Lockout var. That's the approach here as well.
    if((prevIntCUpDown==intCUpDown) && (lockCUpDown == true)){intCUpDown=0;}
    else if((prevIntCUpDown>0) && (intCUpDown<3) && (prevIntCUpDown!=intCUpDown)){prevIntCUpDown=0;lockCUpDown=false;} // If they hold the seccond button and release the first, update prevIntLeftRight to match.   
    else if(prevIntCUpDown>0 && intCUpDown==3){intCUpDown = (intCUpDown ^ prevIntCUpDown); lockCUpDown=true;} // SOCD, 2ip no reactivate. Can't subtract a larger number from a smaller number and stil have a positive int. Do a bit comparison using XOR instead.
    if (intCUpDown==0){cstickY=128;}
    if (intCUpDown==1){cstickY=28;}
    if (intCUpDown==2){cstickY=228;}

}

void scrubDpad() {
  //if JFAKEY then CSTICK ==0 and Cstick inputs become up down left and right for DPAD  
    boolDPadDown  = bitRead(intCUpDown,0);
    boolDPadUp    = bitRead(intCUpDown,1);
    boolDPadLeft  = bitRead(intCLeftRight,0);
    boolDPadRight = bitRead(intCLeftRight,1);
    //Set GCC values
    d.report.dup    = boolDPadUp;
    d.report.ddown  = boolDPadDown;
    d.report.dleft  = boolDPadLeft;
    d.report.dright = boolDPadRight;
    //zero out Cstick vars and GCC values
    intCUpDown = 0;
    prevIntCUpDown = 0;
    intCLeftRight = 0;
    prevIntCLeftRight = 0;
    cstickX = 128;
    cstickY = 128;
}

void scrubAngles() {
  //intLeftRight, intUpDown and intCUpDown and intCLeftRight has been scrubbed by SOCD. There should only be 2 active opposing cardinal inputs.
  intDirs = 0;
  intDirs += (intLeftRight*1);    // bits 0 and 1 = Left & Right
  intDirs += (intUpDown * 4);     // bits 2 and 3 = Down & Up

  intMods = 0;
  intMods += (boolModX * 1);  // bit 0 = MODX  
  intMods += (boolModY * 2);  // bit 1 = MODY
  
  intCstick = 0;
  intCstick += (intCLeftRight*1); // bits 0 and 1 = CLeft & CRight  
  intCstick += (intCUpDown * 4);  // bits 2 and 3 = CDown & CUp

  intRLLSMSB = 0;
  intRLLSMSB += (boolR*1);
  intRLLSMSB += (boolL*2);
  intRLLSMSB += (boolLS*4);
  intRLLSMSB += (boolMS*8);
  // can do an if statement to override the value

  switch (intDirs) {
      // Left
      case 1: 
        axisY=128;axisX=48;
        if(intMods==0 && intCstick==0 && boolL==0 && boolR==0 && boolLS==0 && boolMS==0){axisX=48;} // Left
        else if((intMods==2) && (boolB)){axisX=specialArr4[2];}                         // Left and ModY+B
        else if(intMods==1 && intCstick==0 && intRLLSMSB==0){axisX=modXArr[2];}         // Left and ModX
        else if(intMods==2 && intCstick==0 && intRLLSMSB==0){axisX=modYArr[2];}         // Left and ModY
        else if((intMods==1) && (intCstick>0) && (intRLLSMSB==0)){axisX=modXArr[2];}    // Left and ModX and Cstick
        else if((intMods==2) && (intCstick>0) && (intRLLSMSB==0)){axisX=modYArr[2];}    // Left and ModY and Cstick
        else {axisX=48;}
        break;

      // Right
      case 2:
        axisY=128;axisX=208;
        if(intMods==0 && intCstick==0 && boolL==0 && boolR==0 && boolLS==0 && boolMS==0){axisX=208;} // Right
        else if((intMods==2) && (boolB)){axisX=specialArr4[3];}                         // Right and ModY+B 
        else if(intMods==1 && intCstick==0 && intRLLSMSB==0){axisX=modXArr[3];}         // Right and ModX
        else if(intMods==2 && intCstick==0 && intRLLSMSB==0){axisX=modYArr[3];}         // Right and ModY
        else if((intMods==1) && (intCstick>0) && (intRLLSMSB==0)){axisX=modXArr[3];}    // Right and ModX and Cstick
        else if((intMods==2) && (intCstick>0) && (intRLLSMSB==0)){axisX=modYArr[3];}    // Right and ModY and Cstick
        else {axisX=208;}  
        break;

      // Down
      case 4:
        axisX=128;axisY=48;
        if(intMods==0 && intCstick==0 && boolL==0 && boolR==0 && boolLS==0 && boolMS==0){axisY=48;} // Down
        else if(intMods==1 && intCstick==0 && intRLLSMSB==0){axisY=modXArr[1];}         // Down and ModX
        else if(intMods==2 && intCstick==0 && intRLLSMSB==0){axisY=modYArr[1];}         // Down and ModY
        else if((intMods==1) && (intCstick>0) && (intRLLSMSB==0)){axisY=modXArr[1];}    // Down and ModX and Cstick
        else if((intMods==2) && (intCstick>0) && (intRLLSMSB==0)){axisY=modYArr[1];}    // Down and ModY and Cstick
        else {axisY=48;}
        break;      

      // Down & Left
      case 5:
        // Lots of conditions
        axisY=72;axisX=72;
        if(intMods==0 && intCstick==0 && boolR==0 && boolL==0 && boolLS==0 && boolMS==0){axisY=72;axisX=72;}                // Up Left
        else if(intMods==1 && intCstick==0 && intRLLSMSB==0){axisY=angleArr1[0];axisX=angleArr2[0];}                                    // Down Left and ModX
        else if(intMods>1 && intMods<=3 && intCstick==0 && intRLLSMSB==0){axisY=angleArr1[1];axisX=angleArr2[1];}                       // Down Left and ModY
        else if((boolB) && (intMods==1) && (intCstick==1) && (intRLLSMSB==0)){axisY=angleArr1[25];axisX=angleArr2[25];}                              // Down Left and B and ModX and CL (25)
        else if((boolB) && (intMods>1 && intMods<=3) && (intCstick==1) && (intRLLSMSB==0)){axisY=angleArr1[26];axisX=angleArr2[26];}                 // Down Left and B and ModY and CL (26)
        else if((boolB) && (intMods==1) && (intCstick>=2 && intCstick<4) && (intRLLSMSB==0)){axisY=angleArr1[29];axisX=angleArr2[29];}               // Down Left and B and ModX and CR (29)
        else if((boolB) && (intMods>1 && intMods<=3) && (intCstick>=2 && intCstick<4) && (intRLLSMSB==0)){axisY=angleArr1[30];axisX=angleArr2[30];}  // Down Left and B and ModY and CR (30)
        else if((boolB) && (intMods==1) && (intCstick>=4 && intCstick<8) && (intRLLSMSB==0)){axisY=angleArr1[23];axisX=angleArr2[23];}               // Down Left and B and ModX and CD (23)
        else if((boolB) && (intMods>1 && intMods<=3) && (intCstick>=4 && intCstick<8) && (intRLLSMSB==0)){axisY=angleArr1[24];axisX=angleArr2[24];}  // Down Left and B and ModY and CD (24)
        else if((boolB) && (intMods==1) && (intCstick>=8) && (intRLLSMSB==0)){axisY=angleArr1[27];axisX=angleArr2[27];}                              // Down Left and B and ModX and CU (27) cstickX=128;
        else if((boolB) && (intMods>1 && intMods<=3) && (intCstick>=8) && (intRLLSMSB==0)){axisY=angleArr1[28];axisX=angleArr2[28];}                 // Down Left and B and ModY and CU (28)
        else if((intMods==1) && (intCstick==1) && (intRLLSMSB==0)){axisY=angleArr1[4];axisX=angleArr2[4];}                              // Down Left and ModX and CL
        else if((intMods>1 && intMods<=3) && (intCstick==1) && (intRLLSMSB==0)){axisY=angleArr1[5];axisX=angleArr2[5];}                 // Down Left and ModY and CL
        else if((intMods==1) && (intCstick>=2 && intCstick<4) && (intRLLSMSB==0)){axisY=angleArr1[8];axisX=angleArr2[8];}               // Down Left and ModX and CR
        else if((intMods>1 && intMods<=3) && (intCstick>=2 && intCstick<4) && (intRLLSMSB==0)){axisY=angleArr1[9];axisX=angleArr2[9];}  // Down Left and ModY and CR
        else if((intMods==1) && (intCstick>=4 && intCstick<8) && (intRLLSMSB==0)){axisY=angleArr1[2];axisX=angleArr2[2];}               // Down Left and ModX and CD
        else if((intMods>1 && intMods<=3) && (intCstick>=4 && intCstick<8) && (intRLLSMSB==0)){axisY=angleArr1[3];axisX=angleArr2[3];}  // Down Left and ModY and CD
        else if((intMods==1) && (intCstick>=8) && (intRLLSMSB==0)){axisY=angleArr1[6];axisX=angleArr2[6];cstickX=128;}                  // Down Left and ModX and CU
        else if((intMods>1 && intMods<=3) && (intCstick>=8) && (intRLLSMSB==0)){axisY=angleArr1[7];axisX=angleArr2[7];}                 // Down Left and ModY and CU
        else if((intMods==0) && (boolR)){axisY=angleArr1[13];axisX=angleArr2[13];intR=170;}                         // Down Left and R
        else if((intMods==1) && (boolR)){axisY=angleArr1[14];axisX=angleArr2[14];intR=170;}                         // Down Left and ModX+R
        else if((intMods>1 && intMods<=3) && (boolR)){axisY=angleArr1[15];axisX=angleArr2[15];intR=170;}            // Down Left and ModY+R
        else if((intMods==0) && (boolL)){axisY=angleArr1[10];axisX=angleArr2[10];intL=170;}                         // Down Left and L
        else if((intMods==1) && (boolL)){axisY=angleArr1[11];axisX=angleArr2[11];intL=170;}                         // Down Left and ModX+L
        else if((intMods>1 && intMods<=3) && (boolL)){axisY=angleArr1[12];axisX=angleArr2[12];intL=140;}            // Down Left and ModY+L
        else if((intMods==0) && (boolLS)){axisY=angleArr1[16];axisX=angleArr2[16];intR=80;}                         // Down Left and LS
        else if((intMods==1) && (boolLS)){axisY=angleArr1[17];axisX=angleArr2[17];intR=80;}                         // Down Left and ModX+LS
        else if((intMods>1 && intMods<=3) && (boolLS)){axisY=angleArr1[18];axisX=angleArr2[18];intR=80;}            // Down Left and ModY+LS
        else if((intMods==0) && (boolMS)){axisY=angleArr1[19];axisX=angleArr2[19];intR=125;}                        // Down Left and MS
        else if((intMods==1) && (boolMS)){axisY=angleArr1[20];axisX=angleArr2[20];intR=125;}                        // Down Left and ModX+MS
        else if((intMods>1 && intMods<=3) && (boolMS)){axisY=angleArr1[21];axisX=angleArr2[21];intR=125;}           // Down Left and ModY+MS 
        else{axisY=72;axisX=72;}
        break;

      // Down & Right
      case 6: 
        // Lots of conditions
        axisY=72;axisX=184;
        if(intMods==0 && intCstick==0 && boolL==0 && boolR==0 && boolLS==0 && boolMS==0){axisY=72;axisX=184;}               // Up Left
        else if(intMods==1 && intCstick==0 && intRLLSMSB==0){axisY=angleArr1[0];axisX=angleArr4[0];}                                    // Down Right and ModX
        else if(intMods>1 && intMods<=3 && intCstick==0 && intRLLSMSB==0){axisY=angleArr1[1];axisX=angleArr4[1];}                       // Down Right and ModY
        else if((boolB) && (intMods==1) && (intCstick==1) && (intRLLSMSB==0)){axisY=angleArr1[25];axisX=angleArr4[25];}                              // Down Right and ModX and CL
        else if((boolB) && (intMods>1 && intMods<=3) && (intCstick==1) && (intRLLSMSB==0)){axisY=angleArr1[26];axisX=angleArr4[26];}                 // Down Right and ModY and CL
        else if((boolB) && (intMods==1) && (intCstick>=2 && intCstick<4) && (intRLLSMSB==0)){axisY=angleArr1[29];axisX=angleArr4[29];}               // Down Right and ModX and CR
        else if((boolB) && (intMods>1 && intMods<=3) && (intCstick>=2 && intCstick<4) && (intRLLSMSB==0)){axisY=angleArr1[30];axisX=angleArr4[30];}  // Down Right and ModY and CR
        else if((boolB) && (intMods==1) && (intCstick>=4 && intCstick<8) && (intRLLSMSB==0)){axisY=angleArr1[23];axisX=angleArr4[23];}               // Down Right and ModX and CD
        else if((boolB) && (intMods>1 && intMods<=3) && (intCstick>=4 && intCstick<8) && (intRLLSMSB==0)){axisY=angleArr1[24];axisX=angleArr4[24];}  // Down Right and ModY and CD
        else if((boolB) && (intMods==1) && (intCstick>=8) && (intRLLSMSB==0)){axisY=angleArr1[27];axisX=angleArr4[27];}                              // Down Right and ModX and CU
        else if((boolB) && (intMods>1 && intMods<=3) && (intCstick>=8) && (intRLLSMSB==0)){axisY=angleArr1[28];axisX=angleArr4[28];}                 // Down Right and ModY and CU
        else if((intMods==1) && (intCstick==1) && (intRLLSMSB==0)){axisY=angleArr1[4];axisX=angleArr4[4];}                              // Down Right and ModX and CL
        else if((intMods>1 && intMods<=3) && (intCstick==1) && (intRLLSMSB==0)){axisY=angleArr1[5];axisX=angleArr4[5];}                 // Down Right and ModY and CL
        else if((intMods==1) && (intCstick>=2 && intCstick<4) && (intRLLSMSB==0)){axisY=angleArr1[8];axisX=angleArr4[8];}               // Down Right and ModX and CR
        else if((intMods>1 && intMods<=3) && (intCstick>=2 && intCstick<4) && (intRLLSMSB==0)){axisY=angleArr1[9];axisX=angleArr4[9];}  // Down Right and ModY and CR
        else if((intMods==1) && (intCstick>=4 && intCstick<8) && (intRLLSMSB==0)){axisY=angleArr1[2];axisX=angleArr4[2];}               // Down Right and ModX and CD
        else if((intMods>1 && intMods<=3) && (intCstick>=4 && intCstick<8) && (intRLLSMSB==0)){axisY=angleArr1[3];axisX=angleArr4[3];}  // Down Right and ModY and CD
        else if((intMods==1) && (intCstick>=8) && (intRLLSMSB==0)){axisY=angleArr1[6];axisX=angleArr4[6];}                              // Down Right and ModX and CU
        else if((intMods>1 && intMods<=3) && (intCstick>=8) && (intRLLSMSB==0)){axisY=angleArr1[7];axisX=angleArr4[7];}                 // Down Right and ModY and CU
        else if((intMods==0) && (boolR)){axisY=angleArr1[13];axisX=angleArr4[13];intR=170;}                         // Down Right and R
        else if((intMods==1) && (boolR)){axisY=angleArr1[14];axisX=angleArr4[14];intR=170;}                         // Down Right and ModX+R
        else if((intMods>1 && intMods<=3) && (boolR)){axisY=angleArr1[15];axisX=angleArr4[15];intR=170;}            // Down Right and ModY+R
        else if((intMods==0) && (boolL)){axisY=angleArr1[10];axisX=angleArr4[10];intL=170;}                         // Down Right and L
        else if((intMods==1) && (boolL)){axisY=angleArr1[11];axisX=angleArr4[11];intL=170;}                         // Down Right and ModX+L
        else if((intMods>1 && intMods<=3) && (boolL)){axisY=angleArr1[12];axisX=angleArr4[12];intL=170;}            // Down Right and ModY+L
        else if((intMods==0) && (boolLS)){axisY=angleArr1[16];axisX=angleArr4[16];intR=80;}                         // Down Right and LS
        else if((intMods==1) && (boolLS)){axisY=angleArr1[17];axisX=angleArr4[17];intR=80;}                         // Down Right and ModX+LS
        else if((intMods>1 && intMods<=3) && (boolLS)){axisY=angleArr1[18];axisX=angleArr4[18];intR=80;}            // Down Right and ModY+LS
        else if((intMods==0) && (boolMS)){axisY=angleArr1[19];axisX=angleArr4[19];intR=125;}                        // Down Right and MS
        else if((intMods==1) && (boolMS)){axisY=angleArr1[20];axisX=angleArr4[20];intR=125;}                        // Down Right and ModX+MS
        else if((intMods>1 && intMods<=3) && (boolMS)){axisY=angleArr1[21];axisX=angleArr4[21];intR=125;}           // Down Right and ModY+MS
        else{axisY=72;axisX=184;}
        break;    

      // Up
      case 8:
        axisX=128;axisY=208;
        if(intMods==0 && intCstick==0 && boolL==0 && boolR==0 && boolLS==0 && boolMS==0){axisY=208;} // Up
        else if(intMods==1 && intCstick==0 && intRLLSMSB==0){axisY=modXArr[0];}         // Up and ModX
        else if(intMods==2 && intCstick==0 && intRLLSMSB==0){axisY=modYArr[0];}         // Up and ModY
        else if((intMods==1) && (intCstick>0) && (intRLLSMSB==0)){axisY=modXArr[0];}    // Up and ModX and Cstick
        else if((intMods==2) && (intCstick>0) && (intRLLSMSB==0)){axisY=modYArr[0];}    // Up and ModY and Cstick
        else{axisY=208;}
        break;

      // Up & Left
      case 9: 
        // Lots of conditions
        axisY=184;axisX=72;
        if(intMods==0 && intCstick==0 && boolL==0 && boolR==0 && boolLS==0 && boolMS==0){axisY=184;axisX=72;}               // Up Left
        else if(intMods==1 && intCstick==0 && intRLLSMSB==0){axisY=angleArr3[0];axisX=angleArr2[0];}                                    // Up Left and ModX
        else if(intMods>1 && intMods<=3 && intCstick==0 && intRLLSMSB==0){axisY=angleArr3[1];axisX=angleArr2[1];}                       // Up Left and ModY
        else if((boolB) && (intMods==1) && (intCstick==1) && (intRLLSMSB==0)){axisY=angleArr3[25];axisX=angleArr2[25];}                              // Up Left and ModX and CL
        else if((boolB) && (intMods>1 && intMods<=3) && (intCstick==1) && (intRLLSMSB==0)){axisY=angleArr3[26];axisX=angleArr2[26];}                 // Up Left and ModY and CL
        else if((boolB) && (intMods==1) && (intCstick>=2 && intCstick<4) && (intRLLSMSB==0)){axisY=angleArr3[29];axisX=angleArr2[29];}               // Up Left and ModX and CR
        else if((boolB) && (intMods>1 && intMods<=3) && (intCstick>=2 && intCstick<4) && (intRLLSMSB==0)){axisY=angleArr3[30];axisX=angleArr2[30];}  // Up Left and ModY and CR
        else if((boolB) && (intMods==1) && (intCstick>=4 && intCstick<8) && (intRLLSMSB==0)){axisY=angleArr3[23];axisX=angleArr2[23];}               // Up Left and ModX and CD
        else if((boolB) && (intMods>1 && intMods<=3) && (intCstick>=4 && intCstick<8) && (intRLLSMSB==0)){axisY=angleArr3[24];axisX=angleArr2[24];}  // Up Left and ModY and CD
        else if((boolB) && (intMods==1) && (intCstick>=8) && (intRLLSMSB==0)){axisY=angleArr3[27];axisX=angleArr2[27];}                              // Up Left and ModX and CU
        else if((boolB) && (intMods>1 && intMods<=3) && (intCstick>=8) && (intRLLSMSB==0)){axisY=angleArr3[28];axisX=angleArr2[28];}                 // Up Left and ModY and CU
        else if((intMods==1) && (intCstick==1) && (intRLLSMSB==0)){axisY=angleArr3[4];axisX=angleArr2[4];}                              // Up Left and ModX and CL
        else if((intMods>1 && intMods<=3) && (intCstick==1) && (intRLLSMSB==0)){axisY=angleArr3[5];axisX=angleArr2[5];}                 // Up Left and ModY and CL
        else if((intMods==1) && (intCstick>=2 && intCstick<4) && (intRLLSMSB==0)){axisY=angleArr3[8];axisX=angleArr2[8];}               // Up Left and ModX and CR
        else if((intMods>1 && intMods<=3) && (intCstick>=2 && intCstick<4) && (intRLLSMSB==0)){axisY=angleArr3[9];axisX=angleArr2[9];}  // Up Left and ModY and CR
        else if((intMods==1) && (intCstick>=4 && intCstick<8) && (intRLLSMSB==0)){axisY=angleArr3[2];axisX=angleArr2[2];}               // Up Left and ModX and CD
        else if((intMods>1 && intMods<=3) && (intCstick>=4 && intCstick<8) && (intRLLSMSB==0)){axisY=angleArr3[3];axisX=angleArr2[3];}  // Up Left and ModY and CD
        else if((intMods==1) && (intCstick>=8) && (intRLLSMSB==0)){axisY=angleArr3[6];axisX=angleArr2[6];}                              // Up Left and ModX and CU
        else if((intMods>1 && intMods<=3) && (intCstick>=8) && (intRLLSMSB==0)){axisY=angleArr3[7];axisX=angleArr2[7];}                 // Up Left and ModY and CU
        else if((intMods==0) && (boolR)){axisY=angleArr3[13];axisX=angleArr2[13];intR=170;}                         // Up Left and R
        else if((intMods==1) && (boolR)){axisY=angleArr3[14];axisX=angleArr2[14];intR=170;}                         // Up Left and ModX+R
        else if((intMods>1 && intMods<=3) && (boolR)){axisY=angleArr3[15];axisX=angleArr2[15];intR=170;}            // Up Left and ModY+R
        else if((intMods==0) && (boolL)){axisY=angleArr3[10];axisX=angleArr2[10];intL=170;}                         // Up Left and L
        else if((intMods==1) && (boolL)){axisY=angleArr3[11];axisX=angleArr2[11];intL=170;}                         // Up Left and ModX+L
        else if((intMods>1 && intMods<=3) && (boolL)){axisY=angleArr3[12];axisX=angleArr2[12];intL=170;}            // Up Left and ModY+L
        else if((intMods==0) && (boolLS)){axisY=angleArr3[16];axisX=angleArr2[16];intR=80;}                         // Up Left and LS
        else if((intMods==1) && (boolLS)){axisY=angleArr3[17];axisX=angleArr2[17];intR=80;}                         // Up Left and ModX+LS
        else if((intMods>1 && intMods<=3) && (boolLS)){axisY=angleArr3[18];axisX=angleArr2[18];intR=80;}            // Up Left and ModY+LS
        else if((intMods==0) && (boolMS)){axisY=angleArr3[19];axisX=angleArr2[19];intR=125;}                        // Up Left and MS
        else if((intMods==1) && (boolMS)){axisY=angleArr3[20];axisX=angleArr2[20];intR=125;}                        // Up Left and ModX+MS
        else if((intMods>1 && intMods<=3) && (boolMS)){axisY=angleArr3[21];axisX=angleArr2[21];intR=125;}           // Up Left and ModY+MS
        else {axisY=184;axisX=72;}
        break;

      // Up & Right
      case 10:
        // Lots of conditions
        axisY=184;axisX=184;
        if(intMods==0 && intCstick==0 && boolL==0 && boolR==0 && boolLS==0 && boolMS==0){axisY=184;axisX=184;}              // Up Left
        else if(intMods==1 && intCstick==0 && intRLLSMSB==0){axisY=angleArr3[0];axisX=angleArr4[0];}                                    // Up Right and ModX
        else if(intMods>1 && intMods<=3 && intCstick==0 && intRLLSMSB==0){axisY=angleArr3[1];axisX=angleArr4[1];}                       // Up Right and ModY
        else if((boolB) && (intMods==1) && (intCstick==1) && (intRLLSMSB==0)){axisY=angleArr3[25];axisX=angleArr4[25];}                              // Up Right and ModX and CL
        else if((boolB) && (intMods>1 && intMods<=3) && (intCstick==1) && (intRLLSMSB==0)){axisY=angleArr3[26];axisX=angleArr4[26];}                 // Up Right and ModY and CL
        else if((boolB) && (intMods==1) && (intCstick>=2 && intCstick<4) && (intRLLSMSB==0)){axisY=angleArr3[29];axisX=angleArr4[29];}               // Up Right and ModX and CR
        else if((boolB) && (intMods>1 && intMods<=3) && (intCstick>=2 && intCstick<4) && (intRLLSMSB==0)){axisY=angleArr3[30];axisX=angleArr4[30];}  // Up Right and ModY and CR
        else if((boolB) && (intMods==1) && (intCstick>=4 && intCstick<8) && (intRLLSMSB==0)){axisY=angleArr3[23];axisX=angleArr4[23];}               // Up Right and ModX and CD
        else if((boolB) && (intMods>1 && intMods<=3) && (intCstick>=4 && intCstick<8) && (intRLLSMSB==0)){axisY=angleArr3[24];axisX=angleArr4[24];}  // Up Right and ModY and CD
        else if((boolB) && (intMods==1) && (intCstick>=8) && (intRLLSMSB==0)){axisY=angleArr3[27];axisX=angleArr4[27];}                              // Up Right and ModX and CU
        else if((boolB) && (intMods>1 && intMods<=3) && (intCstick>=8) && (intRLLSMSB==0)){axisY=angleArr3[28];axisX=angleArr4[28];}                 // Up Right and ModY and CU
        else if((intMods==1) && (intCstick==1) && (intRLLSMSB==0)){axisY=angleArr3[4];axisX=angleArr4[4];}                              // Up Right and ModX and CL
        else if((intMods>1 && intMods<=3) && (intCstick==1) && (intRLLSMSB==0)){axisY=angleArr3[5];axisX=angleArr4[5];}                 // Up Right and ModY and CL
        else if((intMods==1) && (intCstick>=2 && intCstick<4) && (intRLLSMSB==0)){axisY=angleArr3[8];axisX=angleArr4[8];}               // Up Right and ModX and CR
        else if((intMods>1 && intMods<=3) && (intCstick>=2 && intCstick<4) && (intRLLSMSB==0)){axisY=angleArr3[9];axisX=angleArr4[9];}  // Up Right and ModY and CR
        else if((intMods==1) && (intCstick>=4 && intCstick<8) && (intRLLSMSB==0)){axisY=angleArr3[2];axisX=angleArr4[2];}               // Up Right and ModX and CD
        else if((intMods>1 && intMods<=3) && (intCstick>=4 && intCstick<8) && (intRLLSMSB==0)){axisY=angleArr3[3];axisX=angleArr4[3];}  // Up Right and ModY and CD
        else if((intMods==1) && (intCstick>=8) && (intRLLSMSB==0)){axisY=angleArr3[6];axisX=angleArr4[6];}                              // Up Right and ModX and CU
        else if((intMods>1 && intMods<=3) && (intCstick>=8) && (intRLLSMSB==0)){axisY=angleArr3[7];axisX=angleArr4[7];}                 // Up Right and ModY and CU
        else if((intMods==0) && (boolR)){axisY=angleArr3[13];axisX=angleArr4[13];intR=170;}                         // Up Right and R
        else if((intMods==1) && (boolR)){axisY=angleArr3[14];axisX=angleArr4[14];intR=170;}                         // Up Right and ModX+R
        else if((intMods>1 && intMods<=3) && (boolR)){axisY=angleArr3[15];axisX=angleArr4[15];intR=170;}            // Up Right and ModY+R
        else if((intMods==0) && (boolL)){axisY=angleArr3[10];axisX=angleArr4[10];intL=170;}                         // Up Right and L
        else if((intMods==1) && (boolL)){axisY=angleArr3[11];axisX=angleArr4[11];intL=170;}                         // Up Right and ModX+L
        else if((intMods>1 && intMods<=3) && (boolL)){axisY=angleArr3[12];axisX=angleArr4[12];intL=170;}            // Up Right and ModY+L
        else if((intMods==0) && (boolLS)){axisY=angleArr3[16];axisX=angleArr4[16];intR=80;}                         // Up Right and LS
        else if((intMods==1) && (boolLS)){axisY=angleArr3[17];axisX=angleArr4[17];intR=80;}                         // Up Right and ModX+LS
        else if((intMods>1 && intMods<=3) && (boolLS)){axisY=angleArr3[18];axisX=angleArr4[18];intR=80;}            // Up Right and ModY+LS
        else if((intMods==0) && (boolMS)){axisY=angleArr3[19];axisX=angleArr4[19];intR=125;}                        // Up Right and MS
        else if((intMods==1) && (boolMS)){axisY=angleArr3[20];axisX=angleArr4[20];intR=125;}                        // Up Right and ModX+MS
        else if((intMods>1 && intMods<=3) && (boolMS)){axisY=angleArr3[21];axisX=angleArr4[21];intR=125;}           // Up Right and ModY+MS
        else {axisY=184;axisX=184;}
        break;
  }
}

void evalBrookMode(){
  // handle Brook mode ("Brook Up" on ModX, ModY, CDown, A)
    if (boolModX || boolModY || boolCDown || boolA){digitalWrite(17, LOW);}
    else {digitalWrite(17, HIGH);}
    if (boolL){digitalWrite(30,LOW);}
    else {digitalWrite(30, HIGH);}
}
