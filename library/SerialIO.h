#ifndef SerialIO_h
#define SerialIO_h

#include "StateVariable.h"

namespace LinearPositionControl {

void waitForSerialHandshake(char handshakeChar = '~', unsigned long waitDelay = 200);

class IntParser {
  public:
    IntParser(char startDelimiter = '[', char endDelimiter = ']');

    StateVariable<int> result;

    void setup();
    void update();

  private:
    char startDelimiter;
    char endDelimiter;

    int receivedNumber;
    bool negative = false;

    StateVariable<char> received;
};

}

#endif

