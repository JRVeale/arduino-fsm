/*------------------------------------------------*\
 * FunctionFSM                                    *
 * By JRVeale                                     *
 * 7th May 2019                                   *
 *                                                *
 * An FSM libary based upon arduino-fsm that uses *
 * std::function<void()> instead of raw function  *
 * pointers. This will allow FSMs to be used      *
 * parent classes.                                *
 *                                                *
 * (c) James Veale 2019                           *
\*================================================*/

#include "FunctionFSM.h"

FunctionState::FunctionState(std::function<void()> on_enter,
                std::function<void()> on_state,
                std::function<void()> on_exit)
: on_enter{std::move(on_enter)},
  on_state{std::move(on_state)},
  on_exit{std::move(on_exit)}
{}

FunctionFsm::FunctionFsm(FunctionState* initial_state)
: m_current_state(initial_state),
  m_transitions(NULL),
  m_num_transitions(0),
  m_num_timed_transitions(0),
  m_initialized(false)
{}

FunctionFsm::~FunctionFsm(){
  free(m_transitions);
  free(m_timed_transitions);
  m_transitions = NULL;
  m_timed_transitions = NULL;
}

void FunctionFsm::add_transition(FunctionState* state_from,
                                 FunctionState* state_to,
                                 int event,
                                 std::function<void()> on_transition){
  if(state_from == NULL || state_to == NULL) return;
  
  Transition transition = FunctionFsm::create_transition(state_from,
                                                         state_to,
                                                         event,
                                                         on_transition);
  //all this manual memory management seems silly. m_transitions could easily be
  //a std::list or whatever instead of a pointer.
  m_transitions = (Transition*) realloc (m_transitions,
                                         (m_num_transitions + 1)
                                          * sizeof(Transition));
  m_transitions[m_num_transitions] = transition;
  m_num_transitions++;
}

void FunctionFsm::add_timed_transition(FunctionState* state_from,
                                       FunctionState* state_to,
                                       unsigned long interval,
                                       std::function<void()> on_transition){
  if (state_from == NULL || state_to == NULL) return;
  
  Transition transition = FunctionFsm::create_transition(state_from,
                                                         state_to,
                                                         0,
                                                         on_transition);
  
  TimedTransition timed_transition;
  timed_transition.transition = transition;
  timed_transition.start = 0;
  timed_transition.interval = interval;
  
  m_timed_transitions = (TimedTransition*) realloc(m_timed_transitions,
                                                   (m_num_timed_transitions + 1)
                                                    * sizeof(TimedTransition));
  m_timed_transitions[m_num_timed_transitions] = timed_transition;
  m_num_timed_transitions++;
}

FunctionFsm::Transition FunctionFsm::create_transition(FunctionState* state_from,
                                                       FunctionState* state_to,
                                                       int event,
                                                       std::function<void()> on_transition){
  Transition t;
  t.state_from = state_from;
  t.state_to = state_to;
  t.event = event;
  t.on_transition = on_transition;
  
  return t;
}

void FunctionFsm::trigger(int event){
  if(m_initialized){
    //Find the transition with the current state and given event.
    for (int i = 0; i < m_num_transitions; ++i){
      if (m_transitions[i].state_from == m_current_state
          && m_transitions[i].event == event) {
        FunctionFsm::make_transition(&(m_transitions[i]));
        return;
      }
    }
  }
}

void FunctionFsm::check_timed_transitions(){
  for (int i = 0; i < m_num_transitions; ++i){
    TimedTransition* transition = &m_timed_transitions[i];
    if(transition->transition.state_from == m_current_state){
      if(transition->start == 0){
        transition->start = millis();
      }
      else {
        unsigned long now = millis();
        if (now - transition->start >= transition->interval){
          FunctionFsm::make_transition(&(transition->transition));
          transition->start = 0;
        }
      }
    }
  }
}

void FunctionFsm::run_machine(){
  //first run must execute first state "on_enter"
  if (!m_initialized){
    m_initialized = true;
    if (!m_current_state->on_enter) { //if nullptr (see [func.wrap.func] paragraph 8 in standard)
      m_current_state->on_enter();
    }
  }
  
  if (!m_current_state->on_state) {  //if nullptr (see [func.wrap.func] paragraph 8 in standard)
    m_current_state->on_state();
  }
  
  FunctionFsm::check_timed_transitions();
}

void FunctionFsm::make_transition(Transition* transition) {
  //Execute handlers in the correct order
  if (!transition->state_from->on_exit) {  //if nullptr (see [func.wrap.func] paragraph 8 in standard)
    transition->state_from->on_exit();
  }
  
  if (!transition->on_transition) {  //if nullptr (see [func.wrap.func] paragraph 8 in standard)
    transition->on_transition();
  }
  
  if (!transition->state_to->on_enter) {
    transition->state_to->on_enter();
  }
  
  m_current_state = transition->state_to;
  
  //Initialise all timed transitions from m_current_state
  unsigned long now = millis();
  for (int i = 0; i < m_num_timed_transitions; ++i) {
    TimedTransition* ttransition = &m_timed_transitions[i];
    if(ttransition->transition.state_from == m_current_state){
      ttransition->start = now;
    }
  }
}



















