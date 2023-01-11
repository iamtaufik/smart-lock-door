#define BLYNK_TEMPLATE_ID           "TMPLUDrWn5g0"
#define BLYNK_DEVICE_NAME           "Quickstart Device"
#define BLYNK_AUTH_TOKEN            "-2DwicgAoLJ72rFRwgYuZbnTQTXag-9a"


// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#define RELAY_ON 0
#define RELAY_OFF 1
#define RELAY_1  D1   // pin yang digunakan yaitu pin D1

// Pin yang digunakan
const int SensorGetar = A0;         // Piezo sensor di pin 0.
const int button = D3;       // digunakan untuk memprogram ketukan baru
const int LED_Merah = 2;              // Status LED
const int LED_Hijau = D5;            // Status LED
const int Buzer = D6; 



const int Sandi_Min = 30;           // Ketukan minimal yang didaftar
const int Tolak_Sandi = 25;        
const int RerataTolak_Sandi = 15;
const int Waktu_Ketukan = 150;       
const int Sandi_Max = 20;      
const int Sandi_OK = 1200;   


// Variabel.
int Rahasia_Sandi[Sandi_Max] = {50, 25, 25, 50, 100, 50, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};  
int Membaca_Sandi[Sandi_Max];   
int Isi_Sandi = 0;           
int Tekan_Reset = false; 
int tekan; 

char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Wifi_Plus";
char pass[] = "kosongan";

BlynkTimer timer;

// This function is called every time the Virtual Pin 0 state changes
BLYNK_WRITE(V1)
{
  // Set incoming value from pin V0 to a variable
  int value = param.asInt();

  // Update state
   if(value == 1){
    digitalWrite(RELAY_1, LOW);
  }else {
    digitalWrite(RELAY_1, HIGH);
  }
  Blynk.virtualWrite(V1, value);
  Serial.println(value);
}

// This function is called every time the device is connected to the Blynk.Cloud
BLYNK_CONNECTED()
{
  // Change Web Link Button message to "Congratulations!"
  Blynk.setProperty(V3, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
  Blynk.setProperty(V3, "onImageUrl",  "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
  Blynk.setProperty(V3, "url", "https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstart-device-was-made");
}

// This function sends Arduino's uptime every second to Virtual Pin 2.
void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V2, millis() / 1000);
}

void setup()
{
   pinMode(RELAY_1, OUTPUT);
  digitalWrite(RELAY_1, RELAY_OFF);
  pinMode(LED_Merah, OUTPUT);
  pinMode(LED_Hijau, OUTPUT);
  pinMode(button, INPUT_PULLUP);
  pinMode(Buzer, OUTPUT);
  digitalWrite(Buzer, LOW);
  delay(200);
  digitalWrite(Buzer, HIGH);
  Serial.println("Program start."); 
    
  digitalWrite(LED_Hijau, HIGH); 
  timer.setInterval(1000L, myTimerEvent);   
  // Debug console
  Serial.begin(115200);

  Blynk.begin(auth, ssid, pass,IPAddress(43,229,135,169), 8080);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);

  // Setup a function to be called every second
  timer.setInterval(1000L, myTimerEvent);
}

void loop()
{
  Blynk.run();
  timer.run();
  delay(250);
  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
  Isi_Sandi = analogRead(SensorGetar);
  tekan = digitalRead(button);
  int SaklarSandi = !tekan;
  
  if (digitalRead(SaklarSandi)==HIGH){ 
    Tekan_Reset = false;
    digitalWrite(LED_Merah, LOW);
           
  } else {
     Tekan_Reset = true;          
    digitalWrite(LED_Merah, HIGH);  
  }
  
  if (Isi_Sandi >=Sandi_Min){
    listenToSecretKnock();
  } 
}

void listenToSecretKnock(){
  Serial.println("knock starting");   

  int i = 0;
  // First lets reset the listening array.
  for (i=0;i<Sandi_Max;i++){
    Membaca_Sandi[i]=0;
  }
  
  int currentKnockNumber=0;               // Incremen untuk array.
  int startTime=millis();                 // Mulai
  int now=millis();
  
  digitalWrite(LED_Hijau, LOW);           
  if (Tekan_Reset==true){
     digitalWrite(LED_Merah, LOW);                        
  }
  delay(Waktu_Ketukan);                                
  digitalWrite(LED_Hijau, HIGH);  
  if (Tekan_Reset==true){
     digitalWrite(LED_Merah, HIGH);                        
  }
  do {
    //listen for the next knock or wait for it to timeout. 
    Isi_Sandi = analogRead(SensorGetar);
    if (Isi_Sandi >=Sandi_Min){                  
      //record the delay time.
      Serial.println("knock.");
      now=millis();
      Membaca_Sandi[currentKnockNumber] = now-startTime;
      currentKnockNumber ++;                             
      startTime=now;          
      // and reset our timer for the next knock
      digitalWrite(LED_Hijau, LOW);  
      if (Tekan_Reset==true){
        digitalWrite(LED_Merah, LOW);                      
      }
      delay(Waktu_Ketukan);                           
      digitalWrite(LED_Hijau, HIGH);
      if (Tekan_Reset==true){
        digitalWrite(LED_Merah, HIGH);                         
      }
    }

    now=millis();
    
   
  } while ((now-startTime < Sandi_OK) && (currentKnockNumber < Sandi_Max));
  
 
  if (Tekan_Reset==false){           
    if (validateKnock() == true){
      triggerDoorUnlock(); 
    } else {
      Serial.println("Secret knock failed.");
  
      
      digitalWrite(LED_Hijau, LOW);    
      for (i=0;i<4;i++){          
        digitalWrite(LED_Merah, HIGH);
        digitalWrite(Buzer, LOW);
        delay(100);
        digitalWrite(LED_Merah, LOW);
        digitalWrite(Buzer, HIGH);
        delay(100);
      }
      digitalWrite(LED_Hijau, HIGH);
    }
  } else { 
    validateKnock();
   
    Serial.println("New lock stored.");
    digitalWrite(LED_Merah, LOW);
    digitalWrite(LED_Hijau, HIGH);
    for (i=0;i<3;i++){
      delay(100);
      digitalWrite(LED_Merah, HIGH);
      digitalWrite(LED_Hijau, LOW);
      delay(100);
      digitalWrite(LED_Merah, LOW);
      digitalWrite(LED_Hijau, HIGH);      
    }
  }
}



void triggerDoorUnlock(){
  Serial.println("Door unlocked!");
  digitalWrite(Buzer, LOW);
  delay(200);
  digitalWrite(Buzer, HIGH);
  
  
  int i=0;
  
  digitalWrite(RELAY_1, RELAY_ON); //lampu menyala 2 detik


  delay (2000);

  
  digitalWrite(LED_Hijau, HIGH);            
  
  delay (5000);                   

  
  
  digitalWrite(RELAY_1, RELAY_OFF);
  digitalWrite(Buzer, LOW);
  delay(200);
  digitalWrite(Buzer, HIGH);
  
  
 
  for (i=0; i < 5; i++){   
      digitalWrite(LED_Hijau, LOW);
      delay(100);
      digitalWrite(LED_Hijau, HIGH);
      delay(100);
      
  }
   
}


boolean validateKnock(){
  int i=0;
 
 
  int currentKnockCount = 0;
  int secretKnockCount = 0;
  int maxKnockInterval = 0;               
  
  for (i=0;i<Sandi_Max;i++){
    if (Membaca_Sandi[i] > 0){
      currentKnockCount++;
    }
    if (Rahasia_Sandi[i] > 0){           
      secretKnockCount++;
    }
    
    if (Membaca_Sandi[i] > maxKnockInterval){  
      maxKnockInterval = Membaca_Sandi[i];
    }
  }
  
 
  if (Tekan_Reset==true){
      for (i=0;i<Sandi_Max;i++){ // normalize the times
        Rahasia_Sandi[i]= map(Membaca_Sandi[i],0, maxKnockInterval, 0, 100); 
      }
     
      digitalWrite(LED_Hijau, LOW);
      digitalWrite(LED_Merah, LOW);
      delay(1000);
      digitalWrite(LED_Hijau, HIGH);
      digitalWrite(LED_Merah, HIGH);
      delay(50);
      for (i = 0; i < Sandi_Max ; i++){
        digitalWrite(LED_Hijau, LOW);
        digitalWrite(LED_Merah, LOW);  
        // only turn it on if there's a delay
        if (Rahasia_Sandi[i] > 0){                                   
          delay( map(Rahasia_Sandi[i],0, 100, 0, maxKnockInterval)); 
          digitalWrite(LED_Hijau, HIGH);
          digitalWrite(LED_Merah, HIGH);
        }
        delay(50);
      }
    return false;   
  }
  
  if (currentKnockCount != secretKnockCount){
    return false; 
  }
  
  /*  
      untuk membandingkan interval relatif mengetuk pada sensor
  */
  int totaltimeDifferences=0;
  int timeDiff=0;
  for (i=0;i<Sandi_Max;i++){ 
    Membaca_Sandi[i]= map(Membaca_Sandi[i],0, maxKnockInterval, 0, 100);      
    timeDiff = abs(Membaca_Sandi[i]-Rahasia_Sandi[i]);
    if (timeDiff > Tolak_Sandi){ 
      return false;
    }
    totaltimeDifferences += timeDiff;
  }
  
  if (totaltimeDifferences/secretKnockCount>RerataTolak_Sandi){
    return false; 
  }
  
  return true;
  
  
}
