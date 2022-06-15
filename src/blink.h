#include <Arduino.h>
#include "macros.h"

enum blinkStates
{
    ON,
    OFF,
    BLINKING,
    FLASHING,
} ;

class Blink
{

public:
    Blink( uint8 ) ;
    void    blink( uint32 ) ;
    void    flash( uint8 ) ;
    void    set( uint8 )  ;

    void    update() ;

private:
    void    toggle() ;
    void    flash() ;

    uint32  prevTime ;
    uint32  blinkInterval ;
    uint8   blinkCounter ;
    uint8   pin ;
    uint8   blinkTimes ;
    uint8   counter ;
    uint8   setCounter ;
    uint8   mode ;
} ;

