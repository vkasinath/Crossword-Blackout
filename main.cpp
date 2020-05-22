#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
//#include <chrono>

#include "words.h"

// intialize constant variables used in the program
const unsigned int CW_MIN_LEN = 3;
const std::string ACROSS = "ACROSS";
const std::string DOWN = "DOWN";

// function to read input arguments
int GetInputArgs(const int inc, char* in[], 
				std::string &dict_file, 
				std::string &grid_file, 
				std::string &sol_mode, 
				std::string &out_mode, 
				std::string &gc_ind);

// function to load dictionary words from file into vec_dict
int LoadDict(std::string fileName, std::vector<std::string> & vec_dict);

// function to load grid and constraints from file into vec_grid and vec_cons
int LoadGrid(std::string fileName, std::vector < std::vector<char> >  & vec_grid, std::vector < std::vector<int> >  & vec_cons);

// utility function to transpose grid (rows become columns)
void Transpose(std::vector < std::vector<char> >  & vec_grid, std::vector < std::vector<char> >  & vec_tgrid);

// utility function to create a blank grid
std::vector< std::vector<char> > BlankGrid(const std::vector <unsigned int>  &g_sz);

// function to recursively search a string for all different substrings found in vec_dict(grid row made into a string)
void FindRecursive(std::string &word, const unsigned int &row, unsigned int &sp, unsigned int &len,
				   const std::string &stype, std::vector<Words> &foundw, const std::vector<std::string> & vec_dict);

// function to read grid rows and call the recursive function
void FindHorizontal(const std::vector < std::vector<char> >  &vec_grid, std::string &stype,
					std::vector<Words> &foundw, const std::vector<std::string> &vec_dict);

// function to read grid columns and call the recursive function
void FindVertical(const std::vector < std::vector<char> > &vec_tgrid, std::string &stype, 
				  std::vector<Words> &foundw, const std::vector<std::string> &vec_dict);

// function to build all valid combinations of words to satisfy the grid constraints
std::vector< std::vector<Words> > BuildWordList(const std::vector< std::vector<int> > &vec_cons, std::vector<Words> foundw,
												const std::vector<unsigned int> &g_sz, const std::vector<std::string> &vec_dict);

// function to build a grid for each set of answer combinations
void BuildAnswerGrid(std::vector< std::vector<char> > &b, const std::vector<Words> &a);

// function to print final asnwer grid (based on input parameters and if needed calling print board)
void PrintAnswers(const std::vector< std::vector<Words> > &word_answ, const std::vector<unsigned int> &g_sz,
				  const std::string &sol_mode, const std::string &out_mode, const std::string &gc_ind,
				  const std::vector<bool> &is_gc);

// utility function to print a grid
void PrintBoard(const std::vector<unsigned int> &g_sz, const std::vector<Words> &a);

// utility function to validate if a given set of answers satisfies constraints
void ValidateAnswers(const std::vector< std::vector<int> > &vec_cons, const std::vector<unsigned int> &g_sz,
				 	 const std::vector<std::string> &vec_dict, std::vector< std::vector<Words> > &word_answ);

// utility function to parse a string and split it into substrings based on the delimiter '#'
std::vector<std::string> SplitWords(const std::string &in_str);

// utility function to search for a word in the dictionary
bool FoundInDict(const std::string s, std::vector<std::string> vec_dict);

// utility function to verify if a grid is gc
void CheckGC(const std::vector< std::vector<Words> > &word_answ, std::vector<bool> &is_gc);

// utility function to identify if a set of words has any excpetions that will make the set invalid
bool CheckWordAdd(const std::vector<Words> &ssT, const Words &fwi, const std::vector<std::string> &vec_dict, const std::vector<unsigned int> &g_sz);

// utility recursive function to collect all connected words
bool FindConnected(const std::vector<Words> &Ans, std::vector<Words> &w, unsigned int ntries);

// utility function to check if w2 is a substring of w1 and w1+w2 abutt each other
bool IsSubstring(const Words &w1, const Words &w2);

/* Main Function */
int main( int argc, char* argv[]) {

	//record start time
	//auto start = std::chrono::steady_clock::now();

	//First check for valid number of arguments
	if (argc < 5) {
		std::cerr << "not enough input arguments";
		return 1;
	}

	std::string dict_file, grid_file, sol_mode, out_mode, gc_ind;
	
	int ac;
	ac = GetInputArgs(argc, argv, dict_file, grid_file, sol_mode, out_mode, gc_ind);
	if (ac > 0){
		if (ac == 10){
			std::cerr << "incorrect solution mode entered" << std::endl;
			return ac;
		}
		else if (ac == 20){
			std::cerr << "incorrect output mode entered" << std::endl;
			return ac;
		}
		else if (ac == 30){
			std:: cerr << "incorrect gc given" << std::endl;
			return ac;
		}
	}

	std::vector<std::string> vec_dict;
	int ld = LoadDict(dict_file, vec_dict);
	if(ld != 0)
		return ld;

	std::vector < std::vector<char> > vec_grid;
	std::vector < std::vector<int> > vec_cons;
	int lg = LoadGrid(grid_file, vec_grid, vec_cons);
	if (lg != 0)
		return lg;

	std::sort(vec_cons.rbegin(), vec_cons.rend());	

	//x = number of rows
	//y = number of columns
	unsigned int x = vec_grid.size();
	unsigned int y = vec_grid[0].size();

	//vector to hold grid size
	std::vector<unsigned int> g_sz {0,x,0,y};

	std::vector < std::vector<char> > vec_tgrid;
	Transpose(vec_grid, vec_tgrid);

	// foundw will be a vector of Words that is found in the dictionary after searching through the grid recursively
	// both in the ACROSS and DOWN orientation
	std::vector<Words> foundw;
	std::string stype;

	FindHorizontal(vec_grid, stype, foundw, vec_dict);
	FindVertical(vec_tgrid, stype, foundw, vec_dict);

	// word_answ will hold all possible answer combinations
	// each answer combination will be a set of words
	std::vector < std::vector<Words> > word_answ;
	word_answ = BuildWordList(vec_cons, foundw, g_sz, vec_dict);

	// validate if word_answ answer sets satisfy the constraints
	// answer sets that do not satisfy constraints are removed in function
	ValidateAnswers(vec_cons, g_sz, vec_dict, word_answ);

	// is_gc is a parallel vector to word_answ indicating if its correpsonding answer set is gc or not
	// is_gc.size() will be the same as wor_answ.size() filled with true/false
	std::vector<bool> is_gc(word_answ.size(), false);
	CheckGC(word_answ, is_gc);


	PrintAnswers(word_answ, g_sz, sol_mode, out_mode, gc_ind, is_gc);


	//auto finish = std::chrono::steady_clock::now();
	//std::cout << "Elapsed Time (ms) = " << std::chrono::duration_cast<std::chrono::milliseconds> (finish - start).count() << std::endl;

	return 0;

}

// ALL REQUIRED FUNCTION DEFINITIONS

int GetInputArgs(const int inc, char* in[], std::string &dict_file, std::string &grid_file, std::string &sol_mode, std::string &out_mode, std::string &gc_ind){

	dict_file = in[1];
	grid_file = in[2];
	sol_mode = in[3];
	out_mode = in[4];
	if (inc >= 6)
		gc_ind = in[5];
	else
		gc_ind = "";

	if (sol_mode != "one_solution" && sol_mode != "all_solutions")
		return 10;
	if (out_mode != "count_only" && out_mode != "print_boards")
		return 20;
	if (gc_ind != "gc" && gc_ind.length() > 0)
		return 30;

	return 0;

}

int LoadDict(std::string fileName, std::vector<std::string> & vec_dict)
{
	// Open the File
 	std::ifstream istr(fileName);
	std::string str = "";

	if (istr.is_open()){
		while (istr >> str){
			vec_dict.push_back(str);
		}
		istr.close();

		return 0;
	}
	else{
		std::cerr << "unable to open dictionary file " << fileName << std::endl;
		return 2;
	}

}

int LoadGrid(std::string fileName, std::vector < std::vector<char> >  & vec_grid, std::vector < std::vector<int> >  & vec_cons){
	// for each word make it into a temp vector of char
	// push each vector of char into vec_grid

	// Open the File
 	std::ifstream istr(fileName);
	std::string str = "";

	if (istr.is_open()){
		while (istr >> str){
			if (str[0] == '+'){
				int wl = std::stoi(str);
				bool found = false;
				for (unsigned int i=0; i < vec_cons.size(); i++){
					if (vec_cons[i][0] == wl){
						vec_cons[i][1]++;
						found = true;
						break;
					}
				}
				if (!found)
					vec_cons.push_back({wl, 1});
			}
			else if (str[0] != '!' && str[0] != '+'){
				std::vector <char> temp;
				for (unsigned int i=0; i <str.length(); i++)
					temp.push_back(str[i]);
				vec_grid.push_back(temp);
			}
		}
		istr.close();

		return 0;
	}
	else{
		std::cerr << "unable to open grid file " << fileName << std::endl;
		return 3;
	}
}

void Transpose(std::vector < std::vector<char> >  & vec_grid, std::vector < std::vector<char> >  & vec_tgrid){
	for (unsigned int i=0; i < vec_grid[0].size(); i++){
		std::vector <char> temp;
		for (unsigned int j=0; j < vec_grid.size(); j++)
			temp.push_back(vec_grid[j][i]);
		vec_tgrid.push_back(temp);
	}
}

 std::vector < std::vector<char> > BlankGrid(const std::vector <unsigned int> &g_sz){
	
 	//create a blank answer grid (filled with #)
  	std::vector <char> g(g_sz[3], '#');                           // fill one row of g_sz[3] columns
  	std::vector< std::vector <char> > vec_bgrid(g_sz[1], g);   // fill g_sz[1] rows
  	return vec_bgrid;

}

void FindRecursive(std::string &word, const unsigned int &row, unsigned int &sp, unsigned int &len,
					 const std::string &stype, std::vector<Words> &foundw, const std::vector<std::string> &vec_dict){

	std::string ss = word.substr(sp, len);
	bool l_fd = FoundInDict(ss, vec_dict);
	if (l_fd){
		if (stype == ACROSS)
			foundw.push_back(Words(ss, stype, len, row, sp));				//if word is across, add it to the foundw vector
		else
			foundw.push_back(Words(ss, stype, len, sp, row));				//if word is down, add it to foundw, but switch sp and row	
	}


	//substring not in the dictionary as is
	if(sp > word.length() - CW_MIN_LEN)
		return;

	if(sp + len >= word.length()){
		sp++;
		len = CW_MIN_LEN;
		FindRecursive(word, row, sp, len, stype, foundw, vec_dict);
	}
	else{
		len++;
		FindRecursive(word, row, sp, len, stype, foundw, vec_dict);
	}
}

void FindHorizontal(const std::vector < std::vector<char> > &vec_grid, std::string &stype, std::vector<Words> &foundw, const std::vector<std::string> &vec_dict){
	
	unsigned int sp, row, len;

	for (unsigned int i=0; i < vec_grid.size(); i++){
		std::string word(vec_grid[i].begin(), vec_grid[i].end());
		row = i;
		sp = 0;
		len = CW_MIN_LEN;
		stype = ACROSS;

		FindRecursive(word, row, sp, len, stype, foundw, vec_dict);
	}

}

void FindVertical(const std::vector < std::vector<char> > &vec_tgrid, std::string &stype, std::vector<Words> &foundw, const std::vector<std::string> &vec_dict){
	
	unsigned int sp, row, len;

	for (unsigned int i=0; i < vec_tgrid.size(); i++){
		std::string word(vec_tgrid[i].begin(), vec_tgrid[i].end());
		row = i;
		sp = 0;
		len = CW_MIN_LEN;
		stype = DOWN;

		FindRecursive(word, row, sp, len, stype, foundw, vec_dict);
	}

}

std::vector< std::vector<Words> > BuildWordList(const std::vector< std::vector<int> > &vec_cons, std::vector<Words> foundw,
	 											const std::vector<unsigned int> &g_sz, const std::vector<std::string> &vec_dict){

	std::vector< std::vector<Words> > sset;
	std::vector<Words> empty;
	sset.push_back(empty);

	// if no constraints, no answers possible
	// return empty set
	if (vec_cons.size() == 0){
		sset.clear();
		return sset;
	}

	unsigned int tot_size = 0;

	// filter for all the words that satisfy the constraints
	for (unsigned int c=0; c<vec_cons.size(); c++){

		std::vector<Words> filt_words;
		for (unsigned int i=0; i < foundw.size(); i++){
			if ((int)foundw[i].wlen == vec_cons[c][0])
				filt_words.push_back(foundw[i]);
		}

		tot_size = tot_size + vec_cons[c][1];

		//create all possible combinations
		//possible combinations are n!/(m!*(n-m)!)

		for(unsigned int i=0; i<filt_words.size(); i++){
			std::vector< std::vector<Words> > ssTemp = sset;

			std::vector <bool> l_good_fwa(ssTemp.size(), true);

			for(unsigned int j=0; j<ssTemp.size(); j++){
				l_good_fwa[j] = CheckWordAdd(ssTemp[j], filt_words[i], vec_dict, g_sz);
				if (l_good_fwa[j])
					ssTemp[j].push_back(filt_words[i]);
			}

			for (unsigned int j=0; j<ssTemp.size(); j++){
				if(ssTemp[j].size() <= tot_size && l_good_fwa[j])
					sset.push_back(ssTemp[j]);

			}

		}

		//explaining algorithm
		//from all possible combinations, we retain those which has a size = sum(constraints-word-count). 
		//if out word list is [ONE,TWO,THREE,FOUR,FIVE,SIX]
		//
		//constraint 1 = 2 three-letter-words, wordlist for constraint 1 = [ONE,TWO,SIX]
		//before loop, sset will be: {[ONE], [TWO], [SIX], [ONE,TWO], [ONE,SIX], [TWO,SIX], [ONE,TWO,SIX]}
		//after the loop below, sset will now be: {[ONE,TWO], [ONE,SIX], [TWO,SIX]}
		//
		//constraint 2 = 1 four-letter-word, wordlist for constraint 2 = [FOUR,FIVE]
		//before the loop sset will be: {[ONE,TWO,FOUR], [ONE,TWO,FIVE], [ONE,TWO,FOUR,FIVE]
		//								 [ONE,SIX,FOUR], [ONE,SIX,FIVE], [ONE,SIX,FOUR,FIVE]
		//								 [TWO,SIX,FOUR], [TWO,SIX,FIVE], [TWO,SIX,FOUR,FIVE]}
		//after the loop below, sset will be: {[ONE,TWO,FOUR], [ONE,TWO,FIVE], [ONE,SIX,FOUR]
		//									   [ONE,SIX,FIVE], [TWO,SIX,FOUR], [TWO,SIX,FIVE]}


		unsigned int k=0;
		while (k < sset.size()){

			if (sset[k].size() != tot_size){
				sset[k] = sset.back();
				sset.pop_back();
			}
			else
				k++;
		}

		// in-place validation of answer sets required only for large grids
		// for smaller grids validation will be done in main
		if(foundw.size() > 4*CW_MIN_LEN)
			ValidateAnswers(vec_cons, g_sz, vec_dict, sset);

	}

	return sset;
}

void PrintAnswers(const std::vector< std::vector<Words> > &word_answ, const std::vector<unsigned int> &g_sz,
					 const std::string &sol_mode, const std::string &out_mode, const std::string &gc_ind, const std::vector<bool> &is_gc){

	// printing all non gc solutions
	if (gc_ind != "gc"){
		if (out_mode == "count_only"){
			if (sol_mode == "all_solutions")
				std::cout << "Number of solution(s): " << word_answ.size() << std::endl;
			else if (sol_mode == "one_solution"){
				if (word_answ.size() == 0)
					std::cout << "Number of solution(s): 0" << std::endl;
				else
					std::cout << "Number of solution(s): 1" << std::endl;
			}
		}
		else if (out_mode == "print_boards"){
			if (sol_mode == "all_solutions"){
				std::cout << "Number of solution(s): " << word_answ.size() << std::endl;
				for (unsigned int i=0; i < word_answ.size(); i++)
					PrintBoard(g_sz, word_answ[i]);
			}
			else if (sol_mode == "one_solution"){
				std::cout << "Number of solution(s): " << (bool)word_answ.size() << std::endl;
				if (word_answ.size() != 0)
					PrintBoard(g_sz, word_answ[0]);
			}
		}
	}

	// printing gc solutions only
	else{

		unsigned int gc_count = 0;
		int gc_answ = -1;
		for(unsigned int k=0; k <is_gc.size(); k++){
			if(is_gc[k]){
				gc_count++;
				gc_answ = k;
			}
		}

		if (out_mode == "count_only"){
			if (sol_mode == "all_solutions")
				std::cout << "Number of solution(s): " << gc_count << std::endl;
			else if (sol_mode == "one_solution"){
				if (gc_count == 0)
					std::cout << "Number of solution(s): 0" << std::endl;
				else
					std::cout << "Number of solution(s): 1" << std::endl;
			}
		}
		else if (out_mode == "print_boards"){
			if (sol_mode == "all_solutions"){
				std::cout << "Number of solution(s): " << gc_count << std::endl;
				for (unsigned int i=0; i < word_answ.size(); i++){
					if(is_gc[i])
						PrintBoard(g_sz, word_answ[i]);
				}
			}
			else if (sol_mode == "one_solution"){
				if(gc_answ == -1)
					std::cout << "Number of solution(s): 0" << std::endl;
				else{
					std::cout << "Number of solution(s): 0" << std::endl;
					PrintBoard(g_sz, word_answ[gc_answ]);
				}
			}
		}

	}
}

void BuildAnswerGrid(std::vector< std::vector<char> > &b, const std::vector<Words> &a){
	for (unsigned int j=0; j < a.size(); j++){

		unsigned int x = a[j].wx, y = a[j].wy;
		unsigned int len = a[j].wlen;
		std::string wtype = a[j].wtype;
		std::string word = a[j].word; 

		// fill grid b with letters from each answer word according to its (x,y) position
		if (wtype == ACROSS){
			for (unsigned int k=0; k < len; k++)
				b[x][k+y] = word[k];
		}
		else if (wtype == DOWN){
			for (unsigned int k=0; k < len; k++)
				b[k+x][y] = word[k];
		}
	}
}

void PrintBoard(const std::vector<unsigned int> &g_sz, const std::vector<Words> &a){

	std::vector <std::vector<char> > b = BlankGrid(g_sz);

	BuildAnswerGrid(b, a);

	//const std::string fmt ("\033[1;31m");
	//const std::string reset ("\033[0m");

	std::cout << "Board:" << std::endl;
	for (unsigned int i=0; i < b.size(); i++){
		for (unsigned int j=0; j < b[i].size(); j++)
			std::cout << b[i][j];
		std::cout << std::endl;
	}
}

void ValidateAnswers(const std::vector< std::vector<int> > &vec_cons, const std::vector<unsigned int> &g_sz,
				 const std::vector<std::string> &vec_dict, std::vector< std::vector<Words> > &word_answ){

	unsigned int i=0;
	while (i <word_answ.size()){

		std::vector< std::vector<char> > b = BlankGrid(g_sz);

		// for each answer word_answ[i], build a grid
		BuildAnswerGrid(b, word_answ[i]);

		std::vector< std::vector<char> > tgrid;
		std::vector<std::string> grid_words;
		std::vector<std::string> t;

		bool nf_grid = false;

		// transpose answer grid, used for checking columns
		Transpose(b, tgrid);

		// break each grid row into substrings delimited by '#'
		for(unsigned int j=0; j < b.size(); j++){
			std::string s(b[j].begin(), b[j].end());
			t = SplitWords(s);

			// if the string is not found in the dictionary, answer set is invalid
			// flag and break for deletion
			for (unsigned int k=0; k < t.size(); k++){
				if (FoundInDict(t[k], vec_dict))
					grid_words.push_back(t[k]);
				else{
					nf_grid = true;
					break;
				}
			}
		}

		//break each grid column into substrings delimited by '#'
		for(unsigned int j=0; j < tgrid.size(); j++){
			std::string s(tgrid[j].begin(), tgrid[j].end());
			t = SplitWords(s);

			// if the string is not found in the dictionary, answer set is invalid
			// flag and break for deletion
			for (unsigned int k=0; k < t.size(); k++){
				if (FoundInDict(t[k], vec_dict))
					grid_words.push_back(t[k]);
				else{
					nf_grid = true;
					break;
				}
			}
		}

		// check if the same word is found twice in the grid
		// if so, answer is invalid, flag and break for deletion
		bool dup_words = false;
		for(unsigned int j=0; j <grid_words.size(); j++){
			for(unsigned int k=j+1; k < grid_words.size(); k++){
				if (grid_words[j]==grid_words[k]){
					dup_words = true;
					break;
				}
			}

			if(dup_words == true)
				break;
		}

		// if any of the flags are triggered, delete the answer set
		if( (grid_words.size() != word_answ[i].size()) || dup_words || nf_grid)
			word_answ.erase(word_answ.begin()+i);
		else
			i++;

	}

}

std::vector<std::string> SplitWords(const std::string &in_str){

	std::vector<std::string> t;
	std::istringstream s(in_str);
	std::string temp;

	while(std::getline(s, temp, '#')){
		if(temp.length() >= CW_MIN_LEN-1)
			t.push_back(temp);
	}

	return t;
}

bool FoundInDict(const std::string s, std::vector<std::string> vec_dict){

	bool found = false;
	for (unsigned int i=0; i < vec_dict.size(); i++){
		if (s == vec_dict[i]){
			found = true;
			break;
		}
	}

	return found;
}

void CheckGC(const std::vector< std::vector<Words> > &word_answ, std::vector<bool> &is_gc){

	for(unsigned int i=0; i < word_answ.size(); i++){
		std::vector<Words> Ans = word_answ[i];
		if (Ans.size() <= 1){
			is_gc[i] = false;
			continue;
		}
		std::vector<Words> w = {Ans[0]};
		unsigned int n_tries = 0;
		is_gc[i] = FindConnected(Ans, w, n_tries);

	}

}

bool FindConnected(const std::vector<Words> &Ans, std::vector<Words> &w, unsigned int ntries){

	//recursively update vector w with words that are linked to the current words in the vector w
	//if our vector gets filled up, we have linkage
	//if after n-tries (where n = number of words) - our vector does not contain all the words
	//there is missing linkage between one or more words
	// we have all the words having linked to each other
	if (w.size() >= Ans.size() )
		return true;
	// after going through all words in Ans, there are missing words; words with no linkage
	if (ntries > Ans.size() && w.size() < Ans.size())
		return false;

	// build linkage between words that intersect in different directions (DOWN & ACROSS)
	// find DOWN x ACROSS intersections
	// if w1 = DOWN, and w2 = ACROSS add w2 to vector w, only if w2 is new add
	for (unsigned int i = 0; i < w.size(); i++){
		for (unsigned int j=0; j < Ans.size(); j++){
			Words w1 = w[i];
			Words w2 = Ans[j];
	// same word... go to next in answer list
			if (w1 == w2)
				continue;
			if ((w1.wtype == DOWN && w2.wtype == ACROSS)
			&&  (w1.wy >= w2.wy && w1.wy <= w2.wy+w2.wlen)
			&&  (w2.wx >= w1.wx && w2.wx <= w1.wx+w1.wlen)){

				if (std::find(w.begin(), w.end(), w2) == w.end())
	 				w.push_back(w2);
			} 

			if ((w1.wtype == ACROSS && w2.wtype == DOWN)
			&&  (w2.wy >= w1.wy && w2.wy <= w1.wy+w1.wlen)
			&&  (w1.wx >= w2.wx && w1.wx <= w2.wx+w2.wlen)){

				if (std::find(w.begin(), w.end(), w2) == w.end())
					 w.push_back(w2);
			} 
		}
	}

	// Build linkage between adjacent words (both words in DOWN direction)
	// find DOWN x DOWN adjacent
	// if w1 = DOWN, and w2 = DOWN add w2 to vector w, only if w2 is new add
	for (unsigned int i = 0; i < w.size(); i++){
		for (unsigned int j=0; j < Ans.size(); j++){
			Words w1 = w[i];
			Words w2 = Ans[j];
			// same word... go to next in answer list
			if (w1 == w2)
				continue;
			if ((w1.wtype == DOWN && w2.wtype == DOWN)
			&&  (std::abs((int)w1.wy - (int)w2.wy) == 1)
			&&  ( (w2.wx >= w1.wx && w2.wx <= w1.wx+w1.wlen)
			|| (w1.wx >= w2.wx && w1.wx <= w2.wx+w2.wlen))){

				if (std::find(w.begin(), w.end(), w2) == w.end())
	 				w.push_back(w2);
			} 
		}
	}

	// Build linkage between adjacent words (both words in ACROSS direction)
	// find ACROSS x ACROSS adjacent
	// if w1 = ACROSS, and w2 = ACROSS add w2 to vector w, only if w2 is new add
	for (unsigned int i = 0; i < w.size(); i++){
		for (unsigned int j=0; j < Ans.size(); j++){
			Words w1 = w[i];
			Words w2 = Ans[j];
			// same word... go to next in answer list
			if (w1 == w2)
				continue;
			if ((w1.wtype == ACROSS && w2.wtype == ACROSS)
			&&  (std::abs((int)w1.wx - (int)w2.wx) == 1)
			&&  ( (w2.wy >= w1.wy && w2.wy <= w1.wy+w1.wlen)
			|| (w1.wy >= w2.wy && w1.wy <= w2.wy + w2.wlen))){

				if (std::find(w.begin(), w.end(), w2) == w.end())
	 				w.push_back(w2);
			} 
		}
	}

	ntries++;
	return FindConnected(Ans, w, ntries);
}

bool CheckWordAdd(const std::vector<Words> &ssT, const Words &fwi, const std::vector<std::string> &vec_dict, const std::vector<unsigned int> &g_sz){

	// dont have to check for small grids, catch them in final ValidateAnswer in main
	if (std::max(g_sz[1], g_sz[3]) < 3*CW_MIN_LEN)
		return true;

	// check by if adding the word, we have a valid grid formation
	// if yes return true, else return false

		// if new word is a substring of existing words, return false
	for(unsigned int i =0; i < ssT.size(); i++){
		Words w1 = ssT[i];
		if (IsSubstring(w1, fwi))
			return false;
	}

	// create a grid with the asnwer set and the new word and look for grid violations
	// if grid rules are not followed, return false
	std::vector<std::string> grid_words;
	std::vector<std::string> t;
	std::vector<Words> temp = ssT;

	temp.push_back(fwi);

	std::vector< std::vector<char> > b = BlankGrid(g_sz);

	// for temporary answer set word_answ[i], build a grid
	BuildAnswerGrid(b, temp);

	std::vector< std::vector<char> > tgrid;
	// transpose answer grid, used for checking columns
	Transpose(b, tgrid);

	bool nf_grid = false;

	// break each grid row into substrings delimited by '#'
	if(fwi.wtype == DOWN){
		for(unsigned int j=0; j < b.size(); j++){
			std::string s(b[j].begin(), b[j].end());
			t = SplitWords(s);

			// if the string is not found in the dictionary, answer set is invalid
			// flag and break for deletion
			for (unsigned int k=0; k < t.size(); k++){
				if (FoundInDict(t[k], vec_dict))
					grid_words.push_back(t[k]);
				else{
					nf_grid = true;
					break;
				}
			}
		}
	}


	//break each grid column into substrings delimited by '#'
	if(fwi.wtype == ACROSS){
		for(unsigned int j=0; j < tgrid.size(); j++){
			std::string s(tgrid[j].begin(), tgrid[j].end());
			t = SplitWords(s);

			// if the string is not found in the dictionary, answer set is invalid
			// flag and break for deletion
			for (unsigned int k=0; k < t.size(); k++){
				if (FoundInDict(t[k], vec_dict))
					grid_words.push_back(t[k]);
				else{
					nf_grid = true;
					break;
				}
			}
		}
	}

	// check if the same word is found twice in the grid
	// if so, answer is invalid, flag and break for deletion
	bool dup_words = false;
	for(unsigned int j=0; j <grid_words.size(); j++){
		for(unsigned int k=j+1; k < grid_words.size(); k++){
			if (grid_words[j]==grid_words[k]){
				dup_words = true;
				break;
			}
		}

		if(dup_words == true)
			break;
	}

	// check for grid condition
	// if not satisfied return false
	if (dup_words || nf_grid)
		return false;
	else
		return true;

}

bool IsSubstring(const Words &w1, const Words &w2){
	
	// two ACROSS words are substrings
    if (w1.wtype == ACROSS && w2.wtype == ACROSS && w1.wx == w2.wx && (w1.word.find(w2.word) != std::string::npos) )
    	return true;

	// two DOWN words are substrings
	else if (w1.wtype == DOWN && w2.wtype == DOWN && w1.wy == w2.wy && (w1.word.find(w2.word) != std::string::npos) )
      	return true;

	// two ACROSS words are abutting each other
	else if (w1.wtype == ACROSS && w2.wtype == ACROSS && w1.wx == w2.wx && (w1.wy+w1.wlen == w2.wy) )
		return true;

	// two DOWN words are abutting each other
	else if (w1.wtype == DOWN && w2.wtype == DOWN && w1.wy == w2.wy && (w1.wx+w1.wlen == w2.wx) )
		return true;

	else
		return false;
}