#define N_READINGS 5

#define WEIGHT_1 -2
#define WEIGHT_2 -1
#define WEIGHT_3  0
#define WEIGHT_4  1
#define WEIGHT_5  2

int IR1 = A1;
int IR2 = A2;
int IR3 = A3;
int IR4 = A4;
int IR5 = A5;

void setup() {
  Serial.begin(9600);
}

void loop() {
  float value = get_readings();
  
  if(-10.0 <= value && value <= 10.0)
  {
    Serial.println("Straight: " + String(value));
  }
  else if(value <= 0)
  {
    Serial.println("Right: " + String(value));
  }
  else if(value >= 0)
  {
    Serial.println("Left: " + String(value));
  }
  else
  {
    Serial.println("Opsie");
  }

  delay(10000);
}

int get_readings() {

  int IR1_readings[N_READINGS];
  int IR2_readings[N_READINGS];
  int IR3_readings[N_READINGS];
  int IR4_readings[N_READINGS];
  int IR5_readings[N_READINGS];

  float weighted_averages[N_READINGS];

  for(int i = 0; i < N_READINGS; i++)
  {
    IR1_readings[i] = analogRead(IR1);
    IR2_readings[i] = analogRead(IR2);
    IR3_readings[i] = analogRead(IR3);
    IR4_readings[i] = analogRead(IR4);
    IR5_readings[i] = analogRead(IR5);
    weighted_averages[i] = (IR1_readings[i]*WEIGHT_1)+(IR2_readings[i]*WEIGHT_2)+(IR3_readings[i]*WEIGHT_3)+(IR4_readings[i]*WEIGHT_4)+(IR5_readings[i]*WEIGHT_5);
  }

  float sum_of_averages = 0;

  for(int i = 0; i < N_READINGS; i++) 
  {
    sum_of_averages += weighted_averages[i];
  }

  return sum_of_averages/N_READINGS;
}