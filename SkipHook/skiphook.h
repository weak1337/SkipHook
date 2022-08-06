#pragma once
#include "hde64.h"
#include "hde32.h"
#include <iostream>
#pragma section(".0dev", execute, read, write)
#pragma comment(linker,"/SECTION:.0dev,ERW")
__declspec(allocate(".0dev"))  uint8_t allocation[0x1000] = {};

namespace skip_hook {

	static uint32_t tracker = 0;

	template <typename T>
	T make_skip_hook(uint64_t address) {

#if _WIN64
		auto is_minhooked = false;
		while (*(uint8_t*)address == 0xE9)
			address += *(int32_t*)(address + 1) + 5;

		while (*(uint16_t*)address == 0x25FF) {
			address = *(uint64_t*)(address + 6);
			is_minhooked = true;
		}

		while (*(uint16_t*)address == 0xB848 && *(uint16_t*)(address + 5) != 0xE0FF)
			address = *(uintptr_t*)(address + 2);

		if (is_minhooked) {
			//Try to walk the func until it jumps back to the trampoline. This is a small implementation and might not work for bigger functions
			while (*(uint8_t*)(address) != 0x48 || *(uint8_t*)(address + 1) != 0xFF || *(uint8_t*)(address + 2) != 0x25)
				address++;
			address = *(uint64_t*)(address + *(int32_t*)(address + 3) + 7);
		}


		hde64s info = {};
		hde64_disasm((void*)address, &info);
#else
		while (*(uint8_t*)address == 0xE9)
			address += *(int32_t*)(address + 1) + 5;
		hde32s info = {};
		hde32_disasm((void*)address, &info);
#endif

		if (info.flags & F_ERROR) //Couldn't decode
			return nullptr;

		if (!info.len) //Couldn't decode
			return nullptr;

		if ((info.modrm & 0xC7) == 0x05) //Rip relative
			return nullptr;

#if _WIN64
		uint8_t jmp_stub[] = { 0xFF, 0x25, 0x00,0x00,0x00,0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
		*(uint64_t*)(&jmp_stub[6]) = address + info.len;
#else
		uint8_t jmp_stub[] = { 0xE9, 0x00, 0x00, 0x00, 0x00 };
		*(int32_t*)(&jmp_stub[1]) = (int32_t)(address - (uint64_t)&allocation[tracker + info.len] + info.len - sizeof(jmp_stub));
#endif
		if (tracker + sizeof(jmp_stub) + info.len > 0x1000) {
			std::cout << "Please increase section size!" << std::endl;
			return nullptr;
		}

		

		for (int i = 0; i < info.len; i++) {
			allocation[tracker + i] = *(uint8_t*)(address + i);
		}
		for (int i = 0; i < sizeof(jmp_stub); i++) {
			allocation[tracker + info.len + i] = jmp_stub[i];
		}
		uint32_t old_track = tracker;
		tracker += info.len + sizeof(jmp_stub);

		return (T)(&allocation[old_track]);
	}

}