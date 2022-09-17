volatile int botao = 0;
int led = 0;

enum State_t {
    OFF = 0,
    E1 = 1,
    ON = 2,
    E2 = 3
};

State_t state = OFF;

void botao_isr(){
    static unsigned long ultima_interrupcao = 0;
    static unsigned long tempo_botao = 50;
    
    unsigned long agora = millis();

    if (agora - ultima_interrupcao >= tempo_botao) {
      Serial.println('b');
      botao = 1;
      ultima_interrupcao = agora;
    }
}

void setup() {
    pinMode(2, INPUT); // sensor pin
    pinMode(13, OUTPUT); // LED (actuator) pin
    Serial.begin(9600); // open serial port to host
    attachInterrupt(digitalPinToInterrupt(2), botao_isr, RISING);
}

/*
void botao_isr(){
    Serial.println('b');
    botao = 1;
  
}
*/

void loop() {
    // Atualiza entrada
    noInterrupts();
    int b = botao;
    botao = 0;
    interrupts();

    // Atualiza máquina de estados
    switch (state){
        case OFF:
            if (b == 1){
                state = E1;
                led = 1;
                Serial.println("OFF -> E1");
                attachInterrupt(digitalPinToInterrupt(2), botao_isr, LOW);
            }
            break;

        case E1:
            if (b == 1){
                state = ON;
                led = 1;
                Serial.println("E1 -> ON");
                attachInterrupt(digitalPinToInterrupt(2), botao_isr, RISING);
            }
            break;

        case ON:
            if (b == 1){
                state = E2;
                led = 0;
                Serial.println("ON -> E2");
                attachInterrupt(digitalPinToInterrupt(2), botao_isr, LOW);
            }
            break;

        case E2:
            if (b == 1){
                state = OFF;
                led = 0;
                Serial.println("E2 -> OFF");
                attachInterrupt(digitalPinToInterrupt(2), botao_isr, RISING);
            }
            break;
    }
  
    // Atualiza saída
    digitalWrite(13, led);
    delay(50);

}
