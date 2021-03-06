# osu-ac
Introduction
osu-ac was written by matt (aevv) and me (Alex).

Note that this is a WIP!

Feel free to ask questions. (email or questions, you can find us on irc as well irc://exp.ulse.net:6667).

How to build osu-ac
We do not provide binaries, as this was by choice. Rather, we provide the necessary source files to compile it yourself. You will need this probably to build it. Note that you only need C++; during the installation, it may have an option for SQL Server or something, those are NOT required. Make two separate Win32 Console Applications (Visual C++ >> Win32 >> Win32 Console Application. Make sure you pick console application, not windows application), one for recording and another for playback. Check empty project at the wizard. Now, you'll need to download the actual source of osu-ac. Download a subversion client, the most popular being something like tortoise svn. Create a new folder, in which you will download the project files. If you have install tortoise correctly, you should right click and see the option svn checkout. In there, paste the line http://osu-ac.googlecode.com/svn/trunk/, and hit ok. All the files should now be downloaded. In C++ Express, hit view and then solution explorer. In the "Source Files" folder, add stdafx.cpp to both of the projects. In the playback project, put pla.cpp in the source files, and the recorder put rec.cpp. In both of them, add to the "Header Files" targetver.h and stdafx.h.

How to use osu-ac
Getting started
The first part of using osu-ac involves configuring it to use the correct memory addresses. This is done primarily with Cheat Engine (link here). Download that, and install it. For the rest of the tutorial, just leave osu! open and attach Cheat Engine to osu!.exe. Note that everytime you restart osu!, you may have to rescan these values (the audio offset and mouse buttons are static, but the mouse pointer can change).

edit: having to re-find the addresses could be remedied by disabling ASLR (address space layout randomization)

Required memory addresses
The recorder uses 4 values to work -

The pointer to the mouse
The right mouse button pressed boolean (rmb)
The left mouse button pressed boolean (lmb)
Byte position (or millisecond) of the audio
Each of these values can be found using the same method in different places. The method to do this easily is to do an initial scan for unknown variable, then use increased/decreased value depending on what you do in the game. Addresses placed in two separate text files - address.txt and address2.txt are in format 0xFFFFFF - 32bit hexadecimal (more on this later).

The text files
osu-ac uses two text files for reading settings

address.txt for recording
address2.txt for playback
Please make sure you make these in the same directory as the executables.

Format for address.txt
Each number represents the line number which the address should be on.

Audio address
Base cursor address
Right mouse button address
Left mouse button address
Example :

0x0031555C
0x03A239D0
0x003153F0
0x00315398
Format for address2.txt
Each number represents the line number which the address should be on.

Audio address
key #1 (the first key you use in osu instead of presses)
key #2 (the second key you use in osu instead of presses)
Example :

0x0031555C
z
x
Addresses of Right Mouse Button/Left Mouse Button
IMPORTANT - YOU MUST DO THIS SEPARATELY FOR EACH INDIVIDUAL BUTTON

Make a replay (make a map with 30sec or whatever of play) where you just click (doesn't matter whether you click or use keyboard), hold for a bit, and release (recommended 10 second intervals between each). Remember to use one button only! A button is represented by a boolean, which has two states. The value of the boolean changes depending on whether it is pressed down or not. When the button is not pressed, it is a 0 and when it is pressed it is 1. Watch the replay, and first scan with "Unknown initial value" in CE. If the button was not held down and then became held down (i.e, when the cursor expands) scan for "increased by 1". Otherwise, if the button was held down and then released, scan for "decreased by 1." Repeat this a few times until you see some values that match up to the pressing/releasing of each button (you don't want the value that relates to EITHER button being pressed). The memory addresses of these go in address.txt on 3rd/4th line, and are constant so don't need to be changed. Now, make sure to do this for the other button as well.

Audio Offset Addresses
Do the same thing as above, except do it while playing the map yourself. Pause the song and search for unknown, then play for a few seconds and search for increased value. Repeat this a few times, then quit out and play again and search for decreased value. It should give you a value that increased quickly (not in milliseconds) as you're playing and relates to the music position (it's the number of bytes processed). So go to main menu and scroll the song with the scroll bar and it should go up/down relative to your changes. Address for this is also constant so doesn't have to be changed. Place it in the 1st line of address.txt. This is also the first line of address2.txt.

Mouse Position Address
The last thing to find changes every run (there is a static version of it, if anyone could tell us how to find it, would be greatly appreciated). Make a replay where you begin with your cursor at the top of the screen, then drag it slowly down to the bottom, and slowly back up to the top. Watch the replay. Scan for unknown at the beginning, and then when the cursor is moving down means your cursor's y value increased, so scan for increased value when it is, and when it's moving up it means the y value decreased, so scan for decreased value. Look for a value that corresponds to the Y (vertical) position of the mouse, as there will be many (note that when your cursor is at about the top it should be close to zero, and when the cursor is near the bottom, it should be close to the height of the window). Once you see this, don't change song or the address will change. Click the value and move it to the bottom with the red arrow, then right click and "find what writes to this address". Agree to attach debugger. You should then see a command being executed "movsd", right click and show in disassembler. a few lines above, there will be a line such as mov edi,03xxxxD0? - the value inside the square brackets is your pointer, should start with 03 and end with D0 (Does not have to). This goes as the 2nd address in address.txt. This value resets each time you run the game, but only takes 20 seconds to find so we didn't bother finding a constant pointer to it.

Using the actual programs
Press F6 in both projects to build them, and put both exes in the same directory, along with address.txt and address2.txt. With all these values in place, you need a folder "replays" inside the same directory as the exes, and then it is almost all automated. Keys Q W E and R change the playback speed, but display the change in console so you can figure it out. With recorder open, any time you enter a song it will automatically record the replay to replays folder. with playback open, it will automatically find a matching replay to use (if it doesn't exist it just does nothing). Press P to stop the current playback (it will also stop if the title changes to osu!).

Additional Notes
Note that the sampling rate during recording is about every 2 ms, so you probably need a decent PC for this to work, otherwise recording/playback will NOT work very well. Note also that not a perfect reproduction of the replay is made, as there can be offsets due to the audio pointer not updating quickly enough. Saved replays should be compatible across machines that use the same osu! resolution.

All in all, do not expect this to be a foolproof method to copy replays - there will be instances where it will fail due to lag during recording or during playback, or a slow updating audio pointer. Secondly, the cursor offset (actual coordinates added in playback) are slightly off, in the future this may need to be fine tuned.

