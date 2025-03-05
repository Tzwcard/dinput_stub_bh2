# dinput_stub_bh2
dinput stub dll for Beach Head 2002 for mordern fps mouse input

## How to install
Compile it as dinput.dll, place it next to ``bh2.exe`` in your steam game's directory, and run the game normally.

## How this works
Basically while the game is running, the window message loop will check the current mouse position, then compare ``X`` to ``320`` (even the game window is larger than ``320x240``) and apply horizontal spinning speed for that, and compare ``Y`` to ``240`` for vertical absolute position.  
It will feel like using a joystick for X positions while mouse for Y positions, I mean that's how this game should work back in the day and I did enjoy that when I was yound, but by playing it now I think we should at least has an option for it to be more like a "mordern" FPS aiming thing.  
So here we will hook the function that the game call for retrieving cursor position, calculate the ``X`` with offset to ``320`` multiple by a scale value then set it back, get the ``Y`` offset, then reset the mouse position to ``(320, Y)``.  
To archive that, we can: 1. use an external program to inject our target hook dll; 2. writing some opcodes for ``LoadLibrary(HOOKDLL);`` into main game executable and ``jmp`` in and out on game run; or 3. write a stub dll for proxified original function while having the hook function working.  
To keep this as clean as possible, here we use ``DINPUT.dll`` as our stub dll, because it only has 1 API calls from the game and won't modify the original files, while no need to inject it externally.
