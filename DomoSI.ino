#include <math.h>
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#define PASSWORD 1234

#define ON 1
#define OFF 0
#define PRIMAVERA 1
#define VERANO 2
#define OTONIO 3
#define INVIERNO 4
#define UP 1
#define DOWN 2

const int pinTemperatura = 8;
const int pinFotosensible = 9;
const int pinMovimiento = 38;
const int pinZumbador = 0;
const int pinVentilador = 0;
const int pinCalefaccion = 0;
const int pinLuces = 0;
const int pinPersiana = 0;

volatile unsigned long antiRebote=0;

unsigned long TIEMPO=0;
unsigned long contadorUSB = 1;
unsigned long timerSONDAS=0;
unsigned long timerUSB=0;
unsigned long timerPANTALLA=0;

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
      Estacion = OTONIO;
      Hora = 24;
      Minutos = 59;
      Segundos = 40;
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
      else Movimiento = OFF;
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
      /*
       * ALGORITMO PARA SENSOR DE LUZ
       */
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
    void setHora(int h){
      if(1 <= h && h <= 24)
        Hora = h;
    }

    int getMinutos(){
      return Minutos;
    }
    void setMinutos(int m){
      if(0 <= m && m <= 59)
        Minutos = m;
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

    void Mediciones(){
      /**************************************************/
      /*  Se actualizan las mediciones de los sensores  */
      /**************************************************/
      actTemperatura();
      actMovimiento();
      actFotosensible();
    }
    
    String entrada = "";
    
    void Bluetooth(){
      /***************************************************************************/
      /*  Comunicacion por Bluetooth con la App Android para manejar el sistema  */
      /***************************************************************************/
      if(Serial1.available() > 0){
        entrada.concat(Serial1.read()); //Leer un caracter
        if(entrada == "7978"){ 
          //Si es "ON", encender la alarma
          setAlarma(ON);
          entrada = "";
        } 
        if(entrada == "797070"){
          //Si es "OFF", apagar la alarma 
          setAlarma(OFF);
          entrada = "";
        }
        if(entrada == "6980"){
          //Si es "EP", estacion es PRIMAVERA
          setEstacion(PRIMAVERA);
          entrada = "";
        }
        if(entrada == "6986"){
          //Si es "EV", estacion es VERANO
          setEstacion(VERANO);
          entrada = "";
        }
        if(entrada == "6979"){
          //Si es "EO", estacion es OTOÑO
          setEstacion(OTONIO);
          entrada = "";
        }
        if(entrada == "6973"){
          //Si es "EI", estacion es INVIERNO
          setEstacion(INVIERNO);
          entrada = "";
        }
        if(entrada == "6743"){
          //Si es "C+", aumentar Consigna
          setConsigna(getConsigna()+1);
          entrada = "";
        }
        if(entrada == "6745"){
          //Si es "C-", aumentar Consigna
          setConsigna(getConsigna()-1);
          entrada = "";
        }
      }
    }
    void eleccion(){
    /*************************************************************************************************************************/
    /*  Cambia los valores de las variables locales, según los datos tomados por sondas, acciones web, y situacion anterior  */
    /*************************************************************************************************************************/
      
      /*
       * IF()
       * ELSE
       */
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
        if(getAlarma() == ON)   Serial.print("ON "); else
        if(getAlarma() == OFF)  Serial.print("OFF");
      Serial.println();

      //ENTRADAS
      Serial.print("      ENTRADAS:\t\t");
      Serial.print("  Temperatura: "+String(getTemperatura()));
      Serial.print("  S.Mov: ");
        if(getMovimiento() == ON)   Serial.print(" DETECT  "); else
        if(getMovimiento() == OFF)  Serial.print("NO_DETECT");
      Serial.print("  S.Foto: ");
        if(getFotosensible() == ON)    Serial.print("DIA  "); else
        if(getFotosensible() == OFF)   Serial.print("NOCHE");
      Serial.println();
      
      //SALIDAS
      Serial.print("      SALIDAS:\t\t");
      Serial.print("  Zumbador: ");
        if(getZumbador() == ON)     Serial.print("ON "); else
        if(getMovimiento() == OFF)  Serial.print("OFF");
      Serial.print("  Ventilador: ");
        if(getVentilador() == ON)   Serial.print("ON "); else
        if(getVentilador() == OFF)  Serial.print("OFF");
      Serial.print("  Calefaccion: ");
        if(getCalefaccion() == ON)  Serial.print("ON "); else
        if(getCalefaccion() == OFF) Serial.print("OFF");
      Serial.print("  Luces: ");
        if(getLuces() == ON)  Serial.print("ON "); else
        if(getLuces() == OFF) Serial.print("OFF");
      Serial.print("  Persianas: ");
        if(getPersianas() == OFF)   Serial.print("OFF "); else
        if(getPersianas() == UP)    Serial.print(" UP "); else
        if(getPersianas() == DOWN)  Serial.print("DOWN");
      Serial.println();
      Serial.println();
      contadorUSB++;
    }
    void INFO_PANTALLA(){
    /*********************************************************************/
    /*  Muestra la informacion del Fancoil a traves de Pantalla por I2C  */
    /*********************************************************************/

    /*
     * [01]  CONFIGURACIONES:    Consigna: 22  Estacion: OTONIO     Hora: 24:59:41  Alarma: OFF
      ENTRADAS:     Temperatura: -21.16  S.Mov: NO_DETECT  S.Foto: NOCHE
      SALIDAS:      Zumbador: OFF  Ventilador: OFF  Calefaccion: OFF  Luces: OFF  Persianas: OFF 

     */
      lcd.home();
      lcd.print
    }
    void BD(){
    /********************************************************************************/
    /*  Envia la informacion del Fancoil a traves de la red hasta la Base de Datos  */
    /********************************************************************************/
     
     /*
      *   EthernetClient cliente;
      *   if (cliente.connect(ipserver, 80)>0) {  // Conexion con el servidor
      *     String mandar = "GET /FAN.php?";
      *     mandar.concat("ip=");
      *   
      *   BD
      *   
      *     cliente.print(mandar); // Enviamos los datos por GET
      *     cliente.println(" HTTP/1.0");
      *     cliente.println("User-Agent: Arduino 1.0");
      *     cliente.println();
      *   } 
      *   else {
      *   Serial.println("Fallo en la conexion con Servidor");
      *   }
      *   if (!cliente.connected()) {
      *     Serial.println("Desconectado del Servidor");
      *   }
      *   cliente.stop();
      *   cliente.flush();
      */
    }
}d(27);
void B_Estado(){
/**********************************************/
/*  Interrupcion procedente del boton Estado  */
/**********************************************/
  if((millis()-antiRebote)>500){
    /*
     * BOTON1
     */
    antiRebote = millis();}
}
void B_MTemp(){
/*********************************************/
/*  Interrupcion procedente del boton +Temp  */
/*********************************************/
  if((millis()-antiRebote)>500){
    /*
     * BOTON2
     */
    antiRebote = millis();}
}
void B_mTemp(){
/*********************************************/
/*  Interrupcion procedente del boton -Temp  */
/*********************************************/
  if((millis()-antiRebote)>500){
    /*
     * BOTON3
     */
    antiRebote = millis();}
}
void B_Ventilador(){
/*******************************************************/
/*  Interrupcion procedente del boton Modo Ventilador  */
/*******************************************************/
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
  Serial1.begin(9600);    // Iniciar la comunicacion Bluetooth
  
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
/*attachInterrupt(digitalPinToInterrupt(pinBEstado), B_Estado, FALLING);
  attachInterrupt(digitalPinToInterrupt(pinBMTemp ), B_MTemp , FALLING); 
  attachInterrupt(digitalPinToInterrupt(pinBmTemp ), B_mTemp , FALLING);
  attachInterrupt(digitalPinToInterrupt(pinBVenti ), B_Ventilador, FALLING);*/
  
  //TOMA LAS PRIMERAS MEDICIONES
  d.Mediciones();
} 
void loop(){
  if((millis()-TIEMPO)>1000){
    d.actHora();
    timerSONDAS++;
    timerUSB++;

    TIEMPO = millis();
  }
  // TOMO MEDICIONES
  d.Mediciones();
  // RECIBE INFO DEL BLUETOOTH
  d.Bluetooth();
  // MANDO INFO POR USB CADA 1s
  if(timerUSB >= 1){
    d.INFO_USB();
    timerUSB = 0;
  }
  
}
