# SkipHook
SkipHook is a header-only library that allows you to create function wrappers that skip the first instruction.

# Why?
Certain AntiCheats (eg. Battleye) try to find Cheaters by verifying the RETURNADDRESS of specific WinApi- / Gamefunctions. 
They either do this by placing a direct hook (0xE9) to their internal module OR they try to catch an exception (0xCC). To prevent this, you can use SkipHook.

# How?
SkipHook trys to disassemble the first instruction of the function passed to `skip_hook::make_skip_hook`. If it was able to decode the instruction, SkipHook creates a trampoline in a local section which executes the first instruction and jumps back to the original code flow.

# Code
Example to create a SkipHook function:
```
#include "skiphook.h"

int main() {

	auto skIsBadReadPtr = skip_hook::make_skip_hook<decltype(&IsBadReadPtr)>((uint64_t)IsBadReadPtr);

	std::cout << "skIsBadReadPtr(" << std::hex << GetModuleHandleA(0) << ", 8) -> " << skIsBadReadPtr(GetModuleHandleA(0), 8) << std::endl;
	
	auto skGetAsyncKeyState = skip_hook::make_skip_hook<decltype(&GetAsyncKeyState)>((uint64_t)GetAsyncKeyState);

	while (!skGetAsyncKeyState(VK_ESCAPE))
	{
		std::cout << "Waiting for ESCAPE key!" << std::endl;
		Sleep(1000);
	}
	
}
```
