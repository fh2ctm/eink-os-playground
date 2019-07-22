
/*
 * @file main_tests.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 14.06.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */

#include <iostream>
#include <memory>
#include <cstring>
#include <unistd.h>


#include "catch.hpp"

#include "Audio/decoder/decoder.hpp"


TEST_CASE( "Decoder tests" ) {

    auto cwd = vfs.getcurrdir();
    cwd = cwd.substr(0,cwd.find_last_of("/\\"));
    cwd = cwd.append("/module-audio/tests/samples");

    SECTION("Sample1.wav 16bit 44100Hz stereo")
    {

        auto dec = decoder::decoder::Create((cwd + "/sample1.wav").c_str());
        REQUIRE(dec != nullptr);
        auto tag = dec->fetchTags();

        SECTION( "Init values:" ) {
        REQUIRE( tag->total_duration_s == 5 );
        REQUIRE( tag->artist == "" );
        REQUIRE( tag->album == "" );
        REQUIRE( tag->title == "sample1.wav" );
        REQUIRE( tag->sample_rate == 44100 );
        REQUIRE( tag->num_channel == 2 );
        }

        /*    SECTION( "Set positions:" ) {
                dec->setPosition(0.5);
                REQUIRE( dec->getCurrentPosition() == 336/2 );
                dec->setPosition(0);
                REQUIRE( dec->getCurrentPosition() == 0 );
                dec->setPosition(1);
                REQUIRE( dec->getCurrentPosition() == 336 );
                dec->setPosition(0.25);
                REQUIRE( dec->getCurrentPosition() == 336/4 );
            }*/

    }

    SECTION("Sample1.mp3-> CBR 320Kb stereo")
    {

        auto dec = decoder::decoder::Create((cwd + "/sample1.mp3").c_str());
        REQUIRE(dec != nullptr);
        auto tag = dec->fetchTags();

        SECTION( "Init values:" ) {
        REQUIRE( tag->total_duration_s == 5 );
        REQUIRE( tag->artist == "" );
        REQUIRE( tag->album == "" );
        REQUIRE( tag->title == "sample1.mp3" );
        REQUIRE( tag->sample_rate == 44100 );
        REQUIRE( tag->num_channel == 2 );
        }

        /*    SECTION( "Set positions:" ) {
                dec->setPosition(0.5);
                REQUIRE( dec->getCurrentPosition() == 336/2 );
                dec->setPosition(0);
                REQUIRE( dec->getCurrentPosition() == 0 );
                dec->setPosition(1);
                REQUIRE( dec->getCurrentPosition() == 336 );
                dec->setPosition(0.25);
                REQUIRE( dec->getCurrentPosition() == 336/4 );
            }*/

    }

    SECTION("Sample1.flac-> stereo")
    {

        auto dec = decoder::decoder::Create((cwd + "/sample1.flac").c_str());
        REQUIRE(dec != nullptr);
        auto tag = dec->fetchTags();

        SECTION( "Init values:" ) {
        REQUIRE( tag->total_duration_s == 5 );
        REQUIRE( tag->artist == "" );
        REQUIRE( tag->album == "" );
        REQUIRE( tag->title == "sample1.flac" );
        REQUIRE( tag->sample_rate == 44100 );
        REQUIRE( tag->num_channel == 2 );
        }

        /*    SECTION( "Set positions:" ) {
                dec->setPosition(0.5);
                REQUIRE( dec->getCurrentPosition() == 336/2 );
                dec->setPosition(0);
                REQUIRE( dec->getCurrentPosition() == 0 );
                dec->setPosition(1);
                REQUIRE( dec->getCurrentPosition() == 336 );
                dec->setPosition(0.25);
                REQUIRE( dec->getCurrentPosition() == 336/4 );
            }*/

    }


}

