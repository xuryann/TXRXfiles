// FOR RYAN AND SHWET

#include <DueTimer.h>

#include <Arduino.h>
#define TX_PIN (13)
#define RX_PIN (2)

/*#define HIGH_FREQ 1000
#define LOW_FREQ 2000
#define PERIOD 8000*/
#define SOFT_MODEM_BAUD_RATE   (1225) /*symbol rate*/
#define SOFT_MODEM_LOW_FREQ    (4900) /*we need multiple of these to represent a symbol Probe R26 to see TX, C46 fgor RX*/
#define SOFT_MODEM_HIGH_FREQ   (7350) 

#define SOFT_MODEM_1_PERIOD_US 136 /*7.35KHz*/
#define SOFT_MODEM_0_PERIOD_US 204 /*4.9Khz*/

/*
#define FREQ_ERR_MARGIN 100
#define PREAMBLE_LENGTH 49

#define STANDBY_STATE 0
#define PREAMBLE_STATE 1
#define RECEIVING_STATE 2


volatile long unsigned int * _txPortReg;
uint8_t _txPortMask;
volatile unsigned long previousMicros=0;
volatile unsigned long int currFreq = 0;
volatile uint8_t recvBuffer[8];
volatile int recvState = 0; 
volatile int bitIndex = 0;
volatile int bufferIndex = 0;
volatile bool started = false;

inline void digitalWriteDirect(int pin, boolean val){
  if(val) g_APinDescription[pin].pPort -> PIO_SODR = g_APinDescription[pin].ulPin;
  else    g_APinDescription[pin].pPort -> PIO_CODR = g_APinDescription[pin].ulPin;
}

inline int digitalReadDirect(int pin){
  return !!(g_APinDescription[pin].pPort -> PIO_PDSR & g_APinDescription[pin].ulPin);
}
*/

/*================================================================================*\
|*    CODE TO DRIVE MODEM PINS  
\*================================================================================*/

inline void digitalWriteDirect(int pin, boolean val){
  if(val) g_APinDescription[pin].pPort -> PIO_SODR = g_APinDescription[pin].ulPin;
  else    g_APinDescription[pin].pPort -> PIO_CODR = g_APinDescription[pin].ulPin;
}

void sendBit(bool bit)
{
  if(bit)
  {
    digitalWriteDirect(TX_PIN, HIGH); // sets the pin on
    delayMicroseconds(SOFT_MODEM_1_PERIOD_US/2);      // pauses for 50 microseconds
    digitalWriteDirect(TX_PIN, LOW);  // sets the pin off
    delayMicroseconds(SOFT_MODEM_1_PERIOD_US/2);      // pauses for 50 microseconds
  }
  else
  {
    digitalWriteDirect(TX_PIN, HIGH); // sets the pin on
    delayMicroseconds(SOFT_MODEM_0_PERIOD_US/2);      // pauses for 50 microseconds
    digitalWriteDirect(TX_PIN, LOW);  // sets the pin off
    delayMicroseconds(SOFT_MODEM_0_PERIOD_US/2);      // pauses for 50 microseconds    
  }
}

/*================================================================================*\
|*    PROGRAM  
\*================================================================================*/


void setup() {

  pinMode(RX_PIN, INPUT);
  //digitalWrite(RX_PIN, LOW);

  pinMode(TX_PIN, OUTPUT);
  digitalWrite(TX_PIN, LOW);



//For now do not setup demodulation handler
/*
  attachInterrupt(13, myfreqhandler, RISING);
  */
}

//maybe repurpose later
/*
void myfreqhandler() // interrupt handler
{
  //If message is starting, set up periodic freq check
  if(!started){
    started = true;
    recvState = STANDBY_STATE;
    Timer2.attachInterrupt(setFreqCheck).start(PERIOD/2);
  }
  //Update current frequency
  unsigned long int currMicros = micros();
  currFreq = currMicros - previousMicros;
  previousMicros = currMicros;
}

void setFreqCheck() //Set up periodic checks of frequency
{
  Timer3.attachInterrupt(mybufferhandler).start(PERIOD);
  Timer2.detachInterrupt();
  Timer2.stop();
}
*/


void loop() 
{
  int preamble = 49;
  
  //do some delay to make it easy to see the difference in whats going on
  
  //Preamble, send 49 1's
  while(preamble--)
    sendBit(1);

  //Send symbol: Send start bit (0)
  sendBit(0);
  
  //Now send bits of character (LSB)
  sendBit(1);                       //LSB Send ASCI '7'
  sendBit(0);
  sendBit(1);
  sendBit(0);
  sendBit(1);
  sendBit(0);
  sendBit(1);
  sendBit(0);                       //MSB
 
  //Now send stop bit (1)
  sendBit(1);

  //loop for more characters....
  //N/A
  //Now send message termination bit (1)
  sendBit(1);
}
