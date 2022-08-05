# SkipHook
SkipHook is a Header only library that allows you to create function wrappers, that skip the first instruction.

# Why?
Certain AntiCheats (eg. Battleye) try to find Cheaters by verifiying the RETURNADDRESS of specific WinApi- / Gamefunctions. 
They either do this by placing a direct hook (0xE9) to their internal module OR they try to catch an exception (0xCC). To prevent this you can use SkipHook.

# How?
SkipHook trys to disassemble the first instruction of the function passed to `skip_hook::make_skip_hook`. If it was able to decode the instruction SkipHook creates a trampoline in a local section which executes the first instruction and jumps back to the original code flow.
