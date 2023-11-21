/* Bibliotecas da inteligencia artificial */
#include "EloquentTinyML.h"
#include "eloquent.h"
#include "eloquent_tinyml/tensorflow/person_detection.h"
#include "eloquent/vision/camera/aithinker.h"
/* Fim das Bibliotecas da Inteligência Artificial */


/*Configuração da Camera */
const uint16_t imageWidth = 160;
const uint16_t imageHeight = 120;

Eloquent::TinyML::TensorFlow::PersonDetection<imageWidth, imageHeight> personDetector;
/* Fim da Configuração da Camera */


/* Definindo Pino do Flash/Led */
#define flash 4
/* Fim da definição */

void setup() {
    Serial2.begin(115200);
    Serial.begin(9600);


    /* --------- Configurações da Camera --------- */
    camera.grayscale();
    camera.qqvga();
    /* Inicia a Camera */
    while (!camera.begin())
        Serial2.println("Cannot init camera");
    
    personDetector.setDetectionAbsoluteThreshold(190);
    personDetector.begin();
    /*Caso Ocorra Erro */
    while (!personDetector.isOk()) {
        Serial2.print("Detector init error: ");
        Serial2.println(personDetector.getErrorMessage());
    }
    /* --------- Fim das Configurações da Camera --------- */

    /* Definir Flash como saida */
    pinMode(flash,OUTPUT);
    /*Fim da definição do flash */
}

void loop() {

    /* --------- Captura imagem uma imagem a cada 1 segundo --------- */
    if (!camera.capture()) {
        Serial2.println("Camera capture error");
        delay(1000);
        return;
    }
    /* Verifica se detectou algo */
    bool isPersonInFrame = personDetector.detectPerson(camera.buffer);
    /* Fim da verificação */

    /* Caso tenha acontecido algum erro: */
    if (!personDetector.isOk()) {
        Serial2.print("Person detector detection error: ");
        Serial2.println(personDetector.getErrorMessage());
        delay(1000);
        return;
    }
    /* --------- Fim do retorno do erro --------- */


    /* ----------- * 

    Abaixo vamos comparar o byte da imagem capturada com um modelo de byte com
    e sem pessoa, para detectar se há ou não uma pessoa na imagem 

    *  ----------- */
    byte viuPessoa = personDetector.getPersonScore();
    byte naoViuPessoa = personDetector.getNotPersonScore();
    /* 
    Com os valores em mãos vamos comparar se está mais proximo
    a uma pessoa ou não.
    */
    if(viuPessoa > naoViuPessoa){
      digitalWrite(flash,HIGH);
      delay(50);
      digitalWrite(flash,LOW);
      int acuracia = map(viuPessoa, 0, 255, 0, 100);
      Serial.println(acuracia);
      Serial2.print("Detectei ");

    }
      Serial2.print("Não Detectei ");
    /*
    Fim da comparação
    */
    delay(1000);
}