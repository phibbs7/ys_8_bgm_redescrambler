# ys_8_bgm_redescrambler
A small program that allows replacing the BGMs in Ys 8.

Note: I didn't RE the descrambling scheme. Credit for the used descrambling scheme goes to Kaldaien on Steam Community for (re-)[posting](https://steamcommunity.com/app/579180/discussions/0/3288067088114993308/?ctp=2#c1694920442952076795) the reversed eng'd descrambling scheme.

Usage:
	ys_8_bgm_redescrambler <operation> <input file> <output file>

	Where mode is either:
		d - descramble the given input file and write the result to the given output file.

		e - scramble the given input file and write the result to the given output file.

	Note, the program will check to see if the input and output files are the same, and will refuse to do anything if they are.

	Giving no arguments to the program will display the usage guide.

Compiling:

	On Linux:
		gcc -o ys_8_bgm_redescrambler ys_8_bgm_redescrambler.c

	On Windows:
		1. Open the Visual Studio Command Prompt
		2. Move to the folder where you put ys_8_bgm_redescrambler.c and type this in:
			cl.exe /TC /EHsc ys_8_bgm_redescrambler.c /Feys_8_bgm_redescrambler.exe

All content within is copyrighted / trademarked / patented by their respective owners. 

