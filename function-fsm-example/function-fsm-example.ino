//Example: Objects that hold a FunctionFSM as an instance variable.

#include "FunctionFSM.h"

/*class MyClass {
private:
  //class variables
  char a = 'a';
  char b = 'b';
  
  //fsm triggers
  enum Trigger {
    TOGGLE_SWITCH
    //can add more triggers here...
  };

  //fsm state functions
  void a_on_enter(){ Serial.print("Entering a: "); }
  void a_on(){ Serial.print(a); }
  void a_on_exit(){ Serial.println(" - exitting a. "); }
  void a_on_trans_b(){ Serial.println("Moving from a to b."); }
  
  void b_on_enter(){ Serial.print("Entering b: "); }
  void b_on(){ Serial.print(b); }
  void b_on_exit(){ Serial.println(" - exitting b. "); }
  void b_on_trans_a(){ Serial.println("Moving from b to a."); }
  
  //fsm states (initialise in MyClass constructor)
  FunctionState state_a;
  FunctionState state_b;
  
  //fsm (initialise with MyClass constructor)
  FunctionFsm fsm;
  
public:
  MyClass()
  : state_a([this]() { a_on_enter(); },
            [this]() { a_on(); },
            [this]() { a_on_exit(); }),
            
    state_b([this]() { b_on_enter(); },
            [this]() { b_on(); },
            [this]() { b_on_exit(); }),
    
    fsm(&state_a)
  {}
  
  void init(){
    //add triggers
    //fsm.add_transition(&state_a, &state_b, TOGGLE_SWITCH, [this]() { a_on_trans_b(); } );
    //fsm.add_transition(&state_b, &state_a, TOGGLE_SWITCH, [this]() { b_on_trans_a(); } );
    fsm.add_transition(&state_a, &state_b, TOGGLE_SWITCH, nullptr);
    fsm.add_transition(&state_b, &state_a, TOGGLE_SWITCH, nullptr);
  }
  
  void update(){
    fsm.run_machine();
  }
  
  void toggleFsm(){
    fsm.trigger(TOGGLE_SWITCH);
  }
  
};

MyClass myclass;
*/

//non-class version.

//variables
char a = 'a';
char b = 'b';

//fsm triggers
enum Trigger {
  TOGGLE_SWITCH
  //can add more triggers here...
};

//fsm state functions
void a_on_enter(){ Serial.print("Entering a: "); }
void a_on(){ Serial.print(a); }
void a_on_exit(){ Serial.println(" - exitting a. "); }
void a_on_trans_b(){ Serial.println("Moving from a to b."); }

void b_on_enter(){ Serial.print("Entering b: "); }
void b_on(){ Serial.print(b); }
void b_on_exit(){ Serial.println(" - exitting b. "); }
void b_on_trans_a(){ Serial.println("Moving from b to a."); }

//fsm states (initialise in MyClass constructor)
FunctionState state_a(&a_on_enter, &a_on, &a_on_exit);
FunctionState state_b(&b_on_enter, &b_on, &b_on_exit);

//fsm (initialise with MyClass constructor)
FunctionFsm fsm(&state_a);

unsigned long start = 0;

void setup(){
  Serial.begin(19200);
  Serial.flush();
  
  Serial.println("DEBUG: Serial opened.");
  //myclass.init();
  fsm.add_transition(&state_a, &state_b, TOGGLE_SWITCH, &a_on_trans_b);
  fsm.add_transition(&state_b, &state_a, TOGGLE_SWITCH, &b_on_trans_a);
  Serial.println("DEBUG: Finished setup.");
}

void loop(){
  //myclass.update();
  fsm.run_machine();
  if (millis() - start >= 2000) {
    fsm.trigger(TOGGLE_SWITCH);
    //myclass.toggleFsm();
  }
  
}