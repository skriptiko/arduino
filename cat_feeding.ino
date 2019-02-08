#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h> 

LiquidCrystal_I2C lcd(0x27,16,2);

int drivePin1 = 8;
int drivePin2 = 9;
int drivePin3 = 10;
int drivePin4 = 11;
int driveDellay = 1;

//Key message
int numMenu = 0;

int MAX_MENU_LENG = 5;
char msgs[6][17] = {
	"Menu           ", 
	"Type feeding   ", 
	"Portion        ", 
	"Delay          ", 
	"Feeding per day",
	"Exit           "
};

int MAX_ARR_TYPE_LENG = 2;
int numTypeFeedingMenu = 0;
char typeFeeding[3][9] = {
  "Request", 
  "Time   ", 
  "Dellay "
};

int MAX_PORTION = 7;
int numPortion = 0;
char arrPortion[8][5] = {
  "20g", 
  "30g", 
  "40g",
  "50g", 
  "60g", 
  "70g",
  "80g",
  "90g"
};

int MAX_FEEDING = 1;
int numFeeding = 0;
char arrFeeding[2][13] = {
  "twice      ",
  "three times"
};

int MAX_DELAY = 300;
int MIN_DELAY = 60;
int INCREMENT_DELAY = 30;
int numDelay = 60;

int MAX_ARR_MENU_LENG = 5;
int adc_key_val[5] ={30, 150, 360, 535, 760 };

int adc_key_in;
int key = -1;
int oldkey = -1;

int numDownMenu = 0;
bool isActivMenu = false;
bool isActivDoubleMenu = false;

byte topArrow[8] = {
	0b00000,
	0b00000,
	0b00000,
	0b00100,
	0b01010,
	0b10001,
	0b00000,
	0b00000
};

byte upArrow[8] = {
	0b00000,
	0b00000,
	0b00000,
	0b10001,
	0b01010,
	0b00100,
	0b00000,
	0b00000
};

byte leftArrow[8] = {
	0b00000,
	0b00010,
	0b00100,
	0b01000,
	0b10000,
	0b01000,
	0b00100,
	0b00010
};

byte rightArrow[8] = {
	0b00000,
	0b01000,
	0b00100,
	0b00010,
	0b00001,
	0b00010,
	0b00100,
	0b01000
};

void setup() { 
	lcd.init();                     
	lcd.backlight();
	Serial.begin(9600);

	lcd.createChar(0, topArrow);
	lcd.createChar(1, upArrow);
	lcd.createChar(2, leftArrow);
	lcd.createChar(3, rightArrow);

	pinMode(drivePin1, OUTPUT);
    pinMode(drivePin2, OUTPUT);
    pinMode(drivePin3, OUTPUT);
    pinMode(drivePin4, OUTPUT);
}


void loop() {
	adc_key_in = analogRead(0);
	key = get_key(adc_key_in);
	if (key != oldkey) {
		delay(50);
		adc_key_in = analogRead(0);
		key = get_key(adc_key_in);
		if (key != oldkey) { 
			lcd.clear();     
			oldkey = key;
			if (key >= 0) {
				if (key == 4) {
					if (numDownMenu == 0) {
						isActivMenu = true;
						numMenu = 0;
					}
					if (numDownMenu == 1) {
						isActivDoubleMenu = true;
					}

					if (numDownMenu == 2) {
						isActivDoubleMenu = false;
						numDownMenu = 0;
					}
					if (numMenu == 5) {
						reset ();
					} else {
						numDownMenu++;
					}
				}
				if (isActivMenu == false) return;
				if (isActivDoubleMenu == true) {
					if (numMenu == 1) {
						// left key
						if (key == 0) {
							numTypeFeedingMenu++;
						}
						// right key
						if (key == 3) {
							numTypeFeedingMenu--;
						}
					}

					if (numMenu == 2) {
						// left key
						if (key == 0) {
							numPortion++;
						}
						// right key
						if (key == 3) {
							numPortion--;
						}
					}
					if (numMenu == 3) {
						// left key
						if (key == 0) {
							numDelay += INCREMENT_DELAY;
						}
						// right key
						if (key == 3) {
							numDelay -= INCREMENT_DELAY;
						}
					}
					if (numMenu == 4) {
						// left key
						if (key == 0) {
							numFeeding++;
						}
						// right key
						if (key == 3) {
							numFeeding--;
						}
					}
				} else {
					// up key
					if (key == 2) {
						numMenu++;
					}
					// bottom key
					if (key == 1) {
						numMenu--;
					}
				}
				checkNumMenu();
				

				Serial.println("------------------------------------");
				Serial.println(key);
				Serial.println(numDownMenu);
				//Serial.println(numTypeFeedingMenu);
				Serial.println("------------------------------------");
			}
		}
	}
	

	if (isActivMenu == true) {
		printTypeMenu();
		printTypeDoubleMenu();
		// lcd.setCursor(5,0);
		// lcd.write("Menu");
		// lcd.setCursor(15,0);
		// lcd.write((uint8_t)0);
		// lcd.setCursor(15,1);
		// lcd.write((uint8_t)1);
	} else {
		writeTime();
		tickDrive();
	}
}

void checkNumMenu () {
	if (numMenu < 0) {
		numMenu = 0;
	}
	if (numMenu > MAX_MENU_LENG) {
		numMenu = MAX_MENU_LENG;
	}
	if (numTypeFeedingMenu < 0) {
		numTypeFeedingMenu = 0;
	}
	if (numTypeFeedingMenu > MAX_ARR_TYPE_LENG) {
		numTypeFeedingMenu = MAX_ARR_TYPE_LENG;
	}
	if (numPortion < 0) {
		numPortion = 0;
	}
	if (numPortion > MAX_PORTION) {
		numPortion = MAX_PORTION;
	}
	if (numDelay < MIN_DELAY) {
		numDelay = MIN_DELAY;
	}
	if (numDelay > MAX_DELAY) {
		numDelay = MAX_DELAY;
	}
	if (numFeeding < 0) {
		numFeeding = 0;
	}
	if (numFeeding > MAX_FEEDING) {
		numFeeding = MAX_FEEDING;
	}
}

// void checkActivDoubleMenu () {
// 	if (numMenu == 5 && numDownMenu == 1) {
// 		reset ();
// 	} else {
// 		numDownMenu++;
// 	}
// }

void reset () {
	numMenu = 0;
	numDownMenu = 0;
	isActivMenu = false;
	isActivDoubleMenu = false;
}

int get_key(unsigned int input){
	int k;
	for (k = 0; k < MAX_ARR_MENU_LENG; k++){
		if (input < adc_key_val[k]){
			return k;
		}
	}
	if (k >= MAX_ARR_MENU_LENG) {
		k = -1;
	}
	return k;
}

void printTypeDoubleMenu () {
	// if (isActivDoubleMenu == false) return;
	// if (numMenu == 1) {
	// 	lcd.setCursor(1,1);
	// 	lcd.write(typeFeeding[numTypeFeedingMenu]);
	// }
}

void printTypeMenu () {
	lcd.setCursor(0,0);
	lcd.write(msgs[numMenu]);
	lcd.setCursor(1,1);
	if (numMenu == 1) {
		lcd.write(typeFeeding[numTypeFeedingMenu]);
	}
	if (numMenu == 2) {
		lcd.write(arrPortion[numPortion]);
	}
	if (numMenu == 3) {
		lcd.print(numDelay);
	}
	if (numMenu == 4) {
		lcd.print(arrFeeding[numFeeding]);
	}
	printArrows();
}

void printArrows () {
	if (isActivDoubleMenu == true) {
		if (numMenu == 1) {
			if (numTypeFeedingMenu > 0) {
				lcd.setCursor(0,1);
				lcd.write((uint8_t)2);
			}
			if (numTypeFeedingMenu < MAX_ARR_TYPE_LENG) {
				lcd.setCursor(15,1);
				lcd.write((uint8_t)3);
			}
		}
		if (numMenu == 2) {
			if (numPortion > 0) {
				lcd.setCursor(0,1);
				lcd.write((uint8_t)2);
			}
			if (numPortion < MAX_PORTION) {
				lcd.setCursor(15,1);
				lcd.write((uint8_t)3);
			}
		}
		if (numMenu == 3) {
			if (numDelay > MIN_DELAY) {
				lcd.setCursor(0,1);
				lcd.write((uint8_t)2);
			}
			if (numDelay < MAX_DELAY) {
				lcd.setCursor(15,1);
				lcd.write((uint8_t)3);
			}
		}
		if (numMenu == 4) {
			if (numFeeding > 0) {
				lcd.setCursor(0,1);
				lcd.write((uint8_t)2);
			}
			if (numFeeding < MAX_FEEDING) {
				lcd.setCursor(15,1);
				lcd.write((uint8_t)3);
			}
		}
	} else {
		if (numMenu > 0) {
			lcd.setCursor(15,0);
			lcd.write((uint8_t)0);
		}

		if (numMenu < MAX_MENU_LENG) {
			lcd.setCursor(15,1);
			lcd.write((uint8_t)1);
		}
	}
}

void writeTime () {
	tmElements_t tm;
	if (RTC.read(tm)) {
		timeTick(0, 0, tm);  
	} else {
    	if (RTC.chipPresent()) {
    		//The DS1307 is stopped. Please run the SetTime
		} else {
			//DS1307 read error! Please check the circuitry
		}
	}
}

void timeTick(int character,int line, tmElements_t tm) {
  String seconds,minutes;
  lcd.setCursor(character,line);
  lcd.print(tm.Hour);
  lcd.print(":");
  if(tm.Minute<10)
  {
    minutes = "0"+String(tm.Minute);
    lcd.print(minutes);
  }else
  {
    lcd.print(tm.Minute);
  }
  lcd.print(":");
  if(tm.Second<10)
  {
    seconds = "0"+String(tm.Second);
    lcd.print(seconds);
  }else
  {
    lcd.print(tm.Second);
  }
}

void tickDrive() {
  digitalWrite( drivePin1, HIGH ); 
  digitalWrite( drivePin2, LOW ); 
  digitalWrite( drivePin3, LOW ); 
  digitalWrite( drivePin4, LOW );
  delay(driveDellay);

  digitalWrite( drivePin1, HIGH ); 
  digitalWrite( drivePin2, HIGH ); 
  digitalWrite( drivePin3, LOW ); 
  digitalWrite( drivePin4, LOW );
  delay(driveDellay);

  digitalWrite( drivePin1, LOW ); 
  digitalWrite( drivePin2, HIGH ); 
  digitalWrite( drivePin3, LOW ); 
  digitalWrite( drivePin4, LOW );
  delay(driveDellay);

  digitalWrite( drivePin1, LOW ); 
  digitalWrite( drivePin2, HIGH ); 
  digitalWrite( drivePin3, HIGH ); 
  digitalWrite( drivePin4, LOW );
  delay(driveDellay);

  digitalWrite( drivePin1, LOW ); 
  digitalWrite( drivePin2, LOW ); 
  digitalWrite( drivePin3, HIGH ); 
  digitalWrite( drivePin4, LOW );
  delay(driveDellay);

  digitalWrite( drivePin1, LOW ); 
  digitalWrite( drivePin2, LOW ); 
  digitalWrite( drivePin3, HIGH ); 
  digitalWrite( drivePin4, HIGH );
  delay(driveDellay);

  digitalWrite( drivePin1, LOW ); 
  digitalWrite( drivePin2, LOW ); 
  digitalWrite( drivePin3, LOW ); 
  digitalWrite( drivePin4, HIGH );
  delay(driveDellay);

  digitalWrite( drivePin1, HIGH ); 
  digitalWrite( drivePin2, LOW ); 
  digitalWrite( drivePin3, LOW ); 
  digitalWrite( drivePin4, HIGH );
  delay(driveDellay);
}