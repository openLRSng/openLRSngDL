

#if 0
uint32_t rf_magic;
uint8_t  flags;
uint8_t  packetSize;
#endif

static bool cliActive = 0;
static char linebuf[16];

void show()
{
  Serial.print("OpenLRSngDataLink ");
  printVersion(version);
  Serial.print(" module type ");
  Serial.println(bind_data.rfmType);
  Serial.print("a) serial baudrate: ");
  Serial.println(bind_data.serial_baudrate);
  Serial.print("b) base frequency:  ");
  Serial.println(bind_data.rf_frequency);
  Serial.print("c) max frequency:   ");
  Serial.println(bind_data.maxFrequency);
  Serial.print("d) channel spacing: ");
  Serial.println(bind_data.rf_channel_spacing);
  Serial.print("e) RF power:        ");
  Serial.println(bind_data.rf_power);
  Serial.print("f) RF parameters:   ");
  Serial.print(bind_data.modem_params);
  Serial.print(" (");
  Serial.print(modem_params[bind_data.modem_params].bps);
  Serial.println("bps)");
  Serial.print("g) packet size:     ");
  Serial.println(bind_data.packetSize);
  Serial.print("h) magic:           ");
  Serial.println(bind_data.rf_magic);
  Serial.print("i) hopschs: ");
  for (uint8_t i=0; (i<MAXHOPS) && (bind_data.hopchannel[i]); i++) {
    if (i) {
      Serial.print(',');
    }
    Serial.print(bind_data.hopchannel[i]);
  }
  Serial.println();
  Serial.println();
  Serial.println("[s]ave [q]uit [p]rint ch freqs");
  Serial.println("Randomize hops per: [r]ssi [u]random");
}

#define INVALID 0xffffffff

uint32_t getValue(const char *q, uint32_t min, uint32_t max)
{
  bool done = false;
  uint8_t len=0;
  linebuf[0] = 0;
  while (!done) {
    Serial.print("\r");
    Serial.print(q);
    Serial.print(" (");
    Serial.print(min);
    Serial.print("-");
    Serial.print(max);
    Serial.print("): ");
    Serial.print(linebuf);
    Serial.print("  \b\b");
    while (!Serial.available());
    char c = Serial.read();
    if ((c >= '0') && (c <= '9')) {
      if (len < 15) {
        linebuf[len++]=c;
        linebuf[len]=0;
      }
    } else if (c == 8) {
      if (len > 0) {
        linebuf[--len]=0;
      }
    } else if (c == 13) {
      done = true;
    }
  }
  char *endptr;
  uint32_t v = strtoul(linebuf, &endptr, 0);
  if (*endptr != 0) {
    v = INVALID;
  }
  if (linebuf[0] == 0) {
    v = INVALID;
  }
  if ((v > max) || (v < min)) {
    v = INVALID;
  }
  return v;
}

void handleCLI()
{
  cliActive = 1;

  while (Serial.available() && Serial.read());

  while (cliActive) {
    show();
    while (!Serial.available());
    char c = tolower(Serial.read());
    bool valid = false;
    bool validShow = true;
    uint32_t   v;
    switch (c) {
    case 'a':
      v = getValue("New baudrate", 1200, 115200);
      if (v != INVALID) {
        bind_data.serial_baudrate = v;
        valid = true;
      }
      break;
    case 'b':
      v = getValue("New basefreq",MIN_RFM_FREQUENCY, (bind_data.maxFrequency - 100000UL));
      if (v != INVALID) {
        bind_data.rf_frequency = v;
        valid = true;
      }
      break;
    case 'c':
      v = getValue("New maxfreq", (bind_data.rf_frequency + 100000UL), MAX_RFM_FREQUENCY);
      if (v != INVALID) {
        bind_data.maxFrequency = v;
        valid = true;
      }
      break;
    case 'd':
      v = getValue("New spacing", 2, 20);
      if (v != INVALID) {
        bind_data.rf_channel_spacing = v;
        valid = true;
      }
      break;
    case 'e':
      v = getValue("New RF power", 0, 7);
      if (v != INVALID) {
        bind_data.rf_power = v;
        valid = true;
      }
      break;
    case 'f':
      v = getValue("New RF params", 0, (sizeof(modem_params)/sizeof(modem_params[0]))-1);
      if (v != INVALID) {
        bind_data.modem_params = v;
        valid = true;
      }
      break;
    case 'g':
      v = getValue("New packet size", 9, 33);
      if (v != INVALID) {
        bind_data.packetSize = v;
        valid = true;
      }
      break;
    case 'h':
      v = getValue("New MAGIC", 0, 0xfffffffe);
      if (v != INVALID) {
        bind_data.rf_magic = v;
        valid = true;
      }
      break;
    case 'i': {
      Serial.println("Enter channels one by one (1-254), invalid value or just enter to finish");
      uint8_t i=0;
      do {
        char q[7];
        snprintf(q,7,"CH%02d: ",i);
        v = getValue(q,1,254);
        if (v != INVALID) {
          bool chvalid = true;
          for (int j = 0; j < i; j++) {
            if (bind_data.hopchannel[j] == v) {
              Serial.println("Cannot use same channel twice\n");
              chvalid = false;
            }
          }
          if ((bind_data.rf_frequency + 10000UL * bind_data.rf_channel_spacing * v) >
              bind_data.maxFrequency) {
            Serial.println("Channel violates max freq limit\n");
            chvalid = false;
          }
          if (chvalid) {
            bind_data.hopchannel[i++] = v;
            Serial.println();
            if (i >= MAXHOPS) {
              v = 0;
            }
          }
        } else {
          if (i) {
            bind_data.hopchannel[i] = 0;
          }
          v = 0;
        }
      } while (v);
    }
    break;
    case 'p':
      Serial.println("Hop channel frequencies:");
      for (uint8_t i=0; (i<MAXHOPS) && (bind_data.hopchannel[i]); i++) {
        Serial.print("CH");
        Serial.print(i);
        Serial.print("\t:");
        Serial.print(bind_data.rf_frequency + 10000UL * bind_data.rf_channel_spacing * bind_data.hopchannel[i]);
        Serial.println("Hz");
      }
      break;
    case 'r':
      bindRandomize(0);
      chooseChannelsPerRSSI();
      validShow = false;
      break;
    case 'u':
      bindRandomize(1);
      validShow = false;
      break;
    case 's':
      bindWriteEeprom();
      Serial.println("SAVED!!");
    // fallthru
    case 'q':
      cliActive = false;
      validShow = false;
      break;
    }
    if (validShow) {
      if (valid) {
        Serial.println("New value accepted");
      } else  {
        Serial.println("New value rejected");
      }
    }
  }
  Serial.println("EXIT from CLI!!");
}
