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