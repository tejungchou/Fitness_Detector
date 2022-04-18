/* 2020 embedded system challenge */

/* system */
#include <mbed.h>
#include<math.h>

/* user */
#include "LIS3DSH.h"

/* setup */
LIS3DSH acc(SPI_MOSI,SPI_MISO,SPI_SCK,PE_3);

/* LEDs and button */
DigitalOut orange(LED3);
DigitalOut green(LED4);
DigitalOut red(LED5);
DigitalOut blue(LED6);
DigitalIn button(BUTTON1);

/* state indicator */
typedef enum state_t{
  waiting,
  lying,
  sitting,
  pushing,
  uping,
  standing,
  jumping,
  squating
}state_t;

/* LED blink function */
void blink3s(DigitalOut L){
  for (uint8_t l=0;l<3;l++){
    L=!L;
    wait_ms(60);
    L=!L;                     //make sure the led returns its original status
  }
}
/* turn on all the leds */
void lumos(){
  orange.write(1);
  green.write(1);
  red.write(1);
  blue.write(1);
}

/* check repetition and do some led tricks after finished all the exercises*/
void checkrep(int st,int pu,int ju,int sqt){
  /*turn off each led as you finish that particular exercise*/
  if(st==0){
    orange.write(0);
  }
  if(pu==0){
    green.write(0);
  }
  if(ju==0){
    red.write(0);
  }
  if(sqt==0){
    blue.write(0);
  }
  /*LED tricks while finish all four exercises*/
  while(st<=0&&pu<=0&&ju<=0&&sqt<=0){  
    for (uint8_t o=0;o<3;o++){
    orange.write(0);
    green.write(0);
    red.write(0);
    blue.write(0);
    orange.write(1);
    wait_ms(160);
    orange=!orange;
    green.write(1);
    wait_ms(160);
    green=!green;
    blue.write(1);
    wait_ms(160);
    blue=!blue;
    red.write(1);
    wait_ms(160);
    red=!red;
    }
    for (uint8_t k=0;k<8;k++){
    orange=!orange;
    green=!green;
    red=!red;
    blue=!blue;
    wait_ms(100);
    }
  }
}
 /* movements function */
void move1(){
  int16_t X, Y;
  int16_t Z=0;    //signed integer variables for raw X,Y,Z values
  float g_z=0;
  float angle=0; 
  const float Pi=3.14159;
  /* get the sample average */
  const uint8_t N=15; // length
  float buffer[N];    // buffer with size ength N
  uint8_t buffer_index=0;
  int state=0;

/*Professor and TA can just simply change the parameters below to 5 each to make the testing procedure quicker!!*/
  float st=15,pu=15,ju=15,sqt=15; // initial repititions requirement

/* error if accelerometer not detected */
  if(acc.Detect() != 1) {
      printf("LIS3DSH Accelerometer not detected!\n");
      wait_ms(200);
  }
  if(button==1){  //press button to start the measurement
  lumos();    //turn on all LEDs to show that you have started the program
    while(1) {
      acc.ReadData(&X, &Y, &Z);           //read X, Y, Z values
      g_z=(float)Z/17694.0;
  
      /*angle calculation*/
      buffer[buffer_index++]=g_z;
      if(buffer_index>=N){
        buffer_index=0;
      }
      float g_zs=0;

      for (uint8_t i=0;i<N;i++){
        g_zs+=buffer[i];
      }
      g_zs/=(float)N;
      if(g_zs>1){
        g_zs=1;
      }     
      float angle=180*acos(g_zs)/Pi;
      /*angle calculation ends*/

      /*movement transition */
      switch(state){
        /*waiting routine to detect type of sports*/
          case waiting:
          checkrep(st,pu,ju,sqt);
            if(Y>-7500&&Z<-5000&&angle>140){
              state=pushing;
            }
            else if(Z>13000&&Y>-4000&&angle<30){
              state = lying;
            }
            
            else if(Y<-16000&&angle>80){
              state=standing;
            }
            break;

        /*sit up*/
          case lying:
          blink3s(orange);
          if(angle>35){
              state=sitting;
             }
          break;
          case sitting:
           st--;
            state=waiting; 
          break;
          /*push up*/
          case pushing:
          blink3s(green);
            if(Y<-6500){
            state=uping;
            }
          break;
          case uping:
              pu--;
            state=waiting;
          break;
          /*jpj & squat*/
          case standing:
            if(Z>13000&&Y>-4000&&angle<30){
              state = lying; //let it go back to waiting and check if it's lying again
            }
            else if(angle>140){
              state=pushing;
            }
            else if(Y<-25000){
              state=jumping;
            }
            else if(Z<-12500){
              state=squating;
            }
          break;
          /*jumping*/
          case jumping:
          blink3s(red);
          blink3s(red);
            ju--;
            state=waiting;
          break;
          /*squat down*/
          case squating:
          blink3s(blue);
          blink3s(blue);
           sqt--;
            state=waiting;
          break;
          default:
          printf("something went wrong!\n");
        } 
      wait_ms(160);  //delay before reading next values
    }
  }
 }
/* Main func, keep it clear*/
int main() {
  while(1) {
   
    move1();
  
  }
 }
