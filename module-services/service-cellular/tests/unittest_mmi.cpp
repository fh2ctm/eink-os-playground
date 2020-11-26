// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include <service-cellular/RequestFactory.hpp>
#include <service-cellular/requests/CallForwardingRequest.hpp>
#include <service-cellular/requests/CallWaitingRequest.hpp>
#include <service-cellular/requests/CallBarringRequest.hpp>
#include <service-cellular/requests/PasswordRegistrationRequest.hpp>
#include <service-cellular/requests/PinChangeRequest.hpp>
#include <service-cellular/requests/ImeiRequest.hpp>
#include <service-cellular/requests/UssdRequest.hpp>
#include <service-cellular/requests/ClipRequest.hpp>
#include <service-cellular/requests/ClirRequest.hpp>
#include <service-cellular/requests/ColpRequest.hpp>

using namespace cellular;

TEST_CASE("MMI requests")
{
    struct TestCase
    {
        const std::string requestString;
        const std::string expectedCommandString;
        const std::type_info &expectedType;
        const bool expectedValid = true;
    };

    std::vector<TestCase> testCases = {
        /// USSD
        {R"(*100*#)", R"(AT+CUSD=1,*100*#,15)", typeid(UssdRequest)},

        /// ImeiRequest
        {R"(*#06#)", R"(AT+GSN)", typeid(ImeiRequest)},

        /// ClipRequest
        // bad procedure type
        {R"(*30#)", std::string(), typeid(ClipRequest), false},
        // query
        {R"(*#30#)", R"(AT+CLIP?)", typeid(ClipRequest)},
        // bad procedure type
        {R"(##30#)", std::string(), typeid(ClipRequest), false},
        // bad procedure type
        {R"(#30#)", std::string(), typeid(ClipRequest), false},
        // bad procedure type
        {R"(**30#)", std::string(), typeid(ClipRequest), false},
        // temporary mode enable
        {R"(*30#600700800)", std::string(), typeid(CallRequest)},
        // temporary mode disable
        {R"(#30#600700800)", std::string(), typeid(CallRequest)},

        /// ClirRequest
        // bad procedure type
        {R"(*31#)", R"(AT+CLIR=1)", typeid(ClirRequest)},
        // query
        {R"(*#31#)", R"(AT+CLIR?)", typeid(ClirRequest)},
        // bad procedure type
        {R"(##31#)", std::string(), typeid(ClirRequest), false},
        // bad procedure type
        {R"(#31#)", R"(AT+CLIR=2)", typeid(ClirRequest)},
        // bad procedure type
        {R"(**31#)", std::string(), typeid(ClirRequest), false},

        /// ColpRequest
        // bad procedure type
        {R"(*76#)", R"(AT+COLP=1)", typeid(ColpRequest)},
        // query
        {R"(*#76#)", R"(AT+COLP?)", typeid(ColpRequest)},
        // bad procedure type
        {R"(##76#)", std::string(), typeid(ColpRequest), false},
        // bad procedure type
        {R"(#76#)", R"(AT+COLP=0)", typeid(ColpRequest)},
        // bad procedure type
        {R"(**76#)", std::string(), typeid(ColpRequest), false},

        /// CallBarringRequest
        // BAOC (Bar All Outgoing Calls)
        {R"(*33#)", R"(AT+CLCK="AO",1)", typeid(CallBarringRequest)},                    // lock
        {R"(*33*1111#)", R"(AT+CLCK="AO",1,"1111")", typeid(CallBarringRequest)},        // lock with pass
        {R"(*33*1111*10#)", R"(AT+CLCK="AO",1,"1111",13)", typeid(CallBarringRequest)},  // lock with pass and BS
        {R"(#33#)", R"(AT+CLCK="AO",0)", typeid(CallBarringRequest)},                    // unlock
        {R"(#33*1111#)", R"(AT+CLCK="AO",0,"1111")", typeid(CallBarringRequest)},        // unlock with pass
        {R"(#33*1111*11#)", R"(AT+CLCK="AO",0,"1111",1)", typeid(CallBarringRequest)},   // unlock with pass and BS
        {R"(*#33#)", R"(AT+CLCK="AO",2)", typeid(CallBarringRequest)},                   // query
        {R"(*#33*1111#)", R"(AT+CLCK="AO",2,"1111")", typeid(CallBarringRequest)},       // query with pass
        {R"(*#33*1111*12#)", R"(AT+CLCK="AO",2,"1111",12)", typeid(CallBarringRequest)}, // query with pass and BS
        {R"(**33#)", std::string(), typeid(CallBarringRequest), false},                  // bad procedure - register
        {R"(##33#)", std::string(), typeid(CallBarringRequest), false},                  // bad procedure - erasure
        {R"(*#33*1111*17#)", std::string(), typeid(CallBarringRequest), false}, // unsupported BS - Voice Group Call
        {R"(*#33*1111*18#)", std::string(), typeid(CallBarringRequest), false}, // unsupported BS - Voice Broadcast
        {R"(*#33*1111*99#)", std::string(), typeid(CallBarringRequest), false}, // unsupported BS - All GPRS bearer
        {R"(*#33*1111*45#)", std::string(), typeid(CallBarringRequest), false}, // unsupported BS - random
        /// BOIC (Bar Outgoing International Calls)
        {R"(*331#)", R"(AT+CLCK="OI",1)", typeid(CallBarringRequest)},                   // lock
        {R"(*331*2222#)", R"(AT+CLCK="OI",1,"2222")", typeid(CallBarringRequest)},       // lock with pass
        {R"(*331*2222*13#)", R"(AT+CLCK="OI",1,"2222",4)", typeid(CallBarringRequest)},  // lock with pass and BS
        {R"(#331#)", R"(AT+CLCK="OI",0)", typeid(CallBarringRequest)},                   // unlock
        {R"(#331*2222#)", R"(AT+CLCK="OI",0,"2222")", typeid(CallBarringRequest)},       // unlock with pass
        {R"(#331*2222*16#)", R"(AT+CLCK="OI",0,"2222",8)", typeid(CallBarringRequest)},  // unlock with pass and BS
        {R"(*#331#)", R"(AT+CLCK="OI",2)", typeid(CallBarringRequest)},                  // query
        {R"(*#331*2222#)", R"(AT+CLCK="OI",2,"2222")", typeid(CallBarringRequest)},      // query with pass
        {R"(*#331*2222*19#)", R"(AT+CLCK="OI",2,"2222",5)", typeid(CallBarringRequest)}, // query with pass and BS
        {R"(**331#)", std::string(), typeid(CallBarringRequest), false},                 // bad procedure - register
        {R"(##331#)", std::string(), typeid(CallBarringRequest), false},                 // bad procedure - erasure
        /// BOIC-exHC (Bar Outgoing International Calls except to home country)
        {R"(*332#)", R"(AT+CLCK="OX",1)", typeid(CallBarringRequest)},                    // lock
        {R"(*332*3333#)", R"(AT+CLCK="OX",1,"3333")", typeid(CallBarringRequest)},        // lock
        {R"(*332*2222*20#)", R"(AT+CLCK="OX",1,"2222",50)", typeid(CallBarringRequest)},  // lock with pass
        {R"(#332#)", R"(AT+CLCK="OX",0)", typeid(CallBarringRequest)},                    // unlock with pass and BS
        {R"(#332*3333#)", R"(AT+CLCK="OX",0,"3333")", typeid(CallBarringRequest)},        // unlock
        {R"(#332*2222*21#)", R"(AT+CLCK="OX",0,"2222",32)", typeid(CallBarringRequest)},  // unlock with pass
        {R"(*#332#)", R"(AT+CLCK="OX",2)", typeid(CallBarringRequest)},                   // query with pass and BS
        {R"(*#332*3333#)", R"(AT+CLCK="OX",2,"3333")", typeid(CallBarringRequest)},       // query with pass
        {R"(*#332*2222*22#)", R"(AT+CLCK="OX",2,"2222",16)", typeid(CallBarringRequest)}, // query with pass and BS
        {R"(**332#)", std::string(), typeid(CallBarringRequest), false},                  // bad procedure - register
        {R"(##332#)", std::string(), typeid(CallBarringRequest), false},                  // bad procedure - erasure
        /// BAIC (Bar All Incoming Calls)
        {R"(*35#)", R"(AT+CLCK="AI",1)", typeid(CallBarringRequest)},                    // lock
        {R"(*35*1234#)", R"(AT+CLCK="AI",1,"1234")", typeid(CallBarringRequest)},        // lock with pass
        {R"(*35*2222*24#)", R"(AT+CLCK="AI",1,"2222",16)", typeid(CallBarringRequest)},  // lock with pass and BS
        {R"(#35#)", R"(AT+CLCK="AI",0)", typeid(CallBarringRequest)},                    // unlock
        {R"(#35*1234#)", R"(AT+CLCK="AI",0,"1234")", typeid(CallBarringRequest)},        // unlock with pass
        {R"(#35*2222*25#)", R"(AT+CLCK="AI",0,"2222",32)", typeid(CallBarringRequest)},  // unlock with pass and BS
        {R"(*#35#)", R"(AT+CLCK="AI",2)", typeid(CallBarringRequest)},                   // query
        {R"(*#35*1234#)", R"(AT+CLCK="AI",2,"1234")", typeid(CallBarringRequest)},       // query with pass
        {R"(*#35*2222*26#)", R"(AT+CLCK="AI",2,"2222",17)", typeid(CallBarringRequest)}, // query with pass and BS
        {R"(**35#)", std::string(), typeid(CallBarringRequest), false},                  // bad procedure - register
        {R"(##35#)", std::string(), typeid(CallBarringRequest), false},                  // bad procedure - erasure
        /// BIC-Roam (Bar Incoming Calls when Roaming outside the home country)
        {R"(*351#)", R"(AT+CLCK="IR",1)", typeid(CallBarringRequest)},                    // lock
        {R"(*351*1234#)", R"(AT+CLCK="IR",1,"1234")", typeid(CallBarringRequest)},        // lock with pass
        {R"(*351*2222*10#)", R"(AT+CLCK="IR",1,"2222",13)", typeid(CallBarringRequest)},  // lock with pass and BS
        {R"(#351#)", R"(AT+CLCK="IR",0)", typeid(CallBarringRequest)},                    // unlock
        {R"(#351*1234#)", R"(AT+CLCK="IR",0,"1234")", typeid(CallBarringRequest)},        // unlock with pass
        {R"(#351*2222*11#)", R"(AT+CLCK="IR",0,"2222",1)", typeid(CallBarringRequest)},   // unlock with pass and BS
        {R"(*#351#)", R"(AT+CLCK="IR",2)", typeid(CallBarringRequest)},                   // query
        {R"(*#351*1234#)", R"(AT+CLCK="IR",2,"1234")", typeid(CallBarringRequest)},       // query with pass
        {R"(*#351*2222*12#)", R"(AT+CLCK="IR",2,"2222",12)", typeid(CallBarringRequest)}, // query with pass and BS
        {R"(**351#)", std::string(), typeid(CallBarringRequest), false},                  // bad procedure - register
        {R"(##351#)", std::string(), typeid(CallBarringRequest), false},                  // bad procedure - erasure
        /// All barring services
        {R"(*330#)", R"(AT+CLCK="AB",1)", typeid(CallBarringRequest)},                   // lock
        {R"(*330*1234#)", R"(AT+CLCK="AB",1,"1234")", typeid(CallBarringRequest)},       // lock with pass
        {R"(*330*2222*13#)", R"(AT+CLCK="AB",1,"2222",4)", typeid(CallBarringRequest)},  // lock with pass and BS
        {R"(#330#)", R"(AT+CLCK="AB",0)", typeid(CallBarringRequest)},                   // unlock
        {R"(#330*1234#)", R"(AT+CLCK="AB",0,"1234")", typeid(CallBarringRequest)},       // unlock with pass
        {R"(#330*2222*16#)", R"(AT+CLCK="AB",0,"2222",8)", typeid(CallBarringRequest)},  // unlock with pass and BS
        {R"(*#330#)", R"(AT+CLCK="AB",2)", typeid(CallBarringRequest)},                  // query
        {R"(*#330*1234#)", R"(AT+CLCK="AB",2,"1234")", typeid(CallBarringRequest)},      // query with pass
        {R"(*#330*2222*19#)", R"(AT+CLCK="AB",2,"2222",5)", typeid(CallBarringRequest)}, // query with pass and BS
        {R"(**330#)", std::string(), typeid(CallBarringRequest), false},                 // bad procedure - register
        {R"(##330#)", std::string(), typeid(CallBarringRequest), false},                 // bad procedure - erasure
        /// All outgoing barring services
        {R"(*333#)", R"(AT+CLCK="AG",1)", typeid(CallBarringRequest)},                    // lock
        {R"(*333*1234#)", R"(AT+CLCK="AG",1,"1234")", typeid(CallBarringRequest)},        // lock with pass
        {R"(*333*2222*20#)", R"(AT+CLCK="AG",1,"2222",50)", typeid(CallBarringRequest)},  // lock with pass and BS
        {R"(#333#)", R"(AT+CLCK="AG",0)", typeid(CallBarringRequest)},                    // unlock
        {R"(#333*1234#)", R"(AT+CLCK="AG",0,"1234")", typeid(CallBarringRequest)},        // unlock with pass
        {R"(#333*2222*21#)", R"(AT+CLCK="AG",0,"2222",32)", typeid(CallBarringRequest)},  // unlock with pass and BS
        {R"(*#333#)", R"(AT+CLCK="AG",2)", typeid(CallBarringRequest)},                   // query
        {R"(*#333*1234#)", R"(AT+CLCK="AG",2,"1234")", typeid(CallBarringRequest)},       // query with pass
        {R"(*#333*2222*22#)", R"(AT+CLCK="AG",2,"2222",16)", typeid(CallBarringRequest)}, // query with pass and BS
        {R"(**333#)", std::string(), typeid(CallBarringRequest), false},                  // bad procedure - register
        {R"(##333#)", std::string(), typeid(CallBarringRequest), false},                  // bad procedure - erasure
        /// All incoming barring services
        {R"(*353#)", R"(AT+CLCK="AC",1)", typeid(CallBarringRequest)},                    // lock
        {R"(*353*1234#)", R"(AT+CLCK="AC",1,"1234")", typeid(CallBarringRequest)},        // lock with pass
        {R"(*353*2222*24#)", R"(AT+CLCK="AC",1,"2222",16)", typeid(CallBarringRequest)},  // lock with pass and BS
        {R"(#353#)", R"(AT+CLCK="AC",0)", typeid(CallBarringRequest)},                    // unlock
        {R"(#353*1234#)", R"(AT+CLCK="AC",0,"1234")", typeid(CallBarringRequest)},        // unlock with pass
        {R"(#353*2222*25#)", R"(AT+CLCK="AC",0,"2222",32)", typeid(CallBarringRequest)},  // unlock with pass and BS
        {R"(*#353#)", R"(AT+CLCK="AC",2)", typeid(CallBarringRequest)},                   // query
        {R"(*#353*1234#)", R"(AT+CLCK="AC",2,"1234")", typeid(CallBarringRequest)},       // query with pass
        {R"(*#353*2222*10#)", R"(AT+CLCK="AC",2,"2222",13)", typeid(CallBarringRequest)}, // query with pass and BS
        {R"(**353#)", std::string(), typeid(CallBarringRequest), false},                  // bad procedure - register
        {R"(##353#)", std::string(), typeid(CallBarringRequest), false},                  // bad procedure - erasure

        /// CallWaitingRequest
        // enable all
        {R"(*43#)", R"(AT+CCWA=1,1)", typeid(CallWaitingRequest)},
        // enable all
        {R"(*43*#)", R"(AT+CCWA=1,1)", typeid(CallWaitingRequest)},
        // enable all tele services
        {R"(*43*10#)", R"(AT+CCWA=1,1,13)", typeid(CallWaitingRequest)},
        // enable voice
        {R"(*43*11#)", R"(AT+CCWA=1,1,1)", typeid(CallWaitingRequest)},
        // enable data
        {R"(*43*12#)", R"(AT+CCWA=1,1,12)", typeid(CallWaitingRequest)},
        // enable fax
        {R"(*43*13#)", R"(AT+CCWA=1,1,4)", typeid(CallWaitingRequest)},
        // enable sms
        {R"(*43*16#)", R"(AT+CCWA=1,1,8)", typeid(CallWaitingRequest)},
        // enable all tele except sms
        {R"(*43*19#)", R"(AT+CCWA=1,1,5)", typeid(CallWaitingRequest)},
        // enable all bearer
        {R"(*43*20#)", R"(AT+CCWA=1,1,50)", typeid(CallWaitingRequest)},
        // enable data circuit sync
        {R"(*43*24#)", R"(AT+CCWA=1,1,16)", typeid(CallWaitingRequest)},
        // enable data circuit async
        {R"(*43*25#)", R"(AT+CCWA=1,1,32)", typeid(CallWaitingRequest)},
        // disable all
        {R"(#43#)", R"(AT+CCWA=1,0)", typeid(CallWaitingRequest)},
        // query all
        {R"(*#43#)", R"(AT+CCWA=1,2)", typeid(CallWaitingRequest)},
        // bad procedure **
        {R"(**43#)", std::string(), typeid(CallWaitingRequest), false},
        // bad procedure ##
        {R"(##43#)", std::string(), typeid(CallWaitingRequest), false},
        // bad service group
        {R"(*43*17#)", std::string(), typeid(CallWaitingRequest), false},
        // bad service group
        {R"(*43*17#)", std::string(), typeid(CallWaitingRequest), false},

        /// CallForwardingRequest
        // all diversions
        {R"(##002#)", R"(AT+CCFC=4,4)", typeid(CallForwardingRequest)},
        // all conditional redirections
        {R"(**004*666555444#)", R"(AT+CCFC=5,3,"666555444")", typeid(CallForwardingRequest)},
        {R"(##004#)", R"(AT+CCFC=5,4)", typeid(CallForwardingRequest)},
        // unconditional divert
        {R"(**21*666555444#)", R"(AT+CCFC=0,3,"666555444")", typeid(CallForwardingRequest)},
        {R"(*21#)", R"(AT+CCFC=0,1)", typeid(CallForwardingRequest)},
        {R"(#21#)", R"(AT+CCFC=0,0)", typeid(CallForwardingRequest)},
        {R"(##21#)", R"(AT+CCFC=0,4)", typeid(CallForwardingRequest)},
        {R"(*#21#)", R"(AT+CCFC=0,2)", typeid(CallForwardingRequest)},
        // divert when not answered
        {R"(**61*666555444#)", R"(AT+CCFC=2,3,"666555444")", typeid(CallForwardingRequest)},
        {R"(*61#)", R"(AT+CCFC=2,1)", typeid(CallForwardingRequest)},
        {R"(#61#)", R"(AT+CCFC=2,0)", typeid(CallForwardingRequest)},
        {R"(##61#)", R"(AT+CCFC=2,4)", typeid(CallForwardingRequest)},
        {R"(*#61#)", R"(AT+CCFC=2,2)", typeid(CallForwardingRequest)},
        // divert when off or not
        {R"(**62*666555444#)", R"(AT+CCFC=3,3,"666555444")", typeid(CallForwardingRequest)},
        {R"(*62#)", R"(AT+CCFC=3,1)", typeid(CallForwardingRequest)},
        {R"(#62#)", R"(AT+CCFC=3,0)", typeid(CallForwardingRequest)},
        {R"(##62#)", R"(AT+CCFC=3,4)", typeid(CallForwardingRequest)},
        {R"(*#62#)", R"(AT+CCFC=3,2)", typeid(CallForwardingRequest)},
        // divert when busy or pressing reject
        {R"(**67*666555444#)", R"(AT+CCFC=1,3,"666555444")", typeid(CallForwardingRequest)},
        {R"(*67#)", R"(AT+CCFC=1,1)", typeid(CallForwardingRequest)},
        {R"(#67#)", R"(AT+CCFC=1,0)", typeid(CallForwardingRequest)},
        {R"(##67#)", R"(AT+CCFC=1,4)", typeid(CallForwardingRequest)},
        {R"(*#67#)", R"(AT+CCFC=1,2)", typeid(CallForwardingRequest)},
        // alternative register and on
        {R"(*21*666555444#)", R"(AT+CCFC=0,1,"666555444")", typeid(CallForwardingRequest)},
        // optional parameters - basic service group
        {R"(*21*666555444*16#)", R"(AT+CCFC=0,1,"666555444",129,8)", typeid(CallForwardingRequest)},
        {R"(*21*+48666555444*19#)", R"(AT+CCFC=0,1,"+48666555444",145,5)", typeid(CallForwardingRequest)},
        // optional parameters - time
        {R"(*21*666555444*16*30#)", R"(AT+CCFC=0,1,"666555444",129,8,,,30)", typeid(CallForwardingRequest)},
        {R"(*21*+48666555444*19*20#)", R"(AT+CCFC=0,1,"+48666555444",145,5,,,20)", typeid(CallForwardingRequest)},
        // not valid - timeout exceeds maximum
        {R"(*21*+48666555444*19*40#)", std::string(), typeid(CallForwardingRequest), false},

        /// PasswordRegistrationRequest
        // total incoming and outgoing service barring (empty string)
        {R"(**03**23*111*111#)", R"(AT+CPWD="AB","23","111")", typeid(PasswordRegistrationRequest)},
        // outgoing call barring
        {R"(**03*33*1234*4321*4321#)", R"(AT+CPWD="AO","1234","4321")", typeid(PasswordRegistrationRequest)},
        // outgoing international call barring
        {R"(**03*331*1234*4321*4321#)", R"(AT+CPWD="OI","1234","4321")", typeid(PasswordRegistrationRequest)},
        // outgoing international call barring, excluding to home
        {R"(**03*332*1234*4321*4321#)", R"(AT+CPWD="OX","1234","4321")", typeid(PasswordRegistrationRequest)},
        // incoming call barring
        {R"(**03*35*1234*4321*4321#)", R"(AT+CPWD="AI","1234","4321")", typeid(PasswordRegistrationRequest)},
        // incoming call barring, when international roaming
        {R"(**03*351*1234*4321*4321#)", R"(AT+CPWD="IR","1234","4321")", typeid(PasswordRegistrationRequest)},
        // total incoming and outgoing service barring
        {R"(**03*330*1234*4321*4321#)", R"(AT+CPWD="AB","1234","4321")", typeid(PasswordRegistrationRequest)},
        // total outgoing service barring
        {R"(**03*333*1234*4321*4321#)", R"(AT+CPWD="AG","1234","4321")", typeid(PasswordRegistrationRequest)},
        // total incoming service barring
        {R"(**03*353*1234*4321*4321#)", R"(AT+CPWD="AC","1234","4321")", typeid(PasswordRegistrationRequest)},
        // alternative procedure type *
        {R"(*03*353*1234*4321*4321#)", R"(AT+CPWD="AC","1234","4321")", typeid(PasswordRegistrationRequest)},
        // bad procedure type * fallback to UUSD
        {R"(*#03*353*1234*4321*4321#)", R"(AT+CUSD=1,*#03*353*1234*4321*4321#,15)", typeid(UssdRequest)},
        // bad procedure type ##
        {R"(##03*353*1234*4321*4321#)", std::string(), typeid(CallRequest)},
        // bad procedure type #
        {R"(#03*353*1234*4321*4321#)", std::string(), typeid(CallRequest)},
        // no password
        {R"(**03*353***#)", std::string(), typeid(PasswordRegistrationRequest), false},
        // no password
        {R"(**03*353*24**#)", std::string(), typeid(PasswordRegistrationRequest), false},
        // no password
        {R"(**03*353**34*#)", std::string(), typeid(PasswordRegistrationRequest), false},
        // no password
        {R"(**03*353***34#)", std::string(), typeid(PasswordRegistrationRequest), false},
        // password does not match
        {R"(**03*353*56*46*74#)", std::string(), typeid(PasswordRegistrationRequest), false},

        /// PinChangeRequest
        // Change PIN
        {R"(**04*0000*1111*1111#)", R"(AT+CPWD="SC","0000","1111")", typeid(PinChangeRequest)},
        // Change PIN2
        {R"(**042*0000*1111*1111#)", R"(AT+CPWD="P2","0000","1111")", typeid(PinChangeRequest)},
        // Change PIN by PUK
        {R"(**05*0000*1111*1111#)", R"(AT+CPIN="0000","1111")", typeid(PinChangeRequest)},
        // Change PIN by PUK more than 4
        {R"(**042*00002*11113*11113#)", R"(AT+CPWD="P2","00002","11113")", typeid(PinChangeRequest)},
        // bad procedure type *
        {R"(*042*00002*11112*11112#)", std::string(), typeid(UssdRequest)},
        // bad procedure type ##
        {R"(##042*00002*11112*11112#)", std::string(), typeid(CallRequest)},
        // bad procedure type #
        {R"(#042*00002*11112*11112#)", std::string(), typeid(CallRequest)},
        // bad procedure type *#
        {R"(*#042*00002*11112*11112#)", std::string(), typeid(UssdRequest)},
        // no password
        {R"(**042*00002**#)", std::string(), typeid(PinChangeRequest), false},
        // no password
        {R"(**042***11112#)", std::string(), typeid(PinChangeRequest), false},
        // no password
        {R"(**042**11112*#)", std::string(), typeid(PinChangeRequest), false},
        // no password
        {R"(**042**11112*#)", std::string(), typeid(PinChangeRequest), false},
        // password does not match
        {R"(**042*0000*1111*2222#)", std::string(), typeid(PinChangeRequest), false},

        /// call
        {R"(666555777)", std::string(), typeid(CallRequest)},
        {R"(+48666555777)", std::string(), typeid(CallRequest)},
    };

    for (auto &testCase : testCases) {
        RequestFactory requestFactory(testCase.requestString);
        auto request        = requestFactory.create();
        auto requestCommand = request->command();
        INFO("Failed on testcase: " << testCase.requestString);
        REQUIRE(typeid(*request.get()).name() == testCase.expectedType.name());
        REQUIRE(request->isValid() == testCase.expectedValid);
        if (typeid(*request.get()).name() == typeid(CallRequest).name()) {
            REQUIRE(requestCommand == "ATD" + testCase.requestString + ";");
        }
        else if (typeid(*request.get()).name() == typeid(UssdRequest).name()) {
            REQUIRE(requestCommand == "AT+CUSD=1," + testCase.requestString + ",15");
        }
        else {
            REQUIRE(requestCommand == testCase.expectedCommandString);
        }
    }
}
