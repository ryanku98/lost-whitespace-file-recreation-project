/*
 * @author: ryanku98 --> https://github.com/ryanku98
 * @date: 26 November 2018
 * @class: COEN179 Theory of Algorithms
 * @professor: Professor N. Shaghaghi
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <ios>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <string.h>
// #include <errno.h>
// #include <sys/stat.h>
using namespace std;

const int max_size = 100000;  // assuming 100,000 words at most
string words[max_size];       // consists of all the words in the dictionary
int ranks[max_size];          // parallel to words[], stores respective word rankings
int dictionary_size = 0;      // holds actual size of dictionary
string compressed_string;
int doc_count = 0;
const char punctuation[] = {'.', '!', '?', ';'};
string most_likely_files[3];
int most_likely_ranks[] = {INT_MAX, INT_MAX, INT_MAX};

void resetFolder();
void load_dictionary(string filepath);
void merge(int l, int m, int r);
void sort(int l, int r);
void load_input(string filepath);
int binary_search(string key);
int* dynamic_stuff(string sub_string);
void alg(string line, string progress, int rank);
int isPunctuation(char character);
void createFile(string progress, int rank);
void printLikelyFile();

/*
 * Description: Deletes old Output directory from previous runs and recreates new Output directory.
 *              If no such folder can be found (i.e. this program has never been run),
 *              system will simply indicate as such as continue.
 */
void resetFolder()
{
  system("rm -r Output");     // remove old Output directory; if DNE, outputs to console and continues
  system("mkdir -p Output");  // make new Output directory
  cout << "Emptying output folder..." << endl;
}

/*
 * Description: Loads the dictionary file used to identify sequences of character as words.
 * @params filepath The filepath of the dictionary .txt file to be loaded.
 */
void load_dictionary(string filepath)
{
  std::ifstream dictionary(filepath);
  string word;
  bool isWord = true;
  while(std::getline(dictionary, word))
  {
    // some text editors automatically add a newline character to the end of a .txt document upon save
    // this if statement deletes that newline character if it was included
    if(isprint(word.at(word.length() - 1)) == 0)
      word = word.substr(0, word.length() - 1);

    if(word.at(0) != '#')
    { // if line is not a comment
      for(int x = 0; x < word.length(); x++)
      { // eliminate words with non-English letters
        if(isprint(word.at(x)) == 0)
        {
          isWord = false;
          break;
        }
      }
      if(isWord && word.length() == 1)
      { // eliminate single-letter words that are not I, A, or a
        if(!(word.at(0) == 'I' || word.at(0) == 'A' || word.at(0) == 'a'))  isWord = false;
      }
      if(isWord)
      { // only include valid words
        words[dictionary_size] = word;        // store word
        ranks[dictionary_size] = ++dictionary_size/100 + 1;   // store word rank
      }
      isWord = true;  // reset isWord
    }
  }
}

/*
 * Description: Merges two subarrays (defined by l and r):
 *              subarray1[l..m]
 *              subarray2[m+1..r]
 * @params l Left index to identify the first element of subarray1.
 * @params m Middle index used to identify the last element of subarray1/first element of subarray2.
 * @params r Right index used to identify the last element of subarray2.
 */
void merge(int l, int m, int r)
{
  int x = m - l + 1;  // length of subarray1
  int y = r - m;      // length of subarray2
  string sub1[x];
  string sub2[y];
  int num1[x];
  int num2[y];

  // fill in subarrays with data
  for(int i = 0; i < x; i++)
  {
    sub1[i] = words[l + i];
    num1[i] = ranks[l + i];
  }
  for(int i = 0; i < y; i++)
  {
    sub2[i] = words[m + 1 + i];
    num2[i] = ranks[m + 1 + i];
  }

  int a = 0, b = 0, c = l;
  while(a < x && b < y)
  { // continuously input smallest (alphabetically first) word
    if(sub1[a].compare(sub2[b]) < 0) // if sub1[a] comes before sub2[b]
    {
      words[c] = sub1[a];
      ranks[c++] = num1[a++];
    }
    else
    {
      words[c] = sub2[b];
      ranks[c++] = num2[b++];
    }
  }
  while(a < x)
  { // input rest of sub1/num1 if any are left
    words[c] = sub1[a];
    ranks[c++] = num1[a++];
  }
  while(b < y)
  { // input rest of sub2/num2 if any are left
    words[c] = sub2[b];
    ranks[c++] = num2[b++];
  }
}

/*
 * Description: Recursive mergesort algorithm used for the dictionary.
 * @params l Left index used for mergesort.
 * @params r Right index used for mergesort.
 */
void sort(int l, int r) // mergesort
{
  if(l < r)
  {
    int m = (l + r) / 2;
    sort(l, m);     // sort first half
    sort(m + 1, r); // sort second half
    merge(l, m, r); // merge two sorted halves
  }
}

/*
 * Description: Loads the compressed input .txt file to be scanned.
 * @params filepath The filepath of the input .txt file to be loaded.
 */
void load_input(string filepath)
{
  std::ifstream compressed_file(filepath);
  std::getline(compressed_file, compressed_string);

  // some text editors automatically add a newline character to the end of a .txt document upon save
  // this if statement deletes that newline character if it was included
  if(isprint(compressed_string.at(compressed_string.length() - 1)) == 0)
    compressed_string = compressed_string.substr(0, compressed_string.length() - 1);
}

/*
 * Description: Binary search for the pre-sorted dictionary for the key.
 * @param key String (word) to be searched.
 * @returns The index in the dictionary array if the key is found, and -1 otherwise.
 */
int binary_search(string key)
{
  // cout << "bin search start" << endl;
  int l = 0, m, r = dictionary_size - 1;
  while(l <= r)
  {
    m = (l + r) / 2;

    // key comes before middle word
    if(key.compare(words[m]) < 0)
    {
      r = m - 1;
    } // key comes after middle word
    else if(key.compare(words[m]) > 0)
    {
      l = m + 1;
    } // key IS the middle word
    else return m;
  }
  return -1;
}

/*
 * Description: Searches for possible words that start with the first letter of sub_string.
 *              A table is generated to reflect the possibility of words appearing in the text.
 *              Punctuations are also identified and reflected in the table.
 *              For a more in-depth description, please refer to algorithm.txt in the same directory:
 *              https://github.com/ryanku98/lost-whitespace-file-recreation-project/blob/master/algorithm.txt
 * @param sub_string Holds the portion of the text that needs to be scanned for word occurences.
 * @returns A table in the form of an array reflecting identified words and punctuation.
 */
int* dynamic_stuff(string sub_string)
{
  int* table = new int[sub_string.length()];
  if(sub_string.length() == 0)  return table;
  int index;

  string temp = "";
  for(int i = 0; i < sub_string.length(); i++)
  {
    temp.append(sub_string, i, 1);

    index = binary_search(temp);

    // if found, indicate 1; otherwise, check and indicate whether it is proper punctuation
    if(index >= 0)  table[i] = 1;
    else            table[i] = isPunctuation(temp.at(temp.length() - 1));

    if(table[i] == 2)
    { // once first punctuation is found, find all other punctuation and exit early (reduces # of useless searches)
      while(i < sub_string.length() - 1)
        table[++i] = isPunctuation(sub_string.at(i));
      break;
    }
  }
  // cout << "Scanned up to: " << temp << endl;
  return table;
}

/*
 * Description: Recursive call that uses the table generated by dynamic_stuff() to identify possible word combinations.
 *              Future recursive calls are called on the substring containing the rest of the string after one word is
 *              found, effectively repeatedly reducing it into subproblems after a word is found.
 *              For a more in-depth description, please refer to algorithm.txt in the same directory:
 *              https://github.com/ryanku98/lost-whitespace-file-recreation-project/blob/master/algorithm.txt
 * @param line Holds the unchecked portion of the compressed text.
 * @param progress Holds the already-completed portion of the compressed text.
 * @param rank Holds the current rank of the progress made so far.
 */
void alg(string line, string progress, int rank)
{
  if(line.length() == 0)
  { // base case
    doc_count++;
    createFile(progress, rank);
    return;
  }

  int* table = dynamic_stuff(line);

  // print generated table; uncomment to read tables
  // cout << endl << line << " | " << progress << endl;
  // for(int i = 0; i < line.length(); i++)
  //   cout << table[i];
  // cout << endl;

  for(int i = 0; i < line.length(); i++)
  {
    int added_rank = 0;

    if (table[i] == 1)
    {
      added_rank = ranks[binary_search(line.substr(0, i + 1))];

      // if next character is a punctuation, include that in the progress
      if(i < line.length() - 1 && table[i + 1] == 2)  i++;

      if(progress.length() != 0)  // if this isn't first step, include whitespace
        alg(line.substr(i + 1, line.length() - i - 1), progress + " " + line.substr(0, i + 1), rank + added_rank);
      else
        alg(line.substr(i + 1, line.length() - i), line.substr(0, i + 1), added_rank);
    }
  }
}

/*
 * Description: Determines if a suspected character is one of the accepted sentence-separating punctuation characters.
 * @param character Character to be examined.
 * @returns The 2 to indicate that character is an accepted punctuation and 0 otherwise.
 */
int isPunctuation(char character)
{
  for(int i = 0; i < sizeof(punctuation); i++)
    if(character == punctuation[i]) return 2;
  return 0;
}

/*
 * Description: Creates a new .txt file with the possible answer and updates the rankings.txt file.
 * @param answer Contains a valid output to being the original content of the compressed file.
 * @param rank The rank of the output, used for updating the rankings file and determining the lowest ranked file.
 */
void createFile(string answer, int rank)
{
  string name = "Output/file" + to_string(doc_count) + ".txt";
  string filename = "file" + to_string(doc_count) + ".txt";
  std::ofstream output(name);
  output << answer << endl;
  output.close();

  cout << filename << " created!" << endl;

  // add new file to ranks file
  std::ofstream ranks;
  ranks.open("Output/rankings.txt", std::ios::app);
  ranks << filename << ":\t" << rank << "\r\n" << endl;
  ranks << endl;
  ranks.close();

  // store most likely files
  if(rank < most_likely_ranks[0])
  { // if lowest, shift top 2 down, store in top
    most_likely_ranks[2] = most_likely_ranks[1];
    most_likely_ranks[1] = most_likely_ranks[0];
    most_likely_ranks[0] = rank;
    most_likely_files[2] = most_likely_files[1];
    most_likely_files[1] = most_likely_files[0];
    most_likely_files[0] = filename;
  }
  else if(rank < most_likely_ranks[1])
  { // if second lowest, shift 2nd down, store in middle
    most_likely_ranks[2] = most_likely_ranks[1];
    most_likely_ranks[1] = rank;
    most_likely_files[2] = most_likely_files[1];
    most_likely_files[1] = filename;
  }
  else if(rank < most_likely_ranks[2])
  { // if third lowest, store in bottom
    most_likely_ranks[2] = rank;
    most_likely_files[2] = filename;
  }
}

/*
 * Description: Prints to console the name(s) of the file(s) with the highest likelihood(s) of being the original file.
 *              If no files were created, it will indicate as such.
 */
void printLikelyFile()
{
  if(doc_count > 0) cout << endl << "Top files of highest probability:" << endl;
  else
  {
    cout << "No possible original files were found!" << endl;
    return;
  }

  for(int i = 0; i < sizeof(most_likely_ranks) / sizeof(most_likely_ranks[0]); i++)
  {
    if(most_likely_ranks[i] != INT_MAX)
      cout << i + 1 << ": " << most_likely_files[i] << " with rank " << most_likely_ranks[i] << endl;
  }
  cout << "More details in Output/ranking.txt" << endl;
}

/*
 * Description: Executes the necessary functions to produce the desired output files.
 * Assumptions: argv[1] contains the filepath for the dictionary.
 *              argv[2] contains the filepath for the compressed .txt file.
 */
int main(int argc, char* argv[])
{
  if(argc != 3)
  { // check number of inputs
    cout << "ERROR: Could not input files." << endl << "Exiting..." << endl;
    exit(EXIT_FAILURE);
  }
  if(argv[1] == NULL)
  { // check dictionary filepoth
    cout << "ERROR: No dictionary .txt file filepath inputted." << endl << "Exiting..." << endl;
    exit(EXIT_FAILURE);
  } else load_dictionary(argv[1]);
  if(argv[2] == NULL)
  { // check input file path
    cout << "ERROR: No input .txt file filepath inputted." << endl << "Exiting..." << endl;
    exit(EXIT_FAILURE);
  } else load_input(argv[2]);

  resetFolder();
  sort(0, dictionary_size - 1);
  alg(compressed_string, "", 0);

  // Identify and print most likely file
  printLikelyFile();

  exit(EXIT_SUCCESS);
}