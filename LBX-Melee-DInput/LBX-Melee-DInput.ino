// This code uses Matthew Heironimus' Arduino Joystick Library https://github.com/MHeironimus/ArduinoJoystickLibrary
// This code is original but based off of the concepts in Simple's DIY repository (from Jan 6 2017) as well as Crane's GCCPCB repository. Special thanks to both of those peeps!
// https://github.com/SimpleControllers/SimpleControllersBuild-a-Box
// https://github.com/Crane1195/GCCPCB/tree/master/code/GCCPCB2-v1.209 (outdated, he has a new repo called CL-FW)
// Values in the code were observed by flashing Crane's GCCPCB2 code to an arduino PCB and observing the values in "Visual Controller Test".

// This code is open source and instuctions for routine flashing, remapping and using it's features (switching to a Brook board) as well as a pinout for our LBX will be posted online.
// Modifications to adapt this code for other game types and protocols will be posted in separate files.
// Code that combines all game types and protocols has not been written.
// An open source license in line with other projects in this space is provided.

//****************THIS IS IN PROGRESS AND NOT FINISHED*********************

//To do:
// /DONE/ Add joystick library
// /TEST/ Fix DPad logic
// /TEST/ Change the way things are sent / remove GCC sending (this library has an automode, but send manually instead)
// /TEST/ test range of Left and Right. Base it on 0-255.

#include <Joystick.h>

Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID,JOYSTICK_TYPE_GAMEPAD,
  17, 0,                 // Button Count, Hat Switch Count
  true, true, false,     // X and Y, but no Z Axis
  false, false, false,   // No Rx, Ry, or Rz
  false, false,          // No rudder or throttle
  false, false, false);  // No accelerator, brake, or steering

// Define ints for holding bit vals
  unsigned long longBtnPress = 0; 
  unsigned long longPrevBtnPress = 0;
  unsigned int intLeftRight = 0;
  unsigned int prevIntLeftRight = 0;
  bool lockLeftRight = false;
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
int modXArr[4] = {151,105,75,181}; // Mod X
int modYArr[4] = {187,69,101,155}; // Mod Y

// Define angle values in 4 arrays. (see spreadsheet tabs 1 & 2)
int angleArr1[23] = {105,69,104,71,97,71,93,65,85,71,73,98,60,85,98,60,73,85,73,73,85,73,69};                       // Down
int angleArr2[23] = {69,105,79,100,76,94,79,83,77,81,85,77,88,85,77,88,28,85,105,28,85,105,105};                    // Left
int angleArr3[23] = {151,187,152,185,159,185,163,191,171,185,171,158,196,171,158,196,228,171,187,228,171,187,187};  // UP
int angleArr4[23] = {187,151,177,156,180,162,177,173,179,175,171,179,168,171,179,168,228,171,151,228,171,151,151};  // Right

// Define special values in 5 arrays. (see spreadsheet tabs 1 & 2)
int specialArr1[4] = {228,28,28,228};   // L, ModX+L, ModY+L, LS, MS
int specialArr2[4] = {171,85,77,179};   // R, ModX+R, ModY+R, ModX+LS, ModX+MS
int specialArr3[4] = {187,69,101,155};  // ModY+LS, ModY+MS
int specialArr4[4] = {187,69,48,208};   // ModY+B
int specialArr5[4] = {73,71,73,185};    // L, LS, ModY+LS, MS, ModY+MS all have diff vals for down angles
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
  
  // Initialize Joystick Library
  Joystick.begin();
    Joystick.setXAxisRange(0, 255);
    Joystick.setYAxisRange(0, 255);
    Joystick.setZAxisRange(0, 255);
    Joystick.setRxAxisRange(0, 255);
    Joystick.setRyAxisRange(0, 255);
    Joystick.setThrottleRange(0, 255);
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
  else{
    
    // ********check longPrevBtnPress - If current loop matches prev loop then skip the loop
    if(longPrevBtnPress == longBtnPress){
        // do nothing - no button changes / end the loop
    }
    else{
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
        if(boolL){intL=140;}
        if(boolR){intR=140;}
        else if(boolMS){intR=125;}
        else if(boolLS){intR=80;}
        }
      
      //If directions, Call SOCD function // Any one of bits 0,1,2,12 must be active
      if ((boolLeft) || (boolRight) || (boolGCUp) || (boolDown)) {scrubDirSOCD();}
      else{
        bitClear(longBtnPress,0);
        bitClear(longBtnPress,1);
        bitClear(longBtnPress,2);
        bitClear(longBtnPress,12);
        lockLeftRight=false; intLeftRight=0; prevIntLeftRight=0; axisX=128;
        lockUpDown=false; intUpDown = 0; prevIntUpDown = 0; axisY=128;
      }
      
      if ((boolCLeft) || (boolCRight) || (boolCUp) || (boolCDown)){scrubCStickSOCD();}
      else{
        bitClear(longBtnPress,7);
        bitClear(longBtnPress,8);
        bitClear(longBtnPress,10);
        bitClear(longBtnPress,11);
        lockCLeftRight=false; intCLeftRight=0; prevIntCLeftRight=0; cstickX=128;
        lockCUpDown=false; intCUpDown = 0; prevIntCUpDown = 0; cstickY=128;
        }
        
      if (boolJFAKey){scrubDpad();}
//      else{
//        d.report.dup    = 0;
//        d.report.ddown  = 0;
//        d.report.dleft  = 0;
//        d.report.dright = 0;
//        }
      scrubAngles();
    }
  }
 
//  d.report.a = boolA;
//  d.report.b = boolB;
//  d.report.x = boolX;
//  d.report.y = boolY;
//  d.report.z = boolZ;
//  d.report.start = boolStart;
//  d.report.r = boolR;
//  d.report.l = boolL;
//  d.report.xAxis  = axisX;
//  d.report.yAxis  = axisY;
//  d.report.cxAxis   = cstickX;
//  d.report.cyAxis   = cstickY;
//  d.report.right = intR;
//  d.report.left = intL;
//  GamecubeConsole.write(d);

      Joystick.setButton(0, boolA);
      Joystick.setButton(1, boolB);
      Joystick.setButton(2, boolX);
      Joystick.setButton(3, boolY);
      Joystick.setButton(4, boolZ);
      Joystick.setButton(5, boolL);
      Joystick.setButton(6, boolR);
      Joystick.setButton(7, boolStart);
      Joystick.setButton(8, boolDPadLeft);
      Joystick.setButton(9, boolDPadUp);
      Joystick.setButton(10, boolDPadRight);
      Joystick.setButton(11, boolDPadDown);
      Joystick.setButton(12, boolLS);
      Joystick.setButton(13, boolMS);
      Joystick.setXAxis(axisX);
      Joystick.setYAxis(axisY);
      Joystick.setRxAxis(cstickX);
      Joystick.setRyAxis(cstickY);
      Joystick.setThrottle(intR);
  }
}

// Scrub directional SOCD
void scrubDirSOCD() {
  //if no common SOCD dirs are held, set dir input tracking vars to 0
  if ((boolLeft==0) && (boolRight==0)){intLeftRight = 0; prevIntLeftRight = 0;}
  //Check left and right
  else if ((boolLeft) || (boolRight)) {
    intLeftRight=0;
    if(boolLeft){bitSet(intLeftRight,0);}
    if(boolRight){bitSet(intLeftRight,1);}
    //Set prevIntLeftRight
    if((prevIntLeftRight==0) && ((intLeftRight>0) && (intLeftRight<3))){prevIntLeftRight = intLeftRight;}
    //2ip no reactivate (melee)
    if((prevIntLeftRight>0) && (intLeftRight<3) && (prevIntLeftRight==intLeftRight) && (lockLeftRight == true)){intLeftRight=0;} // Must check for cleaning having been performed at least once. Crane uses a Lockout var. That's the approach here as well.
    else if((prevIntLeftRight>0) && (intLeftRight<3) && (prevIntLeftRight!=intLeftRight)){prevIntLeftRight=intLeftRight;} //If they hold the seccond button and release the first, update prevIntLeftRight to match.
    else if(prevIntLeftRight>0 && intLeftRight==3){intLeftRight = (intLeftRight ^ prevIntLeftRight); lockLeftRight=true;} // SOCD, 2ip no reactivate. Can't subtract a larger number from a smaller number and stil have a positive int. Do a bit comparison using XOR instead.
  }
  if ((boolGCUp==0) && (boolDown==0)){intUpDown = 0; prevIntUpDown = 0;}
  //Check up and down
  else if ((boolDown) || (boolGCUp)){
    intUpDown=0;
    if(boolDown){bitSet(intUpDown,0);}
    if(boolGCUp){bitSet(intUpDown,1);}
    //Set prevIntUpDown    
    if((prevIntUpDown == 0) && ((intUpDown>0) && (intUpDown<3))){prevIntUpDown = intUpDown;}
    //2ip no reactivate (melee)
    if((prevIntUpDown>0) && (intUpDown<3) && (prevIntUpDown==intUpDown) && (lockUpDown == true)){intUpDown=0;} // Must check for cleaning having been performed at least once. Crane uses a Lockout var. That's the approach here as well.
    else if((prevIntUpDown>0) && (intUpDown<3) && (prevIntUpDown!=intUpDown)){prevIntUpDown=intUpDown;} // If they hold the seccond button and release the first, update prevIntLeftRight to match.   
    else if(prevIntUpDown>0 && intUpDown==3){intUpDown = (intUpDown ^ prevIntUpDown); lockUpDown=true;} // SOCD, 2ip no reactivate. Can't subtract a larger number from a smaller number and stil have a positive int. Do a bit comparison using XOR instead.
  }
}

void scrubCStickSOCD() {
  //if no common SOCD dirs are held, set dir input tracking vars to 0
  if ((boolCLeft==0) && (boolCRight==0)){lockCLeftRight=false; intCLeftRight=0; prevIntCLeftRight=0; cstickX=128;}
  //Check Cleft and Cright
  else if ((boolCLeft) || (boolCRight)) {
    intCLeftRight=0;
    if(boolCLeft){bitSet(intCLeftRight,0);}
    if(boolCRight){bitSet(intCLeftRight,1);}
    //Set prevIntCLeftRight
    if((prevIntCLeftRight==0) && ((intCLeftRight>0) && (intCLeftRight<3))){prevIntCLeftRight = intCLeftRight;}
    //2ip no reactivate (melee)
    if((prevIntCLeftRight>0) && (intCLeftRight<3) && (prevIntCLeftRight==intCLeftRight) && (lockCLeftRight == true)){intCLeftRight=0;} // Must check for cleaning having been performed at least once. Crane uses a Lockout var. That's the approach here as well.
    else if((prevIntCLeftRight>0) && (intCLeftRight<3) && (prevIntCLeftRight!=intCLeftRight)){prevIntCLeftRight=intCLeftRight;} //If they hold the seccond button and release the first, update prevIntLeftRight to match.
    else if(prevIntCLeftRight>0 && intCLeftRight==3){intCLeftRight = (intCLeftRight ^ prevIntCLeftRight); lockCLeftRight=true;} // SOCD, 2ip no reactivate. Can't subtract a larger number from a smaller number and stil have a positive int. Do a bit comparison using XOR instead.
    if (intCLeftRight==0){cstickX=128;}
    if (intCLeftRight==1){cstickX=28;}
    if (intCLeftRight==2){cstickX=228;}
  }
  
  if ((boolCUp==0) && (boolCDown==0)){lockCUpDown=false; intCUpDown = 0; prevIntCUpDown = 0; cstickY=128;}
  //Check up and down  
  else if ((boolCDown) || (boolCUp)){
    intCUpDown=0;
    if(boolCDown){bitSet(intCUpDown,0);}
    if(boolCUp){bitSet(intCUpDown,1);}
    //Set prevIntUpDown    
    if((prevIntCUpDown == 0) && ((intCUpDown>0) && (intCUpDown<3))){prevIntCUpDown = intCUpDown;}
    //2ip no reactivate (melee)
    if((prevIntCUpDown>0) && (intCUpDown<3) && (prevIntCUpDown==intCUpDown) && (lockCUpDown == true)){intCUpDown=0;} // Must check for cleaning having been performed at least once. Crane uses a Lockout var. That's the approach here as well.
    else if((prevIntCUpDown>0) && (intCUpDown<3) && (prevIntCUpDown!=intCUpDown)){prevIntCUpDown=intCUpDown;} // If they hold the seccond button and release the first, update prevIntLeftRight to match.   
    else if(prevIntCUpDown>0 && intCUpDown==3){intCUpDown = (intCUpDown ^ prevIntCUpDown); lockCUpDown=true;} // SOCD, 2ip no reactivate. Can't subtract a larger number from a smaller number and stil have a positive int. Do a bit comparison using XOR instead.
    if (intCUpDown==0){cstickY=128;}
    if (intCUpDown==1){cstickY=28;}
    if (intCUpDown==2){cstickY=228;}
  }
}

void scrubDpad() {
  //if JFAKEY then CSTICK ==0 and Cstick inputs become up down left and right for DPAD  
    boolDPadDown  = bitRead(intCUpDown,0);
    boolDPadUp    = bitRead(intCUpDown,1);
    boolDPadLeft  = bitRead(intCLeftRight,0);
    boolDPadRight = bitRead(intCLeftRight,1);
    //Set GCC values
//    d.report.dup    = boolDPadUp;
//    d.report.ddown  = boolDPadDown;
//    d.report.dleft  = boolDPadLeft;
//    d.report.dright = boolDPadRight;
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
  intRLLSMSB += (boolB*16);

  switch (intDirs) {
      // Left
      case 1: 
        axisY=128;axisX=28;
        if(intMods==0 && intCstick==0 && boolL==0 && boolR==0 && boolLS==0 && boolMS==0 && boolB==0){axisX=28;} // Left
        else if(intMods==1 && intCstick==0 && intRLLSMSB==0){axisX=modXArr[2];}         // Left and ModX
        else if(intMods==2 && intCstick==0 && intRLLSMSB==0){axisX=modYArr[2];}         // Left and ModY
        else if((intMods==1) && (intCstick>0) && (intRLLSMSB==0)){axisX=modXArr[2];}    // Left and ModX and Cstick
        else if((intMods==2) && (intCstick>0) && (intRLLSMSB==0)){axisX=modYArr[2];}    // Left and ModY and Cstick
        else if((intMods==0) && (boolR)){axisX=specialArr2[2];intR=140;}                // Left and R
        else if((intMods==1) && (boolR)){axisX=specialArr2[2];intR=140;}                // Left and ModX+R
        else if((intMods>1 && intMods<=3) && (boolR)){axisX=specialArr2[2];intR=140;}   // Left and ModY+R
        else if((intMods==0) && (boolL)){axisX=specialArr1[2];intL=140;}                // Left and L
        else if((intMods==1) && (boolL)){axisX=specialArr1[2];intL=140;}                // Left and ModX+L
        else if((intMods>1 && intMods<=3) && (boolL)){axisX=specialArr1[2];intL=140;}   // Left and ModY+L
        else if((intMods==0) && (boolLS)){axisX=specialArr1[2];intR=80;}                // Left and LS
        else if((intMods==1) && (boolLS)){axisX=specialArr2[2];intR=80;}                // Left and ModX+LS
        else if((intMods>1 && intMods<=3) && (boolLS)){axisX=specialArr3[2];intR=80;}   // Left and ModY+LS
        else if((intMods==0) && (boolMS)){axisX=specialArr1[2];intR=125;}               // Left and MS
        else if((intMods==1) && (boolMS)){axisX=specialArr2[2];intR=125;}               // Left and ModX+MS
        else if((intMods>1 && intMods<=3) && (boolMS)){axisX=specialArr3[2];intR=125;}  // Left and ModY+MS
        else if((intMods==2) && (boolB)){axisX=specialArr4[2];}                         // Left and ModY+B
        break;

      // Right
      case 2:
        axisY=128;axisX=228;
        if(intMods==0 && intCstick==0 && boolL==0 && boolR==0 && boolLS==0 && boolMS==0 && boolB==0){axisX=228;} // Right
        else if(intMods==1 && intCstick==0 && intRLLSMSB==0){axisX=modXArr[3];}         // Right and ModX
        else if(intMods==2 && intCstick==0 && intRLLSMSB==0){axisX=modYArr[3];}         // Right and ModY
        else if((intMods==1) && (intCstick>0) && (intRLLSMSB==0)){axisX=modXArr[3];}    // Right and ModX and Cstick
        else if((intMods==2) && (intCstick>0) && (intRLLSMSB==0)){axisX=modYArr[3];}    // Right and ModY and Cstick
        else if((intMods==0) && (boolR)){axisX=specialArr2[3];intR=140;}                // Right and R
        else if((intMods==1) && (boolR)){axisX=specialArr2[3];intR=140;}                // Right and ModX+R
        else if((intMods>1 && intMods<=3) && (boolR)){axisX=specialArr2[3];intR=140;}   // Right and ModY+R
        else if((intMods==0) && (boolL)){axisX=specialArr1[3];intL=140;}                // Right and L
        else if((intMods==1) && (boolL)){axisX=specialArr1[3];intL=140;}                // Right and ModX+L
        else if((intMods>1 && intMods<=3) && (boolL)){axisX=specialArr1[3];intL=140;}   // Right and ModY+L
        else if((intMods==0) && (boolLS)){axisX=specialArr1[3];intR=80;}                // Right and LS
        else if((intMods==1) && (boolLS)){axisX=specialArr2[3];intR=80;}                // Right and ModX+LS
        else if((intMods>1 && intMods<=3) && (boolLS)){axisX=specialArr3[3];intR=80;}   // Right and ModY+LS
        else if((intMods==0) && (boolMS)){axisX=specialArr1[3];intR=125;}               // Right and MS
        else if((intMods==1) && (boolMS)){axisX=specialArr2[3];intR=125;}               // Right and ModX+MS
        else if((intMods>1 && intMods<=3) && (boolMS)){axisX=specialArr3[3];intR=125;}  // Right and ModY+MS
        else if((intMods==2) && (boolB)){axisX=specialArr4[3];}                         // Right and ModY+B 
        break;

      // Down
      case 4:
        axisX=128;axisY=28;
        if(intMods==0 && intCstick==0 && boolL==0 && boolR==0 && boolLS==0 && boolMS==0 && boolB==0){axisY=28;} // Down
        else if(intMods==1 && intCstick==0 && intRLLSMSB==0){axisY=modXArr[1];}         // Down and ModX
        else if(intMods==2 && intCstick==0 && intRLLSMSB==0){axisY=modYArr[1];}         // Down and ModY
        else if((intMods==1) && (intCstick>0) && (intRLLSMSB==0)){axisY=modXArr[1];}    // Down and ModX and Cstick
        else if((intMods==2) && (intCstick>0) && (intRLLSMSB==0)){axisY=modYArr[1];}    // Down and ModY and Cstick
        else if((intMods==0) && (boolR)){axisY=specialArr2[1];intR=140;}                // Down and R
        else if((intMods==1) && (boolR)){axisY=specialArr2[1];intR=140;}                // Down and ModX+R
        else if((intMods>1 && intMods<=3) && (boolR)){axisY=specialArr2[1];intR=140;}   // Down and ModY+R
        else if((intMods==0) && (boolL)){axisY=specialArr1[1];intL=140;}                // Down and L
        else if((intMods==1) && (boolL)){axisY=specialArr1[1];intL=140;}                // Down and ModX+L
        else if((intMods>1 && intMods<=3) && (boolL)){axisY=specialArr1[1];intL=140;}   // Down and ModY+L
        else if((intMods==0) && (boolLS)){axisY=specialArr1[1];intR=80;}                // Down and LS
        else if((intMods==1) && (boolLS)){axisY=specialArr2[1];intR=80;}                // Down and ModX+LS
        else if((intMods>1 && intMods<=3) && (boolLS)){axisY=specialArr3[1];intR=80;}   // Down and ModY+LS
        else if((intMods==0) && (boolMS)){axisY=specialArr1[1];intR=125;}               // Down and MS
        else if((intMods==1) && (boolMS)){axisY=specialArr2[1];intR=125;}               // Down and ModX+MS
        else if((intMods>1 && intMods<=3) && (boolMS)){axisY=specialArr3[1];intR=125;}  // Down and ModY+MS
        break;      

      // Down & Left
      case 5:
        // Lots of conditions
        axisY=28;axisX=28;
        if(intMods==0 && intCstick==0 && boolR==0 && boolL==0 && boolLS==0 && boolMS==0 && boolB==0){axisY=28;axisX=28;}                // Up Left
        else if(intMods==1 && intCstick==0 && intRLLSMSB==0){axisY=angleArr1[0];axisX=angleArr2[0];}                                    // Down Left and ModX
        else if(intMods>1 && intMods<=3 && intCstick==0 && intRLLSMSB==0){axisY=angleArr1[1];axisX=angleArr2[1];}                       // Down Left and ModY
        else if((intMods==1) && (intCstick==1) && (intRLLSMSB==0)){axisY=angleArr1[4];axisX=angleArr2[4];}                              // Down Left and ModX and CL
        else if((intMods>1 && intMods<=3) && (intCstick==1) && (intRLLSMSB==0)){axisY=angleArr1[5];axisX=angleArr2[5];}                 // Down Left and ModY and CL
        else if((intMods==1) && (intCstick>=2 && intCstick<4) && (intRLLSMSB==0)){axisY=angleArr1[8];axisX=angleArr2[8];}               // Down Left and ModX and CR
        else if((intMods>1 && intMods<=3) && (intCstick>=2 && intCstick<4) && (intRLLSMSB==0)){axisY=angleArr1[9];axisX=angleArr2[9];}  // Down Left and ModY and CR
        else if((intMods==1) && (intCstick>=4 && intCstick<8) && (intRLLSMSB==0)){axisY=angleArr1[2];axisX=angleArr2[2];}               // Down Left and ModX and CD
        else if((intMods>1 && intMods<=3) && (intCstick>=4 && intCstick<8) && (intRLLSMSB==0)){axisY=angleArr1[3];axisX=angleArr2[3];}  // Down Left and ModY and CD
        else if((intMods==1) && (intCstick>=8) && (intRLLSMSB==0)){axisY=angleArr1[6];axisX=angleArr2[6];cstickX=128;}                  // Down Left and ModX and CU
        else if((intMods>1 && intMods<=3) && (intCstick>=8) && (intRLLSMSB==0)){axisY=angleArr1[7];axisX=angleArr2[7];}                 // Down Left and ModY and CU
        else if((intMods==0) && (boolR)){axisY=angleArr1[13];axisX=angleArr2[13];intR=140;}                         // Down Left and R
        else if((intMods==1) && (boolR)){axisY=angleArr1[14];axisX=angleArr2[14];intR=140;}                         // Down Left and ModX+R
        else if((intMods>1 && intMods<=3) && (boolR)){axisY=angleArr1[15];axisX=angleArr2[15];intR=140;}            // Down Left and ModY+R
        else if((intMods==0) && (boolL)){axisY=specialArr5[0];axisX=specialArr5[1];intL=140;}                       // *Down Left and L
        else if((intMods==1) && (boolL)){axisY=angleArr1[11];axisX=angleArr2[11];intL=140;}                         // Down Left and ModX+L
        else if((intMods>1 && intMods<=3) && (boolL)){axisY=angleArr1[12];axisX=angleArr2[12];intL=140;}            // Down Left and ModY+L
        else if((intMods==0) && (boolLS)){axisY=specialArr5[0];axisX=specialArr5[1];intR=80;}                       // *Down Left and LS
        else if((intMods==1) && (boolLS)){axisY=angleArr1[17];axisX=angleArr2[17];intR=80;}                         // Down Left and ModX+LS
        else if((intMods>1 && intMods<=3) && (boolLS)){axisY=specialArr5[0];axisX=specialArr5[1];intR=80;}          // *Down Left and ModY+LS
        else if((intMods==0) && (boolMS)){axisY=specialArr5[0];axisX=specialArr5[1];intR=125;}                      // *Down Left and MS
        else if((intMods==1) && (boolMS)){axisY=angleArr1[20];axisX=angleArr2[20];intR=125;}                        // Down Left and ModX+MS
        else if((intMods>1 && intMods<=3) && (boolMS)){axisY=specialArr5[0];axisX=specialArr5[1];intR=125;}         // *Down Left and ModY+MS
        break;

      // Down & Right
      case 6: 
        // Lots of conditions
        axisY=28;axisX=228;
        if(intMods==0 && intCstick==0 && boolL==0 && boolR==0 && boolLS==0 && boolMS==0 && boolB==0){axisY=28;axisX=228;}               // Up Left
        else if(intMods==1 && intCstick==0 && intRLLSMSB==0){axisY=angleArr1[0];axisX=angleArr4[0];}                                    // Down Right and ModX
        else if(intMods>1 && intMods<=3 && intCstick==0 && intRLLSMSB==0){axisY=angleArr1[1];axisX=angleArr4[1];}                       // Down Right and ModY
        else if((intMods==1) && (intCstick==1) && (intRLLSMSB==0)){axisY=angleArr1[4];axisX=angleArr4[4];}                              // Down Right and ModX and CL
        else if((intMods>1 && intMods<=3) && (intCstick==1) && (intRLLSMSB==0)){axisY=angleArr1[5];axisX=angleArr4[5];}                 // Down Right and ModY and CL
        else if((intMods==1) && (intCstick>=2 && intCstick<4) && (intRLLSMSB==0)){axisY=angleArr1[8];axisX=angleArr4[8];}               // Down Right and ModX and CR
        else if((intMods>1 && intMods<=3) && (intCstick>=2 && intCstick<4) && (intRLLSMSB==0)){axisY=angleArr1[9];axisX=angleArr4[9];}  // Down Right and ModY and CR
        else if((intMods==1) && (intCstick>=4 && intCstick<8) && (intRLLSMSB==0)){axisY=angleArr1[2];axisX=angleArr4[2];}               // Down Right and ModX and CD
        else if((intMods>1 && intMods<=3) && (intCstick>=4 && intCstick<8) && (intRLLSMSB==0)){axisY=angleArr1[3];axisX=angleArr4[3];}  // Down Right and ModY and CD
        else if((intMods==1) && (intCstick>=8) && (intRLLSMSB==0)){axisY=angleArr1[6];axisX=angleArr4[6];}                              // Down Right and ModX and CU
        else if((intMods>1 && intMods<=3) && (intCstick>=8) && (intRLLSMSB==0)){axisY=angleArr1[7];axisX=angleArr4[7];}                 // Down Right and ModY and CU
        else if((intMods==0) && (boolR)){axisY=angleArr1[13];axisX=angleArr4[13];intR=140;}                         // Down Right and R
        else if((intMods==1) && (boolR)){axisY=angleArr1[14];axisX=angleArr4[14];intR=140;}                         // Down Right and ModX+R
        else if((intMods>1 && intMods<=3) && (boolR)){axisY=angleArr1[15];axisX=angleArr4[15];intR=140;}            // Down Right and ModY+R
        else if((intMods==0) && (boolL)){axisY=specialArr5[0];axisX=specialArr5[3];intL=140;}                       // *Down Right and L
        else if((intMods==1) && (boolL)){axisY=angleArr1[11];axisX=angleArr4[11];intL=140;}                         // Down Right and ModX+L
        else if((intMods>1 && intMods<=3) && (boolL)){axisY=angleArr1[12];axisX=angleArr4[12];intL=140;}            // Down Right and ModY+L
        else if((intMods==0) && (boolLS)){axisY=specialArr5[0];axisX=specialArr5[3];intR=80;}                       // *Down Right and LS
        else if((intMods==1) && (boolLS)){axisY=angleArr1[17];axisX=angleArr4[17];intR=80;}                         // Down Right and ModX+LS
        else if((intMods>1 && intMods<=3) && (boolLS)){axisY=specialArr5[0];axisX=specialArr5[3];intR=80;}          // *Down Right and ModY+LS
        else if((intMods==0) && (boolMS)){axisY=specialArr5[0];axisX=specialArr5[3];intR=125;}                      // *Down Right and MS
        else if((intMods==1) && (boolMS)){axisY=angleArr1[20];axisX=angleArr4[20];intR=125;}                        // Down Right and ModX+MS
        else if((intMods>1 && intMods<=3) && (boolMS)){axisY=specialArr5[0];axisX=specialArr5[3];intR=125;}         // *Down Right and ModY+MS
        break;    

      // Up
      case 8:
        axisX=128;axisY=228;
        if(intMods==0 && intCstick==0 && boolL==0 && boolR==0 && boolLS==0 && boolMS==0 && boolB==0){axisY=228;} // Up
        else if(intMods==1 && intCstick==0 && intRLLSMSB==0){axisY=modXArr[0];}         // Up and ModX
        else if(intMods==2 && intCstick==0 && intRLLSMSB==0){axisY=modYArr[0];}         // Up and ModY
        else if((intMods==1) && (intCstick>0) && (intRLLSMSB==0)){axisY=modXArr[0];}    // Up and ModX and Cstick
        else if((intMods==2) && (intCstick>0) && (intRLLSMSB==0)){axisY=modYArr[0];}    // Up and ModY and Cstick
        else if((intMods==0) && (boolR)){axisY=specialArr2[0];intR=140;}                // Up and R
        else if((intMods==1) && (boolR)){axisY=specialArr2[0];intR=140;}                // Up and ModX+R
        else if((intMods>1 && intMods<=3) && (boolR)){axisY=specialArr2[0];intR=140;}   // Up and ModY+R
        else if((intMods==0) && (boolL)){axisY=specialArr1[0];intL=140;}                // Up and L
        else if((intMods==1) && (boolL)){axisY=specialArr1[0];intL=140;}                // Up and ModX+L
        else if((intMods>1 && intMods<=3) && (boolL)){axisY=specialArr1[0];intL=140;}   // Up and ModY+L
        else if((intMods==0) && (boolLS)){axisY=specialArr1[0];intR=80;}                // Up and LS
        else if((intMods==1) && (boolLS)){axisY=specialArr2[0];intR=80;}                // Up and ModX+LS
        else if((intMods>1 && intMods<=3) && (boolLS)){axisY=specialArr3[0];intR=80;}   // Up and ModY+LS
        else if((intMods==0) && (boolMS)){axisY=specialArr1[0];intR=125;}               // Up and MS
        else if((intMods==1) && (boolMS)){axisY=specialArr2[0];intR=125;}               // Up and ModX+MS
        else if((intMods>1 && intMods<=3) && (boolMS)){axisY=specialArr3[0];intR=125;}  // Up and ModY+MS
        break;

      // Up & Left
      case 9: 
        // Lots of conditions
        axisY=228;axisX=28;
        if(intMods==0 && intCstick==0 && boolL==0 && boolR==0 && boolLS==0 && boolMS==0 && boolB==0){axisY=228;axisX=28;}               // Up Left
        else if(intMods==1 && intCstick==0 && intRLLSMSB==0){axisY=angleArr3[0];axisX=angleArr2[0];}                                    // Up Left and ModX
        else if(intMods>1 && intMods<=3 && intCstick==0 && intRLLSMSB==0){axisY=angleArr3[1];axisX=angleArr2[1];}                       // Up Left and ModY
        else if((intMods==1) && (intCstick==1) && (intRLLSMSB==0)){axisY=angleArr3[4];axisX=angleArr2[4];}                              // Up Left and ModX and CL
        else if((intMods>1 && intMods<=3) && (intCstick==1) && (intRLLSMSB==0)){axisY=angleArr3[5];axisX=angleArr2[5];}                 // Up Left and ModY and CL
        else if((intMods==1) && (intCstick>=2 && intCstick<4) && (intRLLSMSB==0)){axisY=angleArr3[8];axisX=angleArr2[8];}               // Up Left and ModX and CR
        else if((intMods>1 && intMods<=3) && (intCstick>=2 && intCstick<4) && (intRLLSMSB==0)){axisY=angleArr3[9];axisX=angleArr2[9];}  // Up Left and ModY and CR
        else if((intMods==1) && (intCstick>=4 && intCstick<8) && (intRLLSMSB==0)){axisY=angleArr3[2];axisX=angleArr2[2];}               // Up Left and ModX and CD
        else if((intMods>1 && intMods<=3) && (intCstick>=4 && intCstick<8) && (intRLLSMSB==0)){axisY=angleArr3[3];axisX=angleArr2[3];}  // Up Left and ModY and CD
        else if((intMods==1) && (intCstick>=8) && (intRLLSMSB==0)){axisY=angleArr3[6];axisX=angleArr2[6];}                              // Up Left and ModX and CU
        else if((intMods>1 && intMods<=3) && (intCstick>=8) && (intRLLSMSB==0)){axisY=angleArr3[7];axisX=angleArr2[7];}                 // Up Left and ModY and CU
        else if((intMods==0) && (boolR)){axisY=angleArr3[13];axisX=angleArr2[13];intR=140;}                         // Up Left and R
        else if((intMods==1) && (boolR)){axisY=angleArr3[14];axisX=angleArr2[14];intR=140;}                         // Up Left and ModX+R
        else if((intMods>1 && intMods<=3) && (boolR)){axisY=angleArr3[15];axisX=angleArr2[15];intR=140;}            // Up Left and ModY+R
        else if((intMods==0) && (boolL)){axisY=angleArr3[10];axisX=angleArr2[10];intL=140;}                         // Up Left and L
        else if((intMods==1) && (boolL)){axisY=angleArr3[11];axisX=angleArr2[11];intL=140;}                         // Up Left and ModX+L
        else if((intMods>1 && intMods<=3) && (boolL)){axisY=angleArr3[12];axisX=angleArr2[12];intL=140;}            // Up Left and ModY+L
        else if((intMods==0) && (boolLS)){axisY=angleArr3[16];axisX=angleArr2[16];intR=80;}                         // Up Left and LS
        else if((intMods==1) && (boolLS)){axisY=angleArr3[17];axisX=angleArr2[17];intR=80;}                         // Up Left and ModX+LS
        else if((intMods>1 && intMods<=3) && (boolLS)){axisY=angleArr3[18];axisX=angleArr2[18];intR=80;}            // Up Left and ModY+LS
        else if((intMods==0) && (boolMS)){axisY=angleArr3[19];axisX=angleArr2[19];intR=125;}                        // Up Left and MS
        else if((intMods==1) && (boolMS)){axisY=angleArr3[20];axisX=angleArr2[20];intR=125;}                        // Up Left and ModX+MS
        else if((intMods>1 && intMods<=3) && (boolMS)){axisY=angleArr3[21];axisX=angleArr2[21];intR=125;}           // Up Left and ModY+MS
        break;

      // Up & Right
      case 10:
        // Lots of conditions
        axisY=228;axisX=228;
        if(intMods==0 && intCstick==0 && boolL==0 && boolR==0 && boolLS==0 && boolMS==0 && boolB==0){axisY=228;axisX=228;}              // Up Left
        else if(intMods==1 && intCstick==0 && intRLLSMSB==0){axisY=angleArr3[0];axisX=angleArr4[0];}                                    // Up Right and ModX
        else if(intMods>1 && intMods<=3 && intCstick==0 && intRLLSMSB==0){axisY=angleArr3[1];axisX=angleArr4[1];}                       // Up Right and ModY
        else if((intMods==1) && (intCstick==1) && (intRLLSMSB==0)){axisY=angleArr3[4];axisX=angleArr4[4];}                              // Up Right and ModX and CL
        else if((intMods>1 && intMods<=3) && (intCstick==1) && (intRLLSMSB==0)){axisY=angleArr3[5];axisX=angleArr4[5];}                 // Up Right and ModY and CL
        else if((intMods==1) && (intCstick>=2 && intCstick<4) && (intRLLSMSB==0)){axisY=angleArr3[8];axisX=angleArr4[8];}               // Up Right and ModX and CR
        else if((intMods>1 && intMods<=3) && (intCstick>=2 && intCstick<4) && (intRLLSMSB==0)){axisY=angleArr3[9];axisX=angleArr4[9];}  // Up Right and ModY and CR
        else if((intMods==1) && (intCstick>=4 && intCstick<8) && (intRLLSMSB==0)){axisY=angleArr3[2];axisX=angleArr4[2];}               // Up Right and ModX and CD
        else if((intMods>1 && intMods<=3) && (intCstick>=4 && intCstick<8) && (intRLLSMSB==0)){axisY=angleArr3[3];axisX=angleArr4[3];}  // Up Right and ModY and CD
        else if((intMods==1) && (intCstick>=8) && (intRLLSMSB==0)){axisY=angleArr3[6];axisX=angleArr4[6];}                              // Up Right and ModX and CU
        else if((intMods>1 && intMods<=3) && (intCstick>=8) && (intRLLSMSB==0)){axisY=angleArr3[7];axisX=angleArr4[7];}                 // Up Right and ModY and CU
        else if((intMods==0) && (boolR)){axisY=angleArr3[13];axisX=angleArr4[13];intR=140;}                         // Up Right and R
        else if((intMods==1) && (boolR)){axisY=angleArr3[14];axisX=angleArr4[14];intR=140;}                         // Up Right and ModX+R
        else if((intMods>1 && intMods<=3) && (boolR)){axisY=angleArr3[15];axisX=angleArr4[15];intR=140;}            // Up Right and ModY+R
        else if((intMods==0) && (boolL)){axisY=angleArr3[10];axisX=angleArr4[10];intL=140;}                         // Up Right and L
        else if((intMods==1) && (boolL)){axisY=angleArr3[11];axisX=angleArr4[11];intL=140;}                         // Up Right and ModX+L
        else if((intMods>1 && intMods<=3) && (boolL)){axisY=angleArr3[12];axisX=angleArr4[12];intL=140;}            // Up Right and ModY+L
        else if((intMods==0) && (boolLS)){axisY=angleArr3[16];axisX=angleArr4[16];intR=80;}                         // Up Right and LS
        else if((intMods==1) && (boolLS)){axisY=angleArr3[17];axisX=angleArr4[17];intR=80;}                         // Up Right and ModX+LS
        else if((intMods>1 && intMods<=3) && (boolLS)){axisY=angleArr3[18];axisX=angleArr4[18];intR=80;}            // Up Right and ModY+LS
        else if((intMods==0) && (boolMS)){axisY=angleArr3[19];axisX=angleArr4[19];intR=125;}                        // Up Right and MS
        else if((intMods==1) && (boolMS)){axisY=angleArr3[20];axisX=angleArr4[20];intR=125;}                        // Up Right and ModX+MS
        else if((intMods>1 && intMods<=3) && (boolMS)){axisY=angleArr3[21];axisX=angleArr4[21];intR=125;}           // Up Right and ModY+MS
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
