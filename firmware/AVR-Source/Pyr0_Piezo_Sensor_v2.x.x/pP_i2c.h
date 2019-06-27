#include <Wire.h>

/*------------------------------------------------*/

#ifdef I2C
	void i2cInput() {

	  // receive data from Serial and save it into inputBuffer
	  
	  while(Wire.available()) {
		identifyMarkers();
		updateParams();
		i2cReply();
	  }
	}
#endif

/*------------------------------------------------*/
#ifdef I2C
  void i2cReply() {
    if (serialIncoming) {
      Wire.write("OK");
    }
  }
#endif