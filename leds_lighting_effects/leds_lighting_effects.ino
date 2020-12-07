/* LEDs lighting effect sketch
   Created 2017 - March - 25
   By : Hesham Mohamed (LehKeda)
   Modified 2020 - May - 9
   By : Manuel Pedraza
*/

/* Global variables */

/* LEDs array . For proper fading effect you
   should attach your LEDs to PWM digital output
   PWM output pins are (3,5,6,9,10) in Pro Micro board
   the array should be terminated with 0 value ;
*/
int led[] = {5,6, 9, 10, 0};

int led_array_lenth;
int functions_array_lenth;
int on = 200;
int off = 0;

/* LEDs lighting effect functions */
// get lenth of leds array
void get_led_array_lenth() {
  for (int i = 0; led[i]; i++) 
    led_array_lenth++;
}

// To fade in/out each LED individually
void fade_individual() {
  
  int delay_time = 50;
  int min_fade = 0;
  int max_fade = 150;
  int step_fade = 15;
  
  for (int i = 0; i < led_array_lenth ; i++) {
    // Fade in from min to max in increments of 5 points:
    for (int fadeValue = min_fade ; fadeValue <= max_fade; fadeValue += step_fade) {
      // sets the value (range from 0 to 255):
      analogWrite(led[i], fadeValue);
      // wait for 30 milliseconds to see the dimming effect
      delay(delay_time);
    }

    // fade out from max to min in increments of 5 points:
    for (int fadeValue = max_fade ; fadeValue >= min_fade; fadeValue -= step_fade) {
      // sets the value (range from 0 to 255):
      analogWrite(led[i], fadeValue);
      // wait for 30 milliseconds to see the fading in effect
      delay(delay_time);
    }
  }
}

// To fade in/out all LEDs at once .
void fade_all() {
  
  int delay_time = 50;
  int min_fade = 20;
  int max_fade =120;
  int step_fade = 5;
  
  for (int i = 0; i < led_array_lenth; i++) {
    for (int fadeValue = min_fade ; fadeValue <= max_fade; fadeValue += step_fade) {

      // sets the fade value for all LEDs (range from 0 to 255):
      for (int x = 0; x < led_array_lenth; x++) 
        analogWrite(led[x], fadeValue);

      // wait for 30 milliseconds to see the dimming effect
      delay(delay_time);
    }

    // fade out from max to min in increments of 5 points:
    for (int fadeValue = max_fade ; fadeValue >= min_fade; fadeValue -= step_fade) {

      // sets the fade value for all LEDs (range from 0 to 255):
      for (int x = 0; x < led_array_lenth; x++)
        analogWrite(led[x], fadeValue);
      
      // wait for 30 milliseconds to see the fading in effect
      delay(delay_time);
    }
  }
}

// To randomly turn on LEDs
void random_individual_led() {
  int delay_time = 200;
  randomSeed(analogRead(0)); // To shuffle random function
  for (int i = 0; i <= led_array_lenth; i++) {
    /* In order to make all LEDs trun on the max value
       for random function should be as same as LEDs array lenth
       because random function returns MAX-1 .
    */
    int current_led = random(led_array_lenth);

    // 255 value makes LED on all time .
    analogWrite(led[current_led], on);

    // Delay before turn off the LED / Max time the LED will be on .
    delay(delay_time);

    // turn off LED
    analogWrite(led[current_led], off);
  }
}

// To randomly light group of LEDs
void random_led_group() {
  int delay_time = 200;
  
  randomSeed(analogRead(0)); // To shuffle random function
  for (int i = 0; i <= led_array_lenth; i++ ) {
    // LEDs will be lit up ;
    int current_leds[led_array_lenth];

    // Pointer to current_leds array , will be used to set LEDs will be lit up ;
    int *ptr = current_leds;

    // Number of LEDs will be lit
    int number_of_leds = random(led_array_lenth);

    for (int x = 0; x < number_of_leds; x++) {
      *ptr = led[random(led_array_lenth)]; // set LED will be lit up ;

      // 255 value makes LED on all time .
      analogWrite(*ptr, on);

      ++ptr; // go to next item on current_leds array ;
    }

    // Delay before turn off the LED / Max time the LED will be on .
    delay(delay_time);

    // turn off all LEDs have been lit
    for (int y = 0; current_leds[y]; y++) 
      analogWrite(current_leds[y], off);
  }
}

// To light one by one until every led is on, then off from last to the first
void one_after_another_no_loop()
{
  int delayTime = 100; // time (milliseconds) to pause between LEDs 
    
  // turn all the LEDs on:  
  for(int i=0; i<led_array_lenth; i++)
  {
    analogWrite(led[i], on);
    delay(delayTime); //wait delayTime milliseconds 
  }
  
  // turn all the LEDs off:
  for(int j=led_array_lenth-1; j>=0; j--)
  {
    analogWrite(led[j], off); //Turn off LED  
    delay(delayTime); //wait delayTime milliseconds
  }
}


void (*leds_lighting_effects_functions[])() = {fade_all, fade_individual, random_individual_led, random_led_group, one_after_another_no_loop, 0};
//void (*leds_lighting_effects_functions[])() = {random_led_group, 0};

void get_functions_array_lenth() {
  for (int i = 0; leds_lighting_effects_functions[i]; i++) 
    functions_array_lenth++;
}

// the setup function runs once when you press reset or power the board
void setup() {
  // First we need to ge the lenth of LEDs array and functions array
  get_led_array_lenth();
  get_functions_array_lenth();
}

// the loop function runs over and over again forever
void loop() {
  randomSeed(analogRead(0));
  // Here we call functions respective to which effects we want
  for (int i = 0; leds_lighting_effects_functions[i]; i++) {
    leds_lighting_effects_functions[random(functions_array_lenth)]();
  }
}
