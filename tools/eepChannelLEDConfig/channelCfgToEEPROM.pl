#!/usr/bin/perl -w
#
# (C) 2014 by Kai Lauterbach (lauterbach.kai@web.de)
#

use strict;

my $led_per_eeprom_page = 16;

#              Channels:     A   B   C   D   E
my @channel_led_maximum = ( 64, 96, 64, 48, 48 );

my @file = <STDIN>;

my $line_cnt = 0;
foreach my $line (@file) # process data read from STDIN
{
    my $pos = 0;
    my @tmp = split(":", $line); # split channel data and channel name
    my @data = split(",", $tmp[1]); # split channel data into datawords

    print "Channel ". $tmp[0]. ":\n=========="; # debug output

    my $page_cnt = 0;
    foreach my $d (@data) # process channel datawords
    {
        if (($pos % 16) == 0) ## every 16 datawords increment the used eeprom pages and print a newline
        {
            $page_cnt++;
            print "\n";
        }
        printf("%02X%02X", ($d & 0xFF), (($d >> 8) & 0xFF)); # the dataword as hex in low, high byte order for the EEPROM
        
	if ($pos >= $channel_led_maximum[$line_cnt])
        {
            print "\n\nChannel generation aborted:\nTo much LEDs configured for channel $tmp[0], only ". $channel_led_maximum[$line_cnt]. " are allowed.\n\n";
            exit -1;
        }

        $pos++;
    }
    print "\nChannel ". $tmp[0].
          " contains ". $pos.
          " LEDs within ". $page_cnt. "/".
                           ($channel_led_maximum[$line_cnt]/$led_per_eeprom_page).
          " EEPROM pages\n\n"; # debug output with channel results
    $line_cnt++;
}

exit 0;

