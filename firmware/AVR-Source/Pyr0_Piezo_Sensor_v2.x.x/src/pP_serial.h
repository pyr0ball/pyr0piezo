void parseData() {

  // split the data into its parts
  
  char *strtokIndx; // this is used by strtok() as an index
  
  strtokIndx = strtok(inputBuffer, " "); // get the first part - the string
  strcpy(serialMessageIn, strtokIndx);   // copy it to serialMessageIn
  
  strtokIndx = strtok(NULL, " "); // this continues where the previous call left off
  serialLong = atol(strtokIndx);   // convert this part to an integer

}
/*------------------------------------------------*/

void identifyMarkers() {
  
  char x = Serial.read();
  #ifdef I2C_INPUT
  char y = Wire.read();
  #endif // I2C_INPUT

  if (x == '\n' || x == '\r') {
    serialIncoming = true;
    inputBuffer[bytesRecvd] = 0;
    parseData();
    bytesRecvd = 0;
  } else {
    inputBuffer[bytesRecvd] = x;
    bytesRecvd++;
    if (bytesRecvd == buffSize) {
      bytesRecvd = buffSize - 1;
    }
  }

  #ifdef I2C_INPUT
    if (y == '\n' || y == '\r') {
      serialIncoming = true;
      inputBuffer[bytesRecvd] = 0;
      parseData();
      bytesRecvd = 0;
    } else {
      inputBuffer[bytesRecvd] = y;
      bytesRecvd++;
      if (bytesRecvd == buffSize) {
        bytesRecvd = buffSize - 1;
      }
    }
  #endif
}

/*------------------------------------------------*/

void updateGainFactor()
{
  if (serialLong >= 0) {
    GAIN_FACTOR = serialLong;
    adjustGain();
    EEPROM.put(GAIN_FACTOR_ADDRESS, GAIN_FACTOR);
  }
}

/*------------------------------------------------*/

void updateVFol() {
  if (serialLong >= 0) {
    followerThrs = serialLong;
    adjustFollow();
    EEPROM.put(FOLLOWER_THRESHOLD_ADDRESS, followerThrs);
  }
}
/*------------------------------------------------*/

void updateVComp() {
  if (serialLong >= 0) {
    compThrs = serialLong;
    adjustComp();
    EEPROM.put(COMP_THRESHOLD_ADDRESS, compThrs);
  }
}

/*------------------------------------------------*/

void updateLoopDuration() {
  if (serialLong >= 0) {
    LOOP_DUR = serialLong;
    EEPROM.put(LOOP_DUR_ADDRESS, LOOP_DUR);
  }
}
/*------------------------------------------------*/

void updateTrigDuration() {
  if (serialLong >= 0) {
    TRG_DUR = serialLong;
    EEPROM.put(TRG_DUR_ADDRESS, TRG_DUR);
  }
}
/*------------------------------------------------*/

void updateHysteresis() {
  if (serialLong >= 0) {
    Hyst = serialLong;
    EEPROM.put(HYST_ADDRESS, Hyst);
  }
}
/*------------------------------------------------*/

void updateLogic() {
  if (serialLong >= 0) {
    LOGIC = serialLong;
    EEPROM.put(LOGIC_ADDRESS, LOGIC);
    pulse();
  }
}
/*------------------------------------------------*/

void updatePzDet() {
  if (serialLong >= 0) {
    PZDET = serialLong;
    EEPROM.put(PZDET_ADDRESS, PZDET);
  }
}
/*------------------------------------------------*/

void updateConstant() {
  if (serialLong >= 0) {
    voltMeterConstant = (long) serialLong;
    EEPROM.put(VM_CONST_ADDRESS, voltMeterConstant);
  }
}

/*------------------------------------------------*/

void updateDebug() {
  if (serialLong > 0) {
    Debug = 1;
  } else if (serialLong == 0) {
    Debug = 0;
  }
}

/*------------------------------------------------*/

void serialPrintConfig() {
  Serial.print("GAIN_F ");
  Serial.print(GAIN_FACTOR);
  switch (GAIN_FACTOR) {
  case 0:
    Serial.println(" 3x");
    break;
  case 1:
    Serial.println(" 3.5x");
    break;
  case 2:
    Serial.println(" 4.33x");
    break;
  case 3:
    Serial.println(" 6x");
    break;
  case 4:
    Serial.println(" 11x");
    break;
  default:
    Serial.println(" INVALID");
    break;
  }

  Serial.print("VFOL ");
  Serial.println(followerThrs);

  Serial.print("VCOMP ");
  Serial.println(compThrs);

  Serial.print("LOOP_D ");
  Serial.println(LOOP_DUR);

  Serial.print("TRG_D ");
  Serial.println(TRG_DUR);

  Serial.print("HYST ");
  Serial.println(Hyst);

  Serial.print("LOGIC ");
  Serial.println(LOGIC);

  Serial.print("PZDET ");
  Serial.println(PZDET);

  Serial.print("VM_CONST ");
  Serial.println(voltMeterConstant);
  
  Serial.print("Firmware Version ");
  Serial.println(PP_VERSION);
}

void serialPrintState() {
  Serial.print("{");

  Serial.print("\"Vcc\":");
  Serial.print(Vin);
  Serial.print(",");

  Serial.print("\"VComp\":");
  Serial.print((long) VComp * Vin / 1023);
  Serial.print(",");

  Serial.print("\"VFol\":");
  Serial.print((long) VFol * Vin / 1023);
  Serial.print(",");

  Serial.print("\"Err\":");
  Serial.print(ERR_STATE);
  Serial.print(",");

  Serial.print("\"PzCon\":");
  Serial.print(PZ_STATE);
  Serial.print(",");

  Serial.print("\"Firm_Ver\":");
  Serial.print(PP_VERSION);
  Serial.print(",");

  Serial.println("}");
}

void updateParams() {
  serialIncoming = false;
  if (strcmp(serialMessageIn, "GAIN_F") == 0) {
    updateGainFactor();
  }
  else if (strcmp(serialMessageIn, "VFOL") == 0) {
    updateVFol();
  }
  else if (strcmp(serialMessageIn, "VCOMP") == 0) {
    updateVComp();
  }
  else if (strcmp(serialMessageIn, "LOOP_D") == 0) {
    updateLoopDuration();
  }
  else if (strcmp(serialMessageIn, "TRG_D") == 0) {
    updateTrigDuration();
  }
  else if (strcmp(serialMessageIn, "HYST") == 0) {
    updateHysteresis();
  }
  else if (strcmp(serialMessageIn, "LOGIC") == 0) {
    updateLogic();
  }
  else if (strcmp(serialMessageIn, "PZDET") == 0) {
    updatePzDet();
  }
  else if (strcmp(serialMessageIn, "CONST") == 0) {
    updateConstant();
  }
  else if (strcmp(serialMessageIn, "DEBUG") == 0) {
    updateDebug();
  }
  else if (strcmp(serialMessageIn, "CONFIG") == 0) {
    serialPrintConfig();
  }
  else if (strcmp(serialMessageIn, "ERASE") == 0) {
    eraseEEPROM();
    serialPrintConfig();
  }
  else if (strcmp(serialMessageIn, "STATE") == 0) {
    serialPrintState();
  }
  else if (strcmp(serialMessageIn, "HELP") == 0) {
    #if defined(ARDUINO_AVR_ATmega328PB)
      Serial.println("To change gain factor: GAIN_F [integer for gain state - see note*]");
      Serial.println("To change voltage follower voltage (low threshold): VFOL [float value]");
      Serial.println("To change comparator voltage (high threshold): VCOMP [float value]");
      Serial.println("To change main loop period: LOOP_D [integer for milliseconds]");
      Serial.println("To change trigger active duration: TRG_D [integer for milliseconds]");
      Serial.println("To change the output logic: LOGIC [0|1]");
      Serial.println("  (0 for active low, 1 for active high)");
      Serial.println("To enable piezo plugged detection: PZDET [0|1]");
      Serial.println("  (0 for disabled, 1 for enabled)");
      Serial.println("To change ADC hysteresis value: HYST [integer in millivolts]");
      Serial.println("To enable or disable debug output: DEBUG [0|1]");
      Serial.println("To print current config: CONFIG");
      Serial.println("To set config to defaults: ERASE");
      Serial.println("To print current state: STATE");
      Serial.println("");
      Serial.println("Commands are entered in this format:");
      Serial.println("CMD VAL");
      Serial.println("Commands are confirmed with Enter key");
      Serial.println("");
      Serial.println("Examples:");
      Serial.println("GAIN_F 3 <~ set gain factor to index 3 (6x)");
      Serial.println("VFOL 2350 <~ set the vref floor to 2.35V");
    #else
      Serial.println("Check docs.pyroballpcbs.com/config");
    #endif // defined(ARDUINO_AVR_ATmega328PB)
  }
  parseData();
}    


void serialInput() {
  // receive data from Serial and save it into inputBuffer
    if (Serial.available() > 0) {

    // the order of these IF clauses is significant
    identifyMarkers();
  }
}
