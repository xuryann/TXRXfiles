#include <DS3904.h> //make sure to install this library! May need to google it and unzip it in the Arduino libraries folder if
                    //you can't find it under the tools->libraries https://github.com/emdzej/DS3904
                    //Make sure to unzip it in your arduino libraries folder so the ide can find DS3904.h
                    //Folder structure for me is: ~/Arduino/libraries/DS3904

#define VENTILATORLED 3



#define MODEMTXPIN 13 /*PWM is 13 on the DUE mapping*/
#define MODEMRXPIN 2 /*PIOA0 is mapped to Arduino pin 2, or PB25,Peripheral B)*/


#define DIGIPOT /*OK NOTE: PCB design corrected to tie A0 pin to gnd. Device operates without this fix, however behaviour is not specified.*/
#define MODEM /*OK*/

#if defined(DIGIPOT) || defined(MODEM)
DS3904 resistor(BASE_ADDRESS);
int r0ohm = 5000;//10000; //Kohm value
int r1ohm = 10000; //kohm value
int r2ohm = 2000; //Kohm value

#endif //DIGIPOT

//Define globals for Modem ISR to use
#ifdef MODEM
/**************************************************************************************/
/*                        Capture PWM frequency and duty cycle                        */
/*      Hook a jumper between pin 2 (TIOA0) and pin A7 (TIOA1)                        */
/* In this schema, TIOA0 is the interupt input I believe, which is also PWM2 / pin2   */
/**************************************************************************************/
/*
 CODE TO SET UP ISR for modem. Mileage may vary...
volatile uint32_t CaptureCountA, CaptureCountB, TimerCount;
volatile boolean CaptureFlag;
*/

#endif //MODEM

void setup() 
{

  //Setup code
  pinMode(VENTILATORLED,OUTPUT);

  SerialUSB.begin(115200);

  #ifndef SERIAL1
  while(!SerialUSB)
  {
    ;//Needed for not missing out on messages but hangs code if no serial monitor is listenting.
  }
  #endif //SERIAL1
  SerialUSB.println("BOOT OK\n"); //Work
  

  #if defined(DIGIPOT) || defined(MODEM)
  resistor.setValue(RESISTOR_0, HIGH_Z);
  // Setting values in ohms is an approximation only
  resistor.setOhmValue(RESISTOR_0, r0ohm);
  resistor.setOhmValue(RESISTOR_1, r1ohm);
  resistor.setOhmValue(RESISTOR_2, r2ohm);//0x7F); //also hase "setValue" which I guess is less useful?
  SerialUSB.print("Digipot Values set... r0: "); SerialUSB.println(r0ohm); 
  SerialUSB.print(" r1: "); SerialUSB.println(r1ohm);
  SerialUSB.print(" r2: "); SerialUSB.println(r2ohm);
  #endif //DIGIPOT



  #ifdef MODEM
  //Set up the write (pretty simple...)
  pinMode(MODEMTXPIN, OUTPUT);  // sets the pin as output
  pinMode(MODEMRXPIN, INPUT);   // sets the pin as input.
  
   //========================== READ DIGIPOTENTIOMETER (SET IN INIT) ====================
  #ifdef DIGIPOT
  //byte value; you can also read the hex equivilent value with getValue. useful if you are just changing it around or whatever since Ohms is quantized anyway.
  int value;
 
  value = resistor.getOhmValue(RESISTOR_0);
  SerialUSB.print("Resistor 0 was set to : ");SerialUSB.print(r0ohm);SerialUSB.print("Ohm read : "); SerialUSB.println(value);
  value = resistor.getOhmValue(RESISTOR_1);
  SerialUSB.print("Resistor 1 was set to : ");SerialUSB.print(r1ohm);SerialUSB.print("Ohm read : "); SerialUSB.println(value);
  value = resistor.getOhmValue(RESISTOR_2);
  SerialUSB.print("Resistor 2 was set to : ");SerialUSB.print(r2ohm);SerialUSB.print("Ohm read : "); SerialUSB.println(value);
  
  #endif //DIGIPOT

 

  //========================== TEST CUSTOM MODEM (REQUIRES AN OSCILLOSCOPE) ====================
  #ifdef MODEM
  SerialUSB.println("Modem Carrier TX Test DEBUG OUTPUT DUTY 50/50");
  //Set a duty cycle of 50% on the PWM
  //This is not quite right honestly. The modem should be driven by a timer counter in wave generation mode
  //In that way, it would be far easier to use an interrupt based method to push out a FSK buffer.

  //TODO: This pin should be re-routed to a timer counter output multiplexed pin. Specifically, one multiplexed to TC0
  analogWrite(MODEMTXPIN,128); //Default is 1KHz wave. Shape is pretty horrible.
  

  #endif //MODEM
  
  /*Supposed to set up the timer interrupt. Mileage may vary...
  //setup the peripheral to be B...
  
  
  PIOB->PIO_ABSR |= 1<<25;
  */
  //Setup the timer counter to read 
  /*************         Capture a PWM frequency and duty cycle          ****************/
  /*
  PMC->PMC_PCER0 |= PMC_PCER0_PID28;                      // Timer Counter 0 channel 1 IS TC1, TC1 power ON

  TC0->TC_CHANNEL[1].TC_CMR = TC_CMR_TCCLKS_TIMER_CLOCK1 // capture mode, MCK/2 = 42 MHz, clk on rising edge
                              | TC_CMR_ABETRG              // TIOA is used as the external trigger
                              | TC_CMR_LDRA_RISING         // load RA on rising edge of trigger input
                              | TC_CMR_LDRB_FALLING;       // load RB on falling edge of trigger input
                              
  // If you want to capture PWM data from TC1_Handler()
  TC0->TC_CHANNEL[1].TC_IER |= TC_IER_LDRAS | TC_IER_LDRBS; // Trigger interruption on Load RA and load RB
  
  TC0->TC_CHANNEL[1].TC_CCR = TC_CCR_SWTRG | TC_CCR_CLKEN; // Reset TC counter and enable

  NVIC_EnableIRQ(TC1_IRQn);                                // Enable TC1 interrupts
  */
  #endif //MODEM


  digitalWrite(VENTILATORLED, LOW);    // turn the LED off by making the voltage LOW
}

//main program loop
void loop()
{
  SerialUSB.println("Ventilator white box tests... \n");  //Serial Monitor test.
  // wait for a second
  //
  // put your main code here, to run repeatedly:
  //Peripheral Test, LED. Very useful to know that the code is actually running without JTAG capability
  digitalWrite(VENTILATORLED, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);//500);                       // wait for a second
  digitalWrite(VENTILATORLED, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);//500); 

  
 

}


#ifdef MODEM
//================== MODEM FREQUENCY DETECT (REQUIRES A FUNCTION GENERATOR PLUGED INTO MODEM ========
//See: https://forum.arduino.cc/index.php?topic=480228.0
// Note that you could either test status register by polling in loop()
void TC1_Handler() {

 

}
#endif
