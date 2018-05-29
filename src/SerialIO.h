#ifndef SerialIO_h
#define SerialIO_h

#include "StateVariable.h"

namespace LinearPositionControl {

void waitForSerialHandshake(char handshakeChar = '~', unsigned long waitDelay = 200);

class IntParser {
  public:
    IntParser(char startDelimiter = '[', char endDelimiter = ']');

    SimpleStateVariable<int> result;
    bool justUpdated = false;

    void setup();
    void update();

  private:
    char startDelimiter;
    char endDelimiter;

    int receivedNumber;
    bool negative = false;

    SimpleStateVariable<char> received;
};

}

#endif

