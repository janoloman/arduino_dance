// (c) Michael Schoeffler 2017, http://www.mschoeffler.de
#include "Wire.h"               // This library allows you to communicate with I2C devices.
#include "U8glib.h"

// Display
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK); // Display which does not send AC

// IMU
const int MPU = 0x68;   // I2C address of the MPU-6050. If AD0 pin is set to HIGH, 
                             // the I2C address will be 0x69.

// variables for accelerometer & gyro raw data, and temperature
int16_t ax, ay, az, gx, gy, gz, tmp;

//Ratios de conversion
#define A_R 16384.0
#define G_R 131.0
 
//Conversion de radianes a grados 180/PI
#define RAD_A_DEG = 57.295779

//Angulos
float Acc[2];
float Gy[2];
float Angle[2];

/* LEDs array . For proper fading effect you should attach your LEDs 
 *  to PWM digital output PWM output pins are (3,5,6,9,10) in Pro 
 *  Micro board. The array should be terminated with 0 value
*/
int led[] = {5,6,9,10, 0};
int led_array_lenth;
bool moving = false;

/* LEDs lighting effect functions */
// get lenth of leds array
void get_led_array_lenth() {
  led_array_lenth = 0;
  for (int i = 0; led[i]; i++) 
    led_array_lenth++;
}

// To fade in/out all LEDs at once .
void fade_all() {
  int delay_time = 30; // milliseconds
  int min_fade = 20;
  int max_fade = 120;
  int step_fade = 12;
  
  for (int i = 0; i < led_array_lenth; i++) {
    for (int val = min_fade ; val <= max_fade; val += step_fade) {
      // sets the fade value for all LEDs (range from 0 to 255):
      for (int x = 0; x < led_array_lenth; x++) 
        analogWrite(led[x], val);
      // wait for 30 milliseconds to see the dimming effect
      delay(delay_time);
    }

    // fade out from max to min in increments of 5 points:
    for (int val = max_fade ; val >= min_fade; val -= step_fade) {
      // sets the fade value for all LEDs (range from 0 to 255):
      for (int x = 0; x < led_array_lenth; x++)
        analogWrite(led[x], val);      
      // wait for 30 milliseconds to see the fading in effect
      delay(delay_time);
    }
  }
}

void turnoff_all() {
  for (int x = 0; x < led_array_lenth; x++)
    analogWrite(led[x], LOW);
}

void readData(){
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);                         // starting with register 0x3B (ACCEL_XOUT_H) [MPU-6000 and MPU-6050 Register Map and Descriptions Revision 4.2, p.40]
  Wire.endTransmission(false);              // the parameter indicates that the Arduino will send a restart. As a result, the connection is kept active.
  Wire.requestFrom(MPU, 7 * 2, true);  // request a total of 7*2=14 registers

  // "Wire.read()<<8 | Wire.read();" means two registers are read and stored in the same variable
  ax  = Wire.read() << 8 | Wire.read(); // reading: 0x3B (ACCEL_XOUT_H) and 0x3C (ACCEL_XOUT_L)
  ay  = Wire.read() << 8 | Wire.read(); // reading: 0x3D (ACCEL_YOUT_H) and 0x3E (ACCEL_YOUT_L)
  az  = Wire.read() << 8 | Wire.read(); // reading: 0x3F (ACCEL_ZOUT_H) and 0x40 (ACCEL_ZOUT_L)
  tmp = Wire.read() << 8 | Wire.read(); // reading: 0x41 (TEMP_OUT_H) and 0x42 (TEMP_OUT_L)
  gx  = Wire.read() << 8 | Wire.read(); // reading: 0x43 (GYRO_XOUT_H) and 0x44 (GYRO_XOUT_L)
  gy  = Wire.read() << 8 | Wire.read(); // reading: 0x45 (GYRO_YOUT_H) and 0x46 (GYRO_YOUT_L)
  gz  = Wire.read() << 8 | Wire.read(); // reading: 0x47 (GYRO_ZOUT_H) and 0x48 (GYRO_ZOUT_L)  
}

char tmp_str[7];                        // temporary variable used in convert function
char* convert_int16_to_str(int16_t i) { // converts int16 to string. Moreover, resulting strings will have the same length in the debug monitor.
  sprintf(tmp_str, "%6d", i);
  return tmp_str;
}

void printData(){
  Serial.print("aX = "); Serial.print(convert_int16_to_str(ax));
  Serial.print(" | aY = "); Serial.print(convert_int16_to_str(ay));
  Serial.print(" | aZ = "); Serial.print(convert_int16_to_str(az));  
  // the following equation was taken from the documentation [MPU-6000/MPU-6050 Register Map and Description, p.30]
  Serial.print(" | tmp = "); Serial.print(tmp / 340.00 + 36.53);
  Serial.print(" | gX = "); Serial.print(convert_int16_to_str(gx));
  Serial.print(" | gY = "); Serial.print(convert_int16_to_str(gy));
  Serial.print(" | gZ = "); Serial.print(convert_int16_to_str(gz));
  Serial.println();
}

void kalmanFilter(){
  //A partir de los valores del acelerometro, se calculan los angulos Y, X
  //respectivamente, con la formula de la tangente.
  Acc[1] = atan(-1*(ax/A_R)/sqrt(pow((ay/A_R),2) + pow((az/A_R),2)))*RAD_TO_DEG;
  Acc[0] = atan((ay/A_R)/sqrt(pow((ax/A_R),2) + pow((az/A_R),2)))*RAD_TO_DEG;
  
  //Calculo del angulo del Giroscopio
  Gy[0] = gx/G_R;
  Gy[1] = gy/G_R;
  
  //Aplicar el Filtro Complementario
  Angle[0] = 0.98 *(Angle[0]+Gy[0]*0.010) + 0.02*Acc[0];
  Angle[1] = 0.98 *(Angle[1]+Gy[1]*0.010) + 0.02*Acc[1];
  
  //Mostrar los valores por consola
  Serial.print("Angle X: "); Serial.print(Angle[0]); Serial.println();
  Serial.print("Angle Y: "); Serial.print(Angle[1]); Serial.print("\n------------\n");

  u8g.firstPage();  
  do {
    u8g.setFont(u8g_font_unifont);
    u8g.setPrintPos(0,10);
    u8g.print("mpu6050_kalman");
    u8g.drawHLine(0,12,128);
    u8g.setPrintPos(0, 27);
    u8g.print("angle X ");
    u8g.print(Angle[0]);
    u8g.setPrintPos(0, 40);    
    u8g.print("angle Y ");
    u8g.print(Angle[1]);
    //u8g.setPrintPos(0, 53);
    //u8g.print(moving);
  } while( u8g.nextPage() );
}

void checkMovement(){
  int r = 5000; 
  if((gx > r || gx < -r) || (gy > r || gy < -r) || (gz > r || gz < -r)){
    moving = true;
  } 
  else {
    moving = false;
  }

  u8g.firstPage();  
  do {
    u8g.setFont(u8g_font_unifont);
    u8g.setPrintPos(0,10);
    u8g.print("mpu6050_LFade");
    u8g.drawHLine(0,12,128);
    u8g.setPrintPos(0, 27);
    u8g.print("checkMovement ");
    u8g.print(moving);
  } while( u8g.nextPage() );

  if(moving){
    fade_all();
  } else {  
    turnoff_all();    
  }
}
  
void setup() {
  get_led_array_lenth();
  
  turnoff_all();
    
  Wire.begin();
  Wire.beginTransmission(MPU); // Begins a transmission to the I2C slave (GY-521 board)
  Wire.write(0x6B);                 // PWR_MGMT_1 register
  Wire.write(0);                    // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  Serial.begin(9600);
}

void loop() {
  readData();
  //printData();
    
  kalmanFilter();
   
  //checkMovement();
  
  delay(30);
}
