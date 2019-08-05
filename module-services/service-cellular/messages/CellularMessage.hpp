/*
 *  @file CellularMessage.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 03.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_CELLULARMESSAGE_HPP
#define PUREPHONE_CELLULARMESSAGE_HPP

#include <memory>
#include <variant>
#include "Service/Message.hpp"
#include "MessageType.hpp"


class CellularMessage : public sys::DataMessage {
public:
    CellularMessage(MessageType messageType) : sys::DataMessage(static_cast<uint32_t>(messageType)),
                                               type(messageType) {};

    virtual ~CellularMessage() {};

    MessageType type;

};

class CellularNotificationMessage : public CellularMessage {
public:

    enum class Type {
        IncomingCall, 			//device receives connection from other device.
        CallAborted,			//user
        CallBusy,				//user tried to call other device but network returned that it is unable to set connection.
        CallActive,				//call is in progress both if call was initialized by user and when user received incoming call.
		//Ringing				//user provided number to call to and service initialized calling procedure.
        NewIncomingSMS,			//device received new sms from network. (what about sms delivery reports?).
        SignalStrengthUpdate,	//update of the strength of the network's signal.
        ServiceReady,			//Idle state of the service. This is a start state before any call is initialized by user or by network.
								//service returns to this state when call is finished.
    };


    CellularNotificationMessage(Type type) : CellularMessage(
            MessageType::CellularNotification), type(type){}

    ~CellularNotificationMessage() {}

    Type type;
    std::string data;
    uint32_t signalStrength=0;
    int32_t dBmSignalStrength=0;

};

class CellularRequestMessage : public CellularMessage{
public:

    CellularRequestMessage(MessageType messageType):CellularMessage(messageType){}
    ~CellularRequestMessage() {}

    std::string data;

};

class CellularResponseMessage: public sys::ResponseMessage {
public:
    CellularResponseMessage(uint32_t retCode) : sys::ResponseMessage(),retCode(retCode) {};
    virtual ~CellularResponseMessage() {};

    uint32_t retCode;
};


#endif //PUREPHONE_CELLULARMESSAGE_HPP
