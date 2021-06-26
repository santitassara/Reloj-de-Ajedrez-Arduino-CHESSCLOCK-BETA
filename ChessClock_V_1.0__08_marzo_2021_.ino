
// ****************************************************************************************
//                        LIBRERIAS
// ****************************************************************************************
#include <TM1637Display.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// ****************************************************************************************
//                        HARDWARE
// ****************************************************************************************
const int LED_Run =  13;                           // Pin donde se conecta el LED de Run
const int TM1637A_CLK = 2;                         // Pin CLK TM1637 A
const int TM1637A_DIO = 3;                         // Pin DIO TM1637 A
const int TM1637B_CLK = 4;                         // Pin CLK TM1637 B
const int TM1637B_DIO = 5;                         // Pin DIO TM1637 B
const int ButtonA = 6;                             // Pin Boton Jugador A
const int ButtonB = 7;                             // Pin Boton Jugador B
const int Buzzer_Pin = 8;                          // Pin donde se conecta el Buzzer (Beep)
const int ButtonS = 9;
const int Button1 = 10;
const int Button2 = 11;
// ****************************************************************************************
//                        BASE DE TIEMPOS
// ****************************************************************************************
const int BuzzerBeepTime = 80;                     // Tiempo de duracion del sonido Beep en (mS)
const int LEDRunPulse = 1000;                      // Tiempo de cambio del LED de Run (Programa Corriendo) (mS)
const int PointsPulse = 400;                       // Tiempo de parpadeo de los Puntos (mS)
const int TM1637Pulse = 200;                       // Tiempo de Refresh de los TM1637 (mS) TIENE QUE SER SIEMPRE MENOR QUE "PointsPulse"
const int Segundo = 885;                           // Tiempo que dura el Segundo (Aprox segun calculos 885)
int ButtonHold = 1500;                       // Tiempo que hay que mantener algun Boton Presionado (mS)
int ButtonHoldSet = 15;
const int ButtonTime = 250;                        // Tiempo que dura el Retardo Anti-Rebote 
const int ButtonTimeSet = 250;
const int LostTimeConst = 300;                    // Tiempo que tarda el fin de Partida
const int ResetGameTime = 3000;                    // Tiempo que tarda en generarse el Reset

// ****************************************************************************************
//                        MEMORIA RAM
// ****************************************************************************************
uint8_t A[8] = { '0', '3', '0', '0' };             // Inicio Cuenta del Reloj A (INICIO)
uint8_t B[8] = { '0', '3', '0', '0' };             // Inicio Cuenta del Reloj B (INICIO)
uint8_t ASEG[7];                                   // Segmentos del Reloj A
uint8_t BSEG[7];                                   // Segmentos del Reloj B
byte ButtonPress;                                  // Guarda la tecla recuperada
byte ButtonSelect;
byte ButtonUp;
byte ButtonDo;
byte ButtonSet;

// ****************************************************************************************
//                        BANDERAS DE EVENTOS
// ****************************************************************************************
boolean PausaFlag = false;
boolean PausaOffFlag = false;
boolean BlinkLEDFlag = false;                      // Activa/Desactiva Proceso de Flasheo del LED de RUN
boolean BlinkLEDStart = false;                     // Inicio/Paro de timer de LED de RUN
boolean BlinkAPointsFlag = false;                  // Activa/Desactiva Proceso de Flasheo de los Puntos de A
boolean BlinkAPointsStart = false;                 // Inicio/Paro de timer de los Puntos de A
boolean BlinkBPointsFlag = false;                  // Activa/Desactiva Proceso de Flasheo de los Puntos de B
boolean BlinkBPointsStart = false;                 // Inicio/Paro de timer de los Puntos de B
boolean TM1637RefreshFlag = false;                 // Activa/Desactiva Proceso de Actualizacion de los TM1637
boolean TM1637RefreshStart = false;                // Inicio/Paro de timer de Actualizacion de los TM1637
boolean CounterDownAFlag = false;                  // Activa/Desactiva Proceso de Cuenta Regresiva A
boolean CounterDownAStart = false;                 // Inicio/Paro de timer de Proceso de Cuenta Regresiva A
boolean CounterDownBFlag = false;                  // Activa/Desactiva Proceso de Cuenta Regresiva B
boolean CounterDownBStart = false;                 // Inicio/Paro de timer de Proceso de Cuenta Regresiva B
boolean ButtonsFlag = false;                       // Activa/Desactiva Proceso de Lectura de Botones
boolean ButtonsReady = false;                      // Indica cuando algun boton es presionado
boolean ButtonsReadySet = false;
boolean ButtonsBefore = false;                     // Indica si hay Boton presionado previo
boolean ButtonsBeforeSet = false;
boolean ButtonsStart = false;                      // Inicio/Paro de timer de Proceso de Lectura de Botones
boolean ButtonsStartSet = false;
boolean ButtonDebounceStart = false;               // Inicio/Paro de timer Anti-Rebote
boolean ButtonDebounceStartSet = false;



boolean GetButtonFlag = false;                     // Activa/Desactiva Proceso de Decodificacion de Boton Presionado
boolean GetButtonFlag1 = false;
boolean GameRunFlag = false;                       // Activa/Desactiva Proceso de Juego en Curso (RUN)
boolean GameRunButton = false;                     // 0: Boton A Presionado, 1: Boton B Presionado
boolean SelectGameFlag = false;                    // Activa/Desactiva Proceso de seleccion de Tiempos
boolean SelectGameFlag2 = false;
boolean SelectGameFlag3 = false;
boolean ResetGameStart = false;                    // Inicio/Paro de timer de espera para Reset
boolean BuzzerBeepFlag = false;                    // Activa/Desactiva Proceso de apagado de sonido Beep
boolean BuzzerBeepStart = false;                   // Inicio/Paro de timer de duracion de sonido Beep
boolean TimeSegAStart = false;                     // Inicio/Paro del timer de Proceso de Tiempo Segundo A
boolean TimeSegBStart = false;                     // Inicio/Paro del timer de Proceso de Tiempo Segundo B
boolean LostTimeStart = false;                     // Inicio/Paro del timer de "Tiempo Agotado" *SIN PROCESO*
int LastStateSelect = 0;
boolean PausaOn = false;

// ****************************************************************************************
//                        CONTADORES TIMERS VIRTUALES
// ****************************************************************************************
int BlinkLEDCount = 0;                             // Cuenta del Retardo de Encendido/Apagado del LED de RUN
int ButtonsCount = 0;                              // Cuenta del Retardo de Botones Presionados (Hold)
int ButtonsCountSet = 0;
int ButtonDebounceCount = 0;                       // Cuenta del Retardo Antirebote
int ButtonDebounceCountSet = 0;
int BuzzerBeepCount = 0;                           // Cuenta de duracion del Beep
int BlinkAPointsCount = 0;                         // Cuenta del Retardo de Encendido/Apagado de los Puntos de A
int BlinkBPointsCount = 0;                         // Cuenta del Retardo de Encendido/Apagado de los Puntos de B
int TM1637RefreshCount = 0;                        // Cuenta del Retardo del Refresh de los TM1637
int TimeSegACount = 0;                             // Cuenta del Retardo de Tiempo Segundo A
int TimeSegBCount = 0;                             // Cuenta del Retardo de Tiempo Segundo B
int LostTimeCount = 0;                             // Cuenta del Retardo de "Tiempo Agotado"
int ResetGameCount = 0;                            // Cuenta del Retado de Reset
int LastStateSelect2 = 0;
int LastStateSelect3 = 0;
int contadorPausa = 0;
// ****************************************************************************************
//                        CONFIGURACION DISPLAYS TM1637
// ****************************************************************************************
const uint8_t SEG_CLEAR[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
const uint8_t MODESELMinA[19][19] = {{'0','1','0','0'},
                                  {'0','2','0','0'},
                                  {'0','3','0','0'},
                                  {'0','4','0','0'},
                                  {'0','5','0','0'},
                                  {'0','6','0','0'},
                                  {'0','7','0','0'},
                                  {'0','8','0','0'},
                                  {'0','9','0','0'},
                                  {'1','0','0','0'},
                                  {'2','0','0','0'},
                                  {'3','0','0','0'},
                                  {'4','0','0','0'},
                                  {'5','0','0','0'},
                                  {'6','0','0','0'},
                                  {'7','0','0','0'},
                                  {'8','0','0','0'},
                                  {'9','0','0','0'}};  
const uint8_t MODESELMinB[19][19] = {{'0','1','0','0'},
                                  {'0','2','0','0'},
                                  {'0','3','0','0'},
                                  {'0','4','0','0'},
                                  {'0','5','0','0'},
                                  {'0','6','0','0'},
                                  {'0','7','0','0'},
                                  {'0','8','0','0'},
                                  {'0','9','0','0'},   
                                  {'1','0','0','0'},
                                  {'2','0','0','0'},
                                  {'3','0','0','0'},
                                  {'4','0','0','0'},
                                  {'5','0','0','0'},
                                  {'6','0','0','0'},
                                  {'7','0','0','0'},
                                  {'8','0','0','0'},
                                  {'9','0','0','0'}}; 
const uint8_t MODESEL[11][11] = {{'0','3','0','0'},  // 3 Final
                               {'0','3','0','0'},  // 3 +2
                               {'0','5','0','0'},  // 5 Final
                               {'0','5','0','0'},  // 5+3
                               {'1','0','0','0'},  // 10 Final  
                               {'1','0','0','0'},  // 10 + 5
                               {'1','5','0','0'},  // 15 + 10
                               {'3','0','0','0'},  // 30
                               {'3','0','0','0'},  // 30 + 20
                               {'9','0','0','0'}}; // Modo FIDE
                             
TM1637Display TM1637A( TM1637A_CLK, TM1637A_DIO ); // 
TM1637Display TM1637B( TM1637B_CLK, TM1637B_DIO ); //

// ****************************************************************************************
//                        CONFIGURACION DE LA INTERFAZ LCD
// ****************************************************************************************
LiquidCrystal_I2C lcd(0x27,16,2);                  //

// ****************************************************************************************
//                        FUNCIONALIDAD
// ****************************************************************************************
int GameMode = 0;                                  // Modo de Inicio/Defaul
int GameMode2 = -1;
int GameMode3 = -1;
// ****************************************************************************************
//                        CONFIGURACION INICIAL
// ****************************************************************************************
void setup()
{
  //Serial.begin(9600);
  //Configuracion del Hardware
  pinMode( ButtonA,INPUT_PULLUP );                 // Configura PIN como Entrada con Pull Up
  pinMode( ButtonB,INPUT_PULLUP );                 // Configura PIN como Entrada con Pull Up
  pinMode( ButtonS,INPUT_PULLUP );
  pinMode( Button1,INPUT_PULLUP );
  pinMode( Button2,INPUT_PULLUP );
  pinMode( LED_Run, OUTPUT );                      // Configura PIN como Salida
  pinMode( Buzzer_Pin, OUTPUT );                   // Configura PIN como Salida
  digitalWrite( Buzzer_Pin, LOW );                // Apaga el Buzzer con HIGH
  ASEG[1] |= 0x80;                                 // Coloca Puntos A (MOSTRAR AL ESTAR DETENIDO)
  BSEG[1] |= 0x80;                                 // Coloca Puntos B (MOSTRAR AL ESTAR DETENIDO)
  TM1637A.setBrightness( 0x0f );                   // Ajusta el Brillo del Display A
  TM1637B.setBrightness( 0x0f );                   // Ajusta el Brillo del Display B
  TM1637A.setSegments( SEG_CLEAR );                // Limpia Segmentos del Display A
  TM1637B.setSegments( SEG_CLEAR );                // Limpia Segmentos del Display B
  lcd.init();                                      // Inicializa el LCD
  lcd.init();                                      // Confirma Inicializacion
  lcd.backlight();                                 // Enciende la luz de fondo
  lcd.setCursor(0,0);
  lcd.print(" RELOJ AJEDREZ  ");
  lcd.setCursor( 0, 1 );                           // Posiciona Cursor
  lcd.print(" <- A JUGAR! -> ");
  
  //Ejecuta Procesos Iniciales
  BlinkLEDStart = true;                            // Arranca Timer Virtual (Proceso)
  TM1637RefreshStart = true;                       // Arranca Timer Virtual (Proceso)
  ButtonsFlag = true;                              // Arranca Proceso de Lectura de Botones
  GetButtonFlag = true;                            // Arranca Proceso de Presion de Boton (Inicio/Menu)
  GetButtonFlag1 = true;
  //Realiza un Beep
  digitalWrite( Buzzer_Pin, LOW);                 // Enciende el Buzzer con LOW
  BuzzerBeepStart = true;                          // Arranca Timer Virtual
}

// ****************************************************************************************
//                        TASK MANAGER LOOP (ADMINISTRADOR DE PROCESOS)
// ****************************************************************************************
void loop()
{
  //Serial.begin(9600);
  //Serial.println  (ButtonSelect);
  //Serial.println  ( ButtonSelect);
  //if ((ButtonSelect = 0x02) && ( ButtonUp == 0x00 )){
    //A[1] = 9;
  //}
  
  BeatPulse();                                     // Base de tiempos
  // Procesos
  if ( BlinkLEDFlag )
    BlinkLED();
  if ( ButtonsFlag )
    Buttons();
  if ( GetButtonFlag )
    GetButton();
    if ( GetButtonFlag1 )
    GetButton1();
  if ( GameRunFlag )
    GameRun();
  if (PausaFlag)
      Pausa();  
  //if (PausaOffFlag)
   //PausaOff();    

  if ( SelectGameFlag )
    SelectGame();
  if ( SelectGameFlag2 )
    SelectGame2(); 
 // if ( SelectGameFlag3 )
   // SelectGame3();   
  if ( BuzzerBeepFlag )
    BuzzerBeep();
  if ( CounterDownAFlag )
    CounterDownA();
  if ( CounterDownBFlag )
    CounterDownB();
  if ( BlinkAPointsFlag )
    BlinkAPoints();
  if ( BlinkBPointsFlag )
    BlinkBPoints();
  if ( TM1637RefreshFlag )
    TM1637Refresh();
  
  delay( 1 );                                      // Tiempo de Latido a 1 mS Aprox
}

// ****************************************************************************************
//                        ADMINISTRADOR DE TEMPORIZACIONES DE TODOS LOS PROCESOS
// ****************************************************************************************
void BeatPulse()
{
  if ( BlinkLEDStart )                             // Verifica si esta habilitado el Timer Virtual
  {
    if ( BlinkLEDCount >= LEDRunPulse )            // Se cumplio la cuenta?
    {
      BlinkLEDCount = 0;                           // Reinicia variable contadora
      BlinkLEDStart = false;                       // Deten Timer Virtual
      BlinkLEDFlag = true;                         // Habilita Proceso
    }
    else
      BlinkLEDCount++;
  }

if ( ButtonDebounceStartSet )
  {
    if ( ButtonDebounceCountSet >= ButtonTimeSet )
    {
      ButtonDebounceCountSet = 0;
      ButtonDebounceStartSet = false;
    }
    else
      ButtonDebounceCountSet++;
  }
if ( ButtonsStartSet )
  {
    if ( ButtonsCountSet >= ButtonHoldSet )
    {
      ButtonsCountSet = 0;
      ButtonsStartSet = false;
      //Realiza un Beep
      digitalWrite( Buzzer_Pin, LOW );             // Enciende el Buzzer con LOW
      BuzzerBeepStart = true;                      // Arranca Timer Virtual
      lcd.setCursor( 0, 1 );                       // Posiciona Cursor
      //Decodifica Boton Presionado
      switch (ButtonSet){
        case 0x01:                                 // Comienza Decrementando B
          lcd.print("   VAMOS!  ->   ");
          BlinkBPointsStart = true;                // Arranca Timer Virtual
          TimeSegBStart = true;                    // Arranca Timer Virtual (contador de segundos)
          GameRunButton = false;                   // Boton A Presionado
          GameRunFlag = true;                      // Activa Proceso de Juego en Curso
          break;
        case 0x02:                                 // Comienza Decrementando A (Boton B Presionado)
          lcd.print("   <-  VAMOS!   ");
          BlinkAPointsStart = true;                // Arranca Timer Virtual
          TimeSegAStart = true;                    // Arranca Timer Virtual (contador de segundos)
          GameRunButton = true;                    // Boton B Presionado
          GameRunFlag = true;                      // Activa Proceso de Juego en Curso
          
          break;
        
      }
     GetButtonFlag1 = false;                       // Deten Proceso de Mantener Boton
    }
      
    
    else
      ButtonsCountSet++;
}
////////////////////////////////////////////////////////////////////////////////////
  
  if ( ButtonDebounceStart )
  {
    if ( ButtonDebounceCount >= ButtonTime )
    {
      ButtonDebounceCount = 0;
      ButtonDebounceStart = false;
    }
    else
      ButtonDebounceCount++;


      
  }
  if ( ButtonsStart )
  {
    if ( ButtonsCount >= ButtonHold )
    {
      ButtonsCount = 0;
      ButtonsStart = false;
      //Realiza un Beep
      digitalWrite( Buzzer_Pin, LOW );             // Enciende el Buzzer con LOW
      BuzzerBeepStart = true;                      // Arranca Timer Virtual
      lcd.setCursor( 0, 1 );                       // Posiciona Cursor
      //Decodifica Boton Presionado
      
      switch ( ButtonPress ){
        case 0x00:                                 // Entra al Menu
                  lcd.print("<ELIJE TU JUEGO>");
                  GetButtonFlag1 = false; 
          //ButtonHold = 150; 
          
                             // Activa Proceso

          if (ButtonSelect != LastStateSelect ) {
            if (ButtonSelect == 0x01){
               SelectGameFlag = true;
            //SelectGameFlag = false;
             
            }
            /*if (ButtonPress == 0x01){
              SelectGameFlag3 = true;
            }*/
             
          }
             
          else 
          {
            SelectGameFlag2 = true;
            }
 
         
          break;
      
      
      }
      GetButtonFlag = false;                       // Deten Proceso de Mantener Boton
    }
    else
      ButtonsCount++;
  }
  if ( ResetGameStart )
  {
    if ( ResetGameCount >= ResetGameTime )
    {
      ResetGameCount = 0;
      ResetGameStart = false;
      if ( ButtonPress == 0 )                      // Se sostuvieron los 2 botones
      {
        //APLICA EL RESET FORZADO
        lcd.setCursor( 0, 1 );                     // Posiciona Cursor
        lcd.print(" <- A JUGAR! -> ");
        TimeSegAStart = false;                     // Deten Timer Virtual (contador de segundos)
        BlinkAPointsStart = false;                 // Deten Parpadeo de Puntos
        ASEG[1] |= 0x80;                           // Coloca Puntos A (MOSTRAR AL ESTAR DETENIDO)
        TimeSegBStart = false;                     // Deten Timer Virtual (contador de segundos)
        BlinkBPointsStart = false;                 // Deten Parpadeo de Puntos
        BSEG[1] |= 0x80;                           // Coloca Puntos B (MOSTRAR AL ESTAR DETENIDO)
        GameRunFlag = false;                       // Desactiva Proceso de Juego en Curso
        GetButtonFlag1 = true; 
        GetButtonFlag = true;                      // Arranca Proceso de Presion de Boton (Inicio/Menu)
        for ( ResetGameCount = 0; ResetGameCount < 4; ResetGameCount++ )
          A[ ResetGameCount ] = B[ ResetGameCount ] = MODESEL[ GameMode ][ ResetGameCount ];
        ResetGameCount = 0;        
        //Realiza un Beep
        digitalWrite( Buzzer_Pin, HIGH );           // Enciende el Buzzer con LOW
        BuzzerBeepStart = true;                    // Arranca Timer Virtual

      }
    }
    else
      ResetGameCount++;
}




  if ( BlinkAPointsStart )
  {
    if ( BlinkAPointsCount >= PointsPulse )
    {
      BlinkAPointsCount = 0;
      BlinkAPointsStart = false;
      BlinkAPointsFlag = true;
    }
    else
      BlinkAPointsCount++;
  }
  if ( BlinkBPointsStart )
  {
    if ( BlinkBPointsCount >= PointsPulse )
    {
      BlinkBPointsCount = 0;
      BlinkBPointsStart = false;
      BlinkBPointsFlag = true;
    }
    else
      BlinkBPointsCount++;
  }
  if ( TM1637RefreshStart )
  {
    if ( TM1637RefreshCount >= TM1637Pulse )
    {
      TM1637RefreshCount = 0;
      TM1637RefreshStart = false;
      TM1637RefreshFlag = true;
    }
    else
      TM1637RefreshCount++;
  }
  if ( BuzzerBeepStart )
  {
    if ( BuzzerBeepCount >= BuzzerBeepTime )
    {
      BuzzerBeepCount = 0;
      BuzzerBeepStart = false;
      BuzzerBeepFlag = true;
    }
    else
      BuzzerBeepCount++;
  }
  if ( TimeSegAStart )
  {
    if ( TimeSegACount >= Segundo )
    {
      TimeSegACount = 0;      
      CounterDownAFlag = true;                     // Activa Decremento A
    }
    else
      TimeSegACount++;
  }
  if ( TimeSegBStart )
  {
    if ( TimeSegBCount >= Segundo )
    {
      TimeSegBCount = 0;      
      CounterDownBFlag = true;                     // Activa Decremento B
    }
    else
      TimeSegBCount++;
  }
  if ( LostTimeStart )
  {
    if ( LostTimeCount >= LostTimeConst )
    {
      LostTimeCount = 0;
      LostTimeStart = false;
      digitalWrite( Buzzer_Pin, LOW );            // Apaga el Buzzer con HIGH
      //REINICIA COMIENZO DE PARTIDA
      TimeSegAStart = false;                       // Deten Timer Virtual (contador de segundos)
      BlinkAPointsStart = false;                   // Deten Parpadeo de Puntos
      ASEG[1] |= 0x80;                             // Coloca Puntos A (MOSTRAR AL ESTAR DETENIDO)
      TimeSegBStart = false;                       // Deten Timer Virtual (contador de segundos)
      BlinkBPointsStart = false;                   // Deten Parpadeo de Puntos
      BSEG[1] |= 0x80;                             // Coloca Puntos B (MOSTRAR AL ESTAR DETENIDO)
      GetButtonFlag = true;                        // Arranca Proceso de Presion de Boton (Inicio/Menu)
      GetButtonFlag1 = true;
      for ( ResetGameCount = 0; ResetGameCount < 4; ResetGameCount++ )
        A[ ResetGameCount ] = B[ ResetGameCount ] = MODESEL[ GameMode ][ ResetGameCount ];
    }
    else
      LostTimeCount++;
  }
}

// ****************************************************************************************
//                        PROCESO DE PARPADEO DE LED DE RUN
// ****************************************************************************************
void BlinkLED()
{
  // Complementa el estado del LED
  if ( digitalRead( LED_Run ) )
    digitalWrite( LED_Run, LOW );
  else
    digitalWrite( LED_Run, HIGH );
  BlinkLEDStart = true;                            // Arranca Timer Virtual
  BlinkLEDFlag = false;                            // Deten Proceso BlinkLED
}

// ****************************************************************************************
//                        PROCESO DE LECTURA DE BOTONES
// ****************************************************************************************
void Buttons()
{
  ButtonSelect = (digitalRead(ButtonS));

 
  //ButtonUp = (digitalRead (Button1));
  //ButtonDo = (digitalRead (Button2));
 
  
  ButtonPress = ( digitalRead( ButtonB ) << 1 ) | digitalRead( ButtonA ); // Lee Botones
  if ( ButtonPress != 0x03 )                       // Esta algun Boton presionado?
  {
    if ( !(ButtonsBefore || ButtonDebounceStart) ) // No hay Boton Previo Ni Esta corriendo Retardo?
    {
      ButtonsBefore = true;                        // Activa Boton Previo Flag
      ButtonsReady = true;                         // Activa Boton presionado listo
    }
  }
  else
  {
    if ( ButtonsBefore )                           // Hay Boton Previo
    {
      ButtonsBefore = false;                       // Desactiva Boton Previo Flag
      ButtonDebounceStart = true;                  // Arranca Retardo Antirebote
    }
  }

   ButtonSet = ( digitalRead( Button2 ) << 1 ) | digitalRead( Button1 ); // Lee Botones
  if ( ButtonSet != 0x03) 
  {
    if ( !(ButtonsBeforeSet || ButtonDebounceStartSet) ) // No hay Boton Previo Ni Esta corriendo Retardo?
    {
    ButtonsBeforeSet = true;
    ButtonsReadySet = true;
  }
}
  else
  {
    if ( ButtonsBeforeSet )                           // Hay Boton Previo
    {
      ButtonsBeforeSet = false;                       // Desactiva Boton Previo Flag
      ButtonDebounceStartSet = true;                  // Arranca Retardo Antirebote
    }
  }
}

// ****************************************************************************************
//                        PROCESO DE BOTON PRESIONADO (INICIO)
// ****************************************************************************************
void GetButton()
{
  if ( ButtonsReady )                              // Se acaban de presionar Botones (ocurrio cambio)
  {
    ButtonsStart = true;                           // Arranca Timer Virtual
    ButtonsReady = false;                          // Desactiva Boton presionado Listo (Se ocupo)
  }
  if ( !ButtonsBefore )                            // Se solto algun Boton?
  {
    ButtonsCount = 0;                              // Reinicia contador
    ButtonsStart = false;                          // Deten Timer Virtual
  }


}

// ****************************************************************************************
//                        PROCESO DE BOTON PRESIONADO (INICIO)
// ****************************************************************************************
void GetButton1()
{
  if ( ButtonsReadySet )                              // Se acaban de presionar Botones (ocurrio cambio)
  {
    ButtonsStartSet = true;                           // Arranca Timer Virtual
    ButtonsReadySet = false;                          // Desactiva Boton presionado Listo (Se ocupo)
  }
  if ( !ButtonsBeforeSet )                            // Se solto algun Boton?
  {
    ButtonsCountSet = 0;                              // Reinicia contador
    ButtonsStartSet = false;                          // Deten Timer Virtual
  }


}


// ****************************************************************************************
//                        PROCESO DE JUEGO PRESENTE
// ****************************************************************************************
void GameRun()
{

  if ( ButtonsReadySet )                              // Se acaban de presionar Botones (ocurrio cambio)
  {

 
      
     
    
    ResetGameStart = true;                         // Arranca Timer Virtual
    lcd.setCursor( 0, 1 );                         // Posiciona Cursor
    if ( ButtonSet == 0x01 && GameRunButton )    // Se Presiono A y Antes B?
    {
      
      GameRunButton = false;                       // Boton A Presionado (0x01)
      lcd.print("   VAMOS!  ->   ");
      //Realiza un Beep
      //digitalWrite( Buzzer_Pin, HIGH );             // Enciende el Buzzer con LOW
      //BuzzerBeepStart = true;                      // Arranca Timer Virtual
      //Incrementa Contador en modo FIDE
      if ( GameMode == 0x09 )
        Incrementa30s( A );
      if ( GameMode == 0x01 )
        Incrementa2s( A );    
      if ( GameMode == 0x03 )
        Incrementa3s( A );  
      if ( GameMode == 0x05 )
        Incrementa5s( A ); 
      if ( GameMode == 0x06 )
        Incrementa10s( A );   
      if ( GameMode == 0x08 )
        Incrementa20s( A );       
      //if ( GameMode == 0x07 )
       // Seleccion( A );  
      //Continua Decremento de B
      BlinkBPointsStart = true;                    // Arranca Timer Virtual
      TimeSegBStart = true;                        // Arranca Timer Virtual (contador de segundos)
      //Pausa Decremento de A
      BlinkAPointsStart = false;                   // Deten Timer Virtual
      ASEG[1] |= 0x80;                             // Coloca Puntos A (MOSTRAR AL ESTAR DETENIDO)
      TimeSegAStart = false;                       // Deten Timer Virtual (contador de segundos)
    }
    if ( ButtonSet == 0x02 && !GameRunButton )   // Se Presiono B y antes A?
    {
      GameRunButton = true;                        // Boton B Presionado (0x02)
      lcd.print("   <-  VAMOS!   ");
      //Realiza un Beep
      //digitalWrite( Buzzer_Pin, HIGH );             // Enciende el Buzzer con LOW
      //BuzzerBeepStart = true;                      // Arranca Timer Virtual
      //Incrementa Contador en modo FIDE
      if ( GameMode == 0x09 )
        Incrementa30s( B );
      if ( GameMode == 0x01 )
        Incrementa2s( B );  
      if ( GameMode == 0x03 )
        Incrementa3s( B );  
      if ( GameMode == 0x05 )
        Incrementa5s( B ); 
      if ( GameMode == 0x06 )
        Incrementa10s( B );   
      if ( GameMode == 0x08 )
        Incrementa20s( B );   
      //if ( GameMode == 0x07 ) 
       // Seleccion ( B );   
      //Continua Decremento de A
      BlinkAPointsStart = true;                    // Arranca Timer Virtual
      TimeSegAStart = true;                        // Arranca Timer Virtual (contador de segundos)
      //Pausa Decremento de B
      BlinkBPointsStart = false;                   // Deten Timer Virtual
      BSEG[1] |= 0x80;                             // Coloca Puntos B (MOSTRAR AL ESTAR DETENIDO)
      TimeSegBStart = false;                       // Deten Timer Virtual (contador de segundos)
    
    } 
    
  }
   
      
    ButtonsReadySet = false;                          // Desactiva Boton presionado Listo (Se ocupo)
   
      if (ButtonSelect != LastStateSelect3 )
{    
   if (ButtonSelect == 0x00){
     contadorPausa++;
     if (contadorPausa >= 3){
      contadorPausa =1;
     }
   }
     if (ButtonSelect == 0x00 && contadorPausa ==1){
     // GameRunButton = true;
      lcd.setCursor( 0, 1 );
    lcd.print ("     PAUSA      ");
       PausaFlag = true;
       //PausaOffFlag = false;
    //GetButtonFlag1 = false;
    
    //contadorPausa =1;
     }

     
  if (ButtonSelect == 0x00 && contadorPausa == 2  )
{
      GameRunFlag = true;
  PausaFlag = false;
  //lcd.setCursor( 0, 1 );
    //lcd.print ("     RATA      ");
  if (!GameRunButton){

      lcd.setCursor( 0, 1 );
      lcd.print("   VAMOS!  ->   ");
          BlinkBPointsStart = true;                // Arranca Timer Virtual
          TimeSegBStart = true;                    // Arranca Timer Virtual (contador de segundos)
          GameRunButton = false;                   // Boton A Presionado
          GameRunFlag = true;                      // Activa Proceso de Juego en Curso
                                         
         
  }
  else{
    if (GameRunButton){
 
      lcd.setCursor( 0, 1 );
    lcd.print("   <-  VAMOS!   ");
          BlinkAPointsStart = true;                // Arranca Timer Virtual
          TimeSegAStart = true;                    // Arranca Timer Virtual (contador de segundos)
          GameRunButton = true;                    // Boton B Presionado
          GameRunFlag = true;                      // Activa Proceso de Juego en Curso
  }
  }
   
    PausaFlag = false;
  }
 
  
  }
  LastStateSelect3 = ButtonSelect;
 //LastStateSelect3 = ButtonSelect ;
 // ButtonsReadySet = false;
  


      
      
      
      
/* if (ButtonSelect == 0x00 && LastStateSelect3 == 0 )
{    
     
     // GameRunButton = true;
      lcd.setCursor( 0, 1 );
    lcd.print ("     PAUSA      ");
       PausaFlag = true;
       PausaOffFlag = false;
   
  
  }
 LastStateSelect3 = ButtonSelect ;
 // ButtonsReadySet = false;
  if (ButtonPress == 0x01   )
{
    lcd.setCursor( 0, 1 );
    lcd.print ("     RATA      ");
    PausaOffFlag = true;
    PausaFlag = false;
  }

*/
  //LastStateSelect = true;
 
    
  
   
  //LastStateSelect = true;
  
  if ( !ButtonsBefore )                            // Se solto algun Boton?
  {
    ResetGameCount = 0;                            // Reinicia contador
    ResetGameStart = true;                        // Deten Timer Virtual
  }
            
}



void Pausa()

{   

    TimeSegBStart = false;
    BlinkBPointsStart = false;                     // Deten Parpadeo de Puntos
    BSEG[1] |= 0x80;           
    TimeSegAStart = false;
    BlinkAPointsStart = false;                     // Deten Parpadeo de Puntos
    ASEG[1] |= 0x80;                               // Coloca Puntos B (MOSTRAR AL ESTAR DETENIDO)
    //ResetGameCount = 0;                            // Reinicia la cuenta del Reset Forzado
    //ResetGameStart = false;                        // Deten Timer Virtual          
    //digitalWrite( Buzzer_Pin, HIGH );               // Enciende el Buzzer con LOW
   // LostTimeStart = true;                          // Arranca Timer Virtual
    //GameRunFlag = false;                           // Desactiva Proceso de Juego en Curso
    
 }

 /*void PausaOff()
 { 

  GameRunFlag = true;
  PausaFlag = false;
  //lcd.setCursor( 0, 1 );
    //lcd.print ("     RATA      ");
  if (!GameRunButton){

      lcd.setCursor( 0, 1 );
      lcd.print("   VAMOS!  ->   ");
          BlinkBPointsStart = true;                // Arranca Timer Virtual
          TimeSegBStart = true;                    // Arranca Timer Virtual (contador de segundos)
          GameRunButton = false;                   // Boton A Presionado
          GameRunFlag = true;                      // Activa Proceso de Juego en Curso
                                         
         
  }
  else{
    if (GameRunButton){
 
      lcd.setCursor( 0, 1 );
    lcd.print("   <-  VAMOS!   ");
          BlinkAPointsStart = true;                // Arranca Timer Virtual
          TimeSegAStart = true;                    // Arranca Timer Virtual (contador de segundos)
          GameRunButton = true;                    // Boton B Presionado
          GameRunFlag = true;                      // Activa Proceso de Juego en Curso
  }
  }
   

  //ButtonsReadySet = true;
 }
*/

// ****************************************************************************************
//                        PROCESO DE SELECCION DE TIEMPOS
// ****************************************************************************************
void SelectGame()
{
  int i;                                           // Variable de apoyo
  if ( ButtonsReady )                              // Se acaban de presionar Botones (ocurrio cambio)
  {
    
    if ( ButtonPress == 0x02 )   // Realiza Opciones? NECESARIO ANTI-REBOTE
    {
      GameMode++;                                  // Incrementa la Seleccion de Modo de Juego
      if ( GameMode > 0x09 )
        GameMode = 0x00;
        
      for ( i=0; i<8; i++ )
        A[ i ] = B[ i ] = MODESEL[ GameMode ][ i ];// Actualiza Variables
      lcd.setCursor( 0, 1 );
      switch ( GameMode ){
        case 0x00:
          lcd.print("      * 3 *      ");
          break;
        case 0x01:
          lcd.print("    * 3 + 2 *    ");
          break;
        case 0x02:
          lcd.print("      * 5 *      ");
          break;
        case 0x03:
          lcd.print("    * 5 + 3 *    ");
          break;
        case 0x04:
          lcd.print("     * 10 *      ");
          break;  
        case 0x05:
          lcd.print("    * 10 + 5 *   ");
          break;  
         case 0x06:
          lcd.print("    * 15 + 10 *  ");
          break;      
        case 0x07:
          lcd.print("     * 30 *      ");
          break;   
        case 0x08:
          lcd.print("    * 30 + 20 *  ");
          break;    
        case 0x09:
          lcd.print("    * 90 + 30 *  ");
        
          
          
      }
      //Realiza un Beep
      digitalWrite( Buzzer_Pin, HIGH );             // Enciende el Buzzer con LOW
      BuzzerBeepStart = true;                      // Arranca Timer Virtual
    }

    
    if ( ButtonPress == 0x01 )   // Realiza Opciones? NECESARIO ANTI-REBOTE
    {
      GameMode--;                                  // Incrementa la Seleccion de Modo de Juego
      if ( GameMode < 0x00 )
        GameMode = 0x09;
        
      for ( i=0; i<8; i++ )
        A[ i ] = B[ i ] = MODESEL[ GameMode ][ i ];// Actualiza Variables
      lcd.setCursor( 0, 1 );
      switch ( GameMode ){
        case 0x00:
          lcd.print("      * 3 *      ");
          break;
        case 0x01:
          lcd.print("    * 3 + 2 *    ");
          break;
        case 0x02:
          lcd.print("      * 5 *      ");
          break;
        case 0x03:
          lcd.print("    * 5 + 3 *    ");
          break;
        case 0x04:
          lcd.print("     * 10 *      ");
          break;  
        case 0x05:
          lcd.print("    * 10 + 5 *   ");
          break;  
         case 0x06:
          lcd.print("    * 15 + 10 *  ");
          break;      
        case 0x07:
          lcd.print("     * 30 *      ");
          break;   
        case 0x08:
          lcd.print("    * 30 + 20 *  ");
          break;    
        case 0x09:
          lcd.print("    * 90 + 30 *  ");
        
          
          
      }
      //Realiza un Beep
      digitalWrite( Buzzer_Pin, HIGH );             // Enciende el Buzzer con LOW
      BuzzerBeepStart = true;                      // Arranca Timer Virtual
    }  
  
    ButtonsReady = false;                          // Desactiva Boton presionado Listo (Se ocupo)
  }  
    if ( ButtonSelect == 0x00 && LastStateSelect2 )                     // Realiza Seleccion?
    {
      
      lcd.setCursor( 0, 1 );
      lcd.print(" <- A JUGAR! -> ");
      //Realiza un Beep
      //digitalWrite( Buzzer_Pin, HIGH );             // Enciende el Buzzer con LOW
      //BuzzerBeepStart = true;                      // Arranca Timer Virtual
      SelectGameFlag = false;                      // Desactiva Proceso
      GetButtonFlag = true;                        // Arranca Proceso de Presion de Boton (Inicio/Menu)
     GetButtonFlag1 = true; 
    }
  LastStateSelect2 = ButtonSelect;
}


void SelectGame2()
{
  //LastStateSelect = false;
  int i;                                           // Variable de apoyo
  if ( ButtonsReady )                              // Se acaban de presionar Botones (ocurrio cambio)
  {

          
    
    if ( ButtonPress == 0x02 )   // Realiza Opciones? NECESARIO ANTI-REBOTE
    {
      GameMode2++;                                  // Incrementa la Seleccion de Modo de Juego
      if ( GameMode2 > 17 )
        GameMode2 = 0x00;
      for ( i=0; i<8; i++ )
        A[ i ]  = MODESELMinA[ GameMode2 ][ i ];// Actualiza Variables
      lcd.setCursor( 0, 1 );
      
      //Realiza un Beep
      digitalWrite( Buzzer_Pin, HIGH );             // Enciende el Buzzer con LOW
      BuzzerBeepStart = true;                      // Arranca Timer Virtual
    }

   if ( ButtonPress == 0x01 )   // Realiza Opciones? NECESARIO ANTI-REBOTE
    {
      GameMode3++;                                  // Incrementa la Seleccion de Modo de Juego
      if ( GameMode3 > 17 )
        GameMode3 = 0x00;
      for ( i=0; i<8; i++ )
        B[ i ]  = MODESELMinA[ GameMode3 ][ i ];// Actualiza Variables
      lcd.setCursor( 0, 1 );
      
      //Realiza un Beep
      digitalWrite( Buzzer_Pin, HIGH );             // Enciende el Buzzer con LOW
      BuzzerBeepStart = true;                      // Arranca Timer Virtual
    }

 
    
    
    ButtonsReady = false;                          // Desactiva Boton presionado Listo (Se ocupo)
  }  
  if ( ButtonSelect == 0x00 && LastStateSelect2 )                     // Realiza Seleccion?
    {
      
      lcd.setCursor( 0, 1 );
      lcd.print(" <- A JUGAR! -> ");
      //Realiza un Beep
      //digitalWrite( Buzzer_Pin, HIGH );             // Enciende el Buzzer con LOW
      //BuzzerBeepStart = true;                      // Arranca Timer Virtual
      SelectGameFlag = false;                      // Desactiva Proceso
      GetButtonFlag = true;                        // Arranca Proceso de Presion de Boton (Inicio/Menu)
      GetButtonFlag1 = true;
    }
  LastStateSelect2 = ButtonSelect;
}

/*void SelectGame3()
{
  int i;                                           // Variable de apoyo
  if ( ButtonsReady )                              // Se acaban de presionar Botones (ocurrio cambio)
  {

          
    
    if ( ButtonPress == 0x02 )   // Realiza Opciones? NECESARIO ANTI-REBOTE
    {
      GameMode3++;                                  // Incrementa la Seleccion de Modo de Juego
      if ( GameMode3 > 0x08 )
        GameMode2 = 0x00;
      for ( i=0; i<8; i++ )
        A[ i ]  = MODESELMinA[ GameMode3 ][ i ];// Actualiza Variables
      lcd.setCursor( 0, 1 );
      switch ( GameMode3 ){
        case 0x00:
          lcd.print("      * 3 *      ");
          break;


     
      }
      //Realiza un Beep
      digitalWrite( Buzzer_Pin, HIGH );             // Enciende el Buzzer con LOW
      BuzzerBeepStart = true;                      // Arranca Timer Virtual
    }
    if ( ButtonSelect == 0x01 )                     // Realiza Seleccion?
    {
      GetButtonFlag = true;                        // Arranca Proceso de Presion de Boton (Inicio/Menu)
      lcd.setCursor( 0, 1 );
      lcd.print(" <- A JUGAR! -> ");
      //Realiza un Beep
      digitalWrite( Buzzer_Pin, HIGH );             // Enciende el Buzzer con LOW
      BuzzerBeepStart = true;                      // Arranca Timer Virtual
      SelectGameFlag = false;                      // Desactiva Proceso
    }
    ButtonsReady = false;                          // Desactiva Boton presionado Listo (Se ocupo)
  }  
  //LastStateSelect == ButtonSelect;
}
*/

// ****************************************************************************************
//                        PROCESO DE APAGADO DE BUZZER
// ****************************************************************************************
void BuzzerBeep()
{
  digitalWrite( Buzzer_Pin, LOW );                // Apaga el Buzzer con HIGH
  BuzzerBeepFlag = false;
}

// ****************************************************************************************
//                        PROCESO DE CUENTA REGRESIVA A
// ****************************************************************************************
void CounterDownA()
{
  if ( CountDown( A ) )
  {
    //Se agoto el tiempo
    lcd.setCursor( 0, 1 );
    lcd.print("  GANADOR! ->   ");
    TimeSegAStart = false;                         // Deten Timer Virtual (contador de segundos)
    BlinkAPointsStart = false;                     // Deten Parpadeo de Puntos
    ASEG[1] |= 0x80;                               // Coloca Puntos A (MOSTRAR AL ESTAR DETENIDO)
    ResetGameCount = 0;                            // Reinicia la cuenta del Reset Forzado
    ResetGameStart = false;                        // Deten Timer Virtual
    digitalWrite( Buzzer_Pin, HIGH );               // Enciende el Buzzer con LOW
    LostTimeStart = true;                          // Arranca Timer Virtual
    GameRunFlag = false;                           // Desactiva Proceso de Juego en Curso
  }
  CounterDownAFlag = false;                        // Deten Proceso CounterDownA
}

// ****************************************************************************************
//                        PROCESO DE CUENTA REGRESIVA B
// ****************************************************************************************
void CounterDownB()
{
  if ( CountDown( B ) )
  {
    //Se agoto el tiempo
    lcd.setCursor( 0, 1 );
    lcd.print("  <- GANADOR!   ");
    TimeSegBStart = false;                         // Deten Timer Virtual (contador de segundos)
    BlinkBPointsStart = false;                     // Deten Parpadeo de Puntos
    BSEG[1] |= 0x80;                               // Coloca Puntos B (MOSTRAR AL ESTAR DETENIDO)
    ResetGameCount = 0;                            // Reinicia la cuenta del Reset Forzado
    ResetGameStart = false;                        // Deten Timer Virtual          
    digitalWrite( Buzzer_Pin, HIGH );               // Enciende el Buzzer con LOW
    LostTimeStart = true;                          // Arranca Timer Virtual
    GameRunFlag = false;                           // Desactiva Proceso de Juego en Curso
  }
  CounterDownBFlag = false;                        // Deten Proceso CounterDownB
}

// ****************************************************************************************
//                        PROCESO DE PARPADEO DE PUNTOS A
// ****************************************************************************************
void BlinkAPoints()
{
  // Complementa el estado de los Puntos (SIEMPRE DESPUES DE ACTUALIZAR VALORES)
  if ( ASEG[1] & 0x80 )                            // Estan los puntos?
    ASEG[1] &= 0x7f;                               // Quita Puntos
  else
    ASEG[1] |= 0x80;                               // Coloca Puntos 
  BlinkAPointsStart = true;                        // Arranca Timer Virtual
  BlinkAPointsFlag = false;                        // Deten Proceso BlinkAPoints
}


/*void  Pausa()
{

      //GameRunFlag = false;
      //GameRunButton = true;
  if (PausaOn = true ){    
      lcd.setCursor( 0, 1 );
    lcd.print ("       PAUSA     ");
    TimeSegBStart = false;                         // Deten Timer Virtual (contador de segundos)
    TimeSegAStart = false;
    BlinkBPointsStart = false;                     // Deten Parpadeo de Puntos
    BSEG[1] |= 0x80;                               // Coloca Puntos B (MOSTRAR AL ESTAR DETENIDO)
    //ResetGameCount = 0;                            // Reinicia la cuenta del Reset Forzado
    //ResetGameStart = false;                        // Deten Timer Virtual          
    //digitalWrite( Buzzer_Pin, HIGH );               // Enciende el Buzzer con LOW
   // LostTimeStart = true;                          // Arranca Timer Virtual
    GameRunFlag = true;                           // Desactiva Proceso de Juego en Curso
  }
  PausaFlag = false;
}
*/


// ****************************************************************************************
//                        PROCESO DE PARPADEO DE PUNTOS A
// ****************************************************************************************
void BlinkBPoints()
{
  // Complementa el estado de los Puntos (SIEMPRE DESPUES DE ACTUALIZAR VALORES)
  if ( BSEG[1] & 0x80 )                            // Estan los puntos?
    BSEG[1] &= 0x7f;                               // Quita Puntos
  else
    BSEG[1] |= 0x80;                               // Coloca Puntos 
  BlinkBPointsStart = true;                        // Arranca Timer Virtual
  BlinkBPointsFlag = false;                        // Deten Proceso BlinkAPoints
}

// ****************************************************************************************
//                        PROCESO DE ACTUALIZACION DE DIGITOS EN LOS TM1637
// ****************************************************************************************
void TM1637Refresh()
{
  // ACTUALIZA VALORES (SE PIERDEN PUNTOS)
  ASEG[ 0 ] = TM1637A.encodeDigit( A[ 0 ] );
  if ( ASEG[1] & 0x80 )                            // Estan los puntos en A?
    ASEG[ 1 ] = TM1637A.encodeDigit( A[ 1 ] ) | 0x80;
  else
    ASEG[ 1 ] = TM1637A.encodeDigit( A[ 1 ] );
  ASEG[ 2 ] = TM1637A.encodeDigit( A[ 2 ] );
  ASEG[ 3 ] = TM1637A.encodeDigit( A[ 3 ] );

  BSEG[ 0 ] = TM1637B.encodeDigit( B[ 0 ] );
  if ( BSEG[1] & 0x80 )                            // Estan los puntos en B?
    BSEG[ 1 ] = TM1637B.encodeDigit( B[ 1 ] ) | 0x80;
  else
    BSEG[ 1 ] = TM1637B.encodeDigit( B[ 1 ] );
  BSEG[ 2 ] = TM1637B.encodeDigit( B[ 2 ] );
  BSEG[ 3 ] = TM1637B.encodeDigit( B[ 3 ] );
  
  // Refresca Displays TM1637
  TM1637A.setSegments( ASEG, 4, 0 );               // Refresca Display A
  TM1637B.setSegments( BSEG, 4, 0 );               // Refresca Display B
  TM1637RefreshStart = true;                       // Arranca Timer Virtual
  TM1637RefreshFlag = false;                       // Deten Proceso TM1637Refresh
}

// ****************************************************************************************
//                        FUNCION CUENTA REGRESIVA
// ****************************************************************************************
bool CountDown(uint8_t *dir)
{
  if ( dir[ 3 ] < '1' )                            // Las unidades son menores que 1
    if ( dir[ 2 ] < '1' )                          // Las decenas son menores que 1
      if ( dir[ 1 ] < '1' )                        // Las unidades son menores que 1
        if ( dir[ 0 ] < '1' )                      // Las decenas son menores que 1
          return true;                             // Se agoto el tiempo, retornamos true
        else
        {
          dir[ 0 ]--;                              // Decrementa Decenas
          dir[ 1 ] = '9';
          dir[ 2 ] = '5';
          dir[ 3 ] = '9';
        }
      else
      {
        dir[ 1 ]--;                                // Decrementa Unidades
        dir[ 2 ] = '5';
        dir[ 3 ] = '9';
      }
    else
    {
      dir[ 2 ]--;                                  // Decrementa Decenas
      dir[ 3 ] = '9';
    }
  else
    dir[ 3 ]--;                                    // Decrementa Unidades
  return false;                                    // Todavia hay tiempo Regresa Falso
}

// ****************************************************************************************
//                        FUNCION QUE INCREMENTA 30 SEGUNDOS
// ****************************************************************************************
void Incrementa30s(uint8_t *dir)                   // Recibe la direccion del areglo que contiene el tiempo del jugador
{
  dir[ 2 ] = dir[ 2 ] + 0x03;                      // Agregamos 3 decenas de segundo        
  if ( dir[ 2 ] > '5' )
  {
    dir[ 1 ]++;                                    // Incrementa Unidades de minuto
    dir[ 2 ] = dir[ 2 ] - 0x06;                    // Resta la cantidad que se paso
    if ( dir[ 1 ] > '9' )
    {
      dir[ 0 ]++;                                  // Incrementa Decenas de minuto
      dir[ 1 ] = '0';                              // Coloca a "0" Unidades de minuto
      if ( dir[ 0 ] > '9' )
        dir[ 0 ] = '9';                            // Regresa a "9" las decenas de minuto
    }
  }
  if( dir[ 0 ] > '8' )
    dir[ 3 ] = dir[ 2 ] = dir[ 1 ] = '0';          // Regresa variables a cero
}

// ****************************************************************************************
//                        FUNCION QUE INCREMENTA 2 SEGUNDOS
// ****************************************************************************************

void Incrementa2s(uint8_t *dir)                   // Recibe la direccion del areglo que contiene el tiempo del jugador
{
  dir[ 3 ] = dir[ 3 ] + 0x02;                      // Agregamos 3 decenas de segundo        
  if ( dir[ 3 ] > '9' )
  {
    dir[ 2 ]++;                                    // Incrementa Unidades de minuto
    dir[ 3 ] = dir[ 3 ] - 0x08 - 0x02;                    // Resta la cantidad que se paso
    if ( dir[ 2 ] > '5' )
    {
      dir[ 1 ]++;                                  // Incrementa Decenas de minuto
      dir[ 2 ] = '0';                              // Coloca a "0" Unidades de minuto
      if ( dir[ 1 ] > '9' )
        dir[ 0 ] = '1';                            // Regresa a "9" las decenas de minuto
    }
  }
  if( dir[ 1 ] > '8' )
    dir[ 4 ] = dir[ 3 ] = dir[ 2 ] = '0';          // Regresa variables a cero
}

void Incrementa3s(uint8_t *dir)                   // Recibe la direccion del areglo que contiene el tiempo del jugador
{
  dir[ 3 ] = dir[ 3 ] + 0x03;                      // Agregamos 3 decenas de segundo        
  if ( dir[ 3 ] > '9' )
  {
    dir[ 2 ]++;                                    // Incrementa Unidades de minuto
    dir[ 3 ] = dir[ 3 ] - 0x09 -0x01 ;                    // Resta la cantidad que se paso
    if ( dir[ 2 ] > '5' )
    {
      dir[ 1 ]++;                                  // Incrementa Decenas de minuto
      dir[ 2 ] = '0';                              // Coloca a "0" Unidades de minuto
      if ( dir[ 1 ] > '9' )
        dir[ 0 ] = '1';                            // Regresa a "9" las decenas de minuto
    }
  }
  if( dir[ 1 ] > '8' )
    dir[ 4 ] = dir[ 3 ] = dir[ 2 ] = '0';          // Regresa variables a cero
}

void Incrementa5s(uint8_t *dir)                   // Recibe la direccion del areglo que contiene el tiempo del jugador
{
  dir[ 3 ] = dir[ 3 ] + 0x05;                      // Agregamos 3 decenas de segundo        
  if ( dir[ 3 ] > '9' )
  {
    dir[ 2 ]++;                                    // Incrementa Unidades de minuto
    dir[ 3 ] = dir[ 3 ] - 0x09 -0x01;                    // Resta la cantidad que se paso
    if ( dir[ 2 ] > '5' )
    {
      dir[ 1 ]++;                                  // Incrementa Decenas de minuto
      dir[ 2 ] = '0';                              // Coloca a "0" Unidades de minuto
      if ( dir[ 1 ] > '9' )
      {
        dir[ 0 ] ++;                            // Regresa a "9" las decenas de minuto
        dir [1] = '0';
      }
    }
  }
  if( dir[ 1 ] > '9' )
    //dir[0]++;
    dir[ 3 ] = dir[ 2 ] =  '0';          // Regresa variables a cero
}

void Incrementa10s(uint8_t *dir)                   // Recibe la direccion del areglo que contiene el tiempo del jugador
{
    dir[ 2 ] = dir[ 2 ] + 0x01;                      // Agregamos 3 decenas de segundo        
  if ( dir[ 2 ] > '5' )
  {
    dir[ 1 ]++;                                    // Incrementa Unidades de minuto
    dir[ 2 ] = dir[ 2 ] - 0x06;                    // Resta la cantidad que se paso
    if ( dir[ 1 ] > '9' )
    {
      dir[ 0 ]++;                                  // Incrementa Decenas de minuto
      dir[ 1 ] = '0';                              // Coloca a "0" Unidades de minuto
      if ( dir[ 0 ] > '9' )
        dir[ 0 ] = '9';                            // Regresa a "9" las decenas de minuto
    }
  }
  if( dir[ 0 ] > '9' )
    dir[ 3 ] = dir[ 2 ] = dir[ 1 ] = '0';          // Regresa variables a cero
}

void Incrementa20s(uint8_t *dir)                   // Recibe la direccion del areglo que contiene el tiempo del jugador
{
   dir[ 2 ] = dir[ 2 ] + 0x02;                      // Agregamos 3 decenas de segundo        
  if ( dir[ 2 ] > '5' )
  {
    dir[ 1 ]++;                                    // Incrementa Unidades de minuto
    dir[ 2 ] = dir[ 2 ] - 0x06;                    // Resta la cantidad que se paso
    if ( dir[ 1 ] > '9' )
    {
      dir[ 0 ]++;                                  // Incrementa Decenas de minuto
      dir[ 1 ] = '0';                              // Coloca a "0" Unidades de minuto
      if ( dir[ 0 ] > '9' )
        dir[ 0 ] = '9';                            // Regresa a "9" las decenas de minuto
    }
  }
  if( dir[ 0 ] > '9' )
    dir[ 3 ] = dir[ 2 ] = dir[ 1 ] = '0';          // Regresa variables a cero
}

/*bool Seleccion(uint8_t *dir)                   // Recibe la direccion del areglo que contiene el tiempo del jugador
{
  if (ButtonsReady) {
  CounterDownBFlag = false; 
    
  dir[ 3 ] = dir[ 3 ] + 0x02;                      // Agregamos 3 decenas de segundo        
  if ( dir [ 3 ] > '8') {
    dir [ 2 ]++;
  }
  
  if ( dir[ 2 ] > '5' )
  {
    dir[ 1 ]++;                                    // Incrementa Unidades de minuto
    dir[ 2 ] = '0';                    // Resta la cantidad que se paso
    if ( dir[ 1 ] > '9' )
    {
      dir[ 0 ]++;                                  // Incrementa Decenas de minuto
      dir[ 1 ] = '0';                              // Coloca a "0" Unidades de minuto
      if ( dir[ 0 ] > '9' )
        dir[ 0 ] = '9';                            // Regresa a "9" las decenas de minuto  
    }
  }
  if( dir[ 0 ] > '8' )
    dir[ 3 ] = dir[ 2 ] = dir[ 1 ] = '0';          // Regresa variables a cero
  }
}*/


// ****************************************************************************************
//                        FIN DE PROGRAMA
// ****************************************************************************************
