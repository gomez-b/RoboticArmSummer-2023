/*
 * In this project we used MCC to configure UART1 module. 
 * The main.c include several simple calls in which we can send characters or
 * receive characters. 
 *  
 *
 * Author: Pablo Orozco
 */

#include "mcc_generated_files/system/system.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <xc.h>
#include "esp8266.h"
#include "uart1.h"
#include <pic18f46k42.h>



#define _XTAL_FREQ 1000000    // Internal oscillator frequency (1MHz)

// UART initialization
void UART1_Init(unsigned long baud_rate) {
    TRISCbits.TRISC6 = 0;   // TX pin as output
    TRISCbits.TRISC7 = 1;   // RX pin as input
    
    U1TXEN = 1;    // Enable transmission

    // Calculate SPBRG value for desired baud rate
    U1BRGL = (unsigned char)((_XTAL_FREQ / (baud_rate * 16)) - 1);
}

// Send a character via UART1
void UART1_WriteChar(char data) {
    while (U1TXIF == 0); // Wait until transmit buffer is empty
    U1TXB = data;   // Send data using UART1
}

// Send a string via UART1
void UART1_WriteString(const char* data) {
    while (*data) {
        UART1_WriteChar(*data);
        data++;
    }
}

// Send data to ThingSpeak using ESP8266 module
void SendDataToThingSpeak(int Value) {
    // Initialize UART1 with a baud rate of 9600
    UART1_Init(9600);
    
    // Set ESP8266 to Station mode
    UART1_WriteString("AT+CWMODE=1\r\n");
    __delay_ms(1000);

    // Replace with your Wi-Fi credentials
    UART1_WriteString("AT+CWJAP=\"Orozco\",\"FamiliaO\"\r\n");
    __delay_ms(5000); // Adjust the delay as needed

    // Construct the HTTP GET request with your API key and data
    char command[100];
    sprintf(command,"AT+CIPSTART=\"TCP\",\"54.81.196.77\",80\r\n");
    UART1_WriteString(command);
    __delay_ms(2000);

    sprintf(command, "AT+CIPSEND=%d\r\n", (int)strlen("GET /update?api_key=H3GJUSGAROEX3JFI&field1=%d\r\n"));
    UART1_WriteString(command);
    __delay_ms(1000);

    sprintf(command, "GET /update?api_key=H3GJUSGAROEX3JFI&field1=%d\r\n", Value);
    UART1_WriteString(command);
    __delay_ms(1000);

    UART1_WriteString("AT+CIPCLOSE\r\n");
}
//Generates a random number to send to ThingsSpeak. 
int main() {
    srand((unsigned int)_XTAL_FREQ);

    while (1) {
        int Value = rand() % 101;
        SendDataToThingSpeak(Value);
        __delay_ms(10000);
    }

    return 0;
}
