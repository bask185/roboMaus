#include "blink.h"


Blink::Blink( uint8 _pin )
{
    pin = _pin ;
    pinMode( pin, OUTPUT ) ;
}

void Blink::toggle() 
{
    digitalWrite( pin, !digitalRead( pin ) ) ;
}

void Blink::flash()
{
    if( digitalRead( pin ) )                                                    
    {
        digitalWrite( pin, LOW ) ;
        blinkInterval = 600 ;
    }
    else
    {
        digitalWrite( pin, HIGH ) ;
        
        blinkInterval = 200 ;
    }

    if( counter++ == (2*setCounter) ) mode = OFF ;
}


void Blink::blink( uint32 _blinkInterval )                                  // continously
{
    mode = BLINKING ;
    
    blinkInterval = _blinkInterval ;
}

void Blink::flash( uint8 times )                                                    // x amount time
{
    mode = FLASHING ;
    counter = 0 ;
    setCounter = times ;
}

void Blink::set( uint8 _mode )                                                               // on or off
{
    mode = _mode ;
}

void Blink::update()
{
    uint32 currTime = millis() ;
    if( currTime - prevTime >= blinkInterval )
    {   prevTime = currTime ;

        switch( mode )
        {
        case ON :       digitalWrite( pin, HIGH ) ; break ;
        case OFF :      digitalWrite( pin,  LOW ) ; break ;
        case BLINKING : toggle() ;                  break ;
        case FLASHING : flash()  ;                  break ;
        }
    }
}
