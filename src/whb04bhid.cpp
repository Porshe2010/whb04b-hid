#include <iostream>
#include <iterator>
#include <cerrno>
#include <cstdio>
#include <algorithm>
#include <iomanip>
#include <stdlib.h>
#include <math.h>

#include <hidapi.h>
#include <key.hxx>

#include <whb04bhid.hpp>

//#define DEBUG

WHB04B::WHB04B() : 
    writer(WonderRabbitProject::key::writer_t::instance())
{
}
WHB04B::~WHB04B() 
{ 
}

void WHB04B::updateAxis(Axis axis)
{
    switch(axis)
    {
        case Axis::AXIS_X:
            currAxis = axis;
            break;
        case Axis::AXIS_Y:
            currAxis = axis;
            break;
        case Axis::AXIS_Z:
            currAxis = axis;
            break;
        default:
            currAxis = Axis::AXIS_OFF;
            break;
    }
#if defined(DEBUG)
    //std::cout << "AXIS: " << axisName(currAxis) << std::endl;
#endif
}

void WHB04B::updateStep(Step step)
{
    currStep=step;
    switch(step)
    {
        case Step::STEP_0_001:
            writer(CC_025);
            break;
        case Step::STEP_0_01:
            writer(CC_25);
            break;
        case Step::STEP_0_1:
            writer(CC_1);
            break;
        default:
            writer(CC_FAST);
    }
#if defined(DEBUG)
    //std::cout << "STEP: " << stepName(currStep) << std::endl;
#endif
}

void WHB04B::handleButtons(unsigned char key,unsigned char mod)
{
#if defined(DEBUG)
    //std::cout << "KEY: " << keyName(key,mod) << std::endl;
#endif
}

void WHB04B::handleJog(char count)
{
    uint16_t localCount = count;
    const char *symbol;
    if( count < 0)
    {
        localCount = -count;
    }

    switch(currAxis)
    {
        case Axis::AXIS_X:
            symbol = (count<0)?CC_X_DOWN:CC_X_UP;
            break;
        case Axis::AXIS_Y:
            symbol = (count<0)?CC_Y_DOWN:CC_Y_UP;
            break;
        case Axis::AXIS_Z:
            symbol = (count<0)?CC_Z_DOWN:CC_Z_UP;
            break;
        default:
            // Do nothing
            return;
    }
    for( auto i = 0; i < localCount; ++i)
    {
        writer(symbol);
    }
}

void WHB04B::handleHIDBuffer(unsigned char buf [], size_t len)
{
#if defined(DEBUG)
    for (auto i = 0; i < len; i++)
    {
        std::cout << "0x" << PADHEX(2,buf[i]) << " ";
    }
    std::cout << std::endl;
#endif

    // Update feed rate
    if( static_cast<Step>(buf[4]) != currStep )
    {
        updateStep(static_cast<Step>(buf[4]));
    }
    // Update axis
    if( static_cast<Axis>(buf[5]) != currAxis )
    {
        updateAxis(static_cast<Axis>(buf[5]));
    }

    // Handle button press
    unsigned char key = buf[2];
    unsigned char mod = buf[3];
    if(mod)
    {
        mod=buf[2];
        key=buf[3];
    }
    handleButtons(key,mod);

    // Handle jog
    if( buf[6] > 0 )
    {
        handleJog((char)buf[6]);
    }

}

const char *WHB04B::axisName(Axis axis)
{
    switch(axis)
    {
        case Axis::AXIS_OFF: return "AXIS_OFF";
        case Axis::AXIS_X: return "AXIS_X";
        case Axis::AXIS_Y: return "AXIS_Y";
        case Axis::AXIS_Z: return "AXIS_Z";
        case Axis::AXIS_A: return "AXIS_A";
    }
    return "NO AXIS";
}

const char *WHB04B::stepName(Step step)
{
    switch(step)
    {
        case Step::STEP_0_001: return "STEP_0_001";
        case Step::STEP_0_01: return "STEP_0_01";
        case Step::STEP_0_1: return "STEP_0_1";
        case Step::STEP_1: return "STEP_1";
        case Step::STEP_60: return "STEP_60";
        case Step::STEP_100: return "STEP_100";
        case Step::STEP_LOAD: return "STEP_LOAD";
    }
    return "NO STEP";
}

const char *WHB04B::keyName(char key, char mod)
{
    if( mod == FN )
    {
        switch(key)
        {
            case MACRO_1: return "MACRO_1";
            case MACRO_2: return "MACRO_2";
            case MACRO_3: return "MACRO_3";
            case MACRO_4: return "MACRO_4";
            case MACRO_5: return "MACRO_5";
            case MACRO_6: return "MACRO_6";
            case MACRO_7: return "MACRO_7";
            case MACRO_8: return "MACRO_8";
            case MACRO_9: return "MACRO_9";
            case MACRO_10: return "MACRO_10";
        }
    }
    else {
        switch(key)
        {
            case RESET: return "RESET" ;
            case STOP: return "STOP" ;
            case START_PAUSE: return "START_PAUSE" ;
            case FEED_UP: return "FEED_UP" ;
            case FEED_DOWN: return "FEED_DOWN" ;
            case SPINDLE_UP: return "SPINDLE_UP" ;
            case SPINDLE_DOWN: return "SPINDLE_DOWN" ;
            case M_HOME: return "M_HOME" ;
            case SAFE_Z: return "SAFE_Z" ;
            case W_HOME: return "W_HOME" ;
            case S_ON_OFF: return "S_ON_OFF" ;
            case FN: return "FN" ;
            case PROBE_Z: return "PROBE_Z" ;
            case CONTINUOUS: return "CONTINUOUS" ;
            case STEP: return "STEP" ;
            case MACRO_10: return "MACRO_10" ;
        }
    }
    return "NO KEY";
}
