// Apple II virtual drive via COM1
// Copyright (C)2001 Terence J. Boldt
// You may copy and modify this source if
// you give me credit for it.
 

#include <windows.h>
#include <conio.h>
#include <stdio.h>

#define READBLOCK  1
#define WRITEBLOCK 2

// READ:  the Apple sends a command block with READBLOCK, blocknumber and an XOR of those 3 bytes
//        in response, the PC echos the command block and then sends 512 byte block followed by
//        a checksum (XOR of all bytes in block)

// WRITE: the Apple sends a command block with WRITEBLOCK, blocknumber and checksum
//        it also sends the 512 block and a checksum
//        in response, the PC echos the command if successful or garbage command if failure

int main(int argc, char* argv[])
{
	HANDLE hPort, hFile;
	DCB dcb;
	DWORD byteswritten;
	COMMTIMEOUTS commtimeouts;
	DWORD bytesread;
	unsigned char datablock[513];
	unsigned char command[6];
	boolean cmdlineerr = false;

	if (argc != 3) {
		cmdlineerr = true;
	}

	if (cmdlineerr) {
		printf("Apple2VirtualDrive (c)2001 Terence J. Boldt\n");
		printf("Note:  This program requires a null modem connection\n");
		printf("       to an Apple II computer at 19,200 bps and the\n");
		printf("       Apple must be running SERIAL.DRIVE under ProDOS.\n\n");
		printf("USAGE:  Apple2VirtualDrive COMx FILENAME.hdv\n");
		return 1;
	}

	printf("\nWaiting for command from Apple II via COM1...\n");

	// set up comm port
	hPort=CreateFile( argv[1],
					  GENERIC_READ | GENERIC_WRITE, //bidirectional
					  0, 
					 NULL, //no security
					 OPEN_EXISTING,  //this must be set; the ports are already created
					 FILE_ATTRIBUTE_NORMAL, // maybe with | FILE_FLAG_OVERLAPPED  
					 NULL );
	if (hPort == INVALID_HANDLE_VALUE) {
		printf("Failed to connect to %s.",argv[1]);
		return 1;
	}

	GetCommState(hPort, &dcb);

	dcb.BaudRate = CBR_19200;
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;
	dcb.fBinary = 1;
	dcb.fParity = 0;
	dcb.fOutX = 0;
	dcb.fInX = 0;

	SetCommState(hPort, &dcb);

	commtimeouts.ReadIntervalTimeout = 0;
	commtimeouts.ReadTotalTimeoutConstant = 0;
	commtimeouts.ReadTotalTimeoutMultiplier = 1000;
	commtimeouts.WriteTotalTimeoutConstant = 0;
	commtimeouts.WriteTotalTimeoutMultiplier = 1000;

	SetCommTimeouts(hPort, &commtimeouts);

	// set up hard drive file - create if does not exist
	hFile=CreateFile( argv[2],
					  GENERIC_READ | GENERIC_WRITE,
					  0, 
					 NULL, //no security
					 OPEN_ALWAYS,  
					 FILE_FLAG_RANDOM_ACCESS,
					 NULL );

	if (hFile == INVALID_HANDLE_VALUE) {
		CloseHandle(hPort);
		printf("Failed to open or create file %s.",argv[2]);
		return 1;
	}


	while( !_kbhit() ) {


		// wait for a four byte command from the Apple II
		do {
			ReadFile(hPort, &command, 4, &bytesread, NULL);
		}
		while (bytesread == 0 && !_kbhit());

		if (!_kbhit()) {
			if (bytesread == 4) {
				if (command[0] == WRITEBLOCK) {
					printf("write block %04X\n",command[2]*256+command[1]);
					// read data block from port
					ReadFile(hPort, &datablock, 513, &bytesread, NULL);
					// check data integrity
					// write to file
					SetFilePointer(hFile, // handle to file
								   512*command[1]+131072*command[2], // bytes to move pointer
								   NULL,  // bytes to move pointer
								   FILE_BEGIN // starting point
									);
					WriteFile(hFile, datablock, 512, &byteswritten, NULL);
					// echo command to port
					WriteFile(hPort, command, 4, &byteswritten, NULL);
				}
				else if (command[0] == READBLOCK) {
					printf("read block %04X\n",command[2]*256+command[1]);
					// check data integrity
					// echo command to port
					WriteFile(hPort, command, 4, &byteswritten, NULL);
					// read from file
					SetFilePointer(hFile, // handle to file
								   512*command[1]+131072*command[2], // bytes to move pointer
								   NULL,  // bytes to move pointer
								   FILE_BEGIN // starting point
									);
					ReadFile(hFile, datablock, 512, &bytesread, NULL);
					// calculate checksum
					// write block to port
					WriteFile(hPort, datablock, 513, &byteswritten, NULL);
				}
				else {
					printf("invalid command: %02X %02X %02X %02X\n",command[0], command[1], command[2], command[3]);
				}
			}
			else {
				printf("invalid command %d bytes instead of 4: %02X %02X %02X %02X\n",bytesread, command[0], command[1], command[2], command[3]);
			}
		}
	}

	_getch(); // clear key

	CloseHandle(hPort);
	CloseHandle(hFile);

	return 0;
}
