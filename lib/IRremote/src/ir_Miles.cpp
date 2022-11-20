/*
 Assuming the protocol we are adding is for the (imaginary) manufacturer:  Miles

 Our fantasy protocol is a standard protocol, so we can use this standard
 template without too much work. Some protocols are quite unique and will require
 considerably more work in this file! It is way beyond the scope of this text to
 explain how to reverse engineer "unusual" IR protocols. But, unless you own an
 oscilloscope, the starting point is probably to use the rawDump.ino sketch and
 try to spot the pattern!

 Before you start, make sure the IR library is working OK:
 # Open up the Arduino IDE
 # Load up the rawDump.ino example sketch
 # Run it

 Now we can start to add our new protocol...

 1. Copy this file to : ir_Miles.cpp

 2. Replace all occurrences of "Miles" with the name of your protocol.

 3. Tweak the #defines to suit your protocol.

 4. If you're lucky, tweaking the #defines will make the default send() function
 work.

 5. Again, if you're lucky, tweaking the #defines will have made the default
 decode() function work.

 You have written the code to support your new protocol!

 Now you must do a few things to add it to the IRremote system:

 1. Open IRremote.h and make the following change:
 REMEMBER to change occurrences of "MILES" with the name of your protocol
 At the top, in the section "Supported Protocols", add:
 #define DECODE_MILES  1
 #define SEND_MILES    1

 2. Open IRProtocol.h and make the following change:
 In the section "An enum consisting of all supported formats", add:
 MILES,
 to the end of the list (notice there is a comma after the protocol name)

 3. Open IRremoteInt.h and make the following changes:
 A. Further down in "Main class for receiving IR", add:
 //......................................................................
 #if DECODE_MILES
 bool  decodeMiles () ;
 #endif

 B. Further down in "Main class for sending IR", add:
 //......................................................................
 #if SEND_MILES
 void  sendMilesStandard (uint16_t aAddress, uint8_t aCommand, uint_fast8_t aNumberOfRepeats) ;
 #endif

 4. Save your changes and close the files

 5. Now open IRReceive.cpp.h and make the following change:

 A. In the function IRrecv::decode(), add:
 #ifdef DECODE_MILES
 DEBUG_PRINTLN("Attempting Miles decode");
 if (decodeMiles())  return true ;
 #endif

 B. In the function getProtocolString(), add
 case MILES:
 return ("Miles");
 break;

 C. Save your changes and close the file

 6. Now open the Arduino IDE, load up the rawDump.ino sketch, and run it.
 Hopefully it will compile and upload.
 If it doesn't, you've done something wrong. Check your work.
 If you can't get it to work - seek help from somewhere.

 If you get this far, I will assume you have successfully added your new protocol
 There is one last thing to do.

 7. Delete this giant instructional comment.

 8. Send a copy of your work to us so we can include it in the library and
 others may benefit from your hard work and maybe even write a song about how
 great you are for helping them! :)

 Regards,
 BlueChip
 */

/*
 * ir_Miles.cpp
 *
 *  Contains functions for receiving and sending Miles IR Protocol ...
 *
 *  Copyright (C) 2021  Miles Guru
 *  shuzu.guru@gmail.com
 *
 *  This file is part of Arduino-IRremote https://github.com/Arduino-IRremote/Arduino-IRremote.
 *
 ************************************************************************************
 * MIT License
 *
 * Copyright (c) 2017-2021 Unknown Contributor :-)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 ************************************************************************************
 */
#include <Arduino.h>

//#define DEBUG // Activate this for lots of lovely debug output from this decoder.
#include "IRremoteInt.h" // evaluates the DEBUG for DEBUG_PRINT

//#define SEND_MILES  1 // for testing
//#define DECODE_MILES  1 // for testing
//==============================================================================
//
//
//                              M I L E S
//
//
//==============================================================================
// see: https://www....

// LSB first, 1 start bit + 8 bit address + 6 bit command
#define MILES_ADDRESS_BITS      8 // 6 bit address
#define MILES_COMMAND_BITS      6 // Command

#define MILES_BITS              14 // The number of bits in the shot
#define MILES_BITS_CTRL         24
#define MILES_UNIT              600

#define MILES_HEADER_MARK       (4 * MILES_UNIT) // 2400
#define MILES_ONE_MARK          (2 * MILES_UNIT)
#define MILES_ZERO_MARK         MILES_UNIT
#define MILES_SPACE             MILES_UNIT

#define MILES_REPEAT_PERIOD     45000


//+=============================================================================
//
void IRsend::sendMiles(unsigned long data, int nbits) {
    enableIROut(56);

    mark(MILES_HEADER_MARK);
    space(MILES_SPACE);

    sendPulseDistanceWidthData(MILES_ONE_MARK, MILES_SPACE, MILES_ZERO_MARK, MILES_SPACE, data, nbits, PROTOCOL_IS_MSB_FIRST);
}

//+=============================================================================
//
/*
 * First check for right data length
 * Next check start bit
 * Next try the decode
 * Last check stop bit
 */
bool IRrecv::decodeMiles() {

    // Check header
    if (!matchMark(decodedIRData.rawDataPtr->rawbuf[1], MILES_HEADER_MARK)) { return false; }
    bool shot = false;
    bool ctrl = false;
    // Check packet size
    if (decodedIRData.rawDataPtr->rawlen == (2*MILES_BITS)+2){ shot = true; }
    if (decodedIRData.rawDataPtr->rawlen == (2*MILES_BITS_CTRL)+2){ ctrl = true; }
    if (!shot && !ctrl){
        DEBUG_PRINT("Miles: ");
        DEBUG_PRINT("Data length=");
        DEBUG_PRINT(decodedIRData.rawDataPtr->rawlen);
        DEBUG_PRINTLN(" is not shot (14) or ctrl (24)");
        return false;
    }

    // Check header "space"
    if(!matchSpace(decodedIRData.rawDataPtr->rawbuf[2], MILES_SPACE)) {
        DEBUG_PRINT("Miles: ");
        DEBUG_PRINTLN("Header space is wrong");
        return false;
    }

    //Decoding
    if (!decodePulseWidthData((decodedIRData.rawDataPtr->rawlen - 1) / 2, 3, MILES_ONE_MARK, MILES_ZERO_MARK, MILES_SPACE, PROTOCOL_IS_LSB_FIRST)) {
        DEBUG_PRINT("Miles: ");
        DEBUG_PRINTLN("Decode failed");
        return false;
    }

    uint8_t tCommand;
    uint16_t tAddress;

    if(shot){
        tCommand = decodedIRData.decodedRawData & 0xFF; // First 6 bits
        tAddress = decodedIRData.decodedRawData >> 8; //next 8 bits
    } else if (ctrl){
        tCommand = decodedIRData.decodedRawData >> 8 & 0xFF; // First 6 bits
        tAddress = decodedIRData.decodedRawData >> 16; //next 8 bits
    }
    decodedIRData.command = tCommand;
    decodedIRData.address = tAddress;
    decodedIRData.numberOfBits = (decodedIRData.rawDataPtr->rawlen-1)/2;
    decodedIRData.protocol = MILES;

    return true;
}
