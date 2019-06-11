/*------------------------------------------------*/

void serialInput() {

  // receive data from Serial and save it into inputBuffer
  
  if(Serial.available() > 0) {

    // the order of these IF clauses is significant
    identifyMarkers();

  }
}

/*------------------------------------------------*/

void serialReply() {
  if (serialIncoming) {
	serialIncoming = false;
	#ifdef DEBUG
		Serial.print("Comp Reference:");
		Serial.print(VComp);
		Serial.print(" ");
		Serial.print("Comparator State:");
		Serial.print(ADJ_COMP);
		Serial.print(" ");
		Serial.println(compInt);

		Serial.print("Diff");
		Serial.print(" ");
		Serial.print(diffCompL);
		Serial.print(" ");
		Serial.println(diffCompH);
	  
		Serial.print("Amp Sense:");
		Serial.print(VAdj);
		Serial.print(" ");
		Serial.print("Follower State:");
		Serial.print(ADJ_FOLLOW);
		Serial.print(" ");
		Serial.println(senseInt);

		Serial.print("Diff");
		Serial.print(" ");
		Serial.print(diffAdjL);
		Serial.print(" ");
		Serial.println(diffAdjH);
	#endif
  
	Serial.print("Delay:");
	Serial.println(TRG_DUR);
	Serial.print("Error State:");
	Serial.println(ERR_STATE);
	Serial.println("------------------");
  }
}
