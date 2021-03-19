#include <Servo.h>

#define LED 13
#define POTENCIOMETRO A0
#define potenciometerRefreshRate 50
#define BUZZER 2

// Define funções utilizadas no código
void ledControl (char, int);
void servoControl (char, int);
void buzzerControl (char, int);

int inicialTimePot = 0;
int currentTimePot = 0;

int valorAtualPotenciometro;

Servo servo;

void setup(){
	/* Iniciando a Serial e passando a taxa de baudrate */
	Serial.begin(9600); 

	// Configurando componentes do circuito
	pinMode (LED, OUTPUT);
	pinMode(POTENCIOMETRO, INPUT);
	pinMode(BUZZER, OUTPUT);
	servo.attach(9);

	// Inicializando componentes e variáveis
	digitalWrite (LED, LOW);
	servo.write(0);
	valorAtualPotenciometro = 0;

	inicialTimePot = millis();
}
 
void loop(){
	// Atualiza valor do potenciômetro
	valorAtualPotenciometro = map(analogRead(POTENCIOMETRO), 0, 1023, 0, 1000);

	// Verifica se já passou o tempo de atualização para enivar valor do potenciômetro
	currentTimePot = millis();
	if ((currentTimePot - inicialTimePot) > potenciometerRefreshRate){
		Serial.print(String(valorAtualPotenciometro) + ",");  
		inicialTimePot = millis();  
	}
	
	// Verifica se recebeu comando da interface gráfica  
	if (Serial.available()) {
		// Lê char e número que acompanha
		char command = Serial.read();
		const int commandValue = Serial.parseInt();

		// Para cada componente ativado, um caso e função para ativar
		switch (command){
		case 'l':
			ledControl(command, commandValue);
			break;
		case 's':
			servoControl(command, commandValue);
			break;
		case 'b':
			buzzerControl(command, commandValue);
			break;
		default:
			break;
		}
	} 
}

// Escreve um valor de PWM no LED
void ledControl (char command, int lumPWM){
	if (command == 'l'){
		analogWrite (LED, lumPWM);
	}
}

// Escreve o valor em graus para posicionar o motor
void servoControl (char command, int degree){
  	servo.write(degree);
}

// Escreve um valor de PWM no Buzzer
void buzzerControl (char command, int buzzerPWM){
  	analogWrite(BUZZER, buzzerPWM);
}
