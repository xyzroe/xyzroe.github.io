#include <Wiegand.h>
#include <ButtonDebounce.h>
//#include <avr/wdt.h>
#include <DFPlayer_Mini_Mp3.h>

#define FALSE 0
#define TRUE  1
#define ENTER 1
#define EXIT  2
#define ON    1
#define OFF   0

//Input Pins
//uln2804 - 1 - 0,1,2,3,4
#define EnterZK         23
#define EnterAlcoOK     25
#define EnterAlcoAL     27
#define EnterAlcoRD     29
#define EnterAlcoOn     31

//#define TurnReady       33
//#define TurnReady       35


//uln2804 - 2 - 0,1,2,3
#define ExitZK          62
#define ExitAlcoOK      63
#define ExitAlcoAL      64
#define ExitAlcoRD      65
#define ExitAlcoOn      66

//Output Pins
//uln2803 - 1 - 0,1,2,3,4,5,6
#define EnterLedGo      39
#define EnterLedStop    41
#define EnterAlcoRun    43
#define ExitLedGo       45
#define ExitLedStop     47
#define ExitAlcoRun     49
#define TurnEnter       51
#define TurnExit        53

#define Vorota          59
#define DoorStreet      60
#define AlcoAlarm       61

//Times setup
#define LockOpenTime    2000
#define AlarmOnTime     700
#define ResetStageTime  15//seconds
#define AlcoStartTime   500
#define AlcoWaitMaxTime 5 //seconds

//DFPlayer setup
#define busyDFpin       7
#define mp3volume       25

#define soundStartAgain 1 //Начните с начала.
#define soundAlcoHowTo  2 //Сделайте вдох, дождитесь зеленого индикатора, выдохните в прозрачную трубку.
#define soundEnterOK    3 //Спасибо! Хорошей Вам смены!
#define soundExitOK     4 //Спасибо! Хорошего Вам отдыха!
#define soundError      5 //Ошибка!
#define soundNextTry    6 //Попробуйте еще раз!
#define soundEnterAlarm 7 //Вы пришли на работу в алкогольном опьянении, и на работу не допущены.
#define soundExitAlarm  8 //Вы употребляли алкоголь на работе, и за это вас оштрафуют. В следующий раз вы будете уволены!
#define soundGo         9 //Проходите пожалуйста

ButtonDebounce enterZK(EnterZK, 100);
ButtonDebounce enterAlcoOK(EnterAlcoOK, 100);
ButtonDebounce enterAlcoAL(EnterAlcoAL, 100);
ButtonDebounce enterAlcoRD(EnterAlcoRD, 100);
ButtonDebounce enterAlcoOn(EnterAlcoOn, 100);
ButtonDebounce exitZK(ExitZK, 100);
ButtonDebounce exitAlcoOK(ExitAlcoOK, 100);
ButtonDebounce exitAlcoAL(ExitAlcoAL, 100);
ButtonDebounce exitAlcoRD(ExitAlcoRD, 100);
ButtonDebounce exitAlcoOn(ExitAlcoOn, 100);

//ButtonDebounce turnReady(TurnReady, 100);

//unsigned long previousMillis = 0;
unsigned long currentMillis = 0;
unsigned long lastOpenEnter = 0;
unsigned long lastOpenExit = 0;
unsigned long lastOpenStreet = 0;
unsigned long lastOpenVorota = 0;

int cycles = 0;
int cycles_Enter = 0;
int cycles_Exit = 0;

int enterStage = 0;
int exitStage = 0;
int lastEnterEmp = 0;
int lastExitEmp = 0;

bool enterEnable;
bool exitEnable;

WIEGAND wg;

const byte numChars = 16;
char receivedChars[numChars]; // an array to store the received data

boolean newData = false;

char taskPC[3] = {0};
int dirPC = 0;

char recvChar;
char endMarker = '>';

void setup() {
  //Serial begin
  Serial.begin(9600);
  Serial.println(F("Fingerprints Alcometers Turnstile Controler v0.5a"));
  Serial.println(F("start boot"));

  //Initialised output pins
  pinMode(EnterLedGo, OUTPUT); digitalWrite(EnterLedGo, LOW);
  pinMode(TurnEnter, OUTPUT); digitalWrite(TurnEnter, LOW);
  pinMode(EnterAlcoRun, OUTPUT); digitalWrite(EnterAlcoRun, LOW);
  pinMode(ExitLedGo, OUTPUT); digitalWrite(ExitLedGo, LOW);
  pinMode(TurnExit, OUTPUT); digitalWrite(TurnExit, LOW);
  pinMode(ExitAlcoRun, OUTPUT); digitalWrite(ExitAlcoRun, LOW);
  pinMode(AlcoAlarm, OUTPUT); digitalWrite(AlcoAlarm, LOW);
  pinMode(DoorStreet, OUTPUT); digitalWrite(DoorStreet, LOW);
  //Watchdog enable
  //wdt_enable(WDTO_8S);

  //Wiegand reades config and start

  wg.D0PinA = 3;
  wg.D1PinA = 2;
  wg.D0PinB = 19;
  wg.D1PinB = 18;

  wg.begin(TRUE, TRUE);

  //DFPlayer-mini mp3 module config and start
  Serial3.begin(9600);
  pinMode(busyDFpin, INPUT);
  mp3_set_serial (Serial3);
  mp3_set_volume (mp3volume);
  delay(250);

  //Serial for Orange PI
  Serial2.begin(9600);

  //Start AlcoMeter Enter
  delay(100);
  cycles = 0;
  enterAlcoOn.update();
  delay(100);
  if (enterAlcoOn.state() == 0) {
    AlcoOnOff(ON, ENTER);
    delay(100);
    enterAlcoOn.update();
    delay(100);
  }
  enterAlcoRD.update();
  delay(100);
  if (enterAlcoOn.state() == 1) {
    if (enterAlcoRD.state() == 1) cycles++;
    while (enterAlcoRD.state() == 0 && cycles < AlcoWaitMaxTime * 10)  {
      enterAlcoRD.update();
      delay(100);
      cycles++;
    }
    if (cycles > 0 && cycles < AlcoWaitMaxTime * 10 ) {
      AlcoOnOff(OFF, ENTER);
      delay(100);
      enterAlcoRD.update();
      enterAlcoOn.update();
      delay(100);
      if (enterAlcoOn.state() == 0 && enterAlcoRD.state() == 0) {
        Serial.println(F("enable enter alco"));
        enterEnable = 0;//= 1;
      }
      else {
        Serial.println(F("disable enter alco"));
        enterEnable = 0;
      }
    }
    else {
      Serial.println(F("disable enter alco"));
      enterEnable = 0;
    }
  }
  if (enterEnable == 0) { //if error
    digitalWrite(EnterLedStop, HIGH); delay(250); digitalWrite(EnterLedStop, LOW); delay(250); digitalWrite(EnterLedStop, HIGH); delay(250); digitalWrite(EnterLedStop, LOW); delay(250); digitalWrite(EnterLedStop, HIGH); delay(250); digitalWrite(EnterLedStop, LOW);
  }

  //Start AlcoMeter Exit
  delay(100);
  cycles = 0;
  exitAlcoOn.update();
  delay(100);
  if (exitAlcoOn.state() == 0) {
    AlcoOnOff(ON, EXIT);
    delay(100);
    exitAlcoOn.update();
    delay(100);
  }
  exitAlcoRD.update();
  delay(100);
  if (exitAlcoOn.state() == 1) {
    if (exitAlcoRD.state() == 1) cycles++;
    while (exitAlcoRD.state() == 0 && cycles < AlcoWaitMaxTime * 10)  {
      exitAlcoRD.update();
      delay(100);
      cycles++;
    }
    if (cycles > 0 && cycles < AlcoWaitMaxTime * 10 ) {
      AlcoOnOff(OFF, EXIT);
      delay(100);
      exitAlcoRD.update();
      exitAlcoOn.update();
      delay(100);
      if (exitAlcoOn.state() == 0 && exitAlcoRD.state() == 0) {
        Serial.println(F("enable exit alco"));
        exitEnable = 0;//= 1;
      }
      else {
        Serial.println(F("disable exit alco"));
        exitEnable = 0;
      }
    }
    else {
      Serial.println(F("disable exit alco"));
      exitEnable = 0;
    }
  }
  if (exitEnable == 0) { //if error
    digitalWrite(ExitLedStop, HIGH); delay(250); digitalWrite(ExitLedStop, LOW); delay(250); digitalWrite(ExitLedStop, HIGH); delay(250); digitalWrite(ExitLedStop, LOW); delay(250); digitalWrite(ExitLedStop, HIGH); delay(250); digitalWrite(ExitLedStop, LOW);
  }
  Serial.println(F("boot finish"));
  cycles = 0;
}

void loop() {
  currentMillis = millis();
  wiegandCheck();
  checkEnter();
  checkExit();
  checkLocks();
  recvWithEndMarker();
}

void wiegandCheck() {
  if (wg.available())
  {
    Serial.print(F("s| Emp = ")); Serial.print(wg.getCode()); Serial.print(F(" gate = ")); Serial.println(wg.getGateActive());
    if (wg.getGateActive() == 1) {
      lastEnterEmp = wg.getCode();
    }
    if (wg.getGateActive() == 2)
    {
      lastExitEmp = wg.getCode();
    }
  }
}

void checkEnter() {
  enterZK.update();
  switch (enterStage) {
    case 0:
      if (enterZK.state() == LOW && lastEnterEmp != 0) {
        enterStage = 1;
        printStage(ENTER);
        if (enterEnable == 1) {
          digitalWrite(EnterLedGo, HIGH);
          mp3_play (soundAlcoHowTo); delay(250);
          AlcoOnOff(ON, ENTER);
        }
        else {
          mp3_play (soundEnterOK); delay(250);
          enterStage = 5;
          delay(100);
          printStage(ENTER);
          digitalWrite(TurnEnter, HIGH); lastOpenEnter = currentMillis;
          Serial.println(F("s| Turn Enter On"));
        }
      }
      break;
    case 1:
    case 3:
      enterAlcoOn.update();
      enterAlcoOK.update();
      enterAlcoAL.update();
      delay(100);
      if (enterAlcoOK.state() == HIGH && enterAlcoAL.state() == HIGH && cycles_Enter < ResetStageTime * 10 ) {
        delay(100);
        cycles_Enter++;
      }
      else if (enterAlcoOK.state() == LOW) {
        cycles_Enter = 0;
        mp3_play (soundEnterOK); delay(250);
        enterStage = 5;
        printStage(ENTER);
        digitalWrite(TurnEnter, HIGH); lastOpenEnter = currentMillis;
        Serial.println(F("s| Turn Enter On"));
        while (enterAlcoOK.state() == LOW) {
          enterAlcoOK.update();
          delay(100);
        }
      }
      else if (enterAlcoAL.state() == LOW) {
        cycles_Enter = 0;
        mp3_play (soundError); delay(250);
        digitalWrite(EnterLedStop, HIGH);

        while (digitalRead(busyDFpin) == LOW) {
          delay(100);
        }
        digitalWrite(EnterLedStop, LOW);
        //enterStage = enterStage + 1;//вторая попытка
        enterStage = 4; //переходим сразу к сообщению об алкоголизме enterStage = 4; //переходим сразу к сообщению об алкоголизме
        if (enterStage == 4) printStage(ENTER);
        while (enterAlcoAL.state() == LOW) {
          enterAlcoAL.update();
          delay(100);
        }
      }
      else if (cycles_Enter >= ResetStageTime * 10) {
        cycles_Enter = 0;
        Serial.println("s| time out while enter alco test");
        mp3_play (soundError); delay(250);
        while (digitalRead(busyDFpin) == LOW) {
          delay(100);
        }
        mp3_play (soundStartAgain); delay(250);
        enterStage = 5;
      }
      break;
    case 2:
      mp3_play (soundNextTry); delay(250);
      enterStage = 3;
      printStage(ENTER);
      break;
    case 4:
      digitalWrite(EnterLedStop, HIGH);
      digitalWrite(AlcoAlarm, HIGH); delay(AlarmOnTime); digitalWrite(AlcoAlarm, LOW);
      mp3_play (soundEnterAlarm); delay(250);
      enterStage = 5;
      break;
    case 5:
      enterStage = 0;
      lastEnterEmp = 0;
      if (enterEnable == 1) {
        digitalWrite(EnterLedGo, LOW);
        digitalWrite(EnterLedStop, LOW);
        AlcoOnOff(OFF, ENTER);
      }
      Serial.println(F("s| finish entering"));
      break;
  }
}

void checkExit() {
  exitZK.update();
  switch (exitStage) {
    case 0:
      if (exitZK.state() == LOW && lastExitEmp != 0) {
        exitStage = 1;
        printStage(EXIT);
        if (exitEnable == 1) {
          digitalWrite(ExitLedGo, HIGH);
          mp3_play (soundAlcoHowTo); delay(250);
          AlcoOnOff(ON, EXIT);
        }
        else {
          mp3_play (soundExitOK); delay(250);
          exitStage = 5;
          delay(100);
          printStage(EXIT);
          digitalWrite(TurnExit, HIGH); lastOpenExit = currentMillis;
          Serial.println(F("s| Turn Exit On"));
        }
      }
      break;
    case 1:
    case 3:
      // Serial.println("s| case 1 3");
      exitAlcoOn.update();
      exitAlcoOK.update();
      exitAlcoAL.update();
      delay(100);
      if (exitAlcoOK.state() == HIGH && exitAlcoAL.state() == HIGH && cycles_Exit < ResetStageTime * 10 ) {
        delay(100);
        cycles_Exit++;
      }
      else if (exitAlcoOK.state() == LOW) {
        cycles_Exit = 0;
        mp3_play (soundExitOK); delay(250);
        exitStage = 5;
        printStage(EXIT);
        digitalWrite(TurnExit, HIGH); lastOpenExit = currentMillis;
        Serial.println(F("s| Turn Exit On"));
        while (exitAlcoOK.state() == LOW) {
          exitAlcoOK.update();
          delay(100);
        }
      }
      else if (exitAlcoAL.state() == LOW) {
        cycles_Exit = 0;
        mp3_play (soundError); delay(250);
        digitalWrite(ExitLedStop, HIGH);
        while (digitalRead(busyDFpin) == LOW) {
          delay(100);
        }
        digitalWrite(ExitLedStop, LOW);
        //exitStage = exitStage + 1;//вторая попытка
        exitStage = 4; //переходим сразу к сообщению об алкоголизме
        if (exitStage == 4) printStage(EXIT);
        while (exitAlcoAL.state() == LOW) {
          exitAlcoAL.update();
          delay(100);
        }
      }
      else if (cycles_Exit >= ResetStageTime * 10) {
        cycles_Exit = 0;
        Serial.println("s| time out while exit alco test");
        mp3_play (soundError); delay(250);
        while (digitalRead(busyDFpin) == LOW) {
          delay(100);
        }
        mp3_play (soundStartAgain); delay(250);
        exitStage = 5;
      }
      break;
    case 2:
      mp3_play (soundNextTry); delay(250);
      exitStage = 3;
      printStage(EXIT);
      break;
    case 4:
      digitalWrite(ExitLedStop, HIGH);
      digitalWrite(AlcoAlarm, HIGH); delay(AlarmOnTime); digitalWrite(AlcoAlarm, LOW);
      mp3_play (soundExitAlarm); delay(250);
      exitStage = 5;
      break;
    case 5:
      exitStage = 0;
      lastExitEmp = 0;
      if (exitEnable == 1) {
        digitalWrite(ExitLedGo, LOW);
        digitalWrite(ExitLedStop, LOW);
        AlcoOnOff(OFF, EXIT);
      }
      Serial.println(F("s| finish exiting"));
      break;
  }
}

void checkLocks() {
  if (lastOpenEnter != 0 && (currentMillis - lastOpenEnter) >= LockOpenTime) {
    digitalWrite(TurnEnter, LOW);
    lastOpenEnter = 0;
    Serial.println(F("s| Turn Enter Off"));
  }
  if (lastOpenExit != 0 && (currentMillis - lastOpenExit) >= LockOpenTime) {
    digitalWrite(TurnExit, LOW);
    lastOpenExit = 0;
    Serial.println(F("s| Turn Exit Off"));
  }
  if (lastOpenStreet != 0 && (currentMillis - lastOpenStreet) >= LockOpenTime) {
    digitalWrite(DoorStreet, LOW);
    lastOpenStreet = 0;
    Serial.println(F("s| Turn Street Off"));
  }
  if (lastOpenVorota != 0 && (currentMillis - lastOpenVorota) >= LockOpenTime) {
    digitalWrite(Vorota, LOW);
    lastOpenVorota = 0;
    Serial.println(F("s| Turn Vorota Off"));
  }
}

void AlcoOnOff(int State, int Gate) {
  if (Gate == ENTER) {
    enterAlcoOn.update();
    delay(100);
    if (enterAlcoOn.state() != State) {
      digitalWrite(EnterAlcoRun, HIGH); delay(AlcoStartTime); digitalWrite(EnterAlcoRun, LOW);
      if (State == 0) Serial.println(F("s| Alco Enter Off"));
      if (State == 1) Serial.println(F("s| Alco Enter On"));
    }
  }
  if (Gate == EXIT) {
    exitAlcoOn.update();
    delay(100);
    if (exitAlcoOn.state() != State) {
      digitalWrite(ExitAlcoRun, HIGH); delay(AlcoStartTime); digitalWrite(ExitAlcoRun, LOW);
      if (State == 0) Serial.println(F("s| Alco Exit Off"));
      if (State == 1) Serial.println(F("s| Alco Exit On"));
    }
  }
}


void printStage(int Gate) {
  if (Gate == ENTER) {
    Serial2.print(F("ENTER|")); Serial2.print(lastEnterEmp); Serial2.print(F("|")); Serial2.println(enterStage);
  }
  if (Gate == EXIT) {
    Serial2.print(F("EXIT|")); Serial2.print(lastExitEmp); Serial2.print(F("|")); Serial2.println(exitStage);
  }
}

void recvWithEndMarker() {
  static byte ndx = 0;
  char endMarker = '\n';
  char rc;

  while (Serial2.available() > 0 && newData == false) {
    rc = Serial2.read();

    if (rc != endMarker) {
      receivedChars[ndx] = rc;
      ndx++;
      if (ndx >= numChars) {
        ndx = numChars - 1;
      }
    }
    else {
      receivedChars[ndx] = '\0'; // terminate the string
      ndx = 0;
      newData = true;
      parseData();
    }
  }
}

void parseData() {
  char * strtokIndx; // this is used by strtok() as an index

  strtokIndx = strtok(receivedChars, "|"); // get the first part - the string
  strcpy(taskPC, strtokIndx); // copy it to taskPC
  Serial.println(taskPC);
  strtokIndx = strtok(NULL, "|"); // this continues where the previous call left off
  dirPC = atoi(strtokIndx); // convert this part to an integer
  Serial.println(dirPC);
  showParsedData();
}


void showParsedData() {
  if (taskPC[0] == 'O') {
    switch (dirPC) {
      case 1:
        {
          digitalWrite(TurnEnter, HIGH); lastOpenEnter = currentMillis;
          mp3_play (soundGo); delay(250);
          Serial.println(F("s| Turn Enter On"));
          enterStage = 5;
          break;
        }
      case 2:
        {
          digitalWrite(TurnExit, HIGH); lastOpenExit = currentMillis;
          mp3_play (soundGo); delay(250);
          Serial.println(F("s| Turn Exit On"));
          exitStage = 5;
          break;
        }
      case 3:
        {
          digitalWrite(DoorStreet, HIGH); lastOpenStreet = currentMillis;
          Serial.println(F("s| Turn Street On"));
          break;
        }
      case 4:
        {
          digitalWrite(Vorota, HIGH); lastOpenVorota = currentMillis;
          Serial.println(F("s| Turn Vorota On"));
          break;
        }
    }
  }
  newData = false;
}
