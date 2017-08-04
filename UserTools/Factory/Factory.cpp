#include "../Unity.cpp"

Tool* Factory(std::string tool){
Tool* ret=0;

// if (tool=="Type") tool=new Type;
if (tool=="DummyTool") ret=new DummyTool;

if (tool=="FEEReader") ret=new FEEReader;
  if (tool=="Trigger") ret=new Trigger;
if (tool=="InputVariables") ret=new InputVariables;
 if (tool=="RootDataRecorder") ret=new RootDataRecorder;
return ret;
}

