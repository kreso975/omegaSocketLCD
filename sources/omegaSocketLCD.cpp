#include "I2CAccessSystem.h"
#include "I2CDevice.h"
#include "LCD_I2C.h"
#include <iostream>
#include <string>
#include<stdio.h>
#include<stdlib.h>                      //strlen
#include<sys/socket.h>
#include<arpa/inet.h>                   //inet_addr
#include<unistd.h>                      //write
#include<pthread.h>                     //for threading , link with lpthread

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

//the thread function
void *connection_handler( void * );


int main( int argc, char* argv[] )
{
    //
    // INITIALIZE LCD
    //
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
    server.sin_port = htons( 9001 );

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");

    //Listen
    listen(socket_desc , 3);

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);


    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t* restrict)&c) ) )
    {
        puts("Connection accepted");

        pthread_t sniffer_thread;
        new_sock = malloc(1);
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
    //Get the socket descriptor
    int sock = *( int* )socket_desc;
    int read_size;
    char *message , client_message[2000];

    //Send some messages to the client
    message = "Greetings! I am your connection handler\n";
    write( sock , message , strlen(message) );

    message = "Now type something and i shall repeat what you type \n";
    write( sock , message , strlen(message) );

    //Receive a message from client
    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
    {
        //Send the message back to client
        write(sock , client_message , strlen(client_message));

        //
        /* This must be integrated
        //
        if ( argc > 1 )
        {
            std::string arg = argv[1];
            if ( ( arg == "-h" ) || ( arg == "--help" ) )
                show_usage(argv[0]); // Show help

            else if ( ( arg == "-c" ) || ( arg == "--clear" ) )
                lcd->clear(); // Clear LCD - set cursor position to zero

            else if ( ( arg == "-b" ) || ( arg == "--backliteOff" ) )
                lcd->noBacklight();

            else if ( ( arg == "-B" ) || ( arg == "--backliteOn" ) )
                lcd->backlight();

            else if ( ( arg == "-sl" ) || ( arg == "--scrolleft" ) )
                lcd->scrollDisplayLeft();

            else if ( ( arg == "-sr" ) || ( arg == "--scrolright" ) )
                lcd->scrollDisplayRight();

            else if ( ( arg == "-w" ) || ( arg == "--write" ) )
            {
                if ( argv[2] )
                {
                    lcd->print( argv[2] );
                }
                else
                {
                    show_usage(argv[0]); // Show help - no String
                    return 1;
                }

                if ( argv[3] )
                {
                    lcd->setCursor( 0,1 ); // Position LCD cursor to start of second row
                    lcd->print( argv[3] ); // Output second input parameter to second row of LCD
                }

                // Let's play a bit
                sleep(5);
                lcd->noBacklight();
                sleep(5);
                lcd->backlight();
                sleep(5);
                lcd->scrollDisplayRight();

            }
        }
        else
        {
            show_usage( argv[0] );
            return 0;
        }

         */
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