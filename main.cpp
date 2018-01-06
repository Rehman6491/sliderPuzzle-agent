/* 8-Tile Slider Puzzle Program
*
* This program solves an 8-tile slider puzzle utilizing four different search methods.
* These include, breadth-first search (BFS), depth-first search (DFS),
* A* with the number of misplaced tiles, and A* with the Manhattan distance.
*
* Program flow: The user is given seven options to choose from in the main menu.
* The first option allows the user to randomize a starting state. It also lets
* the user know if the randomized state has a solution. (Some puzzle states cannot
* be solved) The next option gives the user a choice to initialize the random state
* for use in a search. The following four choices are for each respective search
* method. At the conclusion of each search, the data structures and STL map are
* reset to allow additional runs. Search results are written to a local .csv file
* and the console screen. The final option shuts down the program.
*
* Program notes: The programmer prefers string manipulation and thus, strings were
* used to represent game states.
*
* File:   main.cpp
* Author: Nick Kinderman
* Created on October 1, 2017, 2:16 PM
*/

//#include "stdafx.h"
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <stack>

using namespace std;

//------------------ Globals Declarations/Definitions ------------------------//

#define ROW 3  // row length
#define COL 3  // col length

// constant goal state of puzzle
const string GOALSTATE = "12345678E";

// write program report to data file
ofstream outFile;

// X & Y points in the puzzle 2d array
struct Point {
	int x;
	int y;
};

// game data object w/ declared "curr" and "neighbor" nodes
struct Node {

	// clear the vector and state data
	void clear() {
		state.clear();
		path.clear();
	}

	int point; // current empty node coordinate

	string state; // current state of the puzzle

	int depth; // current depth of the search

	vector<string>path; // current path of the puzzle

	int count; // counter for generated nodes

	int cheapest;  // the A* f(n) value

}curr, neighbor;

// comparison object for the priority queues
// compares and sorts the queue according to the cheapest A* f(n)value
struct compare{
    bool operator()(const Node & a, const Node & b){
        return a.cheapest > b.cheapest;
     }
};

// a random state to be generated from the GOALSTATE
string generatedState = GOALSTATE;

// start state of the puzzle, initialized to GOALSTATE to check for initialization status
string startState = GOALSTATE;

// final state returned from the chosen search method
string endState;

// 2D array of puzzle
char puzzle[ROW][COL];

// declare and initialize counter
int counter = 1; 

// marks the data structure to use
int dataStructure;

// visited status map
map <string, int> visited;

// create a queue of Nodes for the BFS search
queue<Node> bfsQueue;

// create a stack of Nodes for the DFS search
stack<Node> dfsStack;

// create a priority queue of Nodes for the A* search w/ misplaced tiles heuristic
priority_queue<Node,vector<Node>,compare> aStarOutofPlace;

// create a priority queue of Nodes for the A* search w/ Manhattan distance heuristic
priority_queue<Node,vector<Node>,compare> aStarManhattan;

// permanent puzzle tile coordinates - used in tile swap functions
const Point one = { 0, 0 };
const Point two = { 0, 1 };
const Point three = { 0, 2 };
const Point four = { 1, 0 };
const Point five = { 1, 1 };
const Point six = { 1, 2 };
const Point seven = { 2, 0 };
const Point eight = { 2, 1 };
const Point nine = { 2, 2 };

//------------------------- Function Declarations ----------------------------//

// generate a random starting state and test for a solution
void generateState();

// convert string to int and put this int into an array
void stringToInt(string stringState, int intState, int testArray[9]);

// copied from http://www.geeksforgeeks.org/check-instance-8-puzzle-solvable/
// find inversion count of a given array
int getInvCount(int arr[]);

// copied from http://www.geeksforgeeks.org/check-instance-8-puzzle-solvable/
// return true if inversion count of puzzle is even.
bool isSolvable(int puzzle[3][3]);

// initialize a starting state and clear STL map, queue and stack
void initializeState();

// clear the STL map, queue and stack, and priority queue
void clear();

// populate an int 2D array (puzzle) w/ int array representation of the state
void populateIntPuzzle(int intArray[9], int intPuzzle[ROW][COL]);

// populate a char 2D array (puzzle) w/ string state
void populatePuzzle(string startState, char puzzle[ROW][COL]);

// print 2D array (puzzle)
void printPuzzle(char puzzle[ROW][COL]);

// swap one and two positions
string oneTwo(char puzzle[ROW][COL]);

// swap one and four positions
string oneFour(char puzzle[ROW][COL]);

// swap two and three positions
string twoThree(char puzzle[ROW][COL]);

// swap two and five positions
string twoFive(char puzzle[ROW][COL]);

// swap three and six positions
string threeSix(char puzzle[ROW][COL]);

// swap four and seven positions
string fourSeven(char puzzle[ROW][COL]);

// swap four and five positions
string fourFive(char puzzle[ROW][COL]);

// swap five and eight positions
string fiveEight(char puzzle[ROW][COL]);

// swap five and six positions
string fiveSix(char puzzle[ROW][COL]);

// swap six and nine positions
string sixNine(char puzzle[ROW][COL]);

// swap seven and eight positions
string sevenEight(char puzzle[ROW][COL]);

// swap eight and nine positions
string eightNine(char puzzle[ROW][COL]);

// swap pattern for empty tile on 1
void tileOne(string tempState);

// swap pattern for empty tile on 2
void tileTwo(string tempState);

// swap pattern for empty tile on 3
void tileThree(string tempState);

// swap pattern for empty tile on 4
void tileFour(string tempState);

// swap pattern for empty tile on 5
void tileFive(string tempState);

// swap pattern for empty tile on 6
void tileSix(string tempState);

// swap pattern for empty tile on 7
void tileSeven(string tempState);

// swap pattern for empty tile on 8
void tileEight(string tempState);

// swap pattern for empty tile on 9
void tileNine(string tempState);

// build & return current puzzle state
string puzzleState(char puzzle[ROW][COL]);

// insert non-visited state to map
void insertMap(string state, int depth);

// check if visited status is on map
bool checkMap(string state);

// check for goal state
bool checkGoal(string workingState);

// count/return the number of misplaced tiles from the goal state
int misplacedTiles(string state);

// count/return the Manhattan distance from the goal state
int manhattanDistance(string state);

// build a temporary node
void buildNode(int counter, string tempState, string tempPath);

// insert node into a data structure
void insertDataStructure(string tempPath);

// find and return empty tile location
int findEmpty(string state);

// initiate search based on tile position
void searchPattern(int tile, string workingState);

// Breadth-first search function to find possible solution to puzzle
string bfs(string startState, char puzzle[ROW][COL]);

// Depth-first search function to find possible solution to puzzle
string dfs(string startState, char puzzle[ROW][COL]);

// A* search function to find possible solution to puzzle w/ misplaced tiles heuristic
string oopl(string startState, char puzzle[ROW][COL]);

// A* search function to find possible solution to puzzle w/ Manhattan distance heuristic
string mhttn(string startState, char puzzle[ROW][COL]);

// Print some results to console and write all results to a .csv file
void results(string endState);

//----------------------------- Program Main ---------------------------------//

int main() {

	srand(time(NULL)); // for use in randomizing the random_shuffle function in STL <algorithm>

	int menu = 0; // menu options variable

	cout << "Welcome to Puzzle Slider 9000!" << endl;

	while (menu != 99) {

		cout << endl;

		// Choose an option
		cout << "1. Generate a random state: " << endl;
		cout << "2. Initialize working state: " << endl;
		cout << "3. Breadth-First Search: " << endl;
		cout << "4. Depth-First Search: " << endl;
                cout << "5. A* Search w/ misplaced tiles: " << endl;
                cout << "6. A* Search w/ manhattan distance: " << endl;
		cout << "99. Exit the application: " << endl;

		cout << endl;
		cout << endl;
		cout << "Choose an option. ";

		// user input
		cin >> menu;

		switch (menu) {

		case 1:
			cout << string(50, '\n'); // console spacing for universal output on different IDEs and computers

			// generate a random state and check for solvability
			generateState();
			cout << endl;
			cout << endl;
			cout << "What would you like to do next?" << endl;
			break;

		case 2:
			cout << string(50, '\n'); // console spacing for universal output on different IDEs and computers

			// set a start state from a randomized state and clear data structures for reuse
			initializeState();

			cout << "Your initialized start state is: " << startState << endl;
			cout << endl;
			cout << endl;
			cout << "What would you like to do next?" << endl;
			break;

		case 3:
			cout << string(50, '\n'); // console spacing for universal output on different IDEs and computers

			// BFS of puzzle, return final node state
			endState = bfs(startState, puzzle);

			// Check to see if a random state was generated
			if (endState == "error") {
				cout << "Randomize and initialize a new start state to begin a search!" << endl;
				break;
			}

			// print search results to console and write to a .csv file
			results(endState);

                        // clear data structures and map for reuse
                        clear();

			cout << endl;
			cout << endl;
			cout << "What would you like to do next?" << endl;
			break;

		case 4:
			cout << string(50, '\n'); // console spacing for universal output

			// DFS of puzzle, return final node state
			endState = dfs(startState, puzzle);

			// Check to see if a random state was generated
			if (endState == "error") {
				cout << "Initialize a new startState to begin a search!" << endl;
				break;
			}

			// print search results to console and write to a .csv file
			results(endState);

                        // clear data structures and map for reuse
                        clear();

			cout << endl;
			cout << endl;
			cout << "What would you like to do next?" << endl;
			break;

                case 5:
			cout << string(50, '\n'); // console spacing for universal output

			// A* search of puzzle(with misplaced tiles), return final node state
			endState = oopl(startState, puzzle);

			// Check to see if a random state was generated
			if (endState == "error") {
				cout << "Initialize a new startState to begin a search!" << endl;
				break;
			}

			// print search results to console and write to a .csv file
			results(endState);

                        // clear data structures and map for reuse
                        clear();

                        menu = 99;
			cout << endl;
			cout << endl;
			cout << "What would you like to do next?" << endl;
			break;

                case 6:
			cout << string(50, '\n'); // console spacing for universal output

			// A* search of puzzle(with Manhattan distance), return final node state
			endState = mhttn(startState, puzzle);

			// Check to see if a random state was generated
			if (endState == "error") {
				cout << "Initialize a new startState to begin a search!" << endl;
				break;
			}

			// print search results to console and write to a .csv file
			results(endState);

                        // clear data structures and map for reuse
                        clear();

                        menu= 99;
                        cout << endl;
			cout << endl;
			cout << "What would you like to do next?" << endl;
			break;

		case 99:
			cout << string(50, '\n'); // console spacing for universal output on different IDEs and computers
			cout << "Exiting the application!" << endl;
			break;

		default:
			cout << string(50, '\n'); // console spacing for universal output on different IDEs and computers
			cout << "Incorrect option, choose again!" << endl;
			break;
		}
	}

	return 0;
}

//------------------------- Function Definitions -----------------------------//

void generateState() {

	// shuffle characters the string generatedState - uses STL <algorithm>
	random_shuffle(generatedState.begin(), generatedState.end());

	// The slider puzzle uses strings to represent states. In order to test a random state for solvability,
	// an 1D integer and 2D integer array is needed.

	int intState = 0; // holds a integer representation of a string state
	int testArray[9]; // array to store converted integer

	// converts generatedState to intState and stores intState in testArray
	stringToInt(generatedState, intState, testArray);

	// 2d integer array to test solution
	int intPuzzle[ROW][COL];

	// populate an int 2D array with an int 1D array
	populateIntPuzzle(testArray, intPuzzle);

	cout << generatedState;

	// test 2D integer array for solvability and return a status
	if (isSolvable(intPuzzle)) {
		cout << " is solvable!";
	}
	else {
		cout << " is not solvable!";
	}
	cout << endl;
	cout << endl;
}

void stringToInt(string stringState, int intState, int testArray[9]) {

	// change char 'E' to char '0'
	for (unsigned int i = 0; i < stringState.length(); i++) {
		if (stringState[i] == 'E') {
			stringState[i] = '0';
		}
	}

	// convert string stringState to int intState

	// place stringState into a stringstream buffer
	stringstream convert(stringState);

	// convert buffer into an integer
	convert >> intState;

	// put intState into an array counting backwards using modulus and division
	testArray[9];
	for (int i = 8; i >= 0; i--) {
		testArray[i] = intState % 10;
		intState /= 10;
	}
}

int getInvCount(int arr[])
{
	// find inversion count of a given array
	int inv_count = 0;
	for (int i = 0; i < 9 - 1; i++)
		for (int j = i + 1; j < 9; j++)
			// Value 0 is used for empty space
			if (arr[j] && arr[i] && arr[i] > arr[j])
				inv_count++;
	return inv_count;
}

bool isSolvable(int puzzle[3][3])
{
	// fetch inversion count in given array
	int invCount = getInvCount((int *)puzzle);

	// return true if inversion count is even.
	return (invCount % 2 == 0);
}

void initializeState() {

	// initialize startState with your random selection
	startState = generatedState;
}

// clear the STL map, queue, stack, and priority queue
void clear(){
    // initialize counter to 1
	counter = 1;

	// clear the STL map
	visited.clear();

	// empty the bfsQueue
	while (!bfsQueue.empty()) bfsQueue.pop();

	// empty the dfsStack
	while (!dfsStack.empty()) dfsStack.pop();

        // empty the dfsStack
	while (!aStarOutofPlace.empty()) aStarOutofPlace.pop();

        // empty the dfsStack
	while (!aStarManhattan.empty()) aStarManhattan.pop();
}

// populate an integer 2d array with an integer 1d array
void populateIntPuzzle(int intArray[9], int intPuzzle[ROW][COL]) {
	int count = 0;
	for (int i = 0; i < ROW; i++) {
		for (int j = 0; j < COL; j++) {
			intPuzzle[i][j] = intArray[count];
			count++;
		}
	}
}

// populate a char 2D array (puzzle) w/ a string state
void populatePuzzle(string startState, char puzzle[ROW][COL]) {
	int count = 0;
	for (int i = 0; i < ROW; i++) {
		for (int j = 0; j < COL; j++) {
			puzzle[i][j] = startState[count];
			count++;
		}
	}
}

// print 2D array (puzzle) - used for testing/debugging
void printPuzzle(char puzzle[ROW][COL]) {
	cout << "The Puzzle" << endl;
	for (int i = 0; i < ROW; i++) {
		for (int j = 0; j < COL; j++) {
			cout << puzzle[i][j] << "\t";
		}
		cout << endl;
	}
	cout << endl;
}

// swap one and two positions
string oneTwo(char puzzle[ROW][COL]) {
	char temp = puzzle[one.x][one.y];
	puzzle[one.x][one.y] = puzzle[two.x][two.y];
	puzzle[two.x][two.y] = temp;

	string tempState = puzzleState(puzzle);
	return tempState;
}

// swap one and four positions
string oneFour(char puzzle[ROW][COL]) {
	char temp = puzzle[one.x][one.y];
	puzzle[one.x][one.y] = puzzle[four.x][four.y];
	puzzle[four.x][four.y] = temp;

	string tempState = puzzleState(puzzle);
	return tempState;
}

// swap two and three positions
string twoThree(char puzzle[ROW][COL]) {
	char temp = puzzle[two.x][two.y];
	puzzle[two.x][two.y] = puzzle[three.x][three.y];
	puzzle[three.x][three.y] = temp;

	string tempState = puzzleState(puzzle);
	return tempState;
}

// swap two and five positions
string twoFive(char puzzle[ROW][COL]) {
	char temp = puzzle[two.x][two.y];
	puzzle[two.x][two.y] = puzzle[five.x][five.y];
	puzzle[five.x][five.y] = temp;

	string tempState = puzzleState(puzzle);
	return tempState;
}

// swap three and six positions
string threeSix(char puzzle[ROW][COL]) {
	char temp = puzzle[three.x][three.y];
	puzzle[three.x][three.y] = puzzle[six.x][six.y];
	puzzle[six.x][six.y] = temp;

	string tempState = puzzleState(puzzle);
	return tempState;
}

// swap four and seven positions
string fourSeven(char puzzle[ROW][COL]) {
	char temp = puzzle[four.x][four.y];
	puzzle[four.x][four.y] = puzzle[seven.x][seven.y];
	puzzle[seven.x][seven.y] = temp;

	string tempState = puzzleState(puzzle);
	return tempState;
}

// swap four and five positions
string fourFive(char puzzle[ROW][COL]) {
	char temp = puzzle[four.x][four.y];
	puzzle[four.x][four.y] = puzzle[five.x][five.y];
	puzzle[five.x][five.y] = temp;

	string tempState = puzzleState(puzzle);
	return tempState;
}

// swap five and eight positions
string fiveEight(char puzzle[ROW][COL]) {
	char temp = puzzle[five.x][five.y];
	puzzle[five.x][five.y] = puzzle[eight.x][eight.y];
	puzzle[eight.x][eight.y] = temp;

	string tempState = puzzleState(puzzle);
	return tempState;
}

// swap five and six positions
string fiveSix(char puzzle[ROW][COL]) {
	char temp = puzzle[five.x][five.y];
	puzzle[five.x][five.y] = puzzle[six.x][six.y];
	puzzle[six.x][six.y] = temp;

	string tempState = puzzleState(puzzle);
	return tempState;
}

// swap six and nine positions
string sixNine(char puzzle[ROW][COL]) {
	char temp = puzzle[six.x][six.y];
	puzzle[six.x][six.y] = puzzle[nine.x][nine.y];
	puzzle[nine.x][nine.y] = temp;

	string tempState = puzzleState(puzzle);
	return tempState;
}

// swap seven and eight positions
string sevenEight(char puzzle[ROW][COL]) {
	char temp = puzzle[seven.x][seven.y];
	puzzle[seven.x][seven.y] = puzzle[eight.x][eight.y];
	puzzle[eight.x][eight.y] = temp;

	string tempState = puzzleState(puzzle);
	return tempState;
}

// swap eight and nine positions
string eightNine(char puzzle[ROW][COL]) {
	char temp = puzzle[eight.x][eight.y];
	puzzle[eight.x][eight.y] = puzzle[nine.x][nine.y];
	puzzle[nine.x][nine.y] = temp;

	string tempState = puzzleState(puzzle);
	return tempState;
}

// swap pattern for empty tile on 1
void tileOne(string workingState) {

	// temp 2D puzzle for swap use
	char tempPuzzle[ROW][COL];

	// temp state for swap use
	string tempState;

	// temp path for swap use
	string tempPath;

	// ---------- first check, 1 to 2 ---------- //

	// build a temp puzzle with current workingState
	populatePuzzle(workingState, tempPuzzle);

	// build temp state from swapped puzzle
	tempState = oneTwo(tempPuzzle);

	// check map for visited state
	if (!checkMap(tempState)) {

		counter++; // increment counter
		insertMap(tempState, counter);  // insert state & counter into map
		tempPath = "1 to 2,"; // assign string value to tempPath

		buildNode(counter, tempState, tempPath); // build a temporary node with updated data
		insertDataStructure(tempPath); // insert node into the data structure that the search is using
	}

	// ---------- second check, 1 to 4 ---------- //

	// build a temp puzzle with current workingState
	populatePuzzle(workingState, tempPuzzle);

	// build temp state from swapped puzzle
	tempState = oneFour(tempPuzzle);

	// check map for visited state
	if (!checkMap(tempState)) {

		counter++; // increment counter
		insertMap(tempState, counter); // insert state & counter into map
		tempPath = "1 to 4,"; // assign string value to tempPath

		buildNode(counter, tempState, tempPath); // build a temporary node with updated data
		insertDataStructure(tempPath); // insert node into the data structure that the search is using
	}
}

void tileTwo(string workingState) {

	// temp 2D puzzle for swap use
	char tempPuzzle[ROW][COL];

	// temp state for swap use
	string tempState;

	// temp path for swap use
	string tempPath;

	// ---------- first check, 2 to 3 ---------- //

	// build a temp puzzle with current workingState
	populatePuzzle(workingState, tempPuzzle);

	// build temp state from swapped puzzle
	tempState = twoThree(tempPuzzle);

	// check map for visited state
	if (!checkMap(tempState)) {

		counter++; // increment counter
		insertMap(tempState, counter); // insert state & counter into map
		tempPath = "2 to 3,"; // assign string value to tempPath

		buildNode(counter, tempState, tempPath); // build a temporary node with updated data
		insertDataStructure(tempPath); // insert node into the data structure that the search is using
	}

	// ---------- second check, 2 to 5 ---------- //

	// build a temp puzzle with current workingState
	populatePuzzle(workingState, tempPuzzle);

	// build temp state from swapped puzzle
	tempState = twoFive(tempPuzzle);

	// check map for visited state
	if (!checkMap(tempState)) {

		counter++; // increment counter
		insertMap(tempState, counter); // insert state & counter into map
		tempPath = "2 to 5,"; // assign string value to tempPath

		buildNode(counter, tempState, tempPath); // build a temporary node with updated data
		insertDataStructure(tempPath); // insert node into the data structure that the search is using
	}

	// ---------- third check, 2 to 1 ---------- //

	// build a temp puzzle with current workingState
	populatePuzzle(workingState, tempPuzzle);

	// build temp state from swapped puzzle
	tempState = oneTwo(tempPuzzle);

	// check map for visited state
	if (!checkMap(tempState)) {

		counter++; // increment counter
		insertMap(tempState, counter); // insert state & counter into map
		tempPath = "2 to 1,"; // assign string value to tempPath

		buildNode(counter, tempState, tempPath); // build a temporary node with updated data
		insertDataStructure(tempPath); // insert node into the data structure that the search is using
	}
}

void tileThree(string workingState) {

	// temp 2D puzzle for swap use
	char tempPuzzle[ROW][COL];

	// temp state for swap use
	string tempState;

	// temp path for swap use
	string tempPath;

	// ---------- first check, 3 to 6 ---------- //

	// build a temp puzzle with current workingState
	populatePuzzle(workingState, tempPuzzle);

	// build temp state from swapped puzzle
	tempState = threeSix(tempPuzzle);

	// check map for visited state
	if (!checkMap(tempState)) {

		counter++; // increment counter
		insertMap(tempState, counter); // insert state & counter into map
		tempPath = "3 to 6,"; // assign string value to tempPath

		buildNode(counter, tempState, tempPath); // build a temporary node with updated data
		insertDataStructure(tempPath); // insert node into the data structure that the search is using
	}

	// ---------- second check, 3 to 2 ---------- //

	// build a temp puzzle with current workingState
	populatePuzzle(workingState, tempPuzzle);

	// build temp state from swapped puzzle
	tempState = twoThree(tempPuzzle);

	// check map for visited state
	if (!checkMap(tempState)) {

		counter++; // increment counter
		insertMap(tempState, counter); // insert state & counter into map
		tempPath = "3 to 2,"; // assign string value to tempPath

		buildNode(counter, tempState, tempPath); // build a temporary node with updated data
		insertDataStructure(tempPath); // insert node into the data structure that the search is using
	}
}

void tileFour(string workingState) {

	// temp 2D puzzle for swap use
	char tempPuzzle[ROW][COL];

	// temp state for swap use
	string tempState;

	// temp path for swap use
	string tempPath;

	// ---------- first check, 4 to 5 ---------- //

	// build a temp puzzle with current workingState
	populatePuzzle(workingState, tempPuzzle);

	// build temp state from swapped puzzle
	tempState = fourFive(tempPuzzle);

	// check map for visited state
	if (!checkMap(tempState)) {

		counter++; // increment counter
		insertMap(tempState, counter); // insert state & counter into map
		tempPath = "4 to 5,"; // assign string value to tempPath

		buildNode(counter, tempState, tempPath); // build a temporary node with updated data
		insertDataStructure(tempPath); // insert node into the data structure that the search is using
	}

	// ---------- second check, 4 to 7 ---------- //

	// build a temp puzzle with current workingState
	populatePuzzle(workingState, tempPuzzle);

	// build temp state from swapped puzzle
	tempState = fourSeven(tempPuzzle);

	// check map for visited state
	if (!checkMap(tempState)) {

		counter++; // increment counter
		insertMap(tempState, counter); // insert state & counter into map
		tempPath = "4 to 7,"; // assign string value to tempPath

		buildNode(counter, tempState, tempPath); // build a temporary node with updated data
		insertDataStructure(tempPath); // insert node into the data structure that the search is using
	}

	// ---------- third check, 4 to 1 ---------- //

	// build a temp puzzle with current workingState
	populatePuzzle(workingState, tempPuzzle);

	// build temp state from swapped puzzle
	tempState = oneFour(tempPuzzle);

	// check map for visited state
	if (!checkMap(tempState)) {

		counter++; // increment counter
		insertMap(tempState, counter); // insert state & counter into map
		tempPath = "4 to 1,"; // assign string value to tempPath

		buildNode(counter, tempState, tempPath); // build a temporary node with updated data
		insertDataStructure(tempPath); // insert node into the data structure that the search is using
	}
}

void tileFive(string workingState) {

	// temp 2D puzzle for swap use
	char tempPuzzle[ROW][COL];

	// temp state for swap use
	string tempState;

	// temp path for swap use
	string tempPath;

	// ---------- first check, 5 to 6 ---------- //

	// build a temp puzzle with current workingState
	populatePuzzle(workingState, tempPuzzle);

	// build temp state from swapped puzzle
	tempState = fiveSix(tempPuzzle);

	// check map for visited state
	if (!checkMap(tempState)) {

		counter++; // increment counter
		insertMap(tempState, counter); // insert state & counter into map
		tempPath = "5 to 6,"; // assign string value to tempPath

		buildNode(counter, tempState, tempPath); // build a temporary node with updated data
		insertDataStructure(tempPath); // insert node into the data structure that the search is using
	}

	// ---------- second check, 5 to 8 ---------- //

	// build a temp puzzle with current workingState
	populatePuzzle(workingState, tempPuzzle);

	// build temp state from swapped puzzle
	tempState = fiveEight(tempPuzzle);

	// check map for visited state
	if (!checkMap(tempState)) {

		counter++; // increment counter
		insertMap(tempState, counter); // insert state & counter into map
		tempPath = "5 to 8,"; // assign string value to tempPath

		buildNode(counter, tempState, tempPath); // build a temporary node with updated data
		insertDataStructure(tempPath); // insert node into the data structure that the search is using
	}

	// ---------- third check, 5 to 4 ---------- //

	// build a temp puzzle with current workingState
	populatePuzzle(workingState, tempPuzzle);

	// build temp state from swapped puzzle
	tempState = fourFive(tempPuzzle);

	// check map for visited state
	if (!checkMap(tempState)) {

		counter++; // increment counter
		insertMap(tempState, counter); // insert state & counter into map
		tempPath = "5 to 4,"; // assign string value to tempPath

		buildNode(counter, tempState, tempPath); // build a temporary node with updated data
		insertDataStructure(tempPath); // insert node into the data structure that the search is using
	}

	// ---------- fourth check, 5 to 2 ---------- //

	// build a temp puzzle with current workingState
	populatePuzzle(workingState, tempPuzzle);

	// build temp state from swapped puzzle
	tempState = twoFive(tempPuzzle);

	// check map for visited state
	if (!checkMap(tempState)) {

		counter++; // increment counter
		insertMap(tempState, counter); // insert state & counter into map
		tempPath = "5 to 2,"; // assign string value to tempPath

		buildNode(counter, tempState, tempPath); // build a temporary node with updated data
		insertDataStructure(tempPath); // insert node into the data structure that the search is using
	}
}

void tileSix(string workingState) {

	// temp 2D puzzle for swap use
	char tempPuzzle[ROW][COL];

	// temp state for swap use
	string tempState;

	// temp path for swap use
	string tempPath;

	// ---------- first check, 6 to 9 ---------- //

	// build a temp puzzle with current workingState
	populatePuzzle(workingState, tempPuzzle);

	// build temp state from swapped puzzle
	tempState = sixNine(tempPuzzle);

	// check map for visited state
	if (!checkMap(tempState)) {

		counter++; // increment counter
		insertMap(tempState, counter); // insert state & counter into map
		tempPath = "6 to 9,"; // assign string value to tempPath

		buildNode(counter, tempState, tempPath); // build a temporary node with updated data
		insertDataStructure(tempPath); // insert node into the data structure that the search is using
	}

	// ---------- second check, 6 to 5 ---------- //

	// build a temp puzzle with current workingState
	populatePuzzle(workingState, tempPuzzle);

	// build temp state from swapped puzzle
	tempState = fiveSix(tempPuzzle);

	// check map for visited state
	if (!checkMap(tempState)) {

		counter++; // increment counter
		insertMap(tempState, counter); // insert state & counter into map
		tempPath = "6 to 5,"; // assign string value to tempPath

		buildNode(counter, tempState, tempPath); // build a temporary node with updated data
		insertDataStructure(tempPath); // insert node into the data structure that the search is using
	}

	// ---------- third check, 6 to 3 ---------- //

	// build a temp puzzle with current workingState
	populatePuzzle(workingState, tempPuzzle);

	// build temp state from swapped puzzle
	tempState = threeSix(tempPuzzle);

	// check map for visited state
	if (!checkMap(tempState)) {

		counter++; // increment counter
		insertMap(tempState, counter); // insert state & counter into map
		tempPath = "6 to 3,"; // assign string value to tempPath

		buildNode(counter, tempState, tempPath); // build a temporary node with updated data
		insertDataStructure(tempPath); // insert node into the data structure that the search is using
	}
}

void tileSeven(string workingState) {

	// temp 2D puzzle for swap use
	char tempPuzzle[ROW][COL];

	// temp state for swap use
	string tempState;

	// temp path for swap use
	string tempPath;

	// ---------- first check, 7 to 8 ---------- //

	// build a temp puzzle with current workingState
	populatePuzzle(workingState, tempPuzzle);

	// build temp state from swapped puzzle
	tempState = sevenEight(tempPuzzle);

	// check map for visited state
	if (!checkMap(tempState)) {

		counter++; // increment counter
		insertMap(tempState, counter); // insert state & counter into map
		tempPath = "7 to 8,"; // assign string value to tempPath

		buildNode(counter, tempState, tempPath); // build a temporary node with updated data
		insertDataStructure(tempPath); // insert node into the data structure that the search is using
	}

	// ---------- second check, 7 to 4 ---------- //

	// build a temp puzzle with current workingState
	populatePuzzle(workingState, tempPuzzle);

	// build temp state from swapped puzzle
	tempState = fourSeven(tempPuzzle);

	// check map for visited state
	if (!checkMap(tempState)) {

		counter++; // increment counter
		insertMap(tempState, counter); // insert state & counter into map
		tempPath = "7 to 4,"; // assign string value to tempPath

		buildNode(counter, tempState, tempPath); // build a temporary node with updated data
		insertDataStructure(tempPath); // insert node into the data structure that the search is using
	}
}

void tileEight(string workingState) {

	// temp 2D puzzle for swap use
	char tempPuzzle[ROW][COL];

	// temp state for swap use
	string tempState;

	// temp path for swap use
	string tempPath;

	// ---------- first check, 8 to 9 ---------- //

	// build a temp puzzle with current workingState
	populatePuzzle(workingState, tempPuzzle);

	// build temp state from swapped puzzle
	tempState = eightNine(tempPuzzle);

	// check map for visited state
	if (!checkMap(tempState)) {

		counter++; // increment counter
		insertMap(tempState, counter); // insert state & counter into map
		tempPath = "8 to 9,"; // assign string value to tempPath

		buildNode(counter, tempState, tempPath); // build a temporary node with updated data
		insertDataStructure(tempPath); // insert node into the data structure that the search is using
	}

	// ---------- second check, 8 to 7 ---------- //

	// build a temp puzzle with current workingState
	populatePuzzle(workingState, tempPuzzle);

	// build temp state from swapped puzzle
	tempState = sevenEight(tempPuzzle);


	if (!checkMap(tempState)) {

		counter++; // increment counter
		insertMap(tempState, counter); // insert state & counter into map
		tempPath = "8 to 7,"; // assign string value to tempPath

		buildNode(counter, tempState, tempPath); // build a temporary node with updated data
		insertDataStructure(tempPath); // insert node into the data structure that the search is using
	}

	// ---------- third check, 8 to 5 ---------- //

	// build a temp puzzle with current workingState
	populatePuzzle(workingState, tempPuzzle);

	// build temp state from swapped puzzle
	tempState = fiveEight(tempPuzzle);

	// check map for visited state
	if (!checkMap(tempState)) {

		counter++; // increment counter
		insertMap(tempState, counter); // insert state & counter into map
		tempPath = "8 to 5,";

		buildNode(counter, tempState, tempPath); // build a temporary node with updated data
		insertDataStructure(tempPath); // insert node into the data structure that the search is using
	}
}

void tileNine(string workingState) {

	// temp 2D puzzle for swap use
	char tempPuzzle[ROW][COL];

	// temp state for swap use
	string tempState;

	// temp path for swap use
	string tempPath;

	// ---------- first check, 9 to 8 ---------- //

	// build a temp puzzle with current workingState
	populatePuzzle(workingState, tempPuzzle);

	// build temp state from swapped puzzle
	tempState = eightNine(tempPuzzle);

	// check map for visited state
	if (!checkMap(tempState)) {

		counter++; // increment counter
		insertMap(tempState, counter); // insert state & counter into map
		tempPath = "9 to 8,";

		buildNode(counter, tempState, tempPath); // build a temporary node with updated data
		insertDataStructure(tempPath); // insert node into the data structure that the search is using
	}

	// ---------- second check, 9 to 6 ---------- //

	// build a temp puzzle with current workingState
	populatePuzzle(workingState, tempPuzzle);

	// build temp state from swapped puzzle
	tempState = sixNine(tempPuzzle);

	// check map for visited state
	if (!checkMap(tempState)) {

		counter++; // increment counter
		insertMap(tempState, counter); // insert state & counter into map
		tempPath = "9 to 6,";

		buildNode(counter, tempState, tempPath); // build a temporary node with updated data
		insertDataStructure(tempPath); // insert node into the data structure that the search is using
	}
}

// build and return string state representation of a 2D array (puzzle)
string puzzleState(char puzzle[ROW][COL]) {

	string state; // declaration
	state.resize(9); // set size to 9 characters

	// build a string state from a 2D char puzzle
	int count = 0;
	for (int i = 0; i < ROW; i++) {
		for (int j = 0; j < COL; j++) {
			state[count] = puzzle[i][j];
			count++;
		}
	}
	return state;
};

void insertMap(string state, int counter) {
	// insert state and counter to map
	visited.insert(pair <string, int>(state, counter));
}

// search map for visited status and return boolean value
bool checkMap(string state) {
	map <string, int> ::iterator itr;
	for (itr = visited.begin(); itr != visited.end(); ++itr)
	{
		// if state is found in map return true
		if (state == itr->first) {
			return true;
		}
	}
	return false; // if state is not in the map, return false
}

// check for goal state and return boolean status
bool checkGoal(string state) {
	if (state == GOALSTATE) {
		return true;
	}
	else {
		return false;
	}
}

// count/return the number of misplaced tiles from the goal state
int misplacedTiles(string state){
    int tiles = 0;

    for(int i = 0; i < state.length() - 1; i++){
        // if string element != goal string element, increment tiles #
        if(state[i] != GOALSTATE[i]){
            tiles += 1;
        }
    }
    return tiles;
}

// count/return the Manhattan distance from the goal state
int manhattanDistance(string state){

    int intState = 0; // holds a integer representation of a string state
    int intArray[9]; // array to store converted integer

    // converts temporary String state to integer intState and stores intState in integer intArray
    stringToInt(state, intState, intArray);

    // 2d integer array to calculate Manhattan distance
    int intPuzzle[ROW][COL];

    // populate an int 2D array with an int 1D array
    populateIntPuzzle(intArray, intPuzzle);

    int value = 0; // puzzle tile value
    Point temp = {0,0}; // temporary puzzle coordinates
    Point goal = {0,0}; // goal state coordinates
    int mDis = 0; // Manhattan distance variable

    // compare the current state tiles with the goal tile locations
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            value = intPuzzle[i][j];
            //
            if(value != 0){
                temp = {i,j};
                goal = {(value - 1) / 3, (value - 1) % 3};
                mDis += abs(temp.x - goal.x) + abs(temp.y - goal.y);
            }
        }
    }
    return mDis;
}

// build a temp node with current state data
void buildNode(int counter, string tempState, string tempPath) {

	neighbor.state = tempState;  // assign tempState
	neighbor.depth = curr.depth + 1; // working node depth + 1
	neighbor.count = counter; // assign counter

};

// insert node into the data structure that the search is using
void insertDataStructure(string tempPath) {

	// int dataStructure is initialized in BFS or DFS search functions
	switch (dataStructure) {
	case 1: // 1 equals BFS

                neighbor.path = curr.path; // copy current path
                neighbor.path.push_back(tempPath); // insert temp path into vector
                bfsQueue.push(neighbor);  // enqueue neighbor node

		// clear the vector and state data of this temp node to deallocate memory
		neighbor.clear();
		break;

	case 2: // 2 equals DFS


		dfsStack.push(neighbor); // push neighbor node onto stack

		// clear the vector and state data of this temp node to deallocate memory
		neighbor.clear();
		break;

        case 3: // 3 equals A* search w/ misplaced tiles heuristic

                // f(n) = g(n) + h(n): [cheapest = depth + misplaced tiles]
                neighbor.cheapest = neighbor.depth + misplacedTiles(neighbor.state);

                neighbor.path = curr.path; // copy current path
                neighbor.path.push_back(tempPath); // insert temp path into vector
                aStarOutofPlace.push(neighbor);

                // clear the vector and state data of this temp node to deallocate memory
                neighbor.clear();
                break;

        case 4: // 4 equals A* search w/ Manhattan heuristic

                // f(n) = g(n) + h(n): [cheapest = depth + Manhattan distance]
                neighbor.cheapest = neighbor.depth + manhattanDistance(neighbor.state);

                neighbor.path = curr.path; // copy current path
                neighbor.path.push_back(tempPath); // insert temp path into vector
                aStarManhattan.push(neighbor);

                // clear the vector and state data of this temp node to deallocate memory
                neighbor.clear();
                break;
	}

}

// find and return integer representation of empty tile location
int findEmpty(string state) {

	//return empty "E" tile number
	int number = 0;
	for (unsigned int i = 0; i < state.length(); i++) {
		if (state[i] == 'E') {
			number = i + 1;
		}
		else {
			// do nothing
		}
	}

	switch (number) {
	case 1: return 1;
		break;
	case 2: return 2;
		break;
	case 3: return 3;
		break;
	case 4: return 4;
		break;
	case 5: return 5;
		break;
	case 6: return 6;
		break;
	case 7: return 7;
		break;
	case 8: return 8;
		break;
	case 9: return 9;
		break;
	default: cout << "not a tile";
		break;
	}

}

// initiate search based on empty tile position
void searchPattern(int tile, string workingState) {
	switch (tile) {
	case 1: tileOne(workingState);
		break;
	case 2: tileTwo(workingState);
		break;
	case 3: tileThree(workingState);
		break;
	case 4: tileFour(workingState);
		break;
	case 5: tileFive(workingState);
		break;
	case 6: tileSix(workingState);
		break;
	case 7: tileSeven(workingState);
		break;
	case 8: tileEight(workingState);
		break;
	case 9: tileNine(workingState);
		break;
	default: cout << "not a function";
		break;
	}
}

string bfs(string startState, char puzzle[ROW][COL]) {

	// if the start state = the goal state, then the puzzle was not randomized/initialized, return error
	if (startState == GOALSTATE) {
		string error = "error";
		return error;
	}

	dataStructure = 1; // initialize data structure (1 = queue)

	insertMap(startState, counter); // install startState and counter into map

	int startPoint = findEmpty(startState); // find & assign empty tile to an integer

	Node e;  // create entrance node;

	e.point = startPoint; // empty point in starting state
	e.state = startState; // starting state
	e.depth = 0; // depth of root
	e.count = counter; // start count
	e.path.push_back("Start, ");  // insert entry path into vector

	bfsQueue.push(e); // enqueue entrance node

	int workingPoint; // declare workingPoint int
	string workingState;  // declare workingState string

	// while queue is not empty
	while (!bfsQueue.empty()) {
		curr = bfsQueue.front();  // current node equals front of queue
		workingPoint = curr.point; // assign workingPoint to current node value
		workingState = curr.state; // assign workingState to current node value

		// check for goal state, if it matches, return working state and exit the search
		if (checkGoal(workingState)) {
			return workingState;
		}
		else {
			// do nothing
		}

		bfsQueue.pop();  // Else, dequeue the the front node and start the search

		int emptyPoint = findEmpty(workingState); // find & assign empty tile to an integer

		// begin search pattern based on empty tile position
		searchPattern(emptyPoint, workingState);
	}
}

string dfs(string startState, char puzzle[ROW][COL]) {

	// if the start state = the goal state, then the puzzle was not randomized/initialized, return error
	if (startState == GOALSTATE) {
		return "error";
	}

	dataStructure = 2; // initialize data structure (2 = stack)

	insertMap(startState, counter); // install startState and counter into map

	int startPoint = findEmpty(startState); // find & assign empty tile

	Node e;  // create entrance node;

	e.point = startPoint; // empty point in starting state
	e.state = startState; // starting state
	e.depth = 0; // depth of root
	e.count = counter; // start count
	e.path.push_back("Start, ");  // insert entry path into vector

	dfsStack.push(e); // push entrance node

	int workingPoint; // declare workingPoint string
	string workingState; // declare workingState string

	// while stack is not empty
	while (!dfsStack.empty()) {

		curr = dfsStack.top();  // current node equals top of stack
		workingPoint = curr.point; // assign workingPoint to current node value
		workingState = curr.state; // assign workingState to current node value

		// check for goal state, if it matches, return working state and exit the search
		if (checkGoal(workingState)) {
			return workingState;
		}
		else {
			// do nothing
		}
			dfsStack.pop();  // Else, pop the the top node and start the search

			int emptyPoint = findEmpty(workingState); // find & assign empty tile to an integer

			// begin search pattern based on empty tile position
			searchPattern(emptyPoint, workingState);
	}
}

string oopl(string startState, char puzzle[ROW][COL]){

    // if the start state = the goal state, then the puzzle was not randomized/initialized, return error
	if (startState == GOALSTATE) {
		return "error";
	}

	dataStructure = 3; // initialize data structure (3 = A* search w/ misplaced tiles)

	insertMap(startState, counter); // install startState and counter into map

	int startPoint = findEmpty(startState); // find & assign empty tile

	Node e;  // create entrance node;

	e.point = startPoint; // empty point in starting state
	e.state = startState; // starting state
	e.depth = 0; // depth of root
	e.count = counter; // start count
	e.path.push_back("Start, ");  // insert entry path into vector
        e.cheapest = 0; // outOfPlace = misplaced tiles(0) + depth of node(0)

	aStarOutofPlace.push(e); // push entrance node

	int workingPoint; // declare workingPoint string
	string workingState; // declare workingState string

	// while stack is not empty
	while (!aStarOutofPlace.empty()) {

		curr = aStarOutofPlace.top();  // current node equals top of stack
		workingPoint = curr.point; // assign workingPoint to current node value
		workingState = curr.state; // assign workingState to current node value

		// check for goal state, if it matches, return working state and exit the search
		if (checkGoal(workingState)) {
			return workingState;
		}
		else {
			// do nothing
		}
			aStarOutofPlace.pop();  // Else, pop the the top node and start the search

			int emptyPoint = findEmpty(workingState); // find & assign empty tile to an integer

			// begin search pattern based on empty tile position
			searchPattern(emptyPoint, workingState);
	}
}

string mhttn(string startState, char puzzle[ROW][COL]){

    // if the start state = the goal state, then the puzzle was not randomized/initialized, return error
	if (startState == GOALSTATE) {
		return "error";
	}

	dataStructure = 4; // initialize data structure (4 = A* search w/ manhattan distance)

	insertMap(startState, counter); // install startState and counter into map

	int startPoint = findEmpty(startState); // find & assign empty tile

	Node e;  // create entrance node;

	e.point = startPoint; // empty point in starting state
	e.state = startState; // starting state
	e.depth = 0; // depth of root
	e.count = counter; // start count
	e.path.push_back("Start, ");  // insert entry path into vector
        e.cheapest = 0; // outOfPlace = misplaced tiles(0) + depth of node(0)

	aStarManhattan.push(e); // push entrance node

	int workingPoint; // declare workingPoint string
	string workingState; // declare workingState string

	// while stack is not empty
	while (!aStarManhattan.empty()) {

		curr = aStarManhattan.top();  // current node equals top of stack
		workingPoint = curr.point; // assign workingPoint to current node value
		workingState = curr.state; // assign workingState to current node value

		// check for goal state, if it matches, return working state and exit the search
		if (checkGoal(workingState)) {
			return workingState;
		}
		else {
			// do nothing
		}
			aStarManhattan.pop();  // Else, pop the the top node and start the search

			int emptyPoint = findEmpty(workingState); // find & assign empty tile to an integer

			// begin search pattern based on empty tile position
			searchPattern(emptyPoint, workingState);
	}
}

void results(string endState) {

	// anything but the GOALSTATE results in an unsuccessful search
	if (endState != GOALSTATE) {
		cout << "Solution was not found" << endl;
	}
	else {
		cout << "Search successful!" << endl;
	}

	// open a file
	outFile.open("results.csv");

	// print results to console and the file
	cout << "Starting State: " << startState << endl;
	outFile << "Starting State: " << startState << endl;
	cout << "Final State: " << curr.state << endl;
	outFile << "Final State: " << curr.state << endl;
	cout << "Search Depth: " << curr.depth << endl;
	outFile << "Search Depth: " << curr.depth << endl;
	cout << "Node Count: " << curr.count << endl;
	outFile << "Node Count: " << curr.count << endl;
	cout << "See the (results.csv) file for search path" << endl;

	// iterate the path vector and write to the file
	int i = 0;
	vector<string>::iterator it;  // for loop list iterator
	for (it = curr.path.begin(); it != curr.path.end(); ++it) {
		i++;
		if (i % 25 == 0) {
			outFile << endl;
		}
		outFile << ' ' << *it;
	}

	// close the file
	outFile.close();
}
