#include <math.h>
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

#define PASSWORD 1234
#define ON 1
#define OFF 0
#define DIA 1
#define NOCHE 0
#define PRIMAVERA 1
#define VERANO 2
#define OTONIO 3
#define INVIERNO 4
#define UP 1
#define DOWN 2

const int pinBoton1 = 19;
const int pinBoton2 = 18;
const int pinBoton3 = 2 ;
const int pinBoton4 = 3 ;
const int pinTemperatura = 8;
const int pinFotosensible = 9;
const int pinMovimiento = 38;
const int pinZumbador = 0;
const int pinVentilador = 0;
const int pinCalefaccion = 0;
const int pinLuces = 0;
const int pinPersiana = 0;

int numPant = 1;
volatile unsigned long antiRebote = 0;

unsigned long TIEMPO = 0;
unsigned long contadorUSB = 1;
unsigned long timerTEMP = 0;
unsigned long timerUSB = 0;
unsigned long timerPANTALLA = 0;

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7); // Creamos un LCD en la direccion 0x27


class DomoSI{
  
  private:
    double Temperatura;
    int Consigna;
    int Movimiento;
    int Fotosensible;
    int Estacion;
    int Hora;
    int Minutos;
    int Segundos;
    int Alarma;
    int Zumbador;
    int Ventilador;
    int Calefaccion;
    int Luces;
    int Persianas;
    
  public:
    DomoSI(boolean nulo){
      /*****************************/
      /*  CONSTRUCTOR DE LA CLASE  */
      /*****************************/
      Movimiento = OFF;
      Temperatura = 0;
      Consigna = 22;
      Fotosensible = OFF;
      Estacion = OFF;
      Hora = 0;
      Minutos = 0;
      Segundos = 0;
      Alarma = OFF;
      Ventilador = OFF;
      Calefaccion = OFF;
      Luces = OFF;
      Persianas = OFF;
    }
      /****************/
      /*  GET'S/SET'S */
      /****************/
    int getMovimiento(){
      return Movimiento;
    }
    void actMovimiento(){
      if(digitalRead(pinMovimiento) == HIGH)
        Movimiento = ON;
      else
        Movimiento = OFF;
    }
    
    double getTemperatura(){
      return Temperatura;
    }
    void actTemperatura(){
      int lectura = analogRead(pinTemperatura);
      long Resistance;
      double Temp;
      Resistance =((10240000/lectura) - 10000);
      Temp = log(Resistance);
      Temp = 1 / (0.001129148 + (0.000234125 * Temp) + (0.0000000876741 * Temp * Temp * Temp));
      Temp = Temp - 273.15;
      Temperatura = Temp;
    }

    int getConsigna(){
      return Consigna;
    }
    void setConsigna(int con){
      if(15 <= con && con <= 30)
        Consigna = con;
    }
    
    int getFotosensible(){
      return Fotosensible;
    }
    void actFotosensible(){
      int lectura = analogRead(pinFotosensible);
      if(lectura > 200) Fotosensible = DIA;
      else Fotosensible = NOCHE;
    }

    int getEstacion(){
      return Estacion;
    }
    void setEstacion(int est){
      if(PRIMAVERA <= est && est <= INVIERNO)
        Estacion = est;
    }

    int getHora(){
      return Hora;
    }
    int getMinutos(){
      return Minutos;
    }
    void setHora(int h, int m){
      if(1 <= h && h <= 24)
        if(0 <= m && m <= 59){
          Hora = h;
          Minutos = m;
        }
    }
    void actHora(){
      if(Hora!=0){
        Segundos ++;
        if(Segundos > 59){
          Minutos++;
          Segundos = 0;
        }
        if(Minutos > 59){
          Hora++;
          Minutos = 0;
        }
        if(Hora > 24)
          Hora = 1;
      }
    }
    
    int getAlarma(){
      return Alarma;
    }
    void setAlarma(int al){
      if(al == ON || al == OFF)
        Alarma = al;
    }

    int getZumbador(){
      return Zumbador;
    }
    void setZumbador(int zum){
      if(zum == ON || zum == OFF)
        Zumbador = zum;
    }

    int getVentilador(){
      return Ventilador;
    }
    void setVentilador(int vent){
      if(vent == ON || vent == OFF)
        Ventilador = vent;
    }

    int getCalefaccion(){
      return Calefaccion;
    }
    void setCalefaccion(int cal){
      if(cal == ON || cal == OFF)
        Calefaccion = cal;
    }

    int getLuces(){
      return Luces;
    }
    void setLuces(int luz){
      if(luz == ON || luz == OFF)
        Luces = luz;
    }
    int getPersianas(){
      return Persianas;
    }
    void setPersianas(int per){
      if(per == UP || per == OFF || per == DOWN)
        Persianas = per;
    }
    
    String entrada = "";
    
    void Bluetooth(){
      /***************************************************************************/
      /*  Comunicacion por Bluetooth con la App Android para manejar el sistema  */
      /***************************************************************************/
      if(Serial2.available() > 0){
        entrada.concat(Serial2.read()); //Leer un caracter
        
        Serial.println("Entrada: "+String(entrada));

        if(entrada.endsWith("4572")){
          //Si es "hh:mm-H", establecer la hora
          String H = entrada.substring(0, entrada.indexOf("58"));
          int hora = H.substring(0,2).toInt() - 48;
          if(H.length() > 2){
            hora*=10;
            hora+= (H.substring(2,4).toInt()-48);
          }
          String M = entrada.substring(entrada.indexOf("58")+2,entrada.indexOf("45"));
          int mins = M.substring(0,2).toInt() - 48;
          if(M.length() > 2){
            mins*=10;
            mins+= (M.substring(2,4).toInt()-48);
          }
          setHora(hora, mins);
          entrada = "";
        }
        if(entrada.endsWith("4568")){
          //Si es "dd/mm-D", establecer mes y por tanto estacion del año
          String D = entrada.substring(0, entrada.indexOf("47"));
          int dia = D.substring(0,2).toInt() - 48;
          if(D.length() > 2){
            dia*=10;
            dia+= (D.substring(2,4).toInt()-48);
          }
          String M = entrada.substring(entrada.indexOf("47")+2, entrada.indexOf("45"));
          int mes = M.substring(0,2).toInt() - 48;
          if(M.length() > 2){
            mes *= 10;
            mes += (M.substring(2,4).toInt()-48);
          }
          Serial.print("fecha:"); Serial.print(dia); Serial.print("/"); Serial.println(mes);
          
          if(12 <= mes && mes <= 2){
            Serial.println("ENTRA1");
            if(mes == 12 && 1 <= dia && dia <=20){
              Serial.println("ENTRA2");
              setEstacion(OTONIO);
            }
            else{
              Serial.println("ENTRA3");
              setEstacion(INVIERNO);
            }
              
          }else
          if(3 <= mes && mes <= 5){
            if(mes == 3 && 1 <= dia && dia <=20)
                  setEstacion(INVIERNO);
            else  setEstacion(PRIMAVERA);
              
          }else
          if(6 <= mes && mes <= 8){
            if(mes == 6 && 1 <= dia && dia <=20)
                  setEstacion(PRIMAVERA);
            else  setEstacion(VERANO);
              
          }else
          if(9 <= mes && mes <= 11){
            if(mes == 9 && 1 <= dia && dia <=20)
                  setEstacion(VERANO);
            else  setEstacion(OTONIO);
              
          }
          else Serial.println("NO ENTRA");
          
          entrada = "";
        }
        if(entrada.endsWith("7978")){
          //Si es "ON-$$$$", encender la alarma
          setAlarma(ON);
          entrada = "";
        } 
        if(entrada.endsWith("797070")){
          //Si es "OFF-$$$$", apagar la alarma 
          setAlarma(OFF);
          entrada = "";
        }
        if(entrada.endsWith("6980")){
          //Si es "EP", estacion es PRIMAVERA
          setEstacion(PRIMAVERA);
          entrada = "";
        }
        if(entrada.endsWith("6986")){
          //Si es "EV", estacion es VERANO
          setEstacion(VERANO);
          entrada = "";
        }
        if(entrada.endsWith("6979")){
          //Si es "EO", estacion es OTOÑO
          setEstacion(OTONIO);
          entrada = "";
        }
        if(entrada.endsWith("6973")){
          //Si es "EI", estacion es INVIERNO
          setEstacion(INVIERNO);
          entrada = "";
        }
        if(entrada.endsWith("6743")){
          //Si es "C+", aumentar Consigna
          setConsigna(getConsigna()+1);
          entrada = "";
        }
        if(entrada.endsWith("6745")){
          //Si es "C-", aumentar Consigna
          setConsigna(getConsigna()-1);
          entrada = "";
        }
      }
    }
    void Eleccion(){
    /*************************************************************************************************************************/
    /*  Cambia los valores de las variables locales, según los datos tomados por sondas, acciones web, y situacion anterior  */
    /*************************************************************************************************************************/
      if(getAlarma() == ON && getMovimiento() == ON)
        setZumbador(ON);
      else setZumbador(OFF);


      if(getEstacion() == INVIERNO && getConsigna() > getTemperatura())
        setCalefaccion(ON);
      else if(getEstacion() == VERANO && getConsigna() < getTemperatura())
        setVentilador(ON);
      else{
        setCalefaccion(OFF);
        setVentilador(OFF);
      }


      if(getEstacion() == INVIERNO)
        if(8 < getHora() && getHora() < 18)
          if(getFotosensible() == DIA)
            setPersianas(UP);
          else
            setPersianas(DOWN);
        else
          setPersianas(DOWN);
      else if(getEstacion() == VERANO)
        if(8 < getHora() && getHora() < 21)
          if(getFotosensible() == DIA)
            setPersianas(UP);
          else
            setPersianas(DOWN);
        else
          setPersianas(DOWN);
    }
    void ejecutarCambios(){
    /************************************************************************************/
    /*  Cambia los valores de las salidas digitales segun los valores de las variables  */
    /************************************************************************************/
      
      /*
       * CAMBIO VALORES PINES
       */
    }
    void INFO_USB(){
    /********************************************************/
    /*  Muestra la informacion del Fancoil a traves de USB  */
    /********************************************************/
      if(contadorUSB > 50) contadorUSB = 1;
      if(contadorUSB < 10)  Serial.print("[0");  else Serial.print("[");  Serial.print(String(contadorUSB)+"]");
      
      //CONFIGURACIONES
      Serial.print("  CONFIGURACIONES:  ");
      Serial.print("  Consigna: "+String(getConsigna()));
      Serial.print("  Estacion: ");
        if(getEstacion() == PRIMAVERA)  Serial.print("PRIMAVERA"); else
        if(getEstacion() == VERANO)     Serial.print("VERANO   "); else
        if(getEstacion() == OTONIO)     Serial.print("OTONIO   "); else
        if(getEstacion() == INVIERNO)   Serial.print("INVIERNO ");
      Serial.print("  Hora: "+String(getHora())+":"+String(getMinutos())+":"+String(Segundos));
      Serial.print("  Alarma: "); 
        if(getAlarma() == ON)           Serial.print("ON "); else
        if(getAlarma() == OFF)          Serial.print("OFF");
      Serial.println();

      //ENTRADAS
      Serial.print("      ENTRADAS:\t\t");
      Serial.print("  Temperatura: "+String(getTemperatura()));
      Serial.print("  S.Mov: ");
        if(getMovimiento() == ON)       Serial.print(" DETECT  "); else
        if(getMovimiento() == OFF)      Serial.print("NO_DETECT");
      Serial.print("  S.Foto: ");
        if(getFotosensible() == DIA)    Serial.print("DIA  "); else
        if(getFotosensible() == NOCHE)  Serial.print("NOCHE");
      Serial.println();
      
      //SALIDAS
      Serial.print("      SALIDAS:\t\t");
      Serial.print("  Zumbador: ");
        if(getZumbador() == ON)         Serial.print("ON "); else
        if(getMovimiento() == OFF)      Serial.print("OFF");
      Serial.print("  Ventilador: ");
        if(getVentilador() == ON)       Serial.print("ON "); else
        if(getVentilador() == OFF)      Serial.print("OFF");
      Serial.print("  Calefaccion: ");
        if(getCalefaccion() == ON)      Serial.print("ON "); else
        if(getCalefaccion() == OFF)     Serial.print("OFF");
      Serial.print("  Luces: ");
        if(getLuces() == ON)            Serial.print("ON "); else
        if(getLuces() == OFF)           Serial.print("OFF");
      Serial.print("  Persianas: ");
        if(getPersianas() == OFF)       Serial.print("OFF "); else
        if(getPersianas() == UP)        Serial.print(" UP "); else
        if(getPersianas() == DOWN)      Serial.print("DOWN");
      Serial.println();
      Serial.println();
      contadorUSB++;
    }
    void INFO_PANTALLA(){
    /*********************************************************************/
    /*  Muestra la informacion del Fancoil a traves de Pantalla por I2C  */
    /*********************************************************************/
      lcd.home();
      switch(numPant){
        case 1:
          /*
           * ALARMA + SENSOR DE MOVIMIENTO = ZUMBADOR
           */
          if(getAlarma() == ON)           lcd.print("Alarma  CONECTADA   "); else
          if(getAlarma() == OFF)          lcd.print("Alarma DESCONECTADA ");
          lcd.setCursor(0,1);
          if(getMovimiento() == ON)       lcd.print("S.Mov:  DETECTADO   "); else
          if(getMovimiento() == OFF)      lcd.print("S.Mov: NO DETECTADO ");
          lcd.setCursor(0,2);
          if(getZumbador() == ON)         lcd.print("Zumbador: ON        "); else
          if(getZumbador() == OFF)        lcd.print("Zumbador: OFF       ");
          lcd.setCursor(0,3);
          lcd.print("                    ");
          break;
          
        case 2:
          /*
           * ESTACION + CONSIGNA + TEMPERATURA = VENTILADOR + CALEFACCION
           */
          lcd.print("Estacion: ");
          if(getEstacion() == INVIERNO)   lcd.print("INVIERNO  "); else
          if(getEstacion() == PRIMAVERA)  lcd.print("PRIMAVERA "); else
          if(getEstacion() == VERANO)     lcd.print(" VERANO   "); else
          if(getEstacion() == OTONIO)     lcd.print(" OTONIO   "); else
          if(getEstacion() == OFF)        lcd.print("NOT_DEFINE");
          lcd.setCursor(0,1);
          lcd.print("Temp.Consigna: "+String(getConsigna())+" C ");
          lcd.setCursor(0,2);
          lcd.print("Temperatura: "+String(getTemperatura())+" C");
          lcd.setCursor(0,3);
          if(getCalefaccion() == ON)      lcd.print("Ca: ON    "); else
          if(getCalefaccion() == OFF)     lcd.print("Ca: OFF   ");
          if(getVentilador() == ON)       lcd.print("   Ve: ON "); else
          if(getVentilador() == OFF)      lcd.print("   Ve: OFF");
          break;

        case 3:
          /*
           *  ESTACION + HORA + SENSOR FOTOSENSIBLE = PERSIANAS
           */
          lcd.print("Estacion: ");
          if(getEstacion() == INVIERNO)   lcd.print("INVIERNO  "); else
          if(getEstacion() == PRIMAVERA)  lcd.print("PRIMAVERA "); else
          if(getEstacion() == VERANO)     lcd.print(" VERANO   "); else
          if(getEstacion() == OTONIO)     lcd.print(" OTONIO   ");
          lcd.setCursor(0,1);
          lcd.print("Hora:       ");
          if(getHora()<10)                lcd.print("0"+String(getHora())+":");     else lcd.print(String(getHora())+":");
          if(getMinutos()<10)             lcd.print("0"+String(getMinutos())+":");  else lcd.print(String(getMinutos())+":");
          if(Segundos<10)                 lcd.print("0"+String(Segundos));          else lcd.print(String(Segundos));
          lcd.setCursor(0,2);
          if(getFotosensible() == ON)     lcd.print("S.Foto:     DIA     "); else
          if(getFotosensible() == OFF)    lcd.print("S.Foto:    NOCHE    ");
          lcd.setCursor(0,3);
          if(getPersianas() == UP)        lcd.print("Persianas:  UP     ."); else
          if(getPersianas() == OFF)       lcd.print("Persianas: OFF     ."); else
          if(getPersianas() == DOWN)      lcd.print("Persianas: DOWN    .");
          break;
          
        case 4:
          /*
           *  SENSOR FOTOSENSIBLE = LUCES
           */
          if(getFotosensible() == ON)     lcd.print("S.Foto:     DIA    ."); else
          if(getFotosensible() == OFF)    lcd.print("S.Foto:    NOCHE   .");
          lcd.setCursor(0,1);
          if(getLuces() == ON)            lcd.print("Luces: ON          .") ; else
          if(getLuces() == OFF)           lcd.print("Luces: OFF         .");
          lcd.setCursor(0,2);
          lcd.print(".                  .");          
          lcd.setCursor(0,3);
          lcd.print(".                  .");
          break;
          
        default:
          break;
      } 
    }
}d(27);
void CambioP(){
/*****************************************/
/*  Interrupcion procedente del boton 1  */
/*****************************************/
  if((millis()-antiRebote)>500){
    if(numPant == 1)  numPant = 2; else
    if(numPant == 2)  numPant = 3; else
    if(numPant == 3)  numPant = 1; else
    antiRebote = millis();
  }
}
void Mas(){
/*****************************************/
/*  Interrupcion procedente del boton 2  */
/*****************************************/
  if((millis()-antiRebote)>500){
    d.setConsigna(d.getConsigna() + 1);
    antiRebote = millis();
  }
}
void menos(){
/*****************************************/
/*  Interrupcion procedente del boton 3  */
/*****************************************/
  if((millis()-antiRebote)>500){
    d.setConsigna(d.getConsigna() - 1);
    antiRebote = millis();
  }
}
void Aux(){
/*****************************************/
/*  Interrupcion procedente del boton 4  */
/*****************************************/
  if((millis()-antiRebote)>500){
    /*
     * BOTON4
     */
    antiRebote = millis();
  }
}

void setup(){
  //INICIALIZAR COMUNICACION SERIE
  Serial.begin(9600);     // Iniciar la comunicacion USB
  Serial2.begin(9600);    // Iniciar la comunicacion Bluetooth
  
  //INICIALIZAR PANTALLA
  lcd.begin (20,4);
  lcd.setBacklightPin(3,POSITIVE);
  lcd.setBacklight(HIGH);
  
  //INICIALIZAR PINES
  pinMode(pinZumbador, OUTPUT);
  pinMode(pinVentilador, OUTPUT);
  pinMode(pinCalefaccion, OUTPUT);
  pinMode(pinLuces, OUTPUT);
  pinMode(pinPersiana, OUTPUT);
  
  //INTERRUPCIONES BOTONES
  attachInterrupt(digitalPinToInterrupt(pinBoton1), CambioP, FALLING);
  attachInterrupt(digitalPinToInterrupt(pinBoton2), Mas , FALLING); 
  attachInterrupt(digitalPinToInterrupt(pinBoton3), menos , FALLING);
  attachInterrupt(digitalPinToInterrupt(pinBoton4), Aux, FALLING);
  
  //TOMA LAS PRIMERAS MEDICIONES
  d.actTemperatura();  d.actMovimiento();  d.actFotosensible();
}
void loop(){
  if((millis()-TIEMPO)>1000){
    d.actHora();
    timerTEMP++;
    timerUSB++;
    TIEMPO = millis();
  }
  // MIDO TEMPERATURA
  if(timerTEMP >= 3){
    d.actTemperatura();
    timerTEMP = 0;
  }
  // TOMO MEDICIONES
  d.actMovimiento();
  d.actFotosensible();
  
  // RECIBE INFO DEL BLUETOOTH
  d.Bluetooth();
  
  // HAGO ELECCION
  d.Eleccion();
  
  // MUESTRO INFO POR USB CADA 1s
  if(timerUSB >= 1){
    //d.INFO_USB();
    timerUSB = 0;
  }
  
  // MUESTRO INFO POR PANTALLA
  d.INFO_PANTALLA();
}
