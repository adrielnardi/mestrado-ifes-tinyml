/* importando bibliotecas  Firebase */
#include "FS.h"
#include "SPIFFS.h"
#include <time.h>
#include <WiFi.h>                  //importa biblioteca para conectar esp32 com wifi
#include <IOXhop_FirebaseESP32.h>  //importa biblioteca para esp32 se comunicar com firebase
#include <ArduinoJson.h>           //importa biblioteca para colocar informação no formato json, utilizado no firebase (intalar versão 5)
/* Fim das Importações das Firebase */

/*Portas para comunicação serial */
#define RXp2 32
#define TXp2 33

/* Configurando conexão com wifi e url/token do firebase */
#define WIFI_SSID "Oficial"
#define WIFI_PASSWORD "Paranaue"
#define FIREBASE_HOST "https://tinyml-de108-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "wNEd3ado9rA8RihA1tJnAZaTGY1NxvpBb15phW7y"
/* Fim das declarações conexão com wifi e url/token do firebase */

/* Nome do Ciclo e inicialização da varivel responsavel por contar o numero de pessoas */
String ciclo = "Rodada04SemDeteccaoPessoas";
int contador = 0;
/* Fim do nome do ciclo / variavel contador*/

/* Configuração de Fuso horario */
long timezone = -3;
byte daysavetime = 0;
/* Fim do Fuso*/

void setup() {
  Serial.begin(115200);  //inicia comunicação serial
  Serial2.begin(9600, SERIAL_8N1, RXp2, TXp2);
  /* ------- Conectar ao wifi ------- */
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Conectando ao wifi");

  while (WiFi.status() != WL_CONNECTED) {
    /* Imprime pontos enquanto não conecta */
    Serial.print(".");
    delay(300);
  }
  /* ------- Fim  (Wifi Conectado) ------- */

  /* Uma vez conectado a internet, conectamos ao firebase */
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);  //inicia comunicação com firebase definido anteriormente
  contador = Firebase.getInt("/sinc/pessoas");
  /* Fim da conexão */
}

void loop() {

  /* Configuração das horas */
  configTime(3600 * timezone, daysavetime * 3600, "time.nist.gov", "0.pool.ntp.org", "1.pool.ntp.org");
  struct tm tmstruct;
  tmstruct.tm_year = 0;
  getLocalTime(&tmstruct, 5000);

  int ano = (tmstruct.tm_year) + 1900;
  int mes = (tmstruct.tm_mon) + 1;
  int dia = tmstruct.tm_mday;
  int hora = tmstruct.tm_hour;
  int minuto = tmstruct.tm_min;
  int segundo = tmstruct.tm_sec;
  /* Fim da configuração das horas */

  int bateria = analogRead(36);  //porta pra medição da bateria
  //Serial.println(bateria);
  
  //bateria = map(bateria, 1970, 2366, 0, 100);  //mapeamento da bateria
  // 4,2 100% 2,965v / 3.3*4095 = 3679,29
  // 3.3 0% 2,329v / 3.3*4095 = 2890,07
  //Serial.println(bateria);
  
/*
  if (bateria <= 0) {
    bateria = 0;
  }else if(bateria >= 100){
    bateria=100;
  }
*/
  if (Serial2.available()){
    contador++;
    delay(1500);
    String score = Serial2.readString();
    Serial.println(score);
    String fireCarga = String(contador) + String(";   ") + String(bateria) + String("%; ") + String(ano) + String("/") + String(mes) + String("/") + String(dia) + String("-") + String(hora) + String(":") + String(minuto) + String(":") + String(segundo)+String("; ")+String(score);
    Firebase.pushString(String("/") + String(ciclo) + String("/registro"), fireCarga);
    Firebase.setInt("/sinc/pessoas", contador);
  }
  else 
    {
      String fireCarga = String(contador) + String(";   ") + String(bateria) + String("%; ") + String(ano) + String("/") + String(mes) + String("/") + String(dia) + String("-") + String(hora) + String(":") + String(minuto) + String(":") + String(segundo);
      Firebase.pushString(String("/") + String(ciclo) + String("/registro"), fireCarga);
      Firebase.setInt("/sinc/pessoas", contador);
    }
}
