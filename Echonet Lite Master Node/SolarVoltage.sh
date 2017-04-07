#!/bin/sh

curl -s 'http://150.65.230.132/digital.cgi?chgrp=0' | grep "CH 1<"  | sed -e 's|<td><table height=90 width=155 border=1 cellpadding=0 cellspacing=0 bgcolor=white><tr><td><font size=4 color=#e02830><b>&nbsp;CH 1</b></font></td></tr><tr><td><font size=6 color=#e02830><b>&nbsp;\(.[^<]*\)</b></font></td></tr><tr><td><font size=4 color=#e02830><b>&nbsp; V   </b></font></td></tr></table>|\1|g'
