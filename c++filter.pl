#!/usr/bin/perl

while (<>) {
    $oldskip = $skip;

    if (m!^In file included from /opt/gnome/include/sigc\+\+-1.2/sigc\+\+/node.h:21!) {
	$skip = 1;
    }

    if (m!^In file included from /usr/include/paragui/paragui.h:65!) {
	$skip = 1;
    }

    if ($skip == 0) {
	print "$_";
    }

    if ($oldskip == 1 && $skip == 1 && !/^\s+from /) {
	$skip = 0;
    }
}

