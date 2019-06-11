void setup() {
  pinMode(TRG_OUT, OUTPUT); // declare the Trigger as as OUTPUT
  pinMode(ERR_LED, OUTPUT);
  pinMode(Z_TRG, INPUT_PULLUP); // declare z-sense input with pullup
  pinMode(V_FOLLOW_PIN, INPUT);
  pinMode(VCOMP_SENSE_PIN, INPUT);
  pinMode(GADJ_R0, INPUT); // declare input to break pull to ground
  pinMode(GADJ_R1, INPUT); // declare input to break pull to ground
  pinMode(GADJ_R2, INPUT); // declare input to break pull to ground
  pinMode(GADJ_R3, INPUT); // declare input to break pull to ground
  Serial.begin(9600);

  // Uncomment the following lines to use PCInt pins instead of hardware interrupt
  //#include <PinChangeInterrupt.h>
  //attachPCINT(digitalPinToPCINT(Z_TRG), pulse, FALLING);

  // Uncomment the followoing line to use hardware interrupt pin
  attachInterrupt(digitalPinToInterrupt(Z_TRG), pulse, FALLING);

  Serial.println("Initializing Pyr0-Piezo Sensor...");
  
}

/*------------------------------------------------*/

void pulse() {
  digitalWrite(TRG_OUT, LOW);
  sensorHReading = 1;
  delay(TRG_DUR);
  digitalWrite(TRG_OUT, HIGH);
}

/*------------------------------------------------*/

void adjustFollow() {
  /* Compares diffs of threshold vs read value
   if positive, adjusts the follower to within
   the range set above*/
  if (diffAdjL > 0.0) {
	ADJ_FOLLOW += diffAdjL;
  }
  if (diffAdjH > 0.0) {
	ADJ_FOLLOW -= diffAdjH;
  }

  // Analog output (PWM) of duty cycle
  analogWrite(V_FOL_PWM, ADJ_FOLLOW);
}

/*------------------------------------------------*/

void adjustComp() {
  if (diffCompL > 0.0) {
	ADJ_COMP += diffCompL;
  }
  if (diffCompH > 0.0) {
	ADJ_COMP -= diffCompH;
  }

  analogWrite(VCOMP_PWM, ADJ_COMP);
}

void calibrateAlert() {
	if (diffAdjL > 0.0 || diffAdjH > 0.0 || diffCompL > 0.0 || diffCompH > 0.0) {
		ERR_STATE = 1;
	}
}

/*------------------------------------------------*/

void adjustGain() {

  if (GAIN_FACTOR == 0) {
    pinMode(GADJ_R3, INPUT);
    pinMode(GADJ_R2, INPUT);
    pinMode(GADJ_R1, INPUT);
    pinMode(GADJ_R0, INPUT);
    ERR_STATE = 0;
  }
  else if (GAIN_FACTOR > 0) {
    pinMode(GADJ_R3, OUTPUT);
    digitalWrite(GADJ_R3, LOW);
    pinMode(GADJ_R2, INPUT);
    pinMode(GADJ_R1, INPUT);
    pinMode(GADJ_R0, INPUT);
    ERR_STATE = 0;
  }
  else if (GAIN_FACTOR > 1) {
    pinMode(GADJ_R2, OUTPUT);
    digitalWrite(GADJ_R2, LOW);
    pinMode(GADJ_R1, INPUT);
    pinMode(GADJ_R0, INPUT);
    ERR_STATE = 0;
  }
  else if (GAIN_FACTOR > 2) {
    pinMode(GADJ_R1, OUTPUT);
    digitalWrite(GADJ_R1, LOW);
    pinMode(GADJ_R0, INPUT);
    ERR_STATE = 0;
  }
  else if (GAIN_FACTOR > 3) {
    pinMode(GADJ_R0, OUTPUT);
    digitalWrite(GADJ_R0, LOW);
    ERR_STATE = 0;
  }
}

/*------------------------------------------------*/

void checkError () {
  if (ERR_STATE == 1) {
    digitalWrite(ERR_LED, BlinkState);
    BlinkState = !BlinkState;
  }
  else if (ERR_STATE == 0) {
    BlinkState = LOW;
    digitalWrite(ERR_LED, BlinkState);
  }
}

/*------------------------------------------------*/

void identifyMarkers() {
  
  char x = Serial.read();
//  char y = Wire.read();

  if (x == endMarker) {
    readInProgress = false;
    serialIncoming = true;
    inputBuffer[bytesRecvd] = 0;
    parseData();
  }

  else if(readInProgress) {
    inputBuffer[bytesRecvd] = x;
    bytesRecvd ++;
    if (bytesRecvd == buffSize) {
      bytesRecvd = buffSize - 1;
    }
  }

  else if (x == startMarker) { 
    bytesRecvd = 0; 
    readInProgress = true;
  }
  #ifdef I2C
    if (y == endMarker) {
      readInProgress = false;
      serialIncoming = true;
      inputBuffer[bytesRecvd] = 0;
      parseData();
    }

    if(readInProgress) {
      inputBuffer[bytesRecvd] = y;
      bytesRecvd ++;
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

void parseData() {

  // split the data into its parts
  
  char * strtokIndx; // this is used by strtok() as an index
  
  strtokIndx = strtok(inputBuffer,",");      // get the first part - the string
  strcpy(serialMessageIn, strtokIndx); // copy it to serialMessageIn
  
  strtokIndx = strtok(NULL, ",");   // this continues where the previous call left off
  serialInt = atoi(strtokIndx);     // convert this part to an integer
   
  strtokIndx = strtok(NULL, ","); 
  serialFloat = atof(strtokIndx);     // convert this part to a float

}

/*------------------------------------------------*/

void updateParams() {
  if (strcmp(serialMessageIn, "TRG_D") == 0) {
    updateTrigDuration();
  }
  else if (strcmp(serialMessageIn, "GAIN_F") == 0) {
    updateGainFactor();
  }
  else if (strcmp(serialMessageIn, "VCOMP") == 0) {
    updateVComp();
  }
  else if (strcmp(serialMessageIn, "VADJ") == 0) {
    updateVAdj();
  }
}
/*------------------------------------------------*/

void updateTrigDuration() {
  if (serialInt >= 0) {
    TRG_DUR = serialInt;
  }
}
/*------------------------------------------------*/

void updateGainFactor() {
  if (serialInt >= 0) {
    GAIN_FACTOR = serialInt;
  }
}
/*------------------------------------------------*/

void updateVComp() {
  if (serialInt >= 0) {
    compInt = (serialFloat / 5) * 1024;
    //senseInt = compInt; // syncing these params til #24 is fixed
  }
}
/*------------------------------------------------*/

void updateVAdj() {
  if (serialInt >= 0) {
    senseInt = (serialFloat / 5) * 1024;
    //compInt = senseInt; // syncing these params til #24 is fixed
  }
}
/*------------------------------------------------*/

void updateHysteresis() {
  if (serialInt >= 0) {
    Hyst = serialInt;
  }
}
/*------------------------------------------------*/