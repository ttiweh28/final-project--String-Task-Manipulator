#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/* Determines whether the string s1 starts with s2 */
int starts_with(char *s1, char *s2) {
    if (strncmp(s1, s2, strlen(s2)) == 0) {
        return 1;
    } else {
        return 0;
    }
}

/* 	function to double the string 'str'
	it takes in a string 'str' to be doubled
    it return the string doubled or NULL if the string str supplied is empty ("") or if NULL is passed
*/
const char *double_string(const char *str) {

    /* check if the string is empty and return NULL if true */
    if(*str == 0){
        return NULL;
    }
    /* check if the string is NULL and return NULL */
    if(str == NULL){
        return NULL;
    }

    char *str_duplicate = strdup(str);   //create a duplicate copy of the provided string

    strcat((char *)str, (const char *)str_duplicate);   //concatenate it to the previous str

    return str;
}

/* 	function to reverse the string
	it returns NULL if the string 'str' is empty or NULL is passed to it or the string 'str' in the reverse order
*/
char *reverse_string(char *str) {

	//check for null
	if(str == 0){
		return NULL;
	}
	//skip empty string
	if(*str == 0){
		return NULL;
	}

	//obtain the range of the string
	char *beginning = str;
	char *end = beginning + strlen(str) - 1; //remove the null terminater '\0'
	char temp;	//for swapping

	//as long as end is greater than beginning continue to reverse
	while(end > beginning){

		//swap
		temp = *beginning;
		*beginning = *end;
		*end = temp;

		//go to the next position
		++beginning;
		--end;
	}

	return str;
}

/* 	function to remove a series of characters specified in the positions array 'positions[]'
    it takes the pointer to a character string 'str' , the number of elements in the positions 'positions_size'
	array and the positions array which contain the positions which are to be removed returns the
	string 'str' without the characters at 'positions' specified in the positions arry
    or NULL when str is NULL;
*/
char *delete_characters(char *str, int positions[], int positions_size) {
	if(str == NULL) {
		return NULL;		//if the string 'str' is empty return NULL
	}

	int length = strlen(str);	//number of characters in the  string 'str'
	char buffer[length];		//create a buffer of the same size as the string where u want to store the characters that you need

	int index = 0;				//counter to loop through the buffer
	for(int i = 1; i <= length; ++i){
		bool keep = true;		//boolean to use to decide whether the character at the specified position is needed or not
		for(int j = 0; j < positions_size; ++j){
			if(i == positions[j]){
				keep = false;	//set keep to false and skip this character if it is at the unwanted position
				break;
			}
		}
		//if the character at the position i in the str is wanted then copy it to the buffer
		if(keep){
			buffer[index] = str[i-1];
			++index;
		}
	}

	buffer[index] = '\0';			//terminate the buffer
	strcpy(str, buffer);			//copy the buffer back to the string 'str'

	return str;
}

/* 	function to replace a specified character from the string with another supplied character 'new_char'
	returns the string 'str' with the character at 'position' replaced with 'new_char'
    and or NULL when pos > length of str
*/
char *replace_character(char *str, unsigned char new_char, int position) {
    char buffer[strlen(str)];		//create a buffer of the same size as the string where u want to store the characters that you need
	int counter = 1;                //counter to go through the entire string 'str'
	int pos = 0;                    //tracks the position in the buffer

	/*	check if the position is less than or greater than the number of characters in the str
		i.e. strlen(str) >= position
	*/

	if(position > strlen(str)){
		return NULL;
	}
	else if(position < 0){
		return NULL;
	}

	/* loop through the str and identify the character that is to be replaced */
	while(*str != '\0'){			//as long as the string has more characters
		if(counter != position){
			buffer[pos] = *str;	//add the character to the buffer
		}
		else{
			buffer[pos] = new_char;		//replace that character at the required position
		}

		*str++;
		pos++;
		counter++;
	}
	buffer[pos] = '\0';		//terminate the string

	strcpy(str,buffer);		//copy the buffer to the str

	return str;
}

/*	function to replace a number of different character in the string at specified positions using the replace_character() function
	it takes in the string 'str' some of whose characters are to be replaced, an array having the new characters 'new_chars' , the
	size number of positions 'number_pos' and an array having the positions 'positions'
	it returns NULL if the string 'str' is null or the string 'str' whose characters at the specified positions have been modified
*/
char *replace_characters(char *str, char new_chars[], int positions[], int number_pos){

	/* check that the string 'str' is empty  */
	if(str == NULL){
		return NULL;
	}

	int pos = 0;	//position in a string str

	while(pos < number_pos){
		str = replace_character(str, new_chars[pos], positions[pos]);	//replace that specific char
		++pos;
	}
	return str;
}

/* ENCRIPTION */

//function to seperate the digits that represent the character
void splitting(char* arr, int val){

    int i;
    int x = val/10;
    int y = val - x * 10;
    int size = sizeof(arr);

    char res1 = x + '0';   //converting int to char
    char res2  = y + '0';

    //insert the characters into the array
    for(i=0; i<size; i++){

        if(i == 0){
            arr[i] = res1;
        }
        if(i == 1){
            arr[i] = res2;
        }
    }
}

//function to insert a character at a particular position
void insertion(char* arr, char val, int pos){

    int z;
    int arrsize = sizeof(arr);

    //shift elements one step to the right
    for(z=arrsize; z>=pos; z--){
        arr[z] = arr[z-1];
    }

    arr[pos-1] = val;  //insert character in specified position
}

//function to encrypt each character


//function to encrypt each character
void encrypt_char(char* arr1, char string){
    char criteria[53] = "0abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";   //variable to help determine the characters
    int b,c;
    for(b=1; b<=9; b++){           //for characters represented by single digits
        if(string == criteria[b]){     //determine the character
            arr1[0] = criteria[b+26];  //insert the capital letter of that character
        }else{
            continue;
        }
    }

    for(c=10; c<=52; c++){       //Ffr characters represented by double digits
        if(string == criteria[c]){   //determine the character

            splitting(arr1, c);  //split the digit that represents the character

            int d = c - 1;
            int position1 = 2;

            insertion(arr1, criteria[d], position1); //insert character in specified position

            int e = c - 9;
            int f = e/10;
            int l = e - f * 10;

            char m = f + '0';  //converting int to char
            int position2 = 3;

            insertion(arr1, m, position2);

            char n = l + '0';
            int position3 = 4;

            insertion(arr1, n, position3);

            int g = e - 1;
            int position4 = 4;

            insertion(arr1, criteria[g], position4);

            int h = e - 9;
            int position5 = 4;

            insertion(arr1, criteria[h+26], position5);

        }else{
            continue;
        }
    }
}
