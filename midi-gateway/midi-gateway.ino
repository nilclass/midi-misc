#include <MIDIUSB.h>

void setup() {
  Serial.begin(31250);
}

static void forward(midiEventPacket_t packet, int len) {
  byte buf[3] = {
    packet.byte1,
    packet.byte2,
    packet.byte3
  };
  Serial.write(buf, len);
  Serial.flush();
}

void loop() {
  midiEventPacket_t rx;
  for (;;) {
    rx = MidiUSB.read();
    if (rx.header == 0) {
      break;
    }
    // cable number (not used for anything here)
    //byte cn = rx.header >> 4 & 0xF;
    
    // code index number
    byte cin = rx.header & 0xF;

    switch (cin) {
      case 0x0: // misc function codes; reserved
      case 0x1: // cable events; reserved
        break; // ignore

      case 0x2: // two-byte system common message
      case 0x6: // SysEx ends with following two bytes
      case 0xC: // program-change
      case 0xD: // channel pressure
        forward(rx, 2);
        break;
        
      case 0x3: // three-byte system common message
      case 0x4: // SysEx starts or continues
      case 0x8: // note-off
      case 0x9: // note-on
      case 0xA: // poly-keypress
      case 0xB: // control-change
      case 0xE: // pitchbend change
        forward(rx, 3);
        break;
      
      case 0x5: // SysEx ends with following single byte (or single-byte system common message)
      case 0xF: // single byte
       forward(rx, 1);
       break;
    }
  }
}
