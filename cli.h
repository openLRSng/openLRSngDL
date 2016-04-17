static bool cliActive = 0;
static char linebuf[16];

void show()
{
  printStr("OpenLRSngDL ");
  printVersion(version);
  printStr(" module:");
  printULLn(bind_data.rfmType);
  printStr("a) serial baud: ");
  printUL(bind_data.serial_baudrate);
  printStr(" t) mode: ");
  printStrLn(serial_bits_str[bind_data.serial_mode]);
  printStr("b) base frq:  ");
  printULLn(bind_data.rf_frequency);
  printStr("c) max frq:   ");
  printULLn(bind_data.maxFrequency);
  printStr("d) channel spc: ");
  printULLn(bind_data.rf_channel_spacing);
  printStr("e) Power:        ");
  printULLn(bind_data.rf_power);
  printStr("f) RF params:   ");
  printUL(bind_data.modem_params);
  printStr(" (");
  printUL(modem_params[bind_data.modem_params].bps);
  printStrLn("bps)");
  printStr("g) pkt size:     ");
  printULLn(bind_data.packetSize - 1);
  printStr("h) magic:           ");
  printULLn(bind_data.rf_magic);
  printStr("i) pktmode     : ");
  printULLn((bind_data.flags & PACKET_MODE)?1:0);
  printStr("j)  status: ");
  printULLn((bind_data.flags & STATUSPACKET_MODE)?1:0);
  printStr("k)  crc16 : ");
  printULLn((bind_data.flags & PACKETCRC_MODE)?1:0);
  printStr("l) CHs: ");
  for (uint8_t i=0; (i<MAXHOPS) && (bind_data.hopchannel[i]); i++) {
    if (i) {
      printC(',');
    }
    printUL(bind_data.hopchannel[i]);
  }
  printLf();
  printStr("Pkt interval: ");
  printUL(getInterval(&bind_data));
  printStr("us   rate: ");
  printUL(1000000UL/getInterval(&bind_data));
  printStrLn("Hz");
  printStr("Max bw: ");
  printUL((unsigned long)(bind_data.packetSize - 1) * 1000000UL / getInterval(&bind_data));
  printStrLn(" B/s");
  printLf();
  printStrLn("[s]ave [q]uit [p]rint CHs");
  printStrLn("Randomize: [r]ssi [u]random");
}

#define INVALID 0xffffffff

uint32_t getValue(const char *q, uint32_t min, uint32_t max)
{
  bool done = false;
  uint8_t len=0;
  linebuf[0] = 0;
  while (!done) {
    printC('\r');
    printStr(q);
    printStr(" (");
    printUL(min);
    printC('-');
    printUL(max);
    printStr("): ");
    printStr(linebuf);
    printStr("  \b\b");
    while (!serialAvailable());
    char c = serialRead();
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

  while (serialAvailable() && serialRead());

  while (cliActive) {
    show();
    while (!serialAvailable());
    char c = tolower(serialRead());
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
      v = getValue("New packet size", MIN_PACKET_SIZE - 1, MAX_PACKET_SIZE - 1);
      if (v != INVALID) {
        bind_data.packetSize = v + 1;
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
    case 'i':
      bind_data.flags ^= PACKET_MODE;
      valid = 1;
      break;
    case 'j':
      bind_data.flags ^= STATUSPACKET_MODE;
      valid = 1;
      break;
    case 'k':
      bind_data.flags ^= PACKETCRC_MODE;
      valid = 1;
      break;
    case 't':
      bind_data.serial_mode++;
      if (bind_data.serial_mode > 5) {
        bind_data.serial_mode=0;
      }
      valid = 1;
      break;
    case 'l': {
      printStrLn("Enter chs(1-254), just enter to finish");
      uint8_t i=0;
      do {
        char q[7];
        snprintf(q,7,"CH%02u: ",i);
        v = getValue(q,1,254);
        if (v != INVALID) {
          bool chvalid = true;
          for (int j = 0; j < i; j++) {
            if (bind_data.hopchannel[j] == v) {
              printStrLn("Cannot use same channel twice\n");
              chvalid = false;
            }
          }
          if ((bind_data.rf_frequency + 10000UL * bind_data.rf_channel_spacing * v) >
              bind_data.maxFrequency) {
            printStrLn("Channel violates max freq limit\n");
            chvalid = false;
          }
          if (chvalid) {
            bind_data.hopchannel[i++] = v;
            printLf();
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
      printStrLn("Hop ch frqs:");
      for (uint8_t i=0; (i<MAXHOPS) && (bind_data.hopchannel[i]); i++) {
        printStr("CH");
        printUL(i);
        printStr("\t:");
        printUL(bind_data.rf_frequency + 10000UL * bind_data.rf_channel_spacing * bind_data.hopchannel[i]);
        printStrLn("Hz");
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
      printStrLn("SAVED!!");
      // fallthru
    case 'q':
      bindReadEeprom();
      cliActive = false;
      validShow = false;
      break;
    }
    if (validShow) {
      if (valid) {
        printStrLn("OK");
      } else  {
        printStrLn("NAK");
      }
    }
  }
  printStrLn("EXIT from CLI!!");
}
