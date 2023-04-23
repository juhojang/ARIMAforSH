
void setup() {
  Serial.begin(9600);
}
 
void loop() {
  
  int Soil_moisture = analogRead(A3);
  int sm_percentage=map(Soil_moisture,330,1023,100,0)  ;
  Serial.println(sm_percentage);        
  delay(100);
  
}