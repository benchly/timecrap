/* This is what it looks like when dumb people
   learn to code. I'm so sorry. Hey, look at 
   all these fine libraries I learned about, 
   though! */

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <signal.h>

// At attempt at cross-platform compatibility
#ifdef _WIN32
	#include <conio.h>
	#include CLEAR_SCREEN "cls"
#else
	#include <termios.h>
	#define CLEAR_SCREEN "clear"
#endif

//  ANSI Colors definitions
#define RESET "\033[0m"
#define BOLD "\033[1m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"

// FUNctions
void clearScreen(void);
void displayTime(void);
void displayTimeMessage(const char* time_str);
char getNonBlockingChar(void);
void setupTerminal(void);
void displayHeader(void);
void displayMenu(void);
void restoreTerminal(void);
void displayUpdateMessage(void);
void displayUnixEpoch(void);
void cleanExit(int signum);

// Ooh, Global variables. Exciting!
static struct termios orig_termios;

// The real action
int main(void) {
	bool running = true;
	setupTerminal();
	signal(SIGINT, cleanExit); 

	while (running) {
		clearScreen();
		displayHeader();
		displayTime();
		displayMenu();

		char input = getNonBlockingChar();
		if (input != 0) {
			switch (tolower(input)) {
			
				case 't':
					displayUpdateMessage();
					sleep(1);
					clearScreen();
					displayHeader();
					displayTime();
					displayMenu();
					break;

				case 'u':
					displayUnixEpoch();
					sleep(5);
					clearScreen();
					displayHeader();
					displayTime();
					displayMenu();
					break;
					
				case 'q':
					running = false;
					break;
			}
		}
		sleep(1); // Update once per second. Probably. 
	}
	// Restore the original terminal before quitting for a better time
	restoreTerminal();
	printf("\n%sThere is nothing interesting about this program!%s\n", GREEN, RESET);
	return 0;
}

// A needlessly fancy display
void displayHeader(void) {
	printf("%s%s+-----------------------------+%s\n", BOLD, BLUE, RESET);
	printf("%s%s|    Dumb Time Program 2.4    |%s\n", BOLD, BLUE, RESET);
	printf("%s%s+-----------------------------+%s\n", BOLD, BLUE, RESET);
}

// Actually show the dumb time
void displayTime(void) {
	time_t now = time(NULL);
	char time_str[50];
	char date_str[50];
	struct tm *local_time = localtime(&now);

	// Formatting for the sake of formatting
	strftime(time_str, sizeof(time_str), "%H:%M", local_time);
	strftime(date_str, sizeof(date_str), "%Y-%B-%d", local_time);

	printf("%s%sDate: %s%s\n", BOLD, YELLOW, date_str, RESET);
	printf("%s%sTime: %s%s\n\n", BOLD, GREEN, time_str, RESET);

	displayTimeMessage(time_str);
}

// Pointless conditional alerts! Yay!
void displayTimeMessage(const char* time_str) {
	if (strstr(time_str, ":45") != NULL) {
		printf("%s%sQuarter to the hour!%s\n", BOLD, MAGENTA, RESET);
	}
	if (strstr(time_str, ":00") != NULL) {
		printf("%s%sTop of the hour!%s\n", BOLD, RED, RESET);
	}
	if (strstr(time_str, ":15") != NULL) {
		printf("%s%sQuarter past the hour!%s\n", BOLD, MAGENTA, RESET);
	}
	if (strstr(time_str, ":30") != NULL) {
		printf("%s%sHalf past the hour!%s\n", BOLD, MAGENTA, RESET);
	}
}

// An expandable menu for more useless options!
void displayMenu(void) {
	printf("\n%s%sTime Wasting Options:%s\n", BOLD, BLUE, RESET);
	printf("%s[T]%s Refresh Dumb Time\n", YELLOW, RESET);
	printf("%s[U]%s Show Cool Time\n", YELLOW, RESET);
	printf("%s[Q]%s Quit Stupid Program\n\n", YELLOW, RESET);
}

// Guess what this does
void clearScreen(void) {
	system(CLEAR_SCREEN);
}

/* Since every platform could not agree on just
   one 'clear screen' command, I have to do the following, 
   which I guess is a learning experience since I now 
   have a rudimentary understanding of processor 
   directives. Yes, AI helped me with this part, 
   because it's less people to go ask a robot than 
   it is to look or ask for an answer on the Internet
   only to end up being led astray with some overly 
   complex flex-nuts solution, or general human 
   toxicity. Whatever, I didn't make the mess, I'm 
   just trying to live with it, at this point. */
#ifdef _WIN32
char getNonBlockingChar(void) {
	if (_kbhit()) {
		return _getch();
	}
	return 0;
}
#else
char getNonBlockingChar(void) {
	struct termios old_tio, new_tio;
	char c = 0;

	tcgetattr(STDIN_FILENO, &old_tio);
	new_tio = old_tio;
	new_tio.c_lflag &= (~ICANON & ~ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);

	if (read(STDIN_FILENO, &c, 1) < 0) {
		c = 0;
	}

	tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);
	return c;
}

void setupTerminal(void) {
	tcgetattr(STDIN_FILENO, &orig_termios);
	
	struct termios new_termios = orig_termios;
	new_termios.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
}

void displayUpdateMessage(void) {
	printf("\033[%d;0H", 15); // move cursor down
	printf("%sTick tock, tick tock....%s", BLUE, RESET);
	fflush(stdout); 
}

void displayUnixEpoch(void) {
	time_t epoch_time = time(NULL);
	printf("\033[%d;0H", 15); // move cursor down
	printf("%sUnix Epoch Time: %ld%s", BLUE, (long)epoch_time, RESET);
	fflush(stdout);
}

/* So this was fun, since after implementing the 
   above, every time I exited it did not reset my 
   terminal, so I could not type anything. This
   takes care of that. */
void restoreTerminal(void) {
	tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
}

void cleanExit(int signum) {
	restoreTerminal();

	switch(signum) {
	case SIGINT:
		printf("\n%sT*E*R*M*I*N*A*T*E*D by user with Ctrl+C%s\n", RED, RESET);
		break;
	case SIGTERM:
		printf("\n%sProgram recieved termination request and has stopped.%s\n", RED, RESET);
	default:
		printf("\n%s Terminated by %d%s\n", RED, signum, RESET);
	}
	
	exit(0);	
}
#endif
