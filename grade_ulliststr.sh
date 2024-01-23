TOTAL=35
g++ ulliststr.cpp grade_ulliststr.cpp -o grade_ulliststr `pkg-config --cflags --libs gtest`
retVal=$?
if [ $retVal -eq 0 ]
then
    echo "grade_ulliststr compiled."
else
		echo "grade_ulliststr did not compile. Make sure your ulliststr.cpp compiles with your test code."
		echo "AUTO:$TOTAL:P0:V0:0.0"
		exit -1
fi
PASSED=0
FAILED=0
FAILED_TESTS=""

for test in ListInsertBack.OneItemAdd ListInsertBack.ThreeItemAdd ListInsertBack.ARRSIZEPlusOne ListInsertBack.50RandomElements ListInsertBack.5x1000RandomElements ListInsertFront.OneItemAdd ListInsertFront.ThreeItemAdd ListInsertFront.ARRSIZEPlusOne ListInsertFront.50RandomElements ListInsertFront.5x1000RandomElements ListInsert.EmptyString ListInsert.FrontThenBack ListInsert.BackThenFront ListRemove.OneItemFront ListRemove.OneItemBack ListRemove.RepeatedOneEleRemoveBack ListRemove.RepeatedOneEleRemoveFront ListRemove.Head3Elements ListRemove.Tail3Elements ListRemove.AtARRSIZE ListRemove.AtARRSIZEPlusOne ListRemove.SongOfBackAndFront ListRemove.50ElementsFromBack ListRemove.50ElementsFromFront ListSet.OneItem ListSet.First ListSet.Last ListSet.Middle ListSet.50x50RandomElements UnrolledList.ConstructionDestruction UnrolledList.NoElements UnrolledList.Front_PushBack UnrolledList.Front_PushFront UnrolledList.Back_PushBack UnrolledList.Back_PushFront
do
    echo $test
    ./grade_ulliststr --gtest_filter=$test
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
for test in ListInsertBack.OneItemAdd ListInsertBack.ThreeItemAdd ListInsertBack.ARRSIZEPlusOne ListInsertBack.50RandomElements ListInsertBack.5x1000RandomElements ListInsertFront.OneItemAdd ListInsertFront.ThreeItemAdd ListInsertFront.ARRSIZEPlusOne ListInsertFront.50RandomElements ListInsertFront.5x1000RandomElements ListInsert.EmptyString ListInsert.FrontThenBack ListInsert.BackThenFront ListRemove.OneItemFront ListRemove.OneItemBack ListRemove.RepeatedOneEleRemoveBack ListRemove.RepeatedOneEleRemoveFront ListRemove.Head3Elements ListRemove.Tail3Elements ListRemove.AtARRSIZE ListRemove.AtARRSIZEPlusOne ListRemove.SongOfBackAndFront ListRemove.50ElementsFromBack ListRemove.50ElementsFromFront ListSet.OneItem ListSet.First ListSet.Last ListSet.Middle ListSet.50x50RandomElements UnrolledList.ConstructionDestruction UnrolledList.NoElements UnrolledList.Front_PushBack UnrolledList.Front_PushFront UnrolledList.Back_PushBack UnrolledList.Back_PushFront
do
    echo $test
    valgrind valgrind --error-exitcode=2 --tool=memcheck --leak-check=yes ./grade_ulliststr --gtest_filter=$test
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
