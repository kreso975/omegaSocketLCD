#include "I2CAccessSystem.h"
#include "I2CDevice.h"
#include "LCD_I2C.h"
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>                      //strlen
#include <sys/socket.h>
#include <arpa/inet.h>                   //inet_addr
#include <unistd.h>                      //write
#include <pthread.h>                     //for threading , link with lpthread
#include <stdint.h>
typedef uint32_t socklen_t;

// Set up the following as it applies to your specific LCD device
#define I2C_ADDR 0x3F                   // <<----- The I2C address for your LCD.  Find it using "i2cdetect -y 0"
#define En_pin 2                        // <<----- The enable pin for your LCD
#define Rw_pin 1                        // <<----- The read/write pin for your LCD
#define Rs_pin 0                        // <<----- The reset pin for your LCD
#define D4_pin 4                        // <<----- The Data 4 pin for your LCD
#define D5_pin 5                        // <<----- The Data 5 pin for your LCD
#define D6_pin 6                        // <<----- The Data 6 pin for your LCD
#define D7_pin 7                        // <<----- The Data 7 pin for your LCD
#define BACKLIGHT_PIN 3                 // <<----- The Back light pin for your LCD
#define BACKLIGHT_POLARITY POSITIVE     // <<----- The polarity of the Back light pin for your LCD

#define LCD_NUM_COL 16                  // <<----- The number of columns for your LCD
#define LCD_NUM_ROW 2                   // <<----- The number of rows for your LCD

// Socket Server
#define SERV_PORT 9001



//the thread function
void *connection_handler( void * );


static void show_usage( std::string argv )
{
	std::cerr 	<< "Usage: <option> [<STRING> <STRING>]\n"
				<< "Options:\n"
				<< "\t-h,--help\t\tShow this help message\n"
				<< "\t-c,--clear\t\tClear LCD screen\n"
				<< "\t-b,--backliteOff\tLCD backlite Off\n"
				<< "\t-B,--backliteOn\t\tLCD backlite On\n"
                << "\t-sl,--scrolleft\t\tScroll the display without changing the RAM\n"
                << "\t-sr,--scrolright\tScroll the display without changing the RAMn\n"
				<< "\t-w,--write\t\tWrite on LCD - [<STRING> and or <STRING>]\n"
				<< std::endl;
}

int main( int argc, char* argv[] )
{

    //
    // INITIALIZE SOCKET
    //
    int socket_desc , client_sock , c , *new_sock;
    struct sockaddr_in server , client;

    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( SERV_PORT );

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");

    //Listen
    listen(socket_desc , 20);

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c) ) )
    {
        puts("Connection accepted");

        pthread_t sniffer_thread;
        new_sock = (int*)malloc(sizeof(int));
        *new_sock = client_sock;

        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }

        //Now join the thread , so that we dont terminate before the thread
        pthread_join( sniffer_thread , NULL);
        puts("Handler assigned");
    }

    if ( client_sock < 0 )
    {
        perror("accept failed");
        return 1;
    }

    return 0;
}

//
//  This will handle connection for each client
//
void *connection_handler( void *socket_desc )
{

    I2CAccess * i2cAcc;                 // For general I2C Access
    I2CDevice * lcdDevice;              // Specific I2C Device
    LCD_I2C * lcd;                      // The LCD Display

    // Create and setup I2C Access on channel 0 - the only channel the Omega has
    i2cAcc = new I2CAccessSystem(0);

    // Create and setup I2C LCD device using the I2C Access with the specified I2C Address
    lcdDevice = new I2CDevice(i2cAcc, I2C_ADDR);

    // Create the LCD with access via the I2C Device using the specific data for the actual LCD
    lcd = new LCD_I2C(lcdDevice,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin, BACKLIGHT_PIN, BACKLIGHT_POLARITY);

    // Initialise the LCD for columns and rows
    lcd->begin(LCD_NUM_COL, LCD_NUM_ROW);

    // Position LCD cursor to start of first row
    lcd->setCursor( 0,0 );
    lcd->noBacklight();


    //Get the socket descriptor
    int sock = *( int* )socket_desc;
    int read_size;
    char *message , client_message[2000];

    //Send some messages to the client
    message = "Greetings! I am your connection handler\n";
    write( sock , message , strlen(message) );

    message = "Now type something and i shall repeat what you type \n";
    write( sock , char (message) , strlen(message) );

    //Receive a message from client
    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
    {
        //Send the message back to client
        write(sock , client_message , strlen(client_message)+1);

        if ( ( client_message == "-h" ) || ( client_message == "--help" ) )
            show_usage(0); // Show help

        else if ( ( client_message == "-c" ) || ( client_message == "--clear" ) )
            lcd->clear(); // Clear LCD - set cursor position to zero

        else if ( ( client_message == "-b" ) || ( client_message == "--backliteOff" ) )
        {
            lcd->noBacklight();
            puts("BackLite OFF");
        }
        else if ( ( client_message == "-B" ) || ( client_message == "--backliteOn" ) )
        {
            lcd->backlight();
            puts("BackLite ON");
        }
        else if ( ( client_message == "-sl" ) || ( client_message == "--scrolleft" ) )
            lcd->scrollDisplayLeft();

        else if ( ( client_message == "-sr" ) || ( client_message == "--scrolright" ) )
            lcd->scrollDisplayRight();

        else if ( ( client_message == "-w" ) || ( client_message == "--write" ) )
        {

            lcd->print( "Ovo je prvi red" );
            lcd->setCursor( 0,1 ); // Position LCD cursor to start of second row
            lcd->print( "Ovo je drugi red" ); // Output second input parameter to second row of LCD


        }


            /*
        else
        {
            show_usage( argv[0] );
            return 0;
        }

        */
        memset(client_message,"\n",sizeof(client_message));
    }

    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }

    //Free the socket pointer
    close( sock );
    free(socket_desc);

    return 0;
}