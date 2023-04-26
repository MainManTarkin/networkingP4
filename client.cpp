/*
    CSC 3600 - Network Programming
    P4 - RetroChat Client

    FrontEnd Group
*/
#include <iostream>
#include<string>
#include <csignal>
#include <curses.h>
#include <getopt.h>
#define DEBUG_MODE true

using namespace std;

// Function Prototypes
void InitializeWindows();
void TakeDownWindows();
void SIGINTHandler(int signum);
void HandleGetOptions(int argc, char* argv[], string& serverAddress,string& name);
string server_address = nullptr;
string name = nullptr;
int server_port = 0;
void AddStrToConvoWindow(string stringToDisplay);

bool continueFlag = true; // This will keep track of whether or not our program can continue to run. SIGINT will make this false, causing it to end

WINDOW* fullScr = nullptr; // This will refer to the entire screen window
WINDOW* convoWindow = nullptr; // This will refer to the conversation window
WINDOW* inputWindow = nullptr; // This will refer to the input window

int main(int argc, char* argv[])
{
    // Assigns the SIGINTHandler method to be called async when SIGINT is signaled
    // NOTE: This should be ran first in main()
    signal(SIGINT, SIGINTHandler);
    //This function is using the getopt funtion in here to make our main look pretty :)
    HandleGetOptions(argc, argv, server_address, name);

    InitializeWindows();

    while(continueFlag)
    {
        // Run the program
        cout << "Program running..." << endl; // Placeholder to indicate the main loop is running
        // Check the value of continueFlag several times, break out of the loop if false
    }

    TakeDownWindows();
    return 0;
}
//--
void AddStrToConvoWindow(string stringToDisplay)
{
    //setup for printing
    const char* cStr = stringToDisplay.c_str();
    curs_set(0);

    //scroll, then print at the bottom of the convoWindow
    scroll(convoWindow);
    mvwprintw(convoWindow, getmaxy(convoWindow) - 2, 1, cStr);

    //set cursor back to the (last position??) of the input window, and make cursor visible
    //move() to (last position??)
    curs_set(1);
}
//--
bool GetUserInput(string& buffer)
{
    int ch;
     nodelay(stdscr, TRUE);
     for (;;) {
          if ((ch = getch()) == ERR) {
              /* user hasn't responded
               ...
              */
          }
          else {
              /* user has pressed a key ch
               ...
              */
          }
     }

    int ch = getch();
    switch (ch) {
          case KEY_BACKSPACE: /* user pressed backspace */ 
            
          case KEY_UP:  /* user pressed up arrow key */
            
          case KEY_DOWN:  /* user pressed up arrow key */
            
        //   case 'A'   /* user pressed key 'A' */
            
    }
}
//--
/*This is the function declaration. It takes four arguments:
argc: the number of arguments passed in from the command line.
argv[]: an array of strings containing the arguments.
serverAddress: a reference to a string that will hold the server IP address.
name: a reference to a string that will hold the client name.*/
void HandleGetOptions(int argc, char* argv[], string& serverAddress,string& name)
{
   char c;
   /*This starts a while loop that runs through all the arguments passed in from the command line. getopt() is a C function that parses command line options.
    The string "hs:p:n" specifies the allowed options. The h, s, p, and n characters are the single-character options. 
    If a character is followed by a colon (:), it means that option requires an argument.
    getopt() returns the next option character found, or -1 if there are no more options.*/
	while ((c = getopt(argc, argv, "hs:p;n")) != -1) 
    {
            switch (c) {
                case 'h':
                    cerr << argv[0] << "options:" << endl;
                    cerr << "    -h displays help" << endl;
                    cerr << "    -s server_ip overrides the default server IP address of 127.0.0.1. " << endl;
                    cerr << "    -p port overrides the default port number of 5077." << endl;
                    cerr << "    -n name overrides the default client name of Unknown." <<endl;
                    throw 0;

                case 's':// Get the server IP address argument and assign it to the `serverAddress` string.
                    server_address = string(optarg);
                    break;
                    
                case 'p':// Get the server port number argument and convert it to an integer using `atoi()`. Assign it to the `serverPort` variable.
                    server_port    = atoi(optarg);
                    break;

                case 'n':// Get the client name argument and assign it to the `name` string.
                  name = string(optarg);
                 break;


            }
        }
}
//--
/*
    This function will enable/disable any settings needed for our program to function
    as desired. savetty() will ensure the user's terminal settings are backed up.

    It is important to manually enable/disable whichever settings we want, regardless if they
    are expected to be on by default
*/
void InitializeWindows()
{
    // Saves the user's terminal settings in an internal buffer, 
    // terminal will reset to these settings in TakeDownWindows's resetty()
    savetty();

    // Initialize our window and create our subwindows
    fullScr = initscr();
    // NOTE: WINDOW* subwin(window*, nlines, ncols, y, x)
    convoWindow = subwin(fullScr, LINES - 3, COLS, 0, 0);
    inputWindow = subwin(fullScr, 3, COLS, LINES - 3, 0);

    keypad(inputWindow, true);
    raw(); // Disable any signal processing from the terminal, take input 
    noecho(); // We don't want anything printing to the window without us doing it manually
    curs_set(0); // Turn off the cursor
    nodelay(stdscr, true); // Make getch a non-blocking call
    
    wsetscrreg(convoWindow, 1, getmaxy(convoWindow) - 1); // Sets the conversation window to scroll within the border only

    // Draw the Convo Window's border
    box(convoWindow, 0, 0);
    mvwaddstr(convoWindow, 0, 1, "Conversation");

    // Draw the Input Window's border
    box(inputWindow, 0, 0);
    mvwaddstr(inputWindow, 0, 1, "Text Entry");
}
//--
/*
    This function will reset the terminal back to 'normal'
    along with take down the ncurses windows as we move to end the program
*/
void TakeDownWindows()
{
    // Resets the terminal settings back to whatever 
    // they were before InitializeWindowSettings() was ran [more specifically, savetty()]
    resetty(); 
    endwin();
}
//--
/*
    This function will be called Async'ly whenever SIGINT is flagged (ctrl-C)
    indicating we want to start ending the program.
*/
void SIGINTHandler(int signum)
{
    cerr << "Signint called..." << endl; // Placeholder to signify the interrupt was properly handled
    continueFlag = false;
}

