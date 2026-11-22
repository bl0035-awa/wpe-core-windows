// Windows 移植版 stub：栈回溯（imagehlp/bfd）不支持，GetCalls 返回空
#include "CallStack.h"

using namespace WallpaperEngine::Debugging;
using namespace std;

const char* CallStack::Unknown_Function = "[unknown function]";
const char* CallStack::Unknown_Module = "[unknown module]";

CallStack::CallStack (void) { }
CallStack::~CallStack (void) { }

void CallStack::GetCalls (vector<CallStack::CallInfo>& calls) {
    calls.clear ();
}

string CallStack::Demangle (const char* name) {
    return string (name == nullptr ? CallStack::Unknown_Function : name);
}