#!/bin/sh
#
# Test the lpadmin command.
#
# Copyright © 2020-2026 by OpenPrinting.
# Copyright © 2007-2018 by Apple Inc.
# Copyright © 1997-2005 by Easy Software Products, all rights reserved.
#
# Licensed under Apache License v2.0.  See the file "LICENSE" for more
# information.
#

echo "Add Printer Test"
echo ""
echo "    lpadmin -p Test3 -v file:///dev/null -E -m drv:///sample.drv/deskjet.ppd"
$runcups $VALGRIND ../systemv/lpadmin -p Test3 -v file:///dev/null -E -m drv:///sample.drv/deskjet.ppd 2>&1
if test $? != 0; then
	echo "    FAILED"
	exit 1
else
	if test -f $CUPS_SERVERROOT/ppd/Test3.ppd; then
		echo "    PASSED"
	else
		echo "    FAILED (No PPD)"
		exit 1
	fi
fi
echo ""

echo "Modify Printer Test"
echo ""
echo "    lpadmin -p Test3 -v file:///dev/null -o PageSize=A4"
$runcups $VALGRIND ../systemv/lpadmin -p Test3 -v file:///dev/null -o PageSize=A4 2>&1
if test $? != 0; then
	echo "    FAILED"
	exit 1
else
	echo "    PASSED"
fi
echo ""

echo "Delete Printer Test"
echo ""
echo "    lpadmin -x Test3"
$runcups $VALGRIND ../systemv/lpadmin -x Test3 2>&1
if test $? != 0; then
	echo "    FAILED"
	exit 1
else
	echo "    PASSED"
fi
echo ""

echo "Add Shared Printer Test"
echo ""
echo "    lpadmin -p Test3 -E -v ipp://localhost:$IPP_PORT/printers/Test2 -m everywhere"
$runcups $VALGRIND ../systemv/lpadmin -p Test3 -E -v ipp://localhost:$IPP_PORT/printers/Test2 -m everywhere 2>&1
if test $? != 0; then
	echo "    FAILED"
	exit 1
else
	echo "    PASSED"
fi
echo ""

echo "Add a printer for cupSNMP/IPPSupplies test"
echo ""
echo "    lpadmin -p Test4 -E -v file:///dev/null -m drv:///sample.drv/zebra.ppd"
$runcups $VALGRIND ../systemv/lpadmin -p Test4 -E -v file:///dev/null -m drv:///sample.drv/zebra.ppd 2>&1
if test $? != 0; then
	echo "    FAILED"
	exit 1
else
	echo "    PASSED"
fi
echo ""

echo "Turn on cupsSNMP/IPPSupplies option"
echo ""
echo "    lpadmin -p Test4 -o cupsSNMPSupplies=true -o cupsIPPSupplies=true"
$runcups $VALGRIND ../systemv/lpadmin -p Test4 -o cupsSNMPSupplies=true -o cupsIPPSupplies=true 2>&1
grep '*cupsSNMPSupplies: True' $BASE/ppd/Test4.ppd
if test $? != 0; then
	echo "    FAILED"
	exit 1
else
	echo "    PASSED"
fi
grep '*cupsIPPSupplies: True' $BASE/ppd/Test4.ppd
if test $? != 0; then
	echo "    FAILED"
	exit 1
else
	echo "    PASSED"
fi
echo ""

echo "Turn on cupsSNMP/IPPSupplies option"
echo ""
echo "    lpadmin -p Test4 -o cupsSNMPSupplies=false -o cupsIPPSupplies=false"
$runcups $VALGRIND ../systemv/lpadmin -p Test4 -o cupsSNMPSupplies=false -o cupsIPPSupplies=false 2>&1
grep '*cupsSNMPSupplies: False' $BASE/ppd/Test4.ppd
if test $? != 0; then
	echo "    FAILED"
	exit 1
else
	echo "    PASSED"
fi
grep '*cupsIPPSupplies: False' $BASE/ppd/Test4.ppd
if test $? != 0; then
	echo "    FAILED"
	exit 1
else
	echo "    PASSED"
fi
echo ""

echo "Delete the printer with cupsSNMP/IPPSupplies"
echo ""
echo "    lpadmin -x Test4"
$runcups $VALGRIND ../systemv/lpadmin -x Test4 2>&1
if test $? != 0; then
	echo "    FAILED"
	exit 1
else
	echo "    PASSED"
fi
echo ""

echo "Set Global Default Option Test"
echo ""
echo "    lpadmin -o sides-default=two-sided-long-edge"
$runcups $VALGRIND ../systemv/lpadmin -o sides-default=two-sided-long-edge 2>&1
if test $? != 0; then
	echo "    FAILED"
	exit 1
else
	echo "    PASSED"
fi
echo ""

echo "Verify Global Default in printers.conf Test"
echo ""
echo "    grep sides printers.conf"
if grep -q "Option sides two-sided-long-edge" $CUPS_SERVERROOT/printers.conf; then
	echo "    PASSED"
else
	echo "    FAILED (Option not found in printers.conf)"
	exit 1
fi
echo ""

echo "Set Another Global Default Option Test"
echo ""
echo "    lpadmin -o media-default=A4"
$runcups $VALGRIND ../systemv/lpadmin -o media-default=A4 2>&1
if test $? != 0; then
	echo "    FAILED"
	exit 1
else
	echo "    PASSED"
fi
echo ""

echo "Verify Multiple Global Defaults in printers.conf Test"
echo ""
echo "    grep DefaultOptions printers.conf"
if grep -q "<DefaultOptions>" $CUPS_SERVERROOT/printers.conf && \
   grep -q "Option sides two-sided-long-edge" $CUPS_SERVERROOT/printers.conf && \
   grep -q "Option media A4" $CUPS_SERVERROOT/printers.conf; then
	echo "    PASSED"
else
	echo "    FAILED (DefaultOptions block or options not found)"
	exit 1
fi
echo ""

echo "Remove Global Default Option Test"
echo ""
echo "    lpadmin -R sides-default"
$runcups $VALGRIND ../systemv/lpadmin -R sides-default 2>&1
if test $? != 0; then
	echo "    FAILED"
	exit 1
else
	echo "    PASSED"
fi
echo ""

echo "Verify Global Default Removed from printers.conf Test"
echo ""
echo "    grep -v sides printers.conf"
if grep -q "Option sides" $CUPS_SERVERROOT/printers.conf; then
	echo "    FAILED (Option sides still present after removal)"
	exit 1
else
	echo "    PASSED"
fi
echo ""

echo "Verify Remaining Global Default Still Present Test"
echo ""
echo "    grep media printers.conf"
if grep -q "Option media A4" $CUPS_SERVERROOT/printers.conf; then
	echo "    PASSED"
else
	echo "    FAILED (Option media A4 missing after removing sides)"
	exit 1
fi
echo ""
