/* pyr0-piezo pins configuration file

Default pins (based on Rev.2.x.xPCB layout) 

 * PD2 INT0 (Piezo In 'D2')
 * D7 PCINT23 (Trigger OUT 'D7')
 * PC0 ADC0 (Voltage Reference Check 'A0')
 * PC1 ADC1 (Sensitivity Adjustment Check 'A1')
 * PD4 PCINT20 (Error feedback LED 'D4')
 * PB6 PCINT6 (Voltage Adjustment Resistor 0 'D20')
 * PB7 PCINT7 (Voltage Adjustment Resistor 1 'D21')
 * PD5 T1 (Voltage Adjustment Resistor 2 'D5')
 * PD6 PCINT22 (Voltage Adjustment Resistor 3 'D6')
 * PB1 OC1A (Comparator VRef PWM Out 'D9')
 * PD3 OC2B (Voltage Follower VRef PWM Out 'D3')
*/

// Analog Pin Assignments
#define V_FOLLOW_PIN	A0  // Sense pin to check Voltage Follower stage
#define VCOMP_SENSE_PIN	A1  // Sense pin to check comparator stage voltage

// Digital Pin Assignments
#define TRG_OUT 7         // LED and Z-Min trigger output connected to digital pin 7
//#define TRG_OUT 13      // For testing on Atmega328/2560, Output is moved to onboard LED pin
#define Z_TRG 2           // the piezo is connected to INT0 / digital pin 2
#define ERR_LED 4         // LED will blink if optimal voltage range cannot be achieved
#define GADJ_R0 20        // Auto-adjust ladder pin assignments
#define GADJ_R1 21        // "
#define GADJ_R2 5         // "
#define GADJ_R3 6         // "
#define V_FOL_PWM 3       // PWM analog output pin for voltage follower adjustment
#define VCOMP_PWM 9       // PWM analog output pin for comparator adjustment