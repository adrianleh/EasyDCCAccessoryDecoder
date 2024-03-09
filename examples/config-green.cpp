#ifndef CONFIG
#define CONFIG

#include "TurnoutContainer.cpp"
#include "SignalContainer.cpp"

// Set number of turnouts and signals

#define NO_SIGNALS 0
#define NO_TURNOUTS 6

// Uncomment to print all DCC Packets
//#define NOTIFY_DCC_MSG

// Define the Arduino input Pin number for the DCC Signal
#define DCC_PIN     2



inline void initContainers(TurnoutContainer* turnoutContainer, SignalContainer* signalContainer) {
  // This is an example please edit as needed
  addr_t turnoutBaseAddr = 170;
  addr_t signalBaseAddr = 200;
  auto *mltplx = new MultiplexOutput(6,5,4,4,3);
  turnoutContainer->add(new Turnout(turnoutBaseAddr + 5, mltplx, 0, 1));
  turnoutContainer->add(new Turnout(turnoutBaseAddr + 6, mltplx, 3, 2));
  turnoutContainer->add(new Turnout(turnoutBaseAddr + 7, mltplx, 4, 5));
  turnoutContainer->add(new Turnout(turnoutBaseAddr + 8, mltplx, 6, 7));

  // Track 1
  auto *t6 = new Track(19);
  auto *s10 = new TrackBlockSignal(signalBaseAddr + 10, true, t6, 0, 0);
  signalContainer->add(s10);

 // Non- controlling signals
 auto *s5 = new DistantSignal(signalBaseAddr + 3, true, NULL, 0, 0, 0, 0); // Distant for S3
 auto *s14 = new DistantSignal(signalBaseAddr + 11, true, NULL, 0, 0, 0, 0); // Distant for S 11
 signalContainer->add(s5);
 signalContainer->add(s14);

}

#endif
