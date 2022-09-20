volatile int botao_escala = 0;

enum State_t {
  WAIT = 0,
  ADQUIRE_ENTRADAS = 1,
  PROCESSA_V = 2,
  PROCESSA_mV = 3,
  IMPRIME_SAIDA = 4
};

State_t estado = WAIT;

void botao_isr(){
    static unsigned long ultima_interrupcao = 0;
    static unsigned long tempo_botao = 50;
    
    unsigned long agora = millis();

    if (agora - ultima_interrupcao >= tempo_botao) {
      botao_escala = 1;
      ultima_interrupcao = agora;
    }
}

void setup() {
  // Inicializar HW
  pinMode(2, INPUT);     // botão de hold
  pinMode(3, INPUT);     // botão de escala
  Serial.begin(115200);
  attachInterrupt(digitalPinToInterrupt(3), botao_isr, RISING);
  Serial.println("setup");

}

unsigned long medida;
unsigned long parte_decimal = 0;
unsigned long parte_inteira = 0;

State_t processo_atual = PROCESSA_V;

int a0 = 0;
int botao_hold = 0;

void loop() {
  // Atualizar entrada
  noInterrupts(); 
  int b = botao_escala;
  botao_escala = 0;
  interrupts();

  // Estados
  switch (estado){
    case WAIT:
      estado = ADQUIRE_ENTRADAS;
      delay(500);
      break;

    case ADQUIRE_ENTRADAS:
      a0 = analogRead(A0);
      botao_hold = digitalRead(2);

      if (b == 1 ) {
        if (processo_atual == PROCESSA_V){
          estado = PROCESSA_mV;
        
        } else if (processo_atual == PROCESSA_mV) {
          estado = PROCESSA_V;

        }
      } else {
        estado = processo_atual;
      }

      processo_atual = estado;
      
      attachInterrupt(digitalPinToInterrupt(3), botao_isr, RISING);
      break;

    case PROCESSA_V:
      if (botao_hold == 1){
        estado = WAIT;
      } else {
        estado = IMPRIME_SAIDA;
        medida = (((long) analogRead(A0)) * ((long) 5000000 / (long) 1023)) / 1000;
        parte_inteira = medida / 1000;
        parte_decimal = medida - (parte_inteira * 1000); 
      }
      break;

    case PROCESSA_mV:  
      if (botao_hold == 1){
        estado = WAIT;
      } else {
        estado = IMPRIME_SAIDA;
        medida = (((long) a0) * ((long) 5000000 / (long) 1023)) / 1000;
      }
      break;

    case IMPRIME_SAIDA: 
      estado = WAIT;

      if (processo_atual == PROCESSA_V) {
        Serial.print(parte_inteira);
        Serial.print(",");
        Serial.println(parte_decimal);
      }
      else Serial.println(medida);
      break;
  }
}
