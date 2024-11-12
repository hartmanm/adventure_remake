/*********************************************************************************
**
**  Author: Michael Hartman OSU ID# 932708868
**
**  Email: hartmami@oregonstate.edu
**
**  Date: 2016-07-17
**
**********************************************************************************/

/*********************************************************************************
*
*  Description: Program 2 Adventure
*                   this program is a text based game in which the user must
*                   find their way through a series of connected rooms
*                   searching for the end room.  It's a throwback.
*
**********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>  // used to ensure a psuedo random seed

#define NUMBER_OF_ROOMS 10 // define the number of rooms once here as a constant

char directory[1024];  // stores the directory path for a created folder
char starting_room[1024]; // stores the starting room
char ending_room[1024]; // stores the ending room
char *steps[1024]; // stores the path taken by the player
char current_location[1024]; // stores the current location of the player
int valid_end = 0; // indicator of successful navigation
char user_input[1024]; // stores players input
int number_of_steps = 0; // stores the number of steps taken by the player

// stores the names of the rooms
char *rooms[NUMBER_OF_ROOMS] =
{
    "BLUE","YELLOW","RED","ORANGE","GREEN","PURPLE","WHITE","BLACK","CLEAR","MAGENTA"
};

// stores the 7 psuedo randomly selected rooms for an instance
char *generated_rooms[NUMBER_OF_ROOMS] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// stores the number of psuedo randomly generated connections for each room
int number_of_connections[NUMBER_OF_ROOMS] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// stores each connection between rooms as a pair of 1's in a matrix
// I found this program to have a lot of logical overlap with the first
// assignment.  I used a matrix transformation to implement translation in
// last assignment so matrices are still fresh in my mind.  Using a matrix to store
// the connections seemed the most logical.
int connections[NUMBER_OF_ROOMS][NUMBER_OF_ROOMS] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// if room is used 0 is replaced with one of the three:
// START_ROOM = 1, MID_ROOM = 2, END_ROOM = 3
int type[NUMBER_OF_ROOMS] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};


// function creates a directory to be used for each particular instance of the game
void make_directory(void)
{
    char cwd[1024];
    char temp[1024];

    int pid = getpid(); // get the current process id

    getcwd( cwd, sizeof( cwd ) ); // get the current working directory

    sprintf( temp, "/hartmami.rooms.%d", pid );

    directory[0] = '\0'; // ensures the char array is NULL

    strcat( directory, cwd ); // copy the current working directory to the char array

    strcat( directory, temp ); // copy the new directory name to the char array

    mkdir( directory, 0777 ); // create the new directory

    chdir( directory ); // open the new directory
}


// function stores each of the 7 psuedo randomly selected rooms as a room file
//  then assigns a type to each.  I found it easiest to assign the types here,
// but formatting requirements make it easier to print the types to files later on.
void make_rooms(void)
{
    char room_name[1024];
    char temp[1024];
    int i = 0;
    int x = 0;

    // ensures only 7 rooms are generated
    while( i < 7 )
    {
        int set = 0;
        int instance = 0;
        int used = 0;

        instance = rand() % 10; // generates a psuedo random number between 0 and 9

        // generates a psuedo random number between 0 and 3 then adds 3 to create
        // a ~random number between 3 and 6 inclusive
        int number_connections = ( rand() % 4 ) + 3;

        if( generated_rooms[i] == 0 )
        {
            for( x=0; x<9; x++ )
            {
                // searches to see if a room has been used yet
                if( generated_rooms[x] == rooms[instance] )
                {
                    used = 1;
                }
            }

            // if it has not been
            if( used == 0 )
            {
                // store the room name in the char array
                sprintf( room_name, "%s", rooms[instance] );

                FILE *f;

                // create a file called the room name and open it
           f = fopen( room_name, "a" );

                // add details to the file
                fprintf( f, "ROOM NAME: %s\n", rooms[instance] );

                // add room to the list of those generated
         generated_rooms[i] = rooms[instance];

                // assign the room the previously generated number of connections
         number_of_connections[i] = number_connections;

                if( i == 0 ) // the first room generated is given the starting room type
                {
                    type[i] = 1;

                    set = 1; // indicates a type has been assigned
                }

                if( i == 6 && set == 0 ) // the last room generated is given the ending room type
                {
                    type[i] = 3;

                    set = 1; // indicates a type has been assigned
                }

                // the middle 4 rooms generated are given the middle room type
                if( ( i > 0 || i < 6 ) && set == 0 )
                {
                    type[i] = 2;
                }

                i++; // increments counter

                fclose( f );  // closes the file
            }
        }
    }
}


// function creates the previously generated number of connections for each room
// by storing the connection as a pair of 1's in the connection matrix
void make_connections(void)
{
    int i = 0;
    int instance = 0;

    // must cycle thru all 10 possible rooms as you dont know which rooms will be
    // generated in each instance
    for( i=0; i<10; i++ )
    {
    int current = 0;

        // keep generating connections until previously generated number is reached
    while( current < number_of_connections[i] )
        {
     instance = rand() % 10; // generates a psuedo random number between 0 and 9

      if( instance != i ) // dont try to connect a room to itself
            {
                // check if a connection already exists
                if( ( generated_rooms[i] != 0 ) && ( generated_rooms[instance] != 0 ) )
                {
                    // create the far end connection
                    connections[instance][i] = 1;

                    // create the near end connection
              connections[i][instance] = 1;

              current++; // increments counter
                }
            }
    }
    }
}


// function prints the generated connections to the files in the directed format
// to prevent possible problems related to printing some connections before all
// connections have been generated; I separated the generation of the connections
// which is done in the make_connections function from the printing of them in
// this function.
void output_connections(void)
{
    int i = 0;
    int x = 0;
    char room_name[1024];

  for( i=0; i<10; i++ ) // cycle thru the near end connections
    {
    int current = 1;

    for( x=0; x<10; x++ ) // cycle thru the far end connections
        {
      if( connections[i][x] == 1 ) // if there is a connection
            {
                // store the near end connection room name in the char array
                sprintf( room_name, "%s", generated_rooms[i] );

                FILE *f;

                // use the char array to access the previously generated file
                f = fopen( room_name, "a" );

                // write the near end connection to its file
                fprintf( f, "CONNECTION %d: %s\n", current, generated_rooms[x] );

        fclose( f ); // close the file

        current++;  // increments counter
      }
    }
  }
}


// function prints the generated typed to the files in the directed format
// as the directions stated the type must be listed last in the generated files;
// I found it easier to make a distinct function to print to the files dispite
// the fact that I generated the types eariler in the make rooms function.
void output_types(void)
{
    int i = 0;
    char room_name[1024];

    while( i < 7 ) // for each generated room
    {
        // get the room name and store it in the char array
        sprintf( room_name, "%s", generated_rooms[i] );

        FILE *f;

        // use the char array to access the previously generated file
        f = fopen( room_name, "a" );

        // write the room type to each file in the directed format
        if( type[i] == 1 )
        {
            fprintf( f, "ROOM TYPE: START_ROOM\n" );

            // store the starting room in its own char array
            sprintf( starting_room, "%s", generated_rooms[i] );
        }

        if( type[i] == 3 )
        {
            fprintf( f, "ROOM TYPE: END_ROOM\n" );

            // store the ending room in its own char array
            sprintf( ending_room, "%s", generated_rooms[i] );
        }

        if( type[i] == 2 )
        {
            fprintf( f, "ROOM TYPE: MID_ROOM\n" );
        }

        fclose( f );  // close the file

      i++; // increments counter
  }
}


// this function handles all interaction with the user
void user_interface(void)
{
    char room_name[1024];
    int i = 0;
    int x = 0;
    int set = 0;
    int index = 0;
    int counter = 0;

    // sets the current loction to the starting room
    sprintf( current_location, "%s", starting_room );

    printf( "\n" );

    // do while loop continues until while flag is activated
    do
    {
        counter = 0; // reset counter
        set = 0; // reset flag
        room_name[0] = '\0'; // ensures char array is NULL

        printf("CURRENT LOCATION: %s\n", current_location);

        printf("POSSIBLE CONNECTIONS: ");

        for( x=0; x<7; x++ )
        {
            // cycle thru connection matrix
            if( connections[index][x] == 1  )
            {
                // store each connection in the char array
                sprintf( room_name, "%s", generated_rooms[x] );

                // before each connection is printed print , space
                // starting with the second connection
                if( counter > 0 )
                {
                    printf( ", " );
                }

                // output each connection
                printf( "%s", room_name );

                counter++; // increments counter
            }
        }

        printf( "\n" );

        printf("WHERE TO? >");

        user_input[0] = '\0'; // ensures char array is NULL

        scanf( "%s", user_input ); // gets user input

        printf( "\n" );

        // continues until a valid connection is input by the user
        // set is the varible used as the flag to indicate this
        while( set == 0 )
        {
            for( x=0; x<7; x++ )
            {
                if( connections[index][x] == 1  )
                {
                    // ensures char array is NULL at each execution of the
                    // loop this allows only the current room to be compared
                    // to the user input
                    room_name[0] = '\0';

                    sprintf( room_name, "%s", generated_rooms[x] );

                    // compare user input and current room
                    if( strcmp( room_name, user_input ) == 0 )
                    {
                        // user input is valid, update current location
                        // to the users input
                        sprintf( current_location, "%s", user_input );

                        // store the users input in the steps array
                        steps[number_of_steps] = generated_rooms[x];

                        // increment steps
                        number_of_steps++;

                        // uppdate index to the current location so it
                        // reflects the newly selected rooms connections
                        index = x;

                        // check to see if the player has won the game
                        if( strcmp( current_location, ending_room ) == 0 )
                        {
                            // if they have; throw the flag for the do while loop
                            valid_end = 1;
                        }
                            set = 1; // throw the flag to indicate valid input
                    }
                }
            }

            if( set == 0 ) // if input doesn't match a connection
            {
                printf( "HUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.\n" );

                printf( "\n" );

                printf( "CURRENT LOCATION: %s\n", current_location );

                printf( "POSSIBLE CONNECTIONS: " );

                counter = 0; // reset counter

                room_name[0] = '\0'; // ensures char array is NULL

                // first rooms index is always 0, but all prior vary depending
                // on the psuedo random generation and must be caculated
                for( i=0; i<7; i++ )
                {
                    sprintf( room_name, "%s", generated_rooms[i] );

                    // find the index of the current location in the
                    // array of generated rooms
                    if( strcmp( current_location, generated_rooms[i] ) == 0 )
                    {
                        index = i; // set the index
                    }
                }

                for( x=0; x<7; x++ )
                {
                    // cycle thru connection matrix
                    if( connections[index][x] == 1  )
                    {
                        // store each connection in the char array
                        sprintf( room_name, "%s", generated_rooms[x] );

                        // before each connection is printed print , space
                        // starting with the second connection
                        if( counter > 0 )
                        {
                            printf( ", " );
                        }

                        // output each connection
                        printf( "%s", room_name );

                        counter++; // increments counter
                    }
                }

                printf( "\n" );

                printf("WHERE TO? >");

                user_input[0] = '\0'; // ensures char array is NULL

                scanf( "%s", user_input ); // gets user input
            }
        }

    } while( valid_end == 0 ); // when flag is activated loop ends
}


// function indicates the game is complete and outlines the players path
void mission_complete(void)
{
    int i = 0;

    printf( "YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n" );

    printf( "YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", number_of_steps );

    for( i=0; i<number_of_steps; i++ ) // outputs players path
    {
        printf( "%s\n", steps[i] );
    }

    printf( "\n" );
}


// main function calls all others.  I thought it would be easier to understand the
// logic of the program if it was divided into component functions.  All function
// and varible names were made as self explaintory as possible.
int main()
{
    srand( time( NULL ) ); // seeds the rand function with a psuedo random number

    make_directory();

    make_rooms();

    make_connections();

    output_connections();

    output_types();

    user_interface();

    mission_complete();

    return 0;
}
