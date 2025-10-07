// Number of plants
const int NUM_PLANTS = 3;

// Moisture sensor pins
const int sensorPins[NUM_PLANTS] = {A0, A1, A2};

// Hard-coded profit (priority) for each plant
int profit[NUM_PLANTS] = {5, 3, 8};

// Water tank capacity
int tankCapacity = 1000;
const int fullTank = 1000;

// Moisture threshold
const int moistureThreshold = 500;

// LED pins
const int redLedPin = 2;   // watering
const int greenLedPin = 3; // idle

// Relay pin
const int relayPin = 11;   // relay module for watering

bool startSimulation = false; // Controlled by Python GUI
bool tankEmptyStop = false;

void setup() {
  Serial.begin(9600);
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(relayPin, OUTPUT);
  
  digitalWrite(redLedPin, LOW);
  digitalWrite(greenLedPin, HIGH); // start green on
  digitalWrite(relayPin, LOW);     // relay off initially

  Serial.println("Irrigation System Ready.");
  Serial.println("Waiting for Python GUI to start simulation...");
}

void loop() {
  // Check for commands from Python GUI
  if (Serial.available() > 0) {
    char cmd = Serial.read();
    if (cmd == 's' || cmd == 'S') { // Start simulation
      startSimulation = true;
      Serial.println("Simulation Started!");
    } else if (cmd == 'r' || cmd == 'R') { // Refill tank
      tankCapacity = fullTank;
      Serial.println("Tank Refilled!");
      startSimulation = true;
      tankEmptyStop = false;
    }
    while (Serial.available() > 0) Serial.read(); // clear buffer
  }

  if (!startSimulation) return;

  int sensorValue[NUM_PLANTS];

  // Read sensors
  for (int i = 0; i < NUM_PLANTS; i++) {
    sensorValue[i] = analogRead(sensorPins[i]);
  }

  // Track watered plants
  bool watered[NUM_PLANTS] = {false};
  int remainingPlants = NUM_PLANTS;

  while (remainingPlants > 0 && tankCapacity > 0 && !tankEmptyStop) {
    int bestIndex = -1;
    float bestValue = -1.0;

    // Knapsack: choose plant with highest profit/waterNeeded
    for (int i = 0; i < NUM_PLANTS; i++) {
      if (!watered[i] && sensorValue[i] > moistureThreshold) {
        int waterNeeded = sensorValue[i] - moistureThreshold;
        float ratio = (float)profit[i] / max(1, waterNeeded);
        if (ratio > bestValue) {
          bestValue = ratio;
          bestIndex = i;
        }
      }
    }

    if (bestIndex == -1) break; // no more plants need water

    // Water plant step by step
    int waterStep = 10;
    while (sensorValue[bestIndex] > moistureThreshold && tankCapacity > 0 && !tankEmptyStop) {
      int used = min(waterStep, sensorValue[bestIndex] - moistureThreshold);
      sensorValue[bestIndex] -= used;
      tankCapacity -= used;

      // RED LED ON while watering
      digitalWrite(redLedPin, HIGH);
      digitalWrite(greenLedPin, LOW);
      digitalWrite(relayPin, HIGH); // TURN ON RELAY

      // Send data to Python GUI
      Serial.print("Plant "); Serial.print(bestIndex + 1);
      Serial.print(" moisture: "); Serial.print(sensorValue[bestIndex]);
      Serial.print(" | Tank remaining: "); Serial.println(tankCapacity);

      if (tankCapacity <= 0) {
        Serial.println("Tank empty!");
        tankEmptyStop = true;
        startSimulation = false;
        break;
      }

      delay(200);
    }

    // GREEN LED ON and turn off relay when not watering
    digitalWrite(redLedPin, LOW);
    digitalWrite(greenLedPin, HIGH);
    digitalWrite(relayPin, LOW); // TURN OFF RELAY

    if (sensorValue[bestIndex] <= moistureThreshold) {
      Serial.print("Plant "); Serial.print(bestIndex + 1);
      Serial.println(" fully watered!");
    }

    watered[bestIndex] = true;
    remainingPlants--;
  }

  // Check if all plants are watered
  bool allWatered = true;
  for (int i = 0; i < NUM_PLANTS; i++) {
    if (sensorValue[i] > moistureThreshold) allWatered = false;
  }

  if (allWatered) {
    Serial.println("All plants watered!");
    startSimulation = false;
    digitalWrite(redLedPin, LOW);
    digitalWrite(greenLedPin, HIGH); // idle green
    digitalWrite(relayPin, LOW);     // ensure relay OFF
  }

  delay(500); // small pause before next loop
}
