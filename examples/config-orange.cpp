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
  auto *mltplx = new MultiplexOutput(7,6,5,4,3);
  turnoutContainer->add(new Turnout(turnoutBaseAddr + 11, mltplx, 0, 1));
  turnoutContainer->add(new Turnout(turnoutBaseAddr + 12, mltplx, 3, 2));
  turnoutContainer->add(new Turnout(turnoutBaseAddr + 1, mltplx, 4, 5));
  turnoutContainer->add(new Turnout(turnoutBaseAddr + 2, mltplx, 6, 7));
  turnoutContainer->add(new Turnout(turnoutBaseAddr + 3, mltplx, 8, 9));
  turnoutContainer->add(new Turnout(turnoutBaseAddr + 4, mltplx, 11, 10));

  // Track 1
  auto *t1 = new Track(61);
  auto *s1 = new ExitSignal(signalBaseAddr + 1, true, t1, 0, 0, 0, 0, 0);
  signalContainer->add(s1);
  // Track 5
  auto *t5 = new Track(54);
  auto *s2 = new TrackBlockSignal(signalBaseAddr + 2, true, t5, 0, 0);
  signalContainer->add(s2);
  // Track 2
  auto *t2 = new Track(60);
  auto *s3 = new ExitSignal(signalBaseAddr + 3, true, t2, 0, 0, 0, 0, 0);
  auto *s4 = new TrackBlockSignal(signalBaseAddr + 4, true, t2, 0, 0);
  signalContainer->add(s3);
  signalContainer->add(s4);
  // Track 3
  auto *t3 = new Track(59);
  auto *s6 = new ExitSignal(signalBaseAddr + 6, true, t3, 0, 0, 0, 0, 0);
  auto *s7 = new ExitSignal(signalBaseAddr + 7, true, t3, 0, 0, 0, 0, 0);
  signalContainer->add(s6);
  signalContainer->add(s7);
  // Track 4
  auto *t4 = new Track(58);
  auto *s8 = new ExitSignal(signalBaseAddr + 8, true, t4, 0, 0, 0, 0, 0);
  auto *s9 = new ExitSignal(signalBaseAddr + 9, true, t4, 0, 0, 0, 0, 0);
  signalContainer->add(s8);
  signalContainer->add(s9);
  // Track 10
  auto *t10 = new Track(56);
  auto *s12 = new DistantSignal(signalBaseAddr + 13, true, t10, 0, 0, 0, 0); // Address for signal 13, since distant
  signalContainer->add(s12);
  // Track 11
  auto *t11 = new Track(57);
  auto *s11 = new EntrySignal(signalBaseAddr + 11, true, t11, 0, 0, 0);
  signalContainer->add(s11);
  // Track 24
  auto *t24 = new Track(55);
  auto *s21 = new BlockSignal(signalBaseAddr + 11, true, t24, 0, 0);
  auto *s21_distant_for_s9 = new DistantSignal(s9->getAddress(), true, NULL, 0, 0, 0, 0, s21);
  auto *s22 = new DistantSignal(signalBaseAddr + 24, true, t24, 0, 0, 0, 0); // Address for signal 24, since distant
  signalContainer->add(s21);
  signalContainer->add(s21_distant_for_s9);
  signalContainer->add(s22);
}

#endif
