#
# create unicode sample text file from EastAsianWidth-6.0.0.txt(unicode.org)
#
# (C) oota toshiya (ribbon@users.sourceforge.jp)
#
#  Lisence is GPL2 or later
#
#  require perl 5.12 or lator
#

use strict;
use Encode::Encoding;
use encoding 'utf-8';

binmode STDOUT, ":utf8";

my @hex;
my $ucs2;
my $pos;
while (<>) {
  chop;
  @hex = split //;
  if (@hex[4] eq ';') {
    $ucs2 = pack("CC",eval("0x". @hex[0] . @hex[1]), eval("0x" . @hex[2] . @hex[3]));
    $pos = 4;
  } elsif (@hex[5] eq ';') {
    $ucs2 = pack("CCCC",@hex[0],@hex[1],@hex[2]),@hex[3],@hex[4];
    $pos = 5;
  } else {
    die "input line error |$_";
  }

  Encode::from_to($ucs2,"ucs2","utf-8");

  if ($ucs2 =~ /\p{isprint}/) {
    splice(@hex,$pos,1," $ucs2 ");
  } else {
    splice(@hex,$pos,1," xx ");
  }
  print join("",@hex),"\n";
}
