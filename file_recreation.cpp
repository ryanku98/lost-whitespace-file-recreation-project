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
using namespace std;

const int size = 95686; // 95686 of the 100,000 words in the dictionary do not contain non-English letters
string words[size];     // consists of all the words in the dictionary
int ranks[size];        // parallel to words[], stores respective word rankings
string compressed_string;
int doc_count = 0;
const char punctuation[] = {'.', '!', '?', ';'};
string most_likely_file = "";
int most_likely_rank = INT_MAX;

void resetFolder();
void load_dictionary(string filepath);
void merge(int l, int m, int r);
void sort(int l, int r);
void load_input(string filepath);
int binary_search(string key);
int* dynamic_stuff(string sub_string);
string alg(string line, string progress, int rank);
int isPunctuation(char character);
void createFile(string progress, int rank);


/*
 * @description:  Resets the folder that will be used to hold the potential "original" documents.
 *                If the folder exists, simply empty its contents.
 *                If the folder does not exist, create the folder.
 */
// void resetFolder()
// {
//   String* directories[] = Directory::GetDirectories("", "Output");
//   bool exists = false;
//   for(int i = 0; i < directories.length(); i++)
//   {
//     if(directories[i].compare("Output") == 0) exists = true;
//   }
//   if(exists)
//     system("exec rm -r Output/*");
//   else
//
// }

void load_dictionary(string filepath)
{
  std::ifstream dictionary(filepath);
  string word;
  int i = 0; // index of array
  // int count = 0;
  bool isWord = true;
  while(std::getline(dictionary, word))
  {
    word = word.substr(0, word.length() - 1); // eliminate invisible EOL char
    if(word.at(0) != '#') // if line is not a comment
    {
      for(int x = 0; x < word.length(); x++)  // eliminate words with non-English letters
      {
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
      if(isWord)  // only include valid words
      {
        // count++;
        words[i] = word;        // store word
        i++;                    // increment index/counter
        ranks[i] = i/100 + 1;   // store word rank
      }
      isWord = true;  // reset isWord
    }
  }
}

/*
 * @description: Merges two subarrays (defined by l and r)
 *                subarray1[l..m]
 *                subarray2[m+1..r]
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

void load_input(string filepath)
{
  std::ifstream compressed_file(filepath);
  std::getline(compressed_file, compressed_string);
  compressed_string = compressed_string.substr(0, compressed_string.length() - 1);
  // TODO: CHANGE TO WHITESPACE TRUNCATING FUNCTION
}

int binary_search(string key)
{
  // cout << "bin search start" << endl;
  int l = 0, m, r = size - 1;
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
 *
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
  cout << "Scanned up to: " << temp << endl;
  return table;
}

string alg(string line, string progress, int rank)
{
  if(line.length() == 0)
  { // base case
    doc_count++;
    createFile(progress, rank);
    return "";
  }

  cout << endl << line << " | " << progress << endl;
  int* table = dynamic_stuff(line);

  // print generated table; uncomment to read tables
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

int isPunctuation(char character)
{
  for(int i = 0; i < sizeof(punctuation); i++)
    if(character == punctuation[i]) return 2;
  return 0;
}

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

  // store most likely file
  if(rank < most_likely_rank)
  {
    most_likely_rank = rank;
    most_likely_file = filename;
  }
}

int main() //int argc, char* argv[])
{
  // resetFolder();
  system("exec rm -r Output/*");

  string dictionary_file_path = "dictionary.txt";
  load_dictionary(dictionary_file_path);

  sort(0, size - 1);
  string input_file_path = "Examples/ex4.txt";
  load_input(input_file_path);
  alg(compressed_string, "", 0);

  cout << doc_count << " files created!" << endl;

  // Identify and print most likely file
  cout << "Most likely file is " << most_likely_file << " with rank " << most_likely_rank << endl;
  cout << "More details in ranking.txt" << endl;
}
