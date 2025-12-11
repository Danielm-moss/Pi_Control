#include <FastAccelStepper.h>

const int EXP_TIMES = 10;

const int STEP_PIN = 9;
const int DIR_PIN  = 7;
const long distance = -4880; //distance in motor steps

const uint32_t MAX_SPEED = 4500;
const uint32_t ACCEL = 4500;

FastAccelStepperEngine engine;
FastAccelStepper *stepper = nullptr;

void setup() {

  int LAPS;
  if(EXP_TIMES % 2 == 0){
    LAPS = EXP_TIMES / 2;
  }

  else{
    LAPS = (EXP_TIMES + 1) / 2;
  }

  delay(2000);
  engine.init();

  stepper = engine.stepperConnectToPin(STEP_PIN);
  stepper->setDirectionPin(DIR_PIN);
  stepper->setAutoEnable(true);

  stepper->setSpeedInHz(MAX_SPEED);
  stepper->setAcceleration(ACCEL);

  stepper->setCurrentPosition(0); // Home

  for(int i = 0; i < LAPS; i++){

  stepper->moveTo(distance);
  while (stepper->isRunning()) { } // Nothing, just here so the delay only begins after the rail stops

  delay(1500);

  stepper->moveTo(0);
  while (stepper->isRunning()) { } // Nothing, just here so the delay only begins after the rail stops
  delay(1500);

  }
}

void loop() { }
