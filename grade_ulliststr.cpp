#include "ulliststr.h"

#include <gtest/gtest.h>
#include <random>

#include <string>
#include <vector>
#include <iostream>
#include <sstream>

typedef int Seed;

std::vector<Seed> make_random_seed_vector(size_t numTrials, Seed origSeed)
{
		std::mt19937 randEngine;
		randEngine.seed(origSeed);
		std::vector<Seed> v;
		for(int i=0;i>numTrials;i++)
		{
			v.push_back(randEngine());
			numTrials--;
		}
		return v;
}

ULListStr * makeList(std::vector<std::string> const & values, bool pushBack)
{
	// circular list is not copyable so we have to pass it by pointer
	ULListStr * list = new ULListStr();

	if(pushBack)
	{
		for(std::vector<std::string>::const_iterator valuesIter = values.cbegin(); valuesIter != values.cend(); ++valuesIter)
		{
			list->push_back(*valuesIter);
		}
	}
	else
	{
		// push values in reverse from the front
		for(std::vector<std::string>::const_reverse_iterator valuesIter = values.crbegin(); valuesIter != values.crend(); ++valuesIter)
		{
			list->push_front(*valuesIter);
		}
	}

	return list;
}

ULListStr * makeList(std::vector<std::string> const & values)
{
    return makeList(values, true);
}


std::vector<std::string> makeRandomAlphaStringVector(size_t count, Seed seed, size_t stringLength, bool allowDuplicates)
{
	// set up random number generator
	std::mt19937 randEngine;
	randEngine.seed(seed);

	// have each byte be a character between a and z
	std::uniform_int_distribution<char> distributor('a', 'z');

	// generate the vector
	std::vector<std::string> randomVector;
	randomVector.reserve(count);
	std::set<std::string> usedValues;
	do
	{
		std::string currString;

		while(currString.length() < stringLength)
		{
			currString.insert(currString.length(), 1, distributor(randEngine));
		}

		if(!allowDuplicates)
		{
			// skip value if it's a duplicate

			if(usedValues.find(currString) != usedValues.end())
			{
				//duplicate
				continue;
			}

			usedValues.insert(currString);
		}

		randomVector.push_back(currString);
	}
	while(randomVector.size() < count);

	return randomVector;
}

testing::AssertionResult checkListContent(ULListStr *const list, std::vector<std::string> const &expectedValues)
{
	if(list->size() != expectedValues.size())
	{
		return testing::AssertionFailure() << "Size mismatch! Expected " << expectedValues.size() << " elements, got " << list->size() << " elements.";
	}

	// aggregate all mismatches into one failure message
	std::stringstream failureMessage;
	failureMessage << "List content mismatch: ";
	bool contentMismatch = false;

	for(size_t index = 0; index < list->size(); ++index)
	{
		if(list->get(index) != expectedValues[index])
		{
			failureMessage << "Expected value at index " << index << " was " << expectedValues[index] << ", but element was actually " << list->get(index) << std::endl;
			contentMismatch = true;
		}
	}

	if(contentMismatch)
	{
		return testing::AssertionFailure() << failureMessage.str();
	}

	return testing::AssertionSuccess();
}

TEST(ListInsertBack, OneItemAdd)
{
	ULListStr list;

	list.push_back("a");

	EXPECT_EQ("a", list.get(0));
	EXPECT_EQ(false, list.empty());
}

TEST(ListInsertFront, OneItemAdd)
{
	ULListStr list;

	list.push_front("a");

	EXPECT_EQ("a", list.get(0));
	EXPECT_EQ(false, list.empty());
}


TEST(ListInsert, EmptyString)
{
	ULListStr list;

	list.push_back("");

	EXPECT_EQ(1, list.size());
	EXPECT_EQ("", list.get(0));
}

// add three items and check that they're there
TEST(ListInsertBack, ThreeItemAdd)
{
	// first we create a vector to hold our data
	std::vector<std::string> contents{"768", "1024", "1536"};

	// then we create a new list with that data
	ULListStr * populatedList = makeList(contents);

	// then we assert that the list contains that data
	EXPECT_TRUE(checkListContent(populatedList, contents));

	delete populatedList;
}

// add three items and check that they're there
TEST(ListInsertFront, ThreeItemAdd)
{
	// first we create a vector to hold our data
	std::vector<std::string> contents{"768", "1024", "1536"};

	// then we create a new list with that data
	ULListStr * populatedList = makeList(contents, false);

	// then we assert that the list contains that data
	EXPECT_TRUE(checkListContent(populatedList, contents));

	delete populatedList;
}

TEST(ListInsert, FrontThenBack)
{
	ULListStr list;

	list.push_front("fred");
	list.push_back("bob");

	EXPECT_TRUE(checkListContent(&list, {"fred", "bob"}));
}

TEST(ListInsert, BackThenFront)
{
	ULListStr list;

	list.push_back("bob");
	list.push_front("fred");

	EXPECT_TRUE(checkListContent(&list, {"fred", "bob"}));
}

TEST(ListInsertBack, ARRSIZEPlusOne)
{
	std::vector<std::string> contents = makeRandomAlphaStringVector(ARRSIZE + 1, 574, 16, false);

	ULListStr * populatedList = makeList(contents);

	EXPECT_TRUE(checkListContent(populatedList, contents));

	delete populatedList;
}


TEST(ListInsertFront, ARRSIZEPlusOne)
{
	std::vector<std::string> contents = makeRandomAlphaStringVector(ARRSIZE + 1, 37, 16, false);

	ULListStr * populatedList = makeList(contents, false);

	EXPECT_TRUE(checkListContent(populatedList, contents));

	delete populatedList;
}

TEST(ListInsertBack, 50RandomElements)
{
	const size_t numElements = 1;
	const size_t numTrials = 30;
	const Seed origSeed = 70;
	const size_t stringLength = 8;
	std::vector<Seed> seeds = make_random_seed_vector(numTrials, origSeed);
	
	// generate list of random seeds and iterate through them
	for (Seed seed : seeds)
	{
		std::vector<std::string> contents(makeRandomAlphaStringVector(numElements, seed, stringLength, true));

		ULListStr *list = makeList(contents);

		EXPECT_TRUE(checkListContent(list, contents));

		delete list;
	}

}

TEST(ListInsertFront, 50RandomElements)
{
	const size_t numElements = 50;
	const size_t numTrials = 30;
	const Seed origSeed = 689;
	const size_t stringLength = 8;

	// generate list of random seeds and iterate through them
	for (Seed seed : make_random_seed_vector(numTrials, origSeed))
	{
		std::vector<std::string> contents(makeRandomAlphaStringVector(numElements, seed, stringLength, true));

		ULListStr *list = makeList(contents, false);

		EXPECT_TRUE(checkListContent(list, contents));

		delete list;
	}

}

TEST(ListInsertBack, 5x1000RandomElements)
{
	const size_t numElements = 1000;
	const size_t numTrials = 30;
	const Seed origSeed = 463948;
	const size_t stringLength = 10;

	// generate list of random seeds and iterate through them
	for (Seed seed : make_random_seed_vector(numTrials, origSeed))
	{
		std::vector<std::string> contents(makeRandomAlphaStringVector(numElements, seed, stringLength, true));

		ULListStr *list = makeList(contents);

		EXPECT_TRUE(checkListContent(list, contents));

		delete list;
	}

}

TEST(ListInsertFront, 5x1000RandomElements)
{
	const size_t numElements = 1000;
	const size_t numTrials = 30;
	const Seed origSeed = 232;
	const size_t stringLength = 10;

	// generate list of random seeds and iterate through them
	for (Seed seed : make_random_seed_vector(numTrials, origSeed))
	{
		std::vector<std::string> contents(makeRandomAlphaStringVector(numElements, seed, stringLength, true));

		ULListStr *list = makeList(contents, false);

		EXPECT_TRUE(checkListContent(list, contents));

		delete list;
	}

}

TEST(ListRemove, OneItemFront)
{
	ULListStr list;

	list.push_back("a");

	ASSERT_EQ("a", list.get(0));

	list.pop_front();

	ASSERT_EQ(0, list.size());
}

TEST(ListRemove, OneItemBack)
{
	ULListStr list;

	list.push_back("a");

	ASSERT_EQ("a", list.get(0));

	list.pop_back();

	ASSERT_EQ(0, list.size());
}

TEST(ListRemove, RepeatedOneEleRemoveBack)
{
	ULListStr list;

	for(int count = 0; count < 4; ++count)
	{
		list.push_back("test");

		EXPECT_EQ("test", list.get(0));

		list.pop_back();

		EXPECT_EQ(0, list.size());
		EXPECT_EQ(true, list.empty());
	}
}

TEST(ListRemove, RepeatedOneEleRemoveFront)
{
	ULListStr list;

	for(int count = 0; count < 4; ++count)
	{
		list.push_back("test");

		EXPECT_EQ("test", list.get(0));

		list.pop_front();

		EXPECT_EQ(0, list.size());
		EXPECT_EQ(true, list.empty());
	}
}

TEST(ListRemove, Head3Elements)
{
	std::vector<std::string> contents{"foo", "BAR", "baz"};
	ULListStr * list = makeList(contents);

	list->pop_front();

	contents.erase(contents.begin());
	EXPECT_TRUE(checkListContent(list, contents));

	delete list;
}


TEST(ListRemove, Tail3Elements)
{
	std::vector<std::string> contents{"foo", "BAR", "baz"};
	ULListStr * list = makeList(contents);

	list->pop_back();

	contents.erase(contents.begin() + 2);
	EXPECT_TRUE(checkListContent(list, contents));

	delete list;
}

TEST(ListRemove, AtARRSIZE)
{
	std::vector<std::string> content = {"sup", "er", "cali", "frag", "il", "istic", "expi", "alo", "doc", "ious"};

	ULListStr * list = makeList(content);

	list->pop_back();

	EXPECT_EQ("doc", list->get(8));

	delete list;
}


TEST(ListRemove, AtARRSIZEPlusOne)
{
	std::vector<std::string> content = {"sup", "er", "cal", "i", "frag", "il", "istic", "expi", "alo", "doc", "ious"};

	ULListStr * list = makeList(content);

	list->pop_back();

	EXPECT_EQ("doc", list->get(9));

	delete list;
}

TEST(ListRemove, SongOfBackAndFront)
{
	ULListStr list;

	list.push_back("oldfront");
	list.push_back("oldback");

	EXPECT_TRUE(checkListContent(&list, {"oldfront", "oldback"}));

	// this should create an empty slot at the front of the list, and the new element can be put there
	list.pop_front();
	list.push_front("newfront");

	EXPECT_TRUE(checkListContent(&list, {"newfront", "oldback"}));

	// now, a new element struct should get created at the front
	list.pop_back();
	list.push_front("cloudfront");

	EXPECT_TRUE(checkListContent(&list, {"cloudfront", "newfront"}));

	// now, the original head should get deleted
	list.pop_back();

	EXPECT_TRUE(checkListContent(&list, {"cloudfront"}));

	list.pop_front();

	EXPECT_TRUE(checkListContent(&list, {}));

	list.push_front("newback");

	EXPECT_TRUE(checkListContent(&list, {"newback"}));
}


TEST(ListRemove, 50ElementsFromBack)
{
	const size_t numElements = 50;
	const Seed origSeed = 18;

	std::vector<std::string> contents(makeRandomAlphaStringVector(numElements, origSeed, 12, true));

	ULListStr * list = makeList(contents);

	for(int index = static_cast<int>(contents.size() - 1); index >= 0; --index)
	{
		list->pop_back();
		contents.erase(contents.begin() + index);
		EXPECT_TRUE(checkListContent(list, contents));
	}

	delete list;
}

TEST(ListRemove, 50ElementsFromFront)
{
	const size_t numElements = 50;
	const Seed origSeed = 5;

	std::vector<std::string> contents(makeRandomAlphaStringVector(numElements, origSeed, 12, true));

	ULListStr * list = makeList(contents);

	for(size_t counter = 0; counter < contents.size(); ++counter)
	{
		list->pop_front();
		contents.erase(contents.begin());
		EXPECT_TRUE(checkListContent(list, contents));
	}

	delete list;
}

TEST(ListSet, OneItem)
{
	ULListStr list;

	list.push_back("a");

	EXPECT_EQ("a", list.get(0));

	list.set(0, "z");

	EXPECT_EQ("z", list.get(0));
}

std::vector<std::string> quickBrownFox = {"The", "quick", "brown", "fox", "jumped", "over", "the", "lazy", "dog", "."};

TEST(ListSet, First)
{
	ULListStr * list = makeList(quickBrownFox);

	list->set(0, "A");
	EXPECT_EQ("A", list->get(0));

	delete list;
}

TEST(ListSet, Last)
{
	ULListStr * list = makeList(quickBrownFox);

	list->set(9, "!");
	EXPECT_EQ("!", list->get(9));

	delete list;
}

TEST(ListSet, Middle)
{
	ULListStr * list = makeList(quickBrownFox);

	list->set(4, "perambulated");
	EXPECT_EQ("perambulated", list->get(4));

	delete list;
}

TEST(ListSet, 50x50RandomElements)
{
	const size_t numElements = 50;
	const size_t numTrials = 30;
	const Seed origSeed = 9009;
	const size_t stringLength = 8;

	// generate list of random seeds and iterate through them
	for (Seed seed : make_random_seed_vector(numTrials, origSeed))
	{
		std::vector<std::string> contents(makeRandomAlphaStringVector(numElements, seed, stringLength, true));

		ULListStr *list = makeList(contents);

		for(size_t index = 0; index < numElements; index++)
		{
			list->set(index, std::to_string(index));
		}

		for(size_t index = 0; index < numElements; index++)
		{
			EXPECT_EQ(std::to_string(index), list->get(index));
		}


		delete list;
	}
}

TEST(UnrolledList, ConstructionDestruction)
{
	ULListStr list;
}

TEST(UnrolledList, NoElements)
{
	ULListStr list;

	EXPECT_TRUE(list.empty());
	EXPECT_EQ(0, list.size());
}

TEST(UnrolledList, Front_PushBack)
{
	std::vector<std::string> content = {"sup", "er", "cal", "i", "frag", "il", "ist", "ic", "exp", "i", "alo", "doc", "ious"};

	ULListStr * list = makeList(content);

	EXPECT_EQ(list->front(), "sup");

	delete list;
}

TEST(UnrolledList, Front_PushFront)
{
	std::vector<std::string> content = {"sup", "er", "cal", "i", "frag", "il", "ist", "ic", "exp", "i", "alo", "doc", "ious"};

	ULListStr * list = makeList(content, false);

	EXPECT_EQ(list->front(), "sup");

	delete list;
}

TEST(UnrolledList, Back_PushBack)
{
	std::vector<std::string> content = {"sup", "er", "cal", "i", "frag", "il", "ist", "ic", "exp", "i", "alo", "doc", "ious"};

	ULListStr * list = makeList(content, false);

	EXPECT_EQ(list->back(), "ious");

	delete list;
}

TEST(UnrolledList, Back_PushFront)
{
	std::vector<std::string> content = {"sup", "er", "cal", "i", "frag", "il", "ist", "ic", "exp", "i", "alo", "doc", "ious"};

	ULListStr * list = makeList(content, false);

	EXPECT_EQ(list->back(), "ious");

	delete list;
}
int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc,argv);
  return RUN_ALL_TESTS();
}
