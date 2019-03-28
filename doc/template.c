CLASS �bergabe:

//...............................................................................
//  primary CLASS.h
//...............................................................................
#pragma once
  #include "SEC1.h"
  #include "SEC2.h"

class pri{
public:
//classes
  pri();
  SEC1 sec1;
  SEC2 sec2;   //need sec1
};
//...............................................................................
//  primary CLASS.cpp
//...............................................................................
pri::pri():
    sec2(sec1){
}

//==============================================================================

//...............................................................................
//  secondary CLASS_1.h
//...............................................................................
class SEC1{
public:
//classes
  SEC1();
};
//...............................................................................
//  secondary CLASS_1.cpp
//...............................................................................

//==============================================================================

//...............................................................................
//  secondary CLASS_2.h
//...............................................................................
class SEC2{
public:
//classes
  SEC2(SEC1& sec1);
  SEC1& sec1
};
//...............................................................................
//  secondary CLASS_2.cpp
//...............................................................................

SEC2::SEC2(SEC1& SEC1):
         sek1(sek1){
}

//###############################################################################
//###############################################################################
//###############################################################################

Callback:
//...............................................................................
//  Master CLASS.h
//...............................................................................
#pragma once
  #include "CallbackClass.h"

class Master{
public:
//classes
  Master();
  CallbackClass callbackClass;
  void on_Callback1();
  void on_Callback2(String result);
};

//...............................................................................
//  Master CLASS.cpp
//...............................................................................
#include <CLASS.h>

Master::Master(){

//callback Events
  callbackClass.set_callbacks(std::bind(&Master::on_Callback1, this),
                              std::bind(&Master::on_Callback2, this, std::placeholders::_1));
}

void Master::on_Callback1(){

}
void Master::on_Callback2(String result){

}

//==============================================================================

//...............................................................................
//  CallbackClass.h
//...............................................................................
#pragma once
  #include <functional>
  typedef std::function<void(void)> CallbackFunction;
  typedef std::function<void(String&)> String_CallbackFunction;

class CallbackClass{
public:
  CallbackClass();
  void set_callbacks(CallbackFunction Callback1,
                     String_CallbackFunction Callback2);
  void anyFunction();
private:
  CallbackFunction on_Callback1;
  String_CallbackFunction on_Callback2;
};

//...............................................................................
//  CallbackClass.cpp
//...............................................................................
#include <CallbackClass.h>

CallbackClass::CallbackClass(){
}

//...............................................................................
//  set callbacks
//...............................................................................
void CallbackClass::set_callbacks(CallbackFunction Callback1,
                                  String_CallbackFunction Callback2){
  on_Callback1 = Callback1;
  on_Callback2 = Callback2;
}
//...............................................................................
//  set callbacks
//...............................................................................
void CallbackClass::anyFunction(){
  if (on_Callback1 != nullptr) on_Callback1();  //callback event
  String result = "Callback information"
  if (on_Callback2 != nullptr) on_Callback2(result);  //callback event
}

//###############################################################################
//###############################################################################
//###############################################################################
