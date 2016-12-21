#include <math.h>
#include <SPI.h>
#include <Ethernet.h>
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

const int pinTemperatura = 0, pinMovimiento = 0, pinFotosensible = 0;
const int pinZumbador = 0, pinVentilador = 0, pinCalefaccion = 0, pinLuces = 0, pinPersiana = 0;

volatile unsigned long antiRebote=0;
unsigned long TIEMPO=0, contadorUSB = 1, timerSONDAS=0, timerUSB=0, timerPANTALLA=0;

byte ip[] = {192,168,0,150};
byte mac[] = { 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, ip[3]};

EthernetServer server(80); //Creamos un servidor Web con el puerto 80 que es el puerto HTTP por defecto
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
      if(INVIERNO <= est && est <= OTONIO)
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

    void web(){
    /**
     * Muestra la pagina Web con la que podemos manejar el sistema
     */
      EthernetClient client = server.available(); //Creamos un cliente Web
      if(client){ //Cuando detecte un cliente a través de una petición HTTP
        boolean currentLineIsBlank = true; //Una petición HTTP acaba con una línea en blanco
        String cadena=""; //Creamos una cadena de caracteres vacía
        while (client.connected()){
          if (client.available()){
            char c = client.read(); //Leemos la petición HTTP carácter por carácter
            cadena.concat(c); 
            //Unimos el String 'cadena' con la petición HTTP (c). 
            //De esta manera convertimos la petición HTTP a un String
            //Ya que hemos convertido la petición HTTP a una cadena de caracteres, ahora podremos buscar partes del texto.
            int posicion=cadena.indexOf("/?-"); //Guardamos la posición de la instancia "FAN-" a la variable 'posicion'
            String dir = cadena.substring(posicion);
            String orden = "";
            String variable = "";
            
            
            //  ALARMA
            if(dir == "/?-AL-ON="+String(PASSWORD))       setAlarma(ON);
            if(dir == "/?-AL-OFF="+String(PASSWORD))      setAlarma(OFF);
            
            //  CICLO
            if(dir == "/?-PRIMAVERA")                     setEstacion(PRIMAVERA);
            if(dir == "/?-VERANO")                        setEstacion(VERANO);
            if(dir == "/?-OTONIO")                        setEstacion(OTONIO);
            if(dir == "/?-INVIERNO")                      setEstacion(INVIERNO);
            
            //  TEMPERATRA CONSIGNA
            if(dir =="/?-TE=15")                          setConsigna(15);
            if(dir =="/?-TE=16")                          setConsigna(16);
            if(dir =="/?-TE=17")                          setConsigna(17);
            if(dir =="/?-TE=18")                          setConsigna(18);
            if(dir =="/?-TE=19")                          setConsigna(19);
            if(dir =="/?-TE=20")                          setConsigna(20);
            if(dir =="/?-TE=21")                          setConsigna(21);
            if(dir =="/?-TE=22")                          setConsigna(22);
            if(dir =="/?-TE=23")                          setConsigna(23);
            if(dir =="/?-TE=24")                          setConsigna(24);
            if(dir =="/?-TE=25")                          setConsigna(25);
            if(dir =="/?-TE=26")                          setConsigna(26);
            if(dir =="/?-TE=27")                          setConsigna(27);
            if(dir =="/?-TE=28")                          setConsigna(28);
            if(dir =="/?-TE=29")                          setConsigna(29);
            if(dir =="/?-TE=30")                          setConsigna(30);

            //  HORA
            if(dir =="/?-HORA=1")                         setHora(1);
            if(dir =="/?-HORA=2")                         setHora(2);
            if(dir =="/?-HORA=3")                         setHora(3);
            if(dir =="/?-HORA=4")                         setHora(4);
            if(dir =="/?-HORA=5")                         setHora(5);
            if(dir =="/?-HORA=6")                         setHora(6);
            if(dir =="/?-HORA=7")                         setHora(7);
            if(dir =="/?-HORA=8")                         setHora(8);
            if(dir =="/?-HORA=9")                         setHora(9);
            if(dir =="/?-HORA=10")                        setHora(10);
            if(dir =="/?-HORA=11")                        setHora(11);
            if(dir =="/?-HORA=12")                        setHora(12);
            if(dir =="/?-HORA=13")                        setHora(13);
            if(dir =="/?-HORA=14")                        setHora(14);
            if(dir =="/?-HORA=15")                        setHora(15);
            if(dir =="/?-HORA=16")                        setHora(16);
            if(dir =="/?-HORA=17")                        setHora(17);
            if(dir =="/?-HORA=18")                        setHora(18);
            if(dir =="/?-HORA=19")                        setHora(19);
            if(dir =="/?-HORA=20")                        setHora(20);
            if(dir =="/?-HORA=21")                        setHora(21);
            if(dir =="/?-HORA=22")                        setHora(22);
            if(dir =="/?-HORA=23")                        setHora(23);
            if(dir =="/?-HORA=24")                        setHora(24);

            //  MINUTOS
            if(dir =="/?-MINUTOS=0")                      setMinutos(0);
            if(dir =="/?-MINUTOS=1")                      setMinutos(1);
            if(dir =="/?-MINUTOS=2")                      setMinutos(2);
            if(dir =="/?-MINUTOS=3")                      setMinutos(3);
            if(dir =="/?-MINUTOS=4")                      setMinutos(4);
            if(dir =="/?-MINUTOS=5")                      setMinutos(5);
            if(dir =="/?-MINUTOS=6")                      setMinutos(6);
            if(dir =="/?-MINUTOS=7")                      setMinutos(7);
            if(dir =="/?-MINUTOS=8")                      setMinutos(8);
            if(dir =="/?-MINUTOS=9")                      setMinutos(9);
            if(dir =="/?-MINUTOS=10")                     setMinutos(10);
            if(dir =="/?-MINUTOS=11")                     setMinutos(11);
            if(dir =="/?-MINUTOS=12")                     setMinutos(12);
            if(dir =="/?-MINUTOS=13")                     setMinutos(13);
            if(dir =="/?-MINUTOS=14")                     setMinutos(14);
            if(dir =="/?-MINUTOS=15")                     setMinutos(15);
            if(dir =="/?-MINUTOS=16")                     setMinutos(16);
            if(dir =="/?-MINUTOS=17")                     setMinutos(17);
            if(dir =="/?-MINUTOS=18")                     setMinutos(18);
            if(dir =="/?-MINUTOS=19")                     setMinutos(19);
            if(dir =="/?-MINUTOS=20")                     setMinutos(20);
            if(dir =="/?-MINUTOS=21")                     setMinutos(21);
            if(dir =="/?-MINUTOS=22")                     setMinutos(22);
            if(dir =="/?-MINUTOS=23")                     setMinutos(23);
            if(dir =="/?-MINUTOS=24")                     setMinutos(24);
            if(dir =="/?-MINUTOS=25")                     setMinutos(25);
            if(dir =="/?-MINUTOS=26")                     setMinutos(26);
            if(dir =="/?-MINUTOS=27")                     setMinutos(27);
            if(dir =="/?-MINUTOS=28")                     setMinutos(28);
            if(dir =="/?-MINUTOS=29")                     setMinutos(29);
            if(dir =="/?-MINUTOS=30")                     setMinutos(30);
            if(dir =="/?-MINUTOS=31")                     setMinutos(31);
            if(dir =="/?-MINUTOS=32")                     setMinutos(32);
            if(dir =="/?-MINUTOS=33")                     setMinutos(33);
            if(dir =="/?-MINUTOS=34")                     setMinutos(34);
            if(dir =="/?-MINUTOS=35")                     setMinutos(35);
            if(dir =="/?-MINUTOS=36")                     setMinutos(36);
            if(dir =="/?-MINUTOS=37")                     setMinutos(37);
            if(dir =="/?-MINUTOS=38")                     setMinutos(38);
            if(dir =="/?-MINUTOS=39")                     setMinutos(39);
            if(dir =="/?-MINUTOS=40")                     setMinutos(40);
            if(dir =="/?-MINUTOS=41")                     setMinutos(41);
            if(dir =="/?-MINUTOS=42")                     setMinutos(42);
            if(dir =="/?-MINUTOS=43")                     setMinutos(43);
            if(dir =="/?-MINUTOS=44")                     setMinutos(44);
            if(dir =="/?-MINUTOS=45")                     setMinutos(45);
            if(dir =="/?-MINUTOS=46")                     setMinutos(46);
            if(dir =="/?-MINUTOS=47")                     setMinutos(47);
            if(dir =="/?-MINUTOS=48")                     setMinutos(48);
            if(dir =="/?-MINUTOS=49")                     setMinutos(49);
            if(dir =="/?-MINUTOS=50")                     setMinutos(50);
            if(dir =="/?-MINUTOS=51")                     setMinutos(51);
            if(dir =="/?-MINUTOS=52")                     setMinutos(52);
            if(dir =="/?-MINUTOS=53")                     setMinutos(53);
            if(dir =="/?-MINUTOS=54")                     setMinutos(54);
            if(dir =="/?-MINUTOS=55")                     setMinutos(55);
            if(dir =="/?-MINUTOS=56")                     setMinutos(56);
            if(dir =="/?-MINUTOS=57")                     setMinutos(57);
            if(dir =="/?-MINUTOS=58")                     setMinutos(58);
            if(dir =="/?-MINUTOS=59")                     setMinutos(59);


     
            if (c == '\n' && currentLineIsBlank) { 
              //Cuando reciba una línea en blanco, quiere decir que la petición HTTP 
              // ha acabado y el servidor Web está listo para enviar una respuesta
              // Enviamos al cliente una respuesta HTTP
                client.println("HTTP/1.1 200 OK");
                client.println("Content-Type: text/html");
                client.println();
                //Página web en formato HTML
                client.println("<html align='center'>");
                client.println("<head>");
                client.println("<title>DomoSI</title>");
                client.println("</head>");
                client.println("<style>");
                client.println("body {margin auto; font-family: sans-serif; color: black; border-bottom: solid #72BFFE;}");
                client.println("b    {font-size:60px;}");
                client.println("</style>");
                client.println("<body bgcolor='#72BFFE'>");
                //TITULO
                client.println("<button onClick=location.href='.\' style='font-size:100px;  background-color: #72BFFE; border: solid #72BFFE; width:1000px; font-weight: bold;'>");
                client.println("DomoSI");
                client.println("</button>");
                client.println("<b< style='font-size:100px'><br></b>");
                client.println("<b< style='font-size:100px'><br></b>");


                if(getAlarma()==ON) client.println("<b>ALARMA = ON</b>");
                else                client.println("<b>ALARMA = OFF</b>");
                client.println("<b< style='font-size:100px'><br></b>");
                client.println("<b< style='font-size:100px'><br></b>");

                
                //ALARMA
                client.println("<b>CAMBIAR ALARMA</b>");
                if(getAlarma()==ON){
                  //entrada numerica
                  client.println("<form>");
                  client.println("<b>Valor entre 0 y 9999:  </b>");
                  client.println("<b><input type='number' name='-AL-OFF' min='0' max='9999'style='width:150px;height:100px;font-size:60px;background-color: grey'></b>");
                  //boton ENVIAR
                  client.println("<b>&nbsp</b>");
                  client.println("<b><input type='submit' style='width:150px;height:90px;font-size:40px;background-color: red'></b>");
                  client.println("</form>");
                }
                else{
                  //entrada numerica
                  client.println("<form>");
                  client.println("<b>Valor entre 0 y 9999:  </b>");
                  client.println("<b><input type='number' name='-AL-ON' min='0' max='9999'style='width:150px;height:100px;font-size:60px;background-color: grey'></b>");
                  //boton ENVIAR
                  client.println("<b>&nbsp</b>");
                  client.println("<b><input type='submit' style='width:150px;height:90px;font-size:40px;background-color: green'></b>");
                  client.println("</form>");
                }
                //salto linea
                client.println("<b< style='font-size:15px'><br></b>");
                
                //HORA
                client.println("<b>CAMBIAR HORA</b>");
                  //entrada numerica
                client.println("<form>");
                client.println("<b><input type='number' name='-HORA' min='1' max='24'style='width:150px;height:100px;font-size:60px;background-color: seagreen'></b>");
                //entrada numerica
                client.println("<form>");
                client.println("<b><input type='number' name='-MINUTOS' min='0' max='59'style='width:150px;height:100px;font-size:60px;background-color: seagreen'></b>");  
                  //boton ENVIAR
                client.println("<b>&nbsp</b>");
                client.println("<b><input type='submit' style='width:150px;height:90px;font-size:40px;background-color: green'></b>");
                client.println("</form>");
                  //salto linea
                client.println("<b< style='font-size:15px'><br><br></b>");
                  //valor TEMPERATURA
                client.println("<b>TEMPERATURA = </b>");
                client.println("<b>");client.println("</b>");
                //salto linea
                client.println("<b><br><br></b>");
               
                
                //VENTILADOR
                  //boton 1
                client.println("<button onClick=location.href='./?-M-VE=1\' style='font-size:30px; color: white; border: 5px solid white; width: 200px; background-color: saddlebrown;'>");
                client.println("<h1>1</h1>");
                client.println("</button>");
                client.println("<b>&nbsp</b>");
                  //boton 2
                client.println("<button onClick=location.href='./?-M-VE=2\' style='font-size:30px; color: white; border: 5px solid white; width: 200px; background-color: saddlebrown;'>");
                client.println("<h1>2</h1>");
                client.println("</button>");
                client.println("<b>&nbsp</b>");
                  //boton 3
                client.println("<button onClick=location.href='./?-M-VE=3\' style='font-size:30px; color: white; border: 5px solid white; width: 200px; background-color: saddlebrown;'>");
                client.println("<h1>3</h1>");
                client.println("</button>");
                client.println("<b>&nbsp</b>");
                  //boton AUTO
                client.println("<button onClick=location.href='./?-M-VE=AUTO\' style='font-size:30px; color: white; border: 5px solid white; width: 200px; background-color: indianred;'>");
                client.println("<h1>AUTO</h1>");
                client.println("</button>");
                  //salto linea
                client.println("<b< style='font-size:15px'><br><br></b>");
                  //valor VENTILADOR
                client.println("<b>VENTILADOR = </b>");
                  client.println("<b>AUTO</b>");
                  client.println("<b>3</b>");
                  
                client.println("</body>");
                client.println("</html>");
                break;
            }
            if (c == '\n') {
              currentLineIsBlank = true;
            }
            else if (c != '\r') {
              currentLineIsBlank = false;
            }
          }
        }
        //Dar tiempo al navegador para recibir los datos
        delay(20);
        client.stop();// Cierra la conexión
      } 
    }
    String entrada = "";
    
    void Bluetooth(){
      if(Serial1.available() > 0){
        entrada.concat(Serial1.read()); //Leer un caracter
        if(entrada == "7978"){ 
          //Si es "ON", encender la alarma
          setAlarma(ON);
          Serial.println("Alarma ON");
          entrada = "";
        } 
        if(entrada == "797070"){
          //Si es "OFF", apagar la alarma 
          setAlarma(OFF);
          Serial.println("Alarma OFF");
          entrada = "";
        }
        if(entrada == "6980"){
          //Si es "EP", estacion es PRIMAVERA
          setEstacion(PRIMAVERA);
          Serial.println("Estacion PRIMAVERA");
          entrada = "";
        }
        if(entrada == "6986"){
          //Si es "EV", estacion es VERANO
          setConsigna(VERANO);
          Serial.println("Estacion VERANO");
          entrada = "";
        }
        if(entrada == "6979"){
          //Si es "EO", estacion es OTOÑO
          setConsigna(OTONIO);
          Serial.println("Estacion OTONIO");
          entrada = "";
        }
        if(entrada == "6973"){
          //Si es "EI", estacion es INVIERNO
          setConsigna(INVIERNO);
          Serial.println("Estacion INVIERNO");
          entrada = "";
        }
        if(entrada == "6743"){
          //Si es "C+", aumentar Consigna
          setConsigna(getConsigna()+1);
          Serial.println("Consigna + 1");
          entrada = "";
        }
        if(entrada == "6745"){
          //Si es "C-", aumentar Consigna
          setConsigna(getConsigna()-1);
          Serial.println("Consigna - 1");
          entrada = "";
        }
      }
    }
    void eleccion(){
    /**
     * Cambia los valores de las variables locales, según los datos tomados por sondas, acciones web, y situacion anterior
     */
      /*
       * IF()
       * ELSE
       */
    }
    void ejecutarCambios(){
    /**
     * Cambia los valores de las salidas digitales segun los valores de las variables
     */
      /*
       * CAMBIO VALORES PINES
       */
    }
    void INFO_USB(){
    /**
     * Muestra la informacion del Fancoil a traves de USB
     */
      if(contadorUSB<10)  Serial.print("[0");  else Serial.print("[");  Serial.print(String(contadorUSB)+"]");
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
    /**
     * Muestra la informacion del Fancoil a traves de Pantalla por I2C
     */
      
      /*
       * lcd.home(); 
       * PANTALLA
       */
    }
    void BD(){
    /**
     * Envia la informacion del Fancoil a traves de la red hasta la Base de Datos
     */
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
/**
 * Interrupcion procedente del boton Estado
 */
  if((millis()-antiRebote)>500){
    /*
     * BOTON1
     */
    antiRebote = millis();}
}
void B_MTemp(){
/**
 * Interrupcion procedente del boton +Temp
 */
  if((millis()-antiRebote)>500){
    /*
     * BOTON2
     */
    antiRebote = millis();}
}
void B_mTemp(){
/**
 * Interrupcion procedente del boton -Temp
 */
  if((millis()-antiRebote)>500){
    /*
     * BOTON3
     */
    antiRebote = millis();}
}
void B_Ventilador(){
/**
 * Interrupcion procedente del boton Modo Ventilador
 */
  if((millis()-antiRebote)>500){
    /*
     * BOTON4
     */
    antiRebote = millis();
  }
}
void setup(){
  //INICIALIZAR COMUNICACION SERIE
  Serial.begin(9600);
  Serial1.begin(9600);   //Iniciar la info USB
  
  //INICIALIZAR PANTALLA
  lcd.begin (20,4);
  lcd.setBacklightPin(3,POSITIVE);
  lcd.setBacklight(HIGH);
  
  //INICIALIZAR PINES
  //pinMode(pinVenti1, OUTPUT); pinMode(pinVenti2, OUTPUT); pinMode(pinVenti3, OUTPUT); pinMode(pinValvula, OUTPUT);
  
  //INICIALIZAMOS LA COMUNICACION ETHERNET Y EL SERVIDOR
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("\nserver is at "); Serial.println(Ethernet.localIP());
  
  //INTERRUPCIONES BOTONES
  /*attachInterrupt(digitalPinToInterrupt(pinBEstado), B_Estado, FALLING);
  attachInterrupt(digitalPinToInterrupt(pinBMTemp ), B_MTemp , FALLING); 
  attachInterrupt(digitalPinToInterrupt(pinBmTemp ), B_mTemp , FALLING);
  attachInterrupt(digitalPinToInterrupt(pinBVenti ), B_Ventilador, FALLING);
  
  //TOMA LAS PRIMERAS MEDICIONES
  f.actTAmbiente(); f.actTIda(); f.actTVuelta();
  */
} 
void loop(){
  if((millis()-TIEMPO)>1000){
    d.actHora();
    timerSONDAS++;
    timerUSB++;
    timerUSB++;

    TIEMPO = millis();
  }
  // REFRESCO WEB
  d.web();
  // RECIBE INFO DEL BLUETOOTH
  d.Bluetooth();

  if(timerUSB > 1){
    d.INFO_USB();
    timerUSB = 0;
  }
}
