#!/usr/bin/perl -w
#
# (C) 2014 by Kai Lauterbach (lauterbach.kai@web.de)
#
# Reads a iHex coded file from STDIN and prints out the same file with recalculated CRC checksum.
#

use strict;

my @input = <STDIN>;
chomp(@input);
my $sum = 0;

foreach my $line (@input)
{

    my $tmp = 0;
    my @chars = split(//, $line);
    my $value = 0;
    my $pos = $#chars - 3; # drop last 2 byte and the first in line
    #print $pos. "\n";
    #print $line. "\n";
    print ":";
    foreach my $c (@chars)
    {
        next if ($c eq ":" || $pos <= 0);
        if ($tmp == 1)
        {
            $tmp = 0;
            $value .= $c;
            #print "\n<<". $value. ">>\n";
            $sum += hex($value);
            printf("%02X", hex($value)); # print out current byte
            $value = "";
        } elsif ($tmp == 0) {
            $value = $c;
            $tmp = 1;
        }
        $pos--;

    }

    #printf(">>> sum = %02X", $sum);
    $sum = ((~$sum) & 0xFF) + 1;
    #printf(" ~sum+1 = %02X", $sum);

    my $crc = $sum & 0xFF;
    
    $sum = 0; # reset the sum to get a new crc value
    
    #printf(" crc = %02X\n", $crc);
    printf("%02X\n", $crc); # print out corrected crc
    
}

exit 0;

