#!/bin/sh

echo "Test report"
echo "-----------"
echo ""
echo "Configuration"
echo "-------------"
echo ""
echo "WOLFRAME_TESTREPORT_DIR: $WOLFRAME_TESTREPORT_DIR"
echo "GTEST_OUTPUT: $GTEST_OUTPUT"
echo ""

echo "Summary"
echo "-------"
echo ""

echo "0" > _nof_total_tests
echo "0" > _nof_total_errors
cat tests/reports/gtestReport.txt | \
while read test status nof_tests nof_errors; do
	nof_total_tests=`cat _nof_total_tests`
	nof_total_errors=`cat _nof_total_errors`
	nof_total_tests=`expr $nof_total_tests + $nof_tests`
	echo $nof_total_tests > _nof_total_tests
	nof_total_errors=`expr $nof_total_errors + $nof_errors`
	echo $nof_total_errors > _nof_total_errors
done

nof_total_tests=`cat _nof_total_tests`
echo "$nof_total_tests tests in total"
rm -f _nof_total_tests

nof_total_errors=`cat _nof_total_errors`
echo "$nof_total_errors tests failed"
rm -f _nof_total_errors

echo ""

echo "Test results"
echo "------------"
echo ""
cat tests/reports/gtestReport.txt | sort
echo ""

echo "Test result details"
echo "-------------------"
echo ""

echo "<testresults>"
cat tests/reports/gtestReport.txt | sort | \
while read test status nof_tests nof_errors; do
	echo "<restresult name=\"$test\">"
	cat tests/reports/$test.xml | grep -v '<?xml '
	echo "</restresult>"
done
echo "</testresults>"

