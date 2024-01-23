TOTAL=12
g++ split.cpp grade_split.cpp -o grade_split `pkg-config --cflags --libs gtest`
retVal=$?
if [ $retVal -eq 0 ]
then
    echo "grade_split compiled."
else
    echo "grade_split did not compile. Make sure your split.cpp compiles with your test code."
		echo "AUTO:T$TOTAL:P0:V0:0.0"
		exit -1
fi
g++ -E split.cpp |grep -q "for\|while"
retVal=$?
if [ $retVal -ne 0 ]
then
    echo "split.cpp contains no loops..."
else
    echo "split.cpp appears to contain loops. All code in split.cpp must be recursive."
    echo "AUTO:T$TOTAL:P0:V0:0.0"
    exit -1
fi
PASSED=0
FAILED=0
FAILED_TESTS=""
for test in Split.Empty Split.InputSetToNull Split.OneEven Split.OneOdd Split.TwoNumbers Split.Zero Split.AllOddsOneEven Split.TenNumbers Split.AllEvens Split.AllOdds Split.Duplicates Split.DuplicatesMixed
do
    echo $test
    ./grade_split --gtest_filter=$test
    retVal=$?
    if [ $retVal -eq 0 ]
    then
        let PASSED=PASSED+1
    else
		let FAILED=FAILED+1
		FAILED_TESTS="${FAILED_TESTS} "$test
    fi
done
VPASSED=0
VFAILED=0
VFAILED_TESTS=""
for test in Split.Empty Split.InputSetToNull Split.OneEven Split.OneOdd Split.TwoNumbers Split.Zero Split.AllOddsOneEven Split.TenNumbers Split.AllEvens Split.AllOdds Split.Duplicates Split.DuplicatesMixed
do
    echo $test
    valgrind --error-exitcode=2 --tool=memcheck --leak-check=yes ./grade_split --gtest_filter=$test
    retVal=$?
    if [ $retVal -eq 0 ]
    then
        let VPASSED=VPASSED+1
    else
		let VFAILED=VFAILED+1
		VFAILED_TESTS="${VFAILED_TESTS} "$test
    fi
done
echo "The following tests failed: ${FAILED_TESTS}"
echo "The following tests failed valgrind: ${VFAILED_TESTS}"
S="max(100*($PASSED/$TOTAL - 0.5*($TOTAL-$VPASSED)/$TOTAL),0)"
SCORE=`python -c "print(round($S,2))"`
echo "Score = $S = $SCORE"
echo "AUTO:T$TOTAL:P$PASSED:V$VPASSED:$SCORE"
