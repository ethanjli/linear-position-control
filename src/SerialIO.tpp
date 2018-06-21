#ifndef SerialIO_tpp
#define SerialIO_tpp

namespace LinearPositionControl {

void waitForSerialHandshake(char handshakeChar, unsigned long waitDelay) {
  while (!Serial) {;}
  while (Serial.available() < 1) {
    Serial.print(handshakeChar);
    delay(waitDelay);
  }
  while (Serial.available() > 0) {
    if (Serial.read() == '\n') break;
  }
  delay(waitDelay);
}

// IntParser

IntParser::IntParser(char startDelimiter, char endDelimiter) :
  startDelimiter(startDelimiter), endDelimiter(endDelimiter) {}

void IntParser::setup() {
  result.setup(0);
}

void IntParser::update() {
  result.update(result.current);
  while (Serial.available() > 0) {
    char current = Serial.read();
    received.update(current);
    if (current == endDelimiter || current == startDelimiter) {
      if (current == endDelimiter) {
        if (negative) result.update(-1 * receivedNumber, true);
        else result.update(receivedNumber, true);
        justUpdated = true;
      }
      receivedNumber = 0;
      negative = false;
    } else if (current == '-' && received.previouslyAt(startDelimiter)) {
      negative = true;
    } else if (current >= '0' && current <= '9') {
      receivedNumber *= 10;
      receivedNumber += current - '0';
    }
  }
}

}

#endif

