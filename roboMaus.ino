#include "src/io.h"
#include "src/macros.h"

#include "src/debounceClass.h"
#include "src/XpressNetMaster.h"
#include "src/event.h"
#include "src/ServoSweep.h"


/***** TO TEST ****
 * the buttons and leds to verivy basic controls
*/

// construct objects
ServoSweep point( servoPin1, 85, 95, 20, 1) ; // 85/95 degrees, every 20ms, shift degree, Turn off after reaching end
XpressNetMasterClass Xnet ;

Debounce detector1( detector1pin ) ;
Debounce detector2( detector2pin ) ;
Debounce play(      playPin      ) ;
Debounce stop(      stopPin      ) ;
Debounce record(    recordPin    ) ;

EventHandler eventHandler( 0, 1024 ) ; 

// constants
enum events
{
    accessoryEvent = 3,       // 0,1,2 are used for feedback, start and stop
    speedEvent,
    F0_F4Event,
    F5_F8Event,
    F9_F12Event,
    F13_F20Event,
} ;


// CALLBACK FUNCTIONS

void notifyEvent( uint8 type, uint16 address, uint8 data )                            // CALL BACK FUNCTION FROM EVENT.CPP
{
    switch( type )
    {
    // DEFAULT EVENTS
    case START:                          break ;
    case STOP:            setLed(0,0,0); break ;
    case STOP_RECORDING:  setLed(0,0,0); break ;
    case START_RECORDING: setLed(0,0,1); break ;
    case START_PLAYING:   setLed(1,0,0); break ;
    case STOP_PLAYING:    setLed(0,0,0); break ;
    case FINISHING:       setLed(0,1,0); break ;
    case RESTARTING:                     break ;

    // CUSTOM EVENTS  
    case speedEvent:      Xnet.setSpeed(      address, Loco128, data ) ; break ; 
    case F0_F4Event:      Xnet.setFunc0to4(   address, data    ) ;       break ;
    case F5_F8Event:      Xnet.setFunc5to8(   address, data    ) ;       break ;
    case F9_F12Event:     Xnet.setFunc9to12(  address, data    ) ;       break ;
    case F13_F20Event:    Xnet.setFunc13to20( address, data    ) ;       break ;
    case accessoryEvent:  Xnet.SetTrntPos(    address, data, 1 ) ;
                          POINT_DELAY( 20 ) ;
                          Xnet.SetTrntPos(    address, data, 0 ) ;       break ;
    }
}


void notifyXNetLocoFunc1( uint16_t Address, uint8_t Func1 ) { eventHandler.storeEvent(   F0_F4Event, Address, Func1 ) ; }
void notifyXNetLocoFunc2( uint16_t Address, uint8_t Func2 ) { eventHandler.storeEvent(   F5_F8Event, Address, Func2 ) ; }
void notifyXNetLocoFunc3( uint16_t Address, uint8_t Func3 ) { eventHandler.storeEvent(  F9_F12Event, Address, Func3 ) ; }
void notifyXNetLocoFunc4( uint16_t Address, uint8_t Func4 ) { eventHandler.storeEvent( F13_F20Event, Address, Func4 ) ; }

void notifyXNetLocoDrive128( uint16_t Address, uint8_t Speed )                   
{
    eventHandler.storeEvent( speedEvent, Address, Speed ) ;
}

void notifyXNetTrnt(uint16_t Address, uint8_t data) 
{
    if( bitRead(data,3) == 1 )
    { 
        data &= 0x1 ;
        if( Address == 1 )                                                      // point 1 or 2, I thought 1?
        {
            uint16 sample = analogRead( lowPosPin ) ;                           // update lower and upper positions before setting the point PERHAPS MOVE TO SETUP
            sample = map( sample, 0, 1023, 0 , 180 ) ;
            point.setMin(sample) ;
            
            sample = analogRead( highPosPin ) ;
            sample = map( sample, 0, 1023, 0 , 180 ) ;
            point.setMax(sample) ;
            
            point.setState( data ) ;                                            // physically move servo motor
        }

        eventHandler.storeEvent( accessoryEvent, Address, data ) ;
    }   
}


// FUNCTIONS
void debounceInputs()
{
    REPEAT_MS( 500 )                                                            // debounce detectors every 500ms
    {
        detector1.debounce() ;
        detector2.debounce() ;
    }
    END_REPEAT
    
    REPEAT_MS( 20 )                                                             // debounce buttons every 20ms
    {
        play.debounce() ;
        stop.debounce() ;
        record.debounce() ;
    }
    END_REPEAT
}

void processInputs()
{
    uint8 state = detector1.getState() ;
    if( state == FALLING || state == RISING )
    {
        eventHandler.storeEvent( FEEDBACK, 50000, 1  ) ;
        eventHandler.sendFeedbackEvent( 50000 ) ;
    }

    state = detector2.getState() ;
    if( state == FALLING || state == RISING )
    {
        eventHandler.storeEvent( FEEDBACK, 50001, 1  ) ;
        eventHandler.sendFeedbackEvent( 50001 ) ;
    }

    if(   play.getState() == FALLING ) { eventHandler.startPlaying()   ; }
    if(   stop.getState() == FALLING ) { eventHandler.stopRecording()  ; 
                                         eventHandler.stopPlaying()    ; }  
    if( record.getState() == FALLING ) { eventHandler.startRecording() ; }
}

// SETUP
void setup()
{
    initIO() ;
    Xnet.setup( Loco128,  2) ;
    point.begin() ;
}

// MAIN LOOP
void loop()
{
    debounceInputs() ;
    processInputs() ;

    eventHandler.update() ;
    point.sweep() ;
    Xnet.update() ;
}