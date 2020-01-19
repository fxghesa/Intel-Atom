int pinLed = 9; // LED terhubung dengan pin 3

void setup(){}

void loop() {
   // buat iterasi dari 0 sampai 255 dengan kelipatan 5
   for (int nilai = 0; nilai <= 255; nilai += 5) {
      // set nilai analog menggunakan iterasi di atas
      analogWrite(pinLed, nilai);
      // beri jeda waktu 3 milidetik biar ga terlalu cepat
      delay(30);
   }
}
