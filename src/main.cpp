#include <Arduino.h>
#include <Wire.h>


// const uint8_t SENSOR_ADDRESS = 0x38; // 0x38
// const int OUTPUT_MIN = 1638;
// const int OUTPUT_MAX = 14745;
// const int PRESSURE_MIN_PSI = 0; // 0 psi en mmHg
// const int PRESSURE_MAX_PSI = 3103; // 60 psi en mmHg
// 005PD2A3
const uint8_t SENSOR_ADDRESS = 0x28;
const double OUTPUT_MIN = 1638;
const double OUTPUT_MAX = 14745;
const double PRESSURE_MIN_PSI = -5;// -258.57462051198; // -5 psi en mmHg
const double PRESSURE_MAX_PSI =  5;// 258.57462051198; // 5 psi en mmHg
const double PRESSURE_MIN_MERCURE = -258.57462051198; // -5 psi en mmHg
const double PRESSURE_MAX_MERCURE =  258.57462051198; // 5 psi en mmHg

long readPressureSensor(long defaultValue) {
  Wire.requestFrom(SENSOR_ADDRESS, (uint8_t) 4);
  if (Wire.available()) {
    uint8_t b1 = Wire.read();
    uint8_t b2 = Wire.read();
    uint8_t b3 = Wire.read();
    uint8_t b4 = Wire.read();
    uint8_t status = b1 >> 6; // on garde les 2 bits de poids fort du 1er octet renvoyé par le capteur
    if (status == 0) { // tout est OK
      long rawPressure = (((uint16_t) (b1 & 0x3f)) << 8) | b2;
      // Serial.print("OUTPUT_MAX : ");
      // Serial.println(OUTPUT_MAX);
      // Serial.print("OUTPUT_MIN : ");
      // Serial.println(OUTPUT_MIN);
      // Serial.print("PRESSURE_MAX_PSI : ");
      // Serial.println(PRESSURE_MAX_PSI);
      // Serial.print("PRESSURE_MIN_PSI : ");
      // Serial.println(PRESSURE_MIN_PSI);
      //long rawTemperature = ((((uint16_t) b3) << 8| b4) >> 5; // formule de la doc Honeywell
      Serial.print("Raw pressure: ");
      Serial.println(rawPressure);
      double pressurePsi = ((rawPressure - OUTPUT_MIN) * (PRESSURE_MAX_PSI - PRESSURE_MIN_PSI) 
                            / (OUTPUT_MAX - OUTPUT_MIN)) + PRESSURE_MIN_PSI;
      double pressureMercure = ((rawPressure - OUTPUT_MIN) * (PRESSURE_MAX_MERCURE - PRESSURE_MIN_MERCURE) 
                            / (OUTPUT_MAX - OUTPUT_MIN)) + PRESSURE_MIN_MERCURE;
      Serial.print("Pressure: ");
      Serial.print(pressurePsi);
      Serial.println("psi");
      Serial.print("Pressure: ");
      Serial.print(pressureMercure);
      Serial.println("mmHg");
      return pressurePsi; // formule de la doc Honeywell
    } else {
      Serial.print("Error de lecture du capteur ; status = ");
      Serial.println(status);
      return defaultValue;
    }
  } else {
    //Serial.println("not available");
    return defaultValue;
  }
}

const double CONVERT = 51.714924102396;
const int MPX5010GP = A1;

const double kPa_to_mmHg = 7.5006157584566;
const double ratioPontDiviseur = 0.572727273;
const double vSupply = 5.08;

void readPressureMPX5010GP_KFR1951A() {
  
  double vMax = analogRead(MPX5010GP) / 1024.0 * 3.3;
  double vMin = vMax;
  double vMoy = vMax;
  
  for (int i = 0; i < 9; i++) {
    double currentValue = analogRead(MPX5010GP) / 1024.0 * 3.3;
    if (vMin > currentValue) {
      vMin = currentValue; 
    }
    if (vMax < currentValue) {
      vMax = currentValue;
    }
    vMoy += currentValue;
  }

  vMoy = vMoy / 10.0;
  
  double rawVout = vMoy;

  Serial.print("Raw min: ");
  Serial.print(vMin);
  Serial.print(" - Raw max: ");
  Serial.print(vMax);
  Serial.print(" - Raw moy: ");
  Serial.print(vMoy);

  // Ratio a cause du pont diviseur
  double vOut = rawVout / ratioPontDiviseur;

  double P  = (vOut / vSupply - 0.04) / 0.09;

  Serial.print(" - Pressure (kPA): ");
  Serial.println(P);
  delay(300);


  // Transfer Function (kPa):
  // V out = Vs x (0.09 x P + 0.04) ± 5.0% V FSS
  // VS = 5.0 Vdc => supply voltage
  // TEMP = 0 to 85C


}

void setup () {
  Serial.begin(9600);
  Wire.begin();
  pinMode(A1, INPUT);
}
void loop() {
  //readPressureSensor(0);
  readPressureMPX5010GP_KFR1951A();
  
}