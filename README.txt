HOMEWORK 6: CROSSWORD BLACKOUT


NAME:  Vishaal Kasinath

COLLABORATORS AND OTHER RESOURCES:
List the names of everyone you talked to about this assignment
(classmates, TAs, ALAC tutors, upperclassmen, students/instructor via
LMS, etc.), and all of the resources (books, online reference
material, etc.) you consulted in completing this assignment.

http://www.danzig.us/cpp/recursion
http://www.cplusplus.com/reference/string/string/find/
https://stackoverflow.com/questions/2616906/how-do-i-output-coloured-text-to-a-linux-terminal/35125882#35125882


Remember: Your implementation for this assignment must be done on your
own, as described in "Academic Integrity for Homework" handout.



ESTIMATE OF # OF HOURS SPENT ON THIS ASSIGNMENT:  40



ANALYSIS OF PERFORMANCE OF YOUR ALGORITHM:
(running time order notation & concise paragraph, < 200 words)
The dimensions of the board (w and h): O(w*h)
The number of words in the dictionary: O(2^d)
The number of constraints (c): O(c)
Etc. 

Biggest preformance impact was degree of freedom, number of combinations
for selecting words from the dictionary based on given constraints. The more
constraints given with more possible combinations within, slower
the preformance. Larger the dimensions of the board, slower the
preformance. The fewer words in the dictionary, faster the preformance.
Words found in the grid and part of the dictionary, but not needed
for the final set of solutions worsen overall preformance. The algorithm I worked with
is based on string and substring matches within words in dictionary. Matches between
specific letters and characters was not considered. The number of 
combinations increase as we get closer to half the size of the word list and starts
decreasing as we go past halfway through the word list (similar to pascals triangle). 
The more the number of blacked out spaces in the puzzle, better the preformance as
there are less number of word combinations to deal with. Loading the largest sized
constraints first had a dramatic increase in performance. Recursion was used for parsing
substrings from grid and for finding paths to identify gc solutions.


DESCRIPTION OF YOUR ADDTIONAL PUZZLES/DICTIONARIES:
Briefly explain the design of the puzzles or dictionaries and what
makes them "interesting" test cases (or why they aren't interesting).

I took a commercially available puzzle that had a good combination of short and long
words. I specifically tested for sub-solutions of the main puzzle. One sub-solution 
was constraints of smaller length words, which was more run time intensive as it gave
multiple combinations of solutions. The constraint set inlcuding 4 five letter, 6 six
letter, and 4 seven letter words gave 11 possible solutions with a run time of 8ms.
I chose this puzzle because it has words in all the four corners and I wanted to test
to make sure there are no access violations when searching for strings around the edges.

SUMMARY OF PERFORMANCE OF YOUR PROGRAM ON THE PROVIDED PUZZLES AND 
ON YOUR ADDITIONAL PUZZLES/DICTIONARIES:
# of solutions & approximate wall clock running time for different
puzzles / dictionaries for a single solution or all solutions. Be
specific about which puzzle/dictionary/parameters you were timing.

Used chrono package to collect run time statistics
code commented after collecting run time stats

----------------------------------------------------------------------------------------------------------------------------------------------
Dictionary				Grid File			Avg Run Time (ms)		Num Solutions				GC	Comments
----------------------------------------------------------------------------------------------------------------------------------------------
blackout_dict1.txt		blackout1.txt				1					3				3	
blackout_dict2.txt		blackout2.txt				1					0				0	
blackout_dict1.txt		blackout3.txt				1					10				3	
blackout_dict2.txt		blackout3.txt				1					9				3	
blackout_dict1.txt		gaurdian.txt				1					0				0	No dictionary provided, used blackout_dict1.txt
nyt_mini1_dict.txt		nyt_mini1.txt				1					1				1	
nyt_mini1_dict.txt		nyt_mini1_noblack.txt			1					1				1	
nyt_mini1_dict.txt		nyt_mini2.txt				1					0				0	
nyt_mini1_dict.txt		nyt1.txt				1					0				0	
nyt_mini1_dict.txt		nyt2.txt				1					0				0	No dictionary provided, used nyt_mini1_dict.txt
reverse_ell_dict.txt	reverse_ell1.txt				1					1				1	
reverse_ell_dict.txt	reverse_ell2.txt				1					1				1	
sporcle1_dict1.txt		sporcle1_constraintsA.txt		12					1				1	
sporcle1_dict1.txt		sporcle1_constraintsB.txt		6					40				12	
sporcle1_dict2.txt		sporcle1_constraintsA.txt		6067					1				1	
sporcle1_dict2.txt		sporcle1_constraintsB.txt		9					40				12	
sporcle1_dict3.txt		sporcle1_constraintsA.txt		16					1				1	
sporcle1_dict3.txt		sporcle1_constraintsB.txt		6					40				12	
sporcle1_dict4.txt		sporcle1_constraintsA.txt		318					1				1	
sporcle1_dict4.txt		sporcle1_constraintsB.txt		47					40				12	
hindu1_dict.txt			hindu1.txt				99					1				1	Custom dictionary and puzzle
hindu1_dict.txt			hindu2.txt				5					1				1	Custom dictionary and puzzle
hindu1_dict.txt			hindu3.txt				8					125				1	Custom dictionary and puzzle
hindu4_dict.txt			hindu4.txt				1					1				1	Custom dictionary and puzzle
----------------------------------------------------------------------------------------------------------------------------------------------


MISC. COMMENTS TO GRADER:  
(optional, please be concise!)


