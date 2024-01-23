#include <gtest/gtest.h>

#include <vector>
#include <limits>
#include <cstdlib>
#include <iostream>
#include <map>

#include "split.h"


Node * makeList(std::vector<int> const & content)
{
	Node * head = nullptr;
	
	if(!content.empty())
	{
		// create head
		head = new Node(content[0], nullptr);

		// create rest of list
		Node * currTail = head;
		std::vector<int>::const_iterator nextValIter(content.cbegin());
		++nextValIter;

		for(; nextValIter != content.cend(); ++nextValIter)
		{
			Node * newNode = new Node(*nextValIter, nullptr);
			currTail->next = newNode;

			currTail = newNode;
		}
	}

	return head;

}

testing::AssertionResult checkContent(Node * list, std::vector<int> const & content)
{
	// so that we can handle duplicates, move the content into a map where the value is the number of times it still needs to appear
	std::map<int, size_t> expectedValues;

	for(int value : content)
	{
		if(expectedValues.find(value) == expectedValues.end())
		{
			// add new entry
			expectedValues[value] = 1;
		}
		else
		{
			// duplicate, increment entry
			expectedValues[value] += 1;
		}
	}

	// now iterate through the list and take out the values we find from the map
	Node* currentItem = list;
	size_t currentIndex = 0;
	while(currentItem != nullptr)
	{
		int currentVal = currentItem->value;

		std::map<int, size_t>::iterator currentValIter = expectedValues.find(currentVal);

		if(currentValIter == expectedValues.end())
		{
			return testing::AssertionFailure() << "List validation error: Unexpected value " << currentVal << " in list at index " << currentIndex;
		}
		else
		{
			currentValIter->second -= 1;

			if(currentValIter->second == 0)
			{
				expectedValues.erase(currentValIter);
			}
		}

		++currentIndex;
		currentItem = currentItem->next;
	}

	if(!expectedValues.empty())
	{
		for(std::pair<const int, size_t> const & expectedValue : expectedValues)
		{
			std::stringstream message;
			message << "List validation error: ";

			// print multiple messages when multiple copies of a value were needed
			for(size_t timesNeeded = 0; timesNeeded < expectedValue.second; ++timesNeeded)
			{
				 message << "value " << expectedValue.first << " was supposed to be in the list but wasn't found, ";
			}

			return testing::AssertionFailure() << message.str();
		}
	}

	return testing::AssertionSuccess();
}

void deleteList(Node * list)
{
	Node * currElement = list;

	while(currElement != nullptr)
	{
		Node * nextElement = currElement->next;

		delete currElement;

		currElement = nextElement;
	}
}


TEST(Split, Empty)
{
	Node* list = nullptr;
	Node* odds = nullptr;
	Node* evens = nullptr;

	split(list, odds, evens);

	EXPECT_EQ(nullptr, odds);
	EXPECT_EQ(nullptr, evens);
}

TEST(Split, InputSetToNull)
{
	Node* list = makeList({1});
	Node* odds = nullptr;
	Node* evens = nullptr;

	split(list, odds, evens);

	EXPECT_EQ(nullptr, list);

	deleteList(odds);
	deleteList(evens);
}

TEST(Split, OneEven)
{
	Node* list = makeList({2});
	Node* odds = nullptr;
	Node* evens = nullptr;

	split(list, odds, evens);

	EXPECT_TRUE(checkContent(evens, {2}));

	deleteList(odds);
	deleteList(evens);
}

TEST(Split, OneOdd)
{
	Node* list = makeList({1});
	Node* odds = nullptr;
	Node* evens = nullptr;

	split(list, odds, evens);

	EXPECT_TRUE(checkContent(odds, {1}));

	deleteList(odds);
	deleteList(evens);
}

TEST(Split, TwoNumbers)
{
	Node* list = makeList({7, 8});
	Node* odds = nullptr;
	Node* evens = nullptr;

	split(list, odds, evens);

	EXPECT_TRUE(checkContent(odds, {7}));
	EXPECT_TRUE(checkContent(evens, {8}));

	deleteList(odds);
	deleteList(evens);
}

TEST(Split, Zero)
{
	Node* list = makeList({0, 11, 20});
	Node* odds = nullptr;
	Node* evens = nullptr;

	split(list, odds, evens);

	EXPECT_TRUE(checkContent(odds, {11}));
	EXPECT_TRUE(checkContent(evens, {0, 20}));

	deleteList(odds);
	deleteList(evens);
}

TEST(Split, AllOddsOneEven)
{
	Node* list = makeList({1, 3, 5, 7, 9, 11, 12});
	Node* odds = nullptr;
	Node* evens = nullptr;

	split(list, odds, evens);

	EXPECT_TRUE(checkContent(odds, {1, 3, 5, 7, 9, 11}));
	EXPECT_TRUE(checkContent(evens, {12}));

	deleteList(odds);
	deleteList(evens);
}

TEST(Split, TenNumbers)
{
	Node* list = makeList({0, 5, 6, 7, 8, 10, 72, 256, 300, 9009, 1111});
	Node* odds = nullptr;
	Node* evens = nullptr;

	split(list, odds, evens);

	EXPECT_TRUE(checkContent(odds, {5, 7, 1111, 9009}));
	EXPECT_TRUE(checkContent(evens, {0, 6, 8, 10, 72, 256, 300}));

	deleteList(odds);
	deleteList(evens);
}

TEST(Split, AllEvens)
{
	Node* list = makeList({70, 128, 500, 722, 65536});
	Node* odds = nullptr;
	Node* evens = nullptr;

	split(list, odds, evens);

	EXPECT_TRUE(checkContent(odds, {}));
	EXPECT_TRUE(checkContent(evens, {70, 128, 500, 722, 65536}));

	deleteList(odds);
	deleteList(evens);
}

TEST(Split, AllOdds)
{
	Node* list = makeList({3, 7, 13, 17, 19, 23, 29, 37});
	Node* odds = nullptr;
	Node* evens = nullptr;

	split(list, odds, evens);

	EXPECT_TRUE(checkContent(odds, {3, 7, 13, 17, 19, 23, 29, 37}));
	EXPECT_TRUE(checkContent(evens, {}));

	deleteList(odds);
	deleteList(evens);
}

TEST(Split, Duplicates)
{
	Node* list = makeList({2, 2, 2});
	Node* odds = nullptr;
	Node* evens = nullptr;

	split(list, odds, evens);

	EXPECT_TRUE(checkContent(odds, {}));
	EXPECT_TRUE(checkContent(evens, {2, 2, 2}));

	deleteList(odds);
	deleteList(evens);
}


TEST(Split, DuplicatesMixed)
{
	Node* list = makeList({5, 6, 7, 8, 9, 10, 72, 256, 256, 1111, 1111, 1111, 9009});
	Node* odds = nullptr;
	Node* evens = nullptr;

	split(list, odds, evens);

	EXPECT_TRUE(checkContent(odds, {5, 9, 7, 1111, 1111, 1111, 9009}));
	EXPECT_TRUE(checkContent(evens, {6, 8, 10, 72, 256, 256}));

	deleteList(odds);
	deleteList(evens);
}

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc,argv);
  return RUN_ALL_TESTS();
}
