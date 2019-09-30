void parseData() {

  // split the data into its parts
  
  char *strtokIndx; // this is used by strtok() as an index
  
  strtokIndx = strtok(inputBuffer, " "); // get the first part - the string
  strcpy(serialMessageIn, strtokIndx);   // copy it to serialMessageIn
  
  strtokIndx = strtok(NULL, " "); // this continues where the previous call left off
  serialInt = atoi(strtokIndx);   // convert this part to an integer

}
/*------------------------------------------------*/

void identifyMarkers() {
  
  char x = Serial.read();
//  char y = Wire.read();

  if (x == '\n' || x == '\r')
  {
    serialIncoming = true;
    inputBuffer[bytesRecvd] = 0;
    parseData();
    bytesRecvd = 0;
  } else {
    inputBuffer[bytesRecvd] = x;
    bytesRecvd++;
    if (bytesRecvd == buffSize)
    {
      bytesRecvd = buffSize - 1;
    }
  }

  #ifdef I2C_INPUT
    if (y == endMarker) {
      readInProgress = false;
      serialIncoming = true;
      inputBuffer[bytesRecvd] = 0;
      parseData();
    }

    if (readInProgress) {
      inputBuffer[bytesRecvd] = y;
      bytesRecvd++;
      if (bytesRecvd == buffSize) {
      bytesRecvd = buffSize - 1;
      }
    }

    if (y == startMarker) { 
      bytesRecvd = 0; 
      readInProgress = true;
    }
  #endif
}

/*------------------------------------------------*/

void updateGainFactor()
{
  if (serialInt >= 0)
  {
    GAIN_FACTOR = serialInt;
    adjustGain();
    EEPROM.put(GAIN_FACTOR_ADDRESS, GAIN_FACTOR);
  }
}

/*------------------------------------------------*/

void updateVFol() {
  if (serialInt >= 0)
  {
    followerThrs = serialInt;
    adjustFollow();
    EEPROM.put(FOLLOWER_THRESHOLD_ADDRESS, followerThrs);
  }
}
/*------------------------------------------------*/

void updateVComp() {
  if (serialInt >= 0)
  {
    compThrs = serialInt;
    adjustComp();
    EEPROM.put(COMP_THRESHOLD_ADDRESS, compThrs);
  }
}

/*------------------------------------------------*/

void updateLoopDuration()
{
  if (serialInt >= 0)
  {
    LOOP_DUR = serialInt;
    EEPROM.put(LOOP_DUR_ADDRESS, LOOP_DUR);
  }
}
/*------------------------------------------------*/

void updateTrigDuration() {
  if (serialInt >= 0)
  {
    TRG_DUR = serialInt;
    EEPROM.put(TRG_DUR_ADDRESS, TRG_DUR);
  }
}
/*------------------------------------------------*/

void updateHysteresis() {
  if (serialInt >= 0)
  {
    Hyst = serialInt;
    EEPROM.put(HYST_ADDRESS, Hyst);
  }
}
/*------------------------------------------------*/

void updateConstant() {
  if (serialInt >= 0)
  {
    voltMeterConstant = (long) serialInt;
    EEPROM.put(VM_CONST_ADDRESS, voltMeterConstant);
  }
}

/*------------------------------------------------*/

void updateDebug() {
  if (serialInt > 0) {
    Debug = 1;
  } else if (serialInt == 0) {
    Debug = 0;
  }
}

/*------------------------------------------------*/

void serialPrintConfig()
{
  Serial.print("GAIN_F ");
  Serial.print(GAIN_FACTOR);
  switch (GAIN_FACTOR)
  {
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

  Serial.print("VM_CONST ");
  Serial.println(voltMeterConstant);
}

void serialPrintState()
{
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
    updateTrigDuration();
  }
  else if (strcmp(serialMessageIn, "TRG_D") == 0) {
    updateTrigDuration();
  }
  else if (strcmp(serialMessageIn, "HYST") == 0) {
    updateHysteresis();
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
  else if (strcmp(serialMessageIn, "RESET") == 0) {
    resetConfig();
    serialPrintConfig();
  }
  else if (strcmp(serialMessageIn, "STATE") == 0) {
    serialPrintState();
  }
  else if (strcmp(serialMessageIn, "HELP") == 0) {
    // Serial.println("To change gain factor: GAIN_F [integer for gain state - see note*]");
    // Serial.println("To change voltage follower voltage (low threshold): VFOL [float value]");
    // Serial.println("To change comparator voltage (high threshold): VCOMP [float value]");
    // Serial.println("To change main loop period: LOOP_D [integer for milliseconds]");
    // Serial.println("To change trigger active duration: TRG_D [integer for milliseconds]");
    // Serial.println("To change ADC hysteresis value: HYST [integer]");
    // Serial.println("To enable or disable debug output: DEBUG [0|1]");
    // Serial.println("To print current config: CONFIG");
    // Serial.println("To reset config to defaults: RESET");
    // Serial.println("To print current state: STATE");
    // Serial.println("");
    // Serial.println("Commands are entered in this format:");
    // Serial.println("CMD VAL");
    // Serial.println("Commands are confirmed with Enter key");
    // Serial.println("");
    // Serial.println("Examples:");
    // Serial.println("GAIN_F 3 <~ set gain factor to index 3 (6x)");
    // Serial.println("VFOL 2350 <~ set the vref floor to 2.35V");
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
