//---------------------------
//   PikoPikoBits FM
//   YM2413
//   Sample Program
//   ver 0.2
//   2023/12/27
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

const int MAXVOICE[2] = {9, 6};
uint8_t note_num[16];
const int MAXDRUM = 30 ;

const int drum_map[][2] = {
  {35, 0B10000}, {36, 0B10000}, {37, 0B01000}, {38, 0B01000}, {39, 0B01000}, {40, 0B01000}, {41, 0B00100},
  {42, 0B00001}, {43, 0B00100}, {44, 0B00001}, {45, 0B00100}, {46, 0B00001}, {47, 0B00100}, {48, 0B00100},
  {49, 0B00010}, {50, 0B00100}, {51, 0B00010}, {52, 0B00010}, {53, 0B00010}, {54, 0B00010}, {55, 0B00010},
  {56, 0B00100}, {57, 0B00010}, {58, 0B00010}, {59, 0B00010}, {60, 0B00100}, {61, 0B00100}, {62, 0B00100},
  {63, 0B00100}, {64, 0B00100}
};

const int tone_map[128][2] = {
  {3, 1}, {3, 1}, {3, 2}, {3, 3}, {3, 4}, {3, 5}, {11, 6}, {11, 7},
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
  {3, 0}, {3, 1}, {0, 2}, {0, 3}, {3, 4}, {3, 5}, {3, 6}, {3, 7},
  {3, 8}, {3, 9}, {3, 10}, {3, 11}, {3, 12}, {3, 13}, {3, 14}, {3, 15}
};

const int tone0data[8] = { 33, 33, 63, 0, 255, 255, 255, 255};

//midi
MIDI_CREATE_INSTANCE(HardwareSerial, Serial, MIDI);

int tone_no[16] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
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

  // Drum Volume (BD, SD, TOM, TCY, HH) [0 - 15]
  drumvol(0, 0, 0, 0, 0);

  delay(1000);

  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);
  MIDI.setHandleControlChange( handleControlChange );
  MIDI.setHandleProgramChange( handleProgramChange );
  MIDI.setHandlePitchBend( handlePitchBend );

  MIDI.begin(MIDI_CHANNEL_OMNI);
  MIDI.turnThruOff();

  dipread();
  modeset();

  for ( auto i = 0 ; i < 8 ; i++)
  {
    opllwrite(i, tone0data[i]);
  }

  if (drummode == 1)
  {
    opllwrite(0x16, 0x20);
    opllwrite(0x17, 0x50);
    opllwrite(0x18, 0xC0);
    opllwrite(0x26, 0x05);
    opllwrite(0x27, 0x05);
    opllwrite(0x28, 0x01);
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
    // call function
  }
}
//************************************************

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

void handleNoteOn(byte channel, byte pitch, byte velocity)
{
  int ch;
  int num;
  int inst;
  int vol;
  int keyno ;
  if (channel != 10 )
  {
    digitalWrite(LED1_PIN, HIGH);
    boolean keyflg = false ;
    keyno = pitch;
    keyflg = false ;
    for (auto k = 0 ; k < MAXVOICE[drummode] ; k++)
    {
      if (note_num[k] == 0 and keyflg == false ) {
        keyflg = true;
        //note_on(k, keyno);
        ch =  k;
        num = pitch;
        inst =   tone_no[channel];
        //vol = (128 - velocity) / 16;
        vol = 0;
        noteon(ch, num, inst, vol);
        note_num[k] = keyno ;
      }
    }
    digitalWrite(LED1_PIN, LOW);
  }
  if (channel == 10 && drummode == 1 )
  {
    for ( auto i = 0 ; i < 30 ; i++)
    {
      if (pitch == drum_map[i][0])
      {
        digitalWrite(LED2_PIN, HIGH);
        drum(drum_map[i][1]);
        digitalWrite(LED2_PIN, LOW);
      }
    }
  }
}

void handleNoteOff(byte channel, byte pitch, byte velocity)
{
  int keyno ;
  if (channel = midi_ch)
  {
    keyno = pitch ;
    for (auto k = 0 ; k < MAXVOICE[drummode] ; k++)
    {
      if (note_num[k] == keyno) {
        noteoff(k);
        note_num[k] = 0;
      }
    }
  }
}

void handleControlChange(byte channel , byte number , byte value )
{
  switch (number) {
    case 120: // CC#120 All Sound Off
    case 123: // CC#123 All Notes Off
      drum(0);
      for (auto k = 0 ; k < MAXVOICE[drummode] ; k++)
      {
        noteoff(k);
        note_num[k] = 0;
      }
      break;
  }
}

void midi_panic()
{
  drum(0);
  for (auto k = 0 ; k < MAXVOICE[drummode] ; k++)
  {
    noteoff(k);
    note_num[k] = 0;
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

void noteoff(int ch) {
  // 0x20 - 25 (0 - 5)
  opllwrite((0x20 + ch), 0);
}

void drumvol(int bd, int sd, int tom, int tcy, int hh) {
  opllwrite(0x36, bd);
  opllwrite(0x37, hh << 4 | sd);
  opllwrite(0x38, tom << 4 | tcy);
}

void drum(int val) {
  opllwrite(0x0E, 0);
  opllwrite(0x0E, (val + 32));
}
