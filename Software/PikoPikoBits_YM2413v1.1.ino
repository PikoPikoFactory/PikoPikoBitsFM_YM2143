//---------------------------
//   PikoPikoBits FM
//   YM2413
//   Sample Program
//   ver 0.3
//   2024/1/5
//   by Synth Senpai
//---------------------------

#include <MIDI.h>
USING_NAMESPACE_MIDI

// Pin Define
const int D_PINS[] = { 2, 3, 4, 5, 6, 7, 8, 9 };
const int CS_PIN = 12;
const int A0_PIN = 10;
const int IC_PIN = 13;
const int WE_PIN = 11;
const byte SW1_PIN = A0;
const byte SW2_PIN = A1;
const byte LED1_PIN = A2;
const byte LED2_PIN = A3;
const byte LED3_PIN = A4;
const byte DSW_Pin = A5;

int dsw_val;
int sw1_state, sw2_state ;

uint8_t maxvoice = 9;
uint8_t note_num[16];
uint8_t note_ch[16];

const int MAXDRUM = 30 ;

const int drum_map[][2] = {
  {35, 0B10000}, {36, 0B10000}, {37, 0B01000}, {38, 0B01000}, {39, 0B01000}, {40, 0B01000}, {41, 0B00100},
  {42, 0B00001}, {43, 0B00100}, {44, 0B00001}, {45, 0B00100}, {46, 0B00001}, {47, 0B00100}, {48, 0B00100},
  {49, 0B00010}, {50, 0B00100}, {51, 0B00010}, {52, 0B00010}, {53, 0B00010}, {54, 0B00010}, {55, 0B00010},
  {56, 0B00100}, {57, 0B00010}, {58, 0B00010}, {59, 0B00010}, {60, 0B00100}, {61, 0B00100}, {62, 0B00100},
  {63, 0B00100}, {64, 0B00100}
};

const uint8_t tone_map[128][2] = {
  {3, 0}, {3, 1}, {3, 2}, {3, 3}, {3, 4}, {3, 5}, {11, 6}, {11, 7},
  {11, 8}, {11, 9}, {11, 10}, {12, 11}, {12, 12}, {12, 13}, {12, 14}, {12, 15},
  {8, 0}, {8, 1}, {8, 2}, {8, 3}, {8, 4}, {8, 5}, {8, 6}, {8, 7},
  {2, 8}, {2, 9}, {2, 10}, {2, 11}, {2, 12}, {2, 13}, {2, 14}, {2, 15},
  {14, 0}, {15, 1}, {15, 2}, {14, 3}, {13, 4}, {13, 5}, {13, 6}, {13, 7},
  {1, 8}, {1, 9}, {1, 10}, {1, 11}, {1, 12}, {11, 13}, {11, 14}, {3, 15},
  {1, 0}, {1, 1}, {1, 2}, {1, 3}, {1, 4}, {1, 5}, {1, 6}, {3, 7},
  {7, 8}, {7, 9}, {7, 10}, {7, 11}, {9, 12}, {7, 13}, {7, 14}, {7, 15},
  {6, 0}, {6, 1}, {6, 2}, {6, 3}, {6, 4}, {9, 5}, {9, 6}, {5, 7},
  {4, 8}, {4, 9}, {4, 10}, {4, 11}, {4, 12}, {4, 13}, {4, 14}, {4, 15},
  {10, 0}, {10, 1}, {10, 2}, {10, 3}, {10, 4}, {10, 5}, {10, 6}, {10, 7},
  {10, 8}, {10, 9}, {10, 10}, {10, 11}, {10, 12}, {10, 13}, {10, 14}, {10, 15},
  {3, 0}, {3, 1}, {3, 2}, {3, 3}, {3, 4}, {3, 5}, {3, 6}, {3, 7},
  {11, 8}, {11, 9}, {11, 10}, {11, 11}, {11, 12}, {11, 13}, {11, 14}, {11, 15},
  {3, 0}, {3, 1}, {3, 2}, {3, 3}, {3, 4}, {3, 5}, {3, 6}, {3, 7},
  {3, 8}, {3, 9}, {3, 10}, {3, 11}, {3, 12}, {3, 13}, {3, 14}, {3, 15}
};

const uint8_t tone0data[][8] =
{
  {0x31, 0x22, 0x23, 0x07, 0xF0, 0xF0, 0xE8, 0xF7}, //0
  {0x03, 0x31, 0x68, 0x05, 0xF2, 0x74, 0x79, 0x9C},
  {0x01, 0x51, 0x72, 0x04, 0xF1, 0xD3, 0x9D, 0x8B},
  {0x22, 0x61, 0x1B, 0x05, 0xC0, 0xA1, 0xF8, 0xE8},
  {0x22, 0x61, 0x2C, 0x03, 0xD2, 0xA1, 0xA7, 0xE8},
  {0x31, 0x22, 0xFA, 0x01, 0xF1, 0xF1, 0xF4, 0xEE},
  {0x21, 0x61, 0x28, 0x06, 0xF1, 0xF1, 0xCE, 0x9B},
  {0x27, 0x61, 0x60, 0x00, 0xF0, 0xF0, 0xFF, 0xFD},
  {0x60, 0x21, 0x2B, 0x06, 0x85, 0xF1, 0x79, 0x9D},
  {0x31, 0xA1, 0xFF, 0x0A, 0x53, 0x62, 0x5E, 0xAF},
  {0x03, 0xA1, 0x70, 0x0F, 0xD4, 0xA3, 0x94, 0xBE},
  {0x2B, 0x61, 0xE4, 0x07, 0xF6, 0x93, 0xBD, 0xAC},
  {0x21, 0x63, 0xED, 0x07, 0x77, 0xF1, 0xC7, 0xE8},
  {0x21, 0x61, 0x2A, 0x03, 0xF3, 0xE2, 0xB6, 0xD9},
  {0x21, 0x63, 0x37, 0x03, 0xF3, 0xE2, 0xB6, 0xD9},//14
  //
  {0x49, 0x4c, 0x4c, 0x32, 0x00, 0x00, 0x00, 0x00},//15
  {0x61, 0x61, 0x1e, 0x17, 0xf0, 0x7f, 0x00, 0x17},
  {0x13, 0x41, 0x16, 0x0e, 0xfd, 0xf4, 0x23, 0x23},
  {0x03, 0x01, 0x9a, 0x04, 0xf3, 0xf3, 0x13, 0xf3},
  {0x11, 0x61, 0x0e, 0x07, 0xfa, 0x64, 0x70, 0x17},
  {0x22, 0x21, 0x1e, 0x06, 0xf0, 0x76, 0x00, 0x28},
  {0x21, 0x22, 0x16, 0x05, 0xf0, 0x71, 0x00, 0x18},
  {0x21, 0x61, 0x1d, 0x07, 0x82, 0x80, 0x17, 0x17},
  {0x23, 0x21, 0x2d, 0x16, 0x90, 0x90, 0x00, 0x07},
  {0x21, 0x21, 0x1b, 0x06, 0x64, 0x65, 0x10, 0x17},
  {0x21, 0x21, 0x0b, 0x1a, 0x85, 0xa0, 0x70, 0x07},
  {0x23, 0x01, 0x83, 0x10, 0xff, 0xb4, 0x10, 0xf4},
  {0x97, 0xc1, 0x20, 0x07, 0xff, 0xf4, 0x22, 0x22},
  {0x61, 0x00, 0x0c, 0x05, 0xc2, 0xf6, 0x40, 0x44},
  {0x01, 0x01, 0x56, 0x03, 0x94, 0xc2, 0x03, 0x12},
  {0x21, 0x01, 0x89, 0x03, 0xf1, 0xe4, 0xf0, 0x23},
  {0x07, 0x21, 0x14, 0x00, 0xee, 0xf8, 0xff, 0xf8},
  {0x01, 0x31, 0x00, 0x00, 0xf8, 0xf7, 0xf8, 0xf7},
  {0x25, 0x11, 0x00, 0x00, 0xf8, 0xfa, 0xf8, 0x55},//33
  //
  {0x11, 0x11, 0x92, 0x0C, 0xF1, 0xF2, 0x33, 0x31},//34
  {0x31, 0x3C, 0x00, 0x11, 0xA2, 0xF0, 0x18, 0xEF},
  {0x8f, 0x88, 0x7f, 0xc7, 0xec, 0xe1, 0xb2, 0xf0},
  {0x22, 0x23, 0xc0, 0x86, 0xf0, 0xf3, 0x04, 0x08},
  {0x12, 0x11, 0x92, 0x0e, 0x85, 0xa2, 0x34, 0xf4},
  {0x25, 0x21, 0x5b, 0x0e, 0xf6, 0xc9, 0x66, 0x0a},
  {0x31, 0x11, 0x90, 0x0D, 0xC2, 0xA2, 0xB0, 0xA5},//40
  {0x03, 0x21, 0x59, 0x04, 0xF1, 0xF1, 0x38, 0x28},
  {0x03, 0x21, 0x47, 0x4B, 0xF7, 0x92, 0x23, 0x68},
  {0x03, 0x81, 0x85, 0x4A, 0x74, 0xF3, 0x55, 0x05},
  {0xF1, 0x21, 0x92, 0x15, 0xA5, 0xC3, 0x32, 0x47},
  {0x0f, 0x08, 0x00, 0x07, 0xf0, 0xf7, 0x00, 0x17},
  {0x0a, 0x07, 0xc0, 0xc9, 0xf0, 0xf7, 0x05, 0x75},
  {0x02, 0x22, 0x00, 0x00, 0xfc, 0xf0, 0x7f, 0x0f},
  {0x60, 0x63, 0x05, 0x13, 0x22, 0xF1, 0xF2, 0xA4},
  {0x50, 0x00, 0x12, 0x06, 0xF2, 0xFF, 0xF0, 0x00},
  {0xA5, 0x21, 0x15, 0x0E, 0x85, 0xF4, 0xAA, 0x2A},//50
  {0x03, 0x01, 0x0A, 0x06, 0xF1, 0xFF, 0xF0, 0x00},
  {0x63, 0x21, 0x20, 0x0D, 0x91, 0x61, 0x06, 0x07},
  {0x51, 0x61, 0x17, 0x07, 0x50, 0xf1, 0x5c, 0xdc},
  {0x61, 0x61, 0x1E, 0x17, 0xF0, 0x7F, 0x00, 0x17},
  {0x05, 0x03, 0x10, 0x06, 0x74, 0xA1, 0x13, 0xF4},
  {0x05, 0x01, 0x16, 0x00, 0xF9, 0xA2, 0x15, 0xF5},
  {0x01, 0x41, 0x11, 0x00, 0xA0, 0xA0, 0x83, 0x95},
  {0x01, 0x41, 0x17, 0x00, 0x60, 0xF0, 0x83, 0x95},
  {0x24, 0x41, 0x1F, 0x00, 0x50, 0xB0, 0x94, 0x94},
  {0x05, 0x01, 0x0B, 0x04, 0x65, 0xA0, 0x54, 0x95},//60
  {0x11, 0x41, 0x0E, 0x04, 0x70, 0xC7, 0x13, 0x10},
  {0x02, 0x44, 0x16, 0x06, 0xE0, 0xE0, 0x31, 0x35},
  {0x48, 0x22, 0x22, 0x07, 0x50, 0xA1, 0xA5, 0xF4},
  {0x05, 0xA1, 0x18, 0x00, 0xA2, 0xA2, 0xF5, 0xF5},
  {0x07, 0x81, 0x2B, 0x05, 0xA5, 0xA5, 0x03, 0x03},
  {0x01, 0x41, 0x08, 0x08, 0xA0, 0xA0, 0x83, 0x95},
  {0x21, 0x61, 0x12, 0x00, 0x93, 0x92, 0x74, 0x75},
  {0x21, 0x62, 0x21, 0x00, 0x84, 0x85, 0x34, 0x15},
  {0x21, 0x62, 0x0E, 0x00, 0xA1, 0xA0, 0x34, 0x15},//69

};

int tonemax = 70 ;
int toneno = 0;

//midi
MIDI_CREATE_INSTANCE(HardwareSerial, Serial, MIDI);

//int tone_no[16] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
int tone_no[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


int midi_ch = 1;
int fnum[12] = {172, 181, 192, 204, 216, 229, 242, 257, 272, 288, 305, 323};
int dipa[17] = {0, 33, 100, 166, 231, 296, 361, 426, 487, 548, 613, 677, 739, 802, 865, 928, 1023};
int dipstate[4];
int tonemode, drummode;

//***************************************************


void setup() {
  // Setup Pins
  for (auto i = 0; i < 8; i++) {
    pinMode(D_PINS[i], OUTPUT);
  }
  pinMode(CS_PIN, OUTPUT);
  pinMode(A0_PIN, OUTPUT);
  pinMode(IC_PIN, OUTPUT);
  pinMode(WE_PIN, OUTPUT);
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);

  digitalWrite(WE_PIN, LOW);

  // RESET IC
  digitalWrite(IC_PIN, LOW);
  delayMicroseconds(100);
  digitalWrite(IC_PIN, HIGH);
  delay(100);

  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);
  MIDI.setHandleControlChange( handleControlChange );
  MIDI.setHandleProgramChange( handleProgramChange );
  MIDI.setHandlePitchBend( handlePitchBend );
  MIDI.begin(MIDI_CHANNEL_OMNI);
  MIDI.turnThruOff();

  dipread();
  modeset();
  tonesend(toneno);

  opllwrite(0x0E, 0);

  if (drummode == 1)
  {
    maxvoice = 6;
    opllwrite(0x0E, 32);
    opllwrite(0x16, 0x20);
    opllwrite(0x17, 0x50);
    opllwrite(0x18, 0xC0);
    opllwrite(0x26, 0x05);
    opllwrite(0x27, 0x05);
    opllwrite(0x28, 0x01);
    drumvol(0, 0, 0, 0, 0);
  }
  digitalWrite(LED1_PIN, HIGH);
  delay(200);
  digitalWrite(LED1_PIN, LOW);
  delay(100);
  digitalWrite(LED2_PIN, HIGH);
  delay(200);
  digitalWrite(LED2_PIN, LOW);
  delay(100);
  digitalWrite(LED3_PIN, HIGH);
  delay(200);
  digitalWrite(LED3_PIN, LOW);
}

//************************************************
void loop()
{
  MIDI.read();
  sw1_state = digitalRead(SW1_PIN);
  sw2_state = digitalRead(SW2_PIN);
  if (sw1_state == 0)
  {
    midi_panic();
  }
  if (sw2_state == 0)
  {
    toneno++;
    if (toneno >= tonemax)
    {
      toneno = 0;
    }
    tonesend(toneno);
  }
}
//************************************************

void tonesend(int toneno)
{
  for ( auto i = 0 ; i < 8 ; i++)
  {
    opllwrite(i, tone0data[toneno][i]);
  }
}

void dipread()
{
  dsw_val = analogRead(DSW_Pin);
  for ( auto j = 0; j < 16 ; j++)
  {
    if ( dsw_val >= dipa[j] && dsw_val <= dipa[j + 1] )
    {
      for (auto k = 0 ; k < 4 ; k++)
      {
        dipstate[k] =   bitRead(j, k);
      }
    }
  }
}

void modeset()
{
  tonemode = dipstate[0];
  drummode = dipstate[1];
  //XXXXmode = dipstate[2];
  //XXXXmode = dipstate[3];
}


void handleNoteOn(byte channel, byte keyno, byte velocity)
{
  int inst;
  int vol;
  if (channel != 10 )
  {
    digitalWrite(LED1_PIN, HIGH);
    boolean keyflg = false ;
    keyflg = false ;
    for (auto ch = 0 ; ch < maxvoice ; ch++)
    {
      if (note_num[ch] == 0 and keyflg == false ) {
        keyflg = true;
        inst =   tone_no[channel];
        vol = (128 - velocity) / 16;
        //vol = 0;
        noteon(ch, keyno, inst, vol);
        note_num[ch] = keyno ;
        note_ch[ch] = channel ;
      }
    }
    digitalWrite(LED1_PIN, LOW);
  }
  if (channel == 10 && drummode == 1 )
  {
    for ( auto i = 0 ; i < 30 ; i++)
    {
      if (keyno == drum_map[i][0])
      {
        digitalWrite(LED2_PIN, HIGH);
        drum(drum_map[i][1]);
        digitalWrite(LED2_PIN, LOW);
      }
    }
  }
}

void handleNoteOff(byte channel, byte keyno, byte velocity)
{
  for (auto ch = 0 ; ch < maxvoice ; ch++)
  {
    if (note_num[ch] == keyno && note_ch[ch] == channel) {
      noteoff(ch, keyno);
      note_num[ch] = 0;
      note_ch[ch] = 0;
    }
  }
}

void handleControlChange(byte channel , byte number , byte value )
{
  switch (number) {
    case 120: // CC#120 All Sound Off
    case 123: // CC#123 All Notes Off
      if (drummode == 1)
      {
        drum(0);
      }
      for (auto k = 0 ; k < maxvoice ; k++)
      {
        noteoff(k, 0);
        note_num[k] = 0;
        note_ch[k] = 0;
      }
      break;
  }
}

void midi_panic()
{
  if (drummode == 1)
  {
    drum(0);
  }
  for (auto k = 0 ; k < maxvoice ; k++)
  {
    noteoff(k, 0);
    note_num[k] = 0;
    note_ch[k] = 0;
  }
}

void handleProgramChange(byte channel , byte number )
{
  tone_no[channel] = tone_map[number][tonemode];
}

void handlePitchBend(byte channel , int bend )
{

}

//******************************************************

void opllwrite(unsigned char adr, unsigned char dat)
{
  // address write
  digitalWrite(A0_PIN, LOW);
  opllwrite_data(adr);
  digitalWrite(CS_PIN, LOW);
  delayMicroseconds(12);
  digitalWrite(CS_PIN, HIGH);
  // data write
  digitalWrite(A0_PIN, HIGH);
  opllwrite_data(dat);
  digitalWrite(CS_PIN, LOW);
  delayMicroseconds(84);
  digitalWrite(CS_PIN, HIGH);
}

void opllwrite_data(unsigned char dat)
{
  for (auto i = 0; i < 8; i++) {
    digitalWrite(D_PINS[i], bitRead(dat, i));
  }
}

void noteon(int ch, int num, int inst, int vol) {
  // num = 12 - 107 (MIDI Note Numbers)
  // oct = 0 - 7
  int oct;
  if (num >= 12) {
    num = num - 12;
  };
  oct = num / 12;
  if (oct >= 8) {
    oct = 7;
  };
  num = fnum[num % 12];

  // Note(9ch) 0x10 - 15 (0 - 5)
  opllwrite((0x10 + ch), num);

  // Inst & Vol(9ch)
  opllwrite((0x30 + ch), (inst << 4) | vol);

  // NoteON & Oct & Note
  opllwrite((0x20 + ch), (16 | (oct << 1) | (num >> 8)));
}

void noteoff(int ch, int num) 
{
  int oct;
  if (num >= 12) {
    num = num - 12;
  };
  oct = num / 12;
  if (oct >= 8) {
    oct = 7;
  };
  num = fnum[num % 12];
  // NoteOFF & Oct & Note
  opllwrite((0x20 + ch), (0 | (oct << 1) | (num >> 8)));
}

void drumvol(int bd, int sd, int tom, int tcy, int hh) {
  opllwrite(0x36, bd);
  opllwrite(0x37, hh << 4 | sd);
  opllwrite(0x38, tom << 4 | tcy);
}

void drum(int val) {
  opllwrite(0x0E, (val + 32));
}
