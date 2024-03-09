#define TurnoutDecoder

#include <NmraDcc.h>


#ifdef TurnoutDecoder
#include "Multiplex.cpp"
#include "TurnoutContainer.cpp"
#endif

// This Example shows how to use the library as a DCC Accessory Decoder or a DCC Signalling Decoder
// It responds to both the normal DCC Turnout Control packets and the newer DCC Signal Aspect packets
// You can also print every DCC packet by uncommenting the "#define NOTIFY_DCC_MSG" line below

NmraDcc  Dcc ;
DCC_MSG  Packet ;

// Define the Arduino input Pin number for the DCC Signal
#define DCC_PIN     2

// Uncomment to print all DCC Packets
//#define NOTIFY_DCC_MSG
#define TurnoutDecoder
#ifdef TurnoutDecoder
  #define NO_TURNOUTS 13
  #if NO_TURNOUTS >= 24 
    #error "Only 24 turnouts supported!"
  #endif
  #define BASE_ADDR 200
  
  static MultiplexOutput** mults = static_cast<MultiplexOutput**>(malloc(((NO_TURNOUTS / 8) + 1) * sizeof(MultiplexOutput*)));

  static TurnoutContainer* turnoutContainer = new TurnoutContainer(NO_TURNOUTS);
  
  void initContainers() {
    mults[0] = new MultiplexOutput(3,4,5,6,7);
    #if NO_TURNOUTS >= 8
    mults[1] = new MultiplexOutput(8,9,10,11,12);
    #endif
    #if NO_TURNOUTS >= 16
    mults[2] = new MultiplexOutput(13,14,15,16,17);
    #endif
    for(uint8_t i = 0; i < NO_TURNOUTS; i++) {
      auto mult = mults[i / 8];
      auto mult_idx = i % 8;
      turnoutContainer->add(new Turnout(BASE_ADDR + i, mult, 2 * mult_idx, 2 * mult_idx + 1));
    }     
  }
#endif

struct CVPair
{
  uint16_t  CV;
  uint8_t   Value;
};

CVPair FactoryDefaultCVs [] =
{
  {CV_ACCESSORY_DECODER_ADDRESS_LSB, DEFAULT_ACCESSORY_DECODER_ADDRESS & 0xFF},
  {CV_ACCESSORY_DECODER_ADDRESS_MSB, DEFAULT_ACCESSORY_DECODER_ADDRESS >> 8},
};

uint8_t FactoryDefaultCVIndex = 0;

void notifyCVResetFactoryDefault()
{
  // Make FactoryDefaultCVIndex non-zero and equal to num CV's to be reset
  // to flag to the loop() function that a reset to Factory Defaults needs to be done
  FactoryDefaultCVIndex = sizeof(FactoryDefaultCVs) / sizeof(CVPair);
};

const int DccAckPin = 3 ;

// This function is called by the NmraDcc library when a DCC ACK needs to be sent
// Calling this function should cause an increased 60ma current drain on the power supply for 6ms to ACK a CV Read
void notifyCVAck(void)
{
  Serial.println("notifyCVAck") ;

  digitalWrite( DccAckPin, HIGH );
  delay( 6 );
  digitalWrite( DccAckPin, LOW );
}


#ifdef  NOTIFY_DCC_MSG
void notifyDccMsg( DCC_MSG * Msg)
{
  Serial.print("notifyDccMsg: ") ;
  for (uint8_t i = 0; i < Msg->Size; i++)
  {
    Serial.print(Msg->Data[i], HEX);
    Serial.write(' ');
  }
  Serial.println();
}
#endif

// This function is called whenever a normal DCC Turnout Packet is received and we're in Board Addressing Mode
void notifyDccAccTurnoutBoard( uint16_t BoardAddr, uint8_t OutputPair, uint8_t Direction, uint8_t OutputPower )
{
  Serial.print("notifyDccAccTurnoutBoard: ") ;
  Serial.print(BoardAddr, DEC) ;
  Serial.print(',');
  Serial.print(OutputPair, DEC) ;
  Serial.print(',');
  Serial.print(Direction, DEC) ;
  Serial.print(',');
  Serial.println(OutputPower, HEX) ;
}

// This function is called whenever a normal DCC Turnout Packet is received and we're in Output Addressing Mode
void notifyDccAccTurnoutOutput( uint16_t Addr, uint8_t Direction, uint8_t OutputPower )
{
#ifdef TurnoutDecoder
  Serial.print("notifyDccAccTurnoutOutput: ") ;
  Serial.print(Addr, DEC) ;
  Serial.print(',');
  Serial.print(Direction, DEC) ;
  Serial.print(',');
  Serial.println(OutputPower, HEX) ;
  turnoutContainer->transition(Addr, static_cast<TurnoutState>(Direction));
#endif
 }
// This function is called whenever a DCC Signal Aspect Packet is received
void notifyDccSigOutputState( uint16_t Addr, uint8_t State)
{
  Serial.print("notifyDccSigOutputState: ") ;
  Serial.print(Addr, DEC) ;
  Serial.print(',');
  Serial.println(State, HEX) ;
}

void setup()
{
  Serial.begin(115200);
  uint8_t maxWaitLoops = 255;
  while (!Serial && maxWaitLoops--) {
    delay(20);
  }
  Serial.println("Launching...");
  delay(200);
  initContainers();

  // Configure the DCC CV Programing ACK pin for an output
  pinMode( DccAckPin, OUTPUT );

  Serial.println("NMRA DCC Example 1");

  // Setup which External Interrupt, the Pin it's associated with that we're using and enable the Pull-Up
  // Many Arduino Cores now support the digitalPinToInterrupt() function that makes it easier to figure out the
  // Interrupt Number for the Arduino Pin number, which reduces confusion.
#ifdef digitalPinToInterrupt
  Dcc.pin(DCC_PIN, 0);
#else
  Dcc.pin(0, DCC_PIN, 1);
#endif

//   Call the main DCC Init function to enable the DCC Receiver
  Dcc.init( MAN_ID_DIY, 10, CV29_ACCESSORY_DECODER | CV29_OUTPUT_ADDRESS_MODE, 0 );

  Serial.println("Init Done");
}
int muxAddr = 0;
void loop()
{
//   You MUST call the NmraDcc.process() method frequently from the Arduino loop() function for correct library operation
  Dcc.process();
  if ( FactoryDefaultCVIndex && Dcc.isSetCVReady())
  {
    FactoryDefaultCVIndex--; // Decrement first as initially it is the size of the array
    Dcc.setCV( FactoryDefaultCVs[FactoryDefaultCVIndex].CV, FactoryDefaultCVs[FactoryDefaultCVIndex].Value);
  }
}