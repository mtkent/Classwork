// ==========================================================================
// Palindrome program in C        
// Marina Kent             26/10/15
// ==========================================================================
// Prints the different words in a sentence if palindrome

// Inf2C-CS Coursework 1. Task B

//---------------------------------------------------------------------------
// C definitions for SPIM system calls
//---------------------------------------------------------------------------
#include <stdio.h>

int read_char() { return getchar(); }
void read_string(char* s, int size) { fgets(s, size, stdin); }

void print_char(int c)     { putchar(c); }   
void print_string(char* s) { printf("%s", s); }

// Check if the current character is a delimiting character
int is_delimiting_char(char ch)
{
  if(ch == ' ')			//White space
    return 1;
  else if(ch == ',')		//Comma
    return 1;
  else if(ch == '.')		//Period
    return 1;
  else if(ch == '!')		//Exclamation
    return 1;
  else if(ch == '?')		//Question mark
    return 1;
  else if(ch == '_')		//Underscore
    return 1;
  else if(ch == '-')		//Hyphen
    return 1;
  else if(ch == '(')		//Opening parentheses
    return 1;
  else if(ch == ')')		//Closing parentheses
    return 1;
  else if(ch == '\n')		//Newline (the input ends with it)
    return 1;
  else
    return 0;
}

//returns the length of a string
int strLength (char *string) {
  
  int length; 
  length = 0;
  
  //as long as there are characters, advance through the string, increase the length
  while (*string){                                     
    string++;
    length++;
  }
  return length;
}

//if a letter is uppercase, convert it to lowercase using ascii values
char toLower (char c) {
  
  if (c >= 65 && c<=90) 
    c = (c + 32);     
  
  return c;
}

//checks if a string is a palindrome
int palCheck (char *string) {
  
  int length, i, bool, char1, char2;
  char *orig, *revStr;
  
  bool = 1;   //set to return true - only changes if it finds a reason that word is not palindrome
  
  length = strLength(string); //length of the string
  
  orig = string;  //used to compare first half of string
  revStr = string; //used to compare second half of string 
  
  if (length < 2) //cannot have single letter palindrome
    return bool = 0;
  
  //will move towards the end of the string - for second half comparisons
  for (i = 0; i < (length - 1); i++) {
    revStr++;
  }

  //will compare first and second half of string
  for (i = 0; i < (length/2); i++) {  
    char1 = toLower(*orig);     //character from fist half, lowercase
    char2 = toLower(*revStr);   //character from second half, lowercase
    
    if (char1 != char2)   //if they are different, is not a palindrome
      bool = 0;
    
    orig++;  //advance up the string
    revStr--;  //move back along the string
  }
  
  return bool; 
}

//---------------------------------------------------------------------------
// MAIN function
//---------------------------------------------------------------------------

int main (void)
{
  char input_sentence[100];
  int i=0,j,k, intstr;
  char current_char;
  
  char word[20];
  int char_index, delimiting_char, noneFound;
  
  /////////////////////////////////////////////
  print_string("input: ");
  
  /* Read the input sentence. 
   * It is just a sequence of character terminated by a new line (\n) character. 
   */
  
  do {           
    current_char=read_char();
    input_sentence[i]=current_char;
    i++;
  } while (current_char != '\n');
  
  /////////////////////////////////////////////  
  
  print_string("output:\n");
  
  char_index = 0;
  noneFound = 1;   //this is a boolean that keeps track of whether any palindromes are found

  /* The loop goes over the input sentence one character at a time.
   * Looks for delimiting characters to detect the word boundries and
   * print the words on screen.
   */
  
  for(k=0; k<i; k++)  {		
    current_char = input_sentence[k];
    delimiting_char = is_delimiting_char(current_char);
    
    if(delimiting_char) {
      if (char_index > 0) {	    	//Avoids printing a blank line in case of consecutive delimiting characters.
	word[char_index++] = '\0';    	//Puts a null character so there won't be problems when palindrome checking.

	
	if (palCheck(word)) { //if palindrome, print word
	  
	  for(j=0; j<char_index; j++)  {
	    
	    print_char(word[j]);  
	  }
	  print_char ('\n');   //print a newline after printing the word
	  noneFound = 0;       //found a palindrome - no need to print out "No palindromes found."
	  
	}

	char_index = 0;
      }
    }
    else {
      word[char_index++] = current_char;
    }
  }
  
  if (noneFound)  //if there are no palindromes, print message
       print_string ("No palindromes found. \n");
  
  /////////////////////////////////////////////
  
  return 0;
}

//---------------------------------------------------------------------------
// End of file
//---------------------------------------------------------------------------
