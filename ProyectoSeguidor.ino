//  Código Adaptado por: Felix R. NAvas V. y Anthony Urresta
//  Fecha: 26/07/2023
//  Basado en un código de ejemplo de Pololu:
//  https://pololu.github.io/qtr-sensors-arduino/md_usage.html

#include <QTRSensors.h>

// Change the values below to suit your robot's motors, weight, wheel type, etc.
#define AIN1 2               // Pin AIN1 del driver DRV8833
#define AIN2 10              // Pin AIN2 del driver DRV8833
#define BIN1 3               // Pin BIN1 del driver DRV8833
#define BIN2 11              // Pin BIN2 del driver DRV8833
#define KP .2
#define KD 5
#define M1 50               // Velocidad Base del Motor M1
#define M2 50               // Velocidad Base del Motor M2
#define NUM_SENSORS  8      // number of sensors used
#define TIMEOUT       2500  // waits for 2500 us for sensor outputs to go low
#define EMITTER_PIN   2     // emitter is controlled by digital pin 2
#define interruptor   5     // Interruptor de control

QTRSensors qtr;

unsigned int sensorValues[NUM_SENSORS];
int position;
int lastError = 0;

void setup()
{
  pinMode (interruptor,INPUT);
  qtr.setTypeRC();    //Selecciono el tipo de sensor en este caso RC
                        //Indico los pines conectados a los sensores
  qtr.setSensorPins ((const uint8_t[]) {14,15,16,17,18,19,1,0},8);
  qtr.setEmitterPin(2);
  pinMode(AIN2,LOW);
  pinMode(BIN2,LOW);
  delay(1000);
  manual_calibration(); 

}


void loop()
{
  if (digitalRead(interruptor)==HIGH){                                 //Valida la orden para que se empiece a mover
  static uint16_t lastError = 0;
  uint16_t sensors[8];
  int16_t position = qtr.readLineBlack(sensors);                       //Obtiene una lectura entre 0-8000
  int16_t error = position - 4000;                                     //Si la lectura es de 4000 el sensor está en la mitad de la linea
  int16_t motorSpeed = KP * error + KD * (error - lastError);          //Se calcula la corrección de la velocidad
  lastError = error;                                                   //Se almacena el error de posición
  int16_t m1Speed = M1 + motorSpeed;                                   //Se calcula la corrección de velocidad para el motor M1
  int16_t m2Speed = M2 - motorSpeed;                                   //Se calcula la corrección de velocidad para el motor M2
  
  if (m1Speed < 0) { m1Speed = 0; }                                    //Se evitan velocidades negativas
  if (m2Speed < 0) { m2Speed = 0; }

  velocidadMotor(m1Speed,m2Speed);                                     //Se actualizan las velocidades
}
}

void velocidadMotor(int16_t m1Speed, int16_t m2Speed){
  
  int16_t duty1 = map(m1Speed,0,1023,0,255); //Se calcula la velocidad del Motor M1
  int16_t duty2 = map(m2Speed,0,1023,0,255); //Se calcula la velocidad del Motor M2

  analogWrite(AIN1,duty1);                   //Se actualizan las velocidades
  analogWrite(BIN1,duty2);
}


void manual_calibration() {

  int i;
  for (i = 0; i < 250; i++)  // the calibration will take a few seconds
  {
    qtr.calibrate();
    delay(20);
  }

}
