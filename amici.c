//author: Scott Bullock
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include "HashADT.h"

//keeps track of the amount of people in the hash table
size_t size_of_hashtable = 0;

/// str_hash function returns the hash of a native C-string.
/// @param element the c-string to hash
/// @return the hash value of the c-string
static size_t str_hash( const void *element ) {
    unsigned char *str = (unsigned char *) element;
    size_t hash = 5381;
    int c;

    while( (c = *str++) ) {
        hash = ((hash << 5) + hash) + c;
    }
    return (size_t)hash;
}

/// str_equals function - compares the elements as two C-strings.
/// @param element1 first c-string
/// @param element2 second c-string
/// @return true if the two strings are equal
static bool str_equals( const void *element1, const void *element2 ) {
    return strcmp( (char*)element1, (char*)element2 ) == 0;
}
typedef struct person_s {
    char *name;     //name of the person      
    char *handle;     //handle of the person
    struct person_s **friends;     //dynamic collection of friends
    size_t friend_count;     //current number of friends
    size_t max_friends;     //current limit on friends      
} person_t;

/**
Adds a person to the hashtable where the key is the handle and the struct is the value
@param hashtable: Hashtable used to add the person
@param first_name: the first name of the person
@param last_name: the last name of the person
@param handle: the handle of the person
@param file: true if command was called from file input, false otherwise
**/
void add_person(HashADT hashtable, char* first_name, char* last_name, char* handle, bool file)
{
  if(file == false)
  {
    printf("Amici> + \"add\" \"%s\" \"%s\" \"%s\"\n", first_name, last_name, handle);
  }
  if(ht_has(hashtable, handle) == true)
  {
    fprintf(stdout, "error: handle \"%s\" is already in use\n", handle);
    fflush(stdout);
    return;
  }
  else
  {
    size_t first_len = strlen(first_name);
    size_t last_len = strlen(last_name);
    char* full_name = (char*)malloc(first_len + 1 + last_len + 1);
    snprintf(full_name, first_len + 1 + last_len + 1, "%s %s", first_name, last_name);
    person_t* person = (person_t*)malloc(sizeof(struct person_s));
    person->name = strdup(full_name);
    person->handle = strdup(handle);
    person->max_friends = 16;
    person->friends = calloc(person->max_friends, sizeof(struct person_s));
    person->friend_count = 0;
    bool first_name_alphabet = true;
    bool last_name_alphabet = true;
    bool handle_alphabet_number = true;
    for(size_t i = 0; i < strlen(first_name); i++)
    {
      if(isalpha(first_name[i]) == 0)
      {
        first_name_alphabet = false;
        break;
      }
    }
    for(size_t i = 0; i < strlen(last_name); i++) 
    {
      if(isalpha(last_name[i]) == 0)
      {
        last_name_alphabet = false;
        break;
      }
    }
    for(size_t i = 0; i < strlen(handle); i++) 
    {
      if(!isalpha(handle[i]) && !isdigit(handle[i]))
      {
        handle_alphabet_number = false;
        break;
      }
    }
    if(first_name_alphabet == false)
    {
      fprintf(stdout,"error: argument \"%s\" is invalid\n", first_name);
      free(person->name);
      free(person->handle);
      free(person->friends);
      free(person);
      free(full_name);
      fflush(stdout);
      return;
    }
    else if(last_name_alphabet == false)
    {
      fprintf(stdout,"error: argument \"%s\" is invalid\n", last_name); 
      free(person->name);
      free(person->handle);
      free(person->friends);
      free(person);
      free(full_name);
      fflush(stdout);
      return;
    }
    else if(handle_alphabet_number == false)
    {
      fprintf(stdout,"error: argument \"%s\" is invalid\n", handle); 
      free(person->name);
      free(person->handle);
      free(person->friends);
      free(person);
      free(full_name);
      fflush(stdout);
      return;
    }
    ht_put(hashtable, person->handle, person);
    size_of_hashtable+=1;
    free(full_name);
  }
}

/**
Creates a friendship between two people and added to each other friends list
@param hashtable: Hashtable containing people
@param handle1: the handle of one of the people
@param handle2: the handle of the other person
@param file: true if command was called from file input, false otherwise
**/
void add_friend(HashADT hashtable, char* handle1, char* handle2, bool file)
{
  if(file == false)
  {
    printf("Amici> + \"friend\" \"%s\" \"%s\"\n", handle1, handle2);
  }
  if(ht_has(hashtable, handle1) == false)
  {
    fprintf(stdout, "error: handle \"%s\" is unknown\n", handle1);
    fflush(stdout);
  }
  else if(ht_has(hashtable, handle2) == false)
  {
    fprintf(stdout, "error: handle \"%s\" is unknown\n", handle2);
    fflush(stdout);
  }
  else if(strcmp(handle1, handle2) == 0)
  {
    fprintf(stdout, "error: \"%s\" and \"%s\" are the same person\n", handle1, handle2);
    fflush(stdout);
  }
  else
  {
    person_t* person1 = (person_t*)ht_get(hashtable, handle1);
    person_t* person2 = (person_t*)ht_get(hashtable, handle2);
    bool friend_already = false;
    for(size_t i = 0; i < person1->friend_count; i++)
    {
      char* handle_friend = (person1->friends[i])->handle;
      if(strcmp(handle_friend, handle2) == 0)
      {
        friend_already = true;
        break;
      }
    }
    if(friend_already == true)
    {
      fprintf(stdout,"%s and %s are already friends.\n", handle1, handle2);
      fflush(stdout);
    }
    else
    {
      if(person1->friend_count == person1->max_friends)
      {
        person1->max_friends*=2;
        person1->friends = realloc(person1->friends, sizeof(struct person_s) * person1->max_friends);
        for(size_t i = person1->max_friends/2; i < person1->max_friends; i++)
        {
          person1->friends[i] = NULL;
        }
      }
      if(person2->friend_count == person2->max_friends)
      {
        person2->max_friends*=2;
        person2->friends = realloc(person2->friends, sizeof(struct person_s) * person2->max_friends);
        for(size_t i = person2->max_friends/2; i < person2->max_friends; i++)
        {
          person2->friends[i] = NULL;
        }
      }
      for(size_t i = 0; i < person1->max_friends; i++)
      {
        if(person1->friends[i] == NULL)
        {
          person1->friends[i] = person2;
          person1->friend_count+=1;
          break;
        }
      }
      for(size_t i = 0; i < person2->max_friends; i++)
      {
        if(person2->friends[i] == NULL)
        {
          person2->friends[i] = person1;
          person2->friend_count+=1;
          break;
        }
      }
      printf("%s and %s are now friends.\n", handle1, handle2);
    }
  }
}

/**
Unfriends a person by getting rid of them in there friends list
@param hashtable: Hashtable containing people
@param handle1: the handle of one of the people
@param handle2: the handle of the other person
@param file: true if command was called from file input, false otherwise
**/
void unfriend_person(HashADT hashtable, char* handle1, char* handle2, bool file)
{
  if(file == false)
  {
    printf("Amici> + \"unfriend\" \"%s\" \"%s\"\n", handle1, handle2);
  }
  if(ht_has(hashtable, handle1) == false)
  {
    fprintf(stdout, "error: handle \"%s\" is unknown\n", handle1);
    fflush(stdout);
  }
  else if(ht_has(hashtable, handle2) == false)
  {
    fprintf(stdout, "error: handle \"%s\" is unknown\n", handle2);
    fflush(stdout);
  }
  else if(strcmp(handle1, handle2) == 0)
  {
    fprintf(stdout, "error: \"%s\" and \"%s\" are the same person\n", handle1, handle2);
    fflush(stdout);
  }
  else
  {
    person_t* person1 = (person_t*)ht_get(hashtable, handle1);
    person_t* person2 = (person_t*)ht_get(hashtable, handle2);
    bool friends = false;
    for(size_t i = 0; i < person1->max_friends; i++)
    {
      if(person1->friends[i] == 0)
      {
        //do nothing because NULL/0
      }
      else if(strcmp((person1->friends[i])->handle, handle2) == 0)
      {
        person1->friends[i] = 0;
        person1->friend_count-=1;
        friends = true;
        break;
      }
    }
    for(size_t i = 0; i < person2->max_friends; i++)
    {
      if(person2->friends[i] == 0)
      {
        //do nothing because NULL/0
      }
      else if(strcmp((person2->friends[i])->handle, handle1) == 0)
      {
        person2->friends[i] = 0;
        person2->friend_count-=1;
        break;
      }
    }
    if(friends == true)
    {
      printf("%s and %s are no longer friends.\n", handle1, handle2);
    }
    else
    {
      fprintf(stdout,"%s and %s are not friends.\n", handle1, handle2);
      fflush(stdout);
    }
  }
}

/**
Prints the persons handle along with the person friends
@param hashtable: Hashtable containing people
@param handle: the handle of the person
@param file: true if command was called from file input, false otherwise
**/
void print_handle(HashADT hashtable, char* handle, bool file)
{
  if(file == false)
  {
    printf("Amici> + \"print\" \"%s\"\n", handle);
  }
  if(ht_has(hashtable, handle) == false)
  {
    fprintf(stdout,"error: handle \"%s\" is unknown\n", handle);
    fflush(stdout);
  }
  else
  {
    person_t* person = (person_t*)ht_get(hashtable, handle);
    if(person->friend_count > 1)
    {
      printf("%s (%s) has %ld friends\n", handle, person->name, person->friend_count);
      for(size_t i = 0; i < person->max_friends; i++)
      {
        if(person->friends[i] != 0)
        {
          printf("\t%s (%s)\n", (person->friends[i])->handle, (person->friends[i])->name);
        }
      }
    }
    else if(person->friend_count == 1)
    {
      printf("%s (%s) has 1 friend\n", handle, person->name);
      for(size_t i = 0; i < person->max_friends; i++)
      {
        if(person->friends[i] != 0)
        {
          printf("\t%s (%s)\n", (person->friends[i])->handle, (person->friends[i])->name);
          break;
        }
      }
    }
    else
    {
      printf("%s (%s) has no friends\n", handle, person->name);
    }
  }
}
/**
Prints the persons handle along with the number of friends the person has
@param hashtable: Hashtable containing people
@param handle: the handle of the person
@param file: true if command was called from file input, false otherwise
**/

void print_size(HashADT hashtable, char* handle, bool file)
{
  if(file == false)
  {
    printf("Amici> + \"size\" \"%s\"\n", handle);
  }
  if(ht_has(hashtable, handle) == false)
  {
    fprintf(stdout,"error: handle \"%s\" is unknown\n", handle);
    fflush(stdout);
  }
  else
  {
    person_t* person = (person_t*)ht_get(hashtable, handle);
    if(person->friend_count > 1)
    {
      printf("%s (%s) has %ld friends\n", handle, person->name, person->friend_count);
    }
    else if(person->friend_count == 1)
    {
      printf("%s (%s) has 1 friend\n", handle, person->name);
    }
    else
    {
      printf("%s (%s) has no friends\n", handle, person->name);
    }
  }
}
/**
Loops through the hashtable, and prints the number of people in the hashtable
as well as the number of friendships
@param hashtable: Hashtable containing people
@param file: true if command was called from file input, false otherwise
**/
void print_stats(HashADT hashtable, bool file)
{
  if(file == false)
  {
    printf("Amici> + \"stats\"\n");
  }
  char** keys = (char**)ht_keys(hashtable);
  if(keys != NULL)
  {
    size_t total_friends = 0;
    for(size_t i = 0; i < size_of_hashtable; i++)
    {
      person_t* person = (person_t*)ht_get(hashtable, keys[i]);
      total_friends+=person->friend_count;
    }
    if(size_of_hashtable == 1)
    {
      printf("Statistics:  1 person, no friendships\n");
    }
    else if(size_of_hashtable > 1 && total_friends == 0)
    {
      printf("Statistics:  %ld people, no friendships\n", size_of_hashtable);
    }
    else if(size_of_hashtable > 1 && total_friends/2 == 1)
    {
      printf("Statistics:  %ld people, 1 friendship\n", size_of_hashtable);
    }
    else
    {
      printf("Statistics:  %ld people, %ld friendships\n", size_of_hashtable, total_friends/2);
    }
    free(keys);
  }
  else
  {
    printf("Statistics:  no people, no friendships\n");
  }
}

/**
Gets rid of all dynamic allocated memory and the hashtable, and creates a new hashtable
@param hashtable: Hashtable containing people
@param file: true if command was called from file input, false otherwise
**/
HashADT init(HashADT hashtable, bool file)
{
  if(file == false)
  {
    printf("Amici> + \"init\"\n");
  }
  char** keys = (char**)ht_keys(hashtable);
  for(size_t i = 0; i < size_of_hashtable; i++)
  {
    person_t* person = (person_t*)ht_get(hashtable, keys[i]);
    free(person->friends);
    free(person->name);
    free(person);
  }
  for(size_t i = 0; i < size_of_hashtable; i++)
  {
    free(keys[i]);
  }
  free(keys);
  ht_destroy(hashtable);
  hashtable = ht_create(str_hash, str_equals, NULL, NULL);
  size_of_hashtable = 0;
  printf("System re-initialized\n");
  return(hashtable);
}

/**
Gets rid of all dynamic allocated memory and the hashtable and closes the program
@param hashtable: Hashtable containing people
**/
int quit(HashADT hashtable)
{
  printf("Amici> + \"quit\"\n");
  if(size_of_hashtable == 0)
  {
    ht_destroy(hashtable);
    return(EXIT_SUCCESS);
  }
  else
  {
    char** keys = (char**)ht_keys(hashtable);
    for(size_t i = 0; i < size_of_hashtable; i++)
    {
      person_t* person = (person_t*)ht_get(hashtable, keys[i]);
      free(person->friends);
      free(person->name);
      free(person);
    }
    for(size_t i = 0; i < size_of_hashtable; i++)
    {
      free(keys[i]);
    }
    free(keys);
    ht_destroy(hashtable);
    size_of_hashtable = 0;
    return(EXIT_SUCCESS);
  }
}

/**
Given a command from the stdin or file, it determines which function must be
done to complete the command and gives errors if argument numbers are incorrect
@param hashtable: Hashtable containing people
@param tokens: the command in a 5 word char array
@param file: true if command was called from file input, false otherwise
**/
void process_command(HashADT* hashtable, char* tokens[5], bool file)
{
  if(strcasecmp(tokens[0], "add") == 0)
  {
    if(tokens[4] != NULL)
    {
      fprintf(stdout, "Amici> error: usage: add first-name last-name handle\n");
      fflush(stdout);
    }
    else if(tokens[3] == NULL)
    {
      fprintf(stdout, "Amici> error: usage: add first-name last-name handle\n");
      fflush(stdout);
    }
    else
    {
      add_person(*hashtable, tokens[1], tokens[2], tokens[3], file);
    }
  }
  else if(strcasecmp(tokens[0], "friend") == 0)
  {
    if(tokens[3] != NULL)
    {
      fprintf(stdout, "Amici> error: usage: friend handle1 handle2\n");
      fflush(stdout);
    }
    else if(tokens[2] == NULL)
    {
      fprintf(stdout, "Amici> error: usage: friend handle1 handle2\n");
      fflush(stdout);
    }
    else
    {
      add_friend(*hashtable, tokens[1], tokens[2], file);
    }
  }
  else if(strcasecmp(tokens[0], "unfriend") == 0)
  {
    if(tokens[3] != NULL)
    {
      fprintf(stdout, "Amici> error: usage: unfriend handle1 handle2\n");
      fflush(stdout);
    }
    else if(tokens[2] == NULL)
    {
      fprintf(stdout, "Amici> error: usage: unfriend handle1 handle2\n");
      fflush(stdout);
    }
    else
    {
      unfriend_person(*hashtable, tokens[1], tokens[2], file);
    }
  }
  else if(strcasecmp(tokens[0], "print") == 0)
  {
    if(tokens[2] != NULL)
    {
      fprintf(stdout, "Amici> error: usage: print handle\n");
      fflush(stdout);
    }
    else if(tokens[1] == NULL)
    {
      fprintf(stdout, "Amici> error: usage: print handle\n");
      fflush(stdout);
    }
    else
    {
      print_handle(*hashtable, tokens[1], file);
    }
  }
  else if(strcasecmp(tokens[0], "size") == 0)
  {
    if(tokens[2] != NULL)
    {
      fprintf(stdout, "Amici> error: usage: size handle\n");
      fflush(stdout);
    }
    else if(tokens[1] == NULL)
    {
      fprintf(stdout, "Amici> error: usage: size handle\n");
      fflush(stdout);
    }
    else
    {
      print_size(*hashtable, tokens[1], file);
    }
  }
  else if(strcasecmp(tokens[0], "stats") == 0)
  {
    if(tokens[1] != NULL)
    {
      fprintf(stdout, "Amici> error: usage: stats\n");
      fflush(stdout);
    }
    print_stats(*hashtable, file);
  }
  else if(strcasecmp(tokens[0], "init") == 0)
  {
    if(tokens[1] != NULL)
    {
      fprintf(stdout, "Amici> error: usage: init\n");
      fflush(stdout);
    }
    *hashtable = init(*hashtable, file);
  }
  else if(strcasecmp(tokens[0], "quit") == 0)
  {
    quit(*hashtable);
  }
  else
  {
    printf("Amici> ");
  }
}

/**
Loops through a file until EOF or stdin until the quit command.
Takes in the commands from stdin or file and gives it to the
process_command function
@param argc the number of args
@param argv the args itself in a character array
**/
int main(int argc, char * argv[])
{
  (void)argv;
  if(argc > 2)
  {
    fprintf(stderr, "usage: amici [ datafile ]\n");
    return(EXIT_FAILURE);
  }
  if (argc == 1)
  {
    char buffer[1024];
    char* tokens[5];
    bool quits = false;
    HashADT hashtable = ht_create(str_hash, str_equals, NULL, NULL);
    while (fgets(buffer, sizeof(buffer), stdin) != NULL)
    {
      char *token = strtok(buffer, " \t\n");
      int index_counter = 0;
      tokens[0] = NULL;
      tokens[1] = NULL;
      tokens[2] = NULL;
      tokens[3] = NULL;
      tokens[4] = NULL;
      while(token != NULL)
      {
        if(index_counter < 5)
        {
          tokens[index_counter] = token;
        }
        if(strcasecmp(tokens[0], "quit") == 0)
        {
          quits = true;
          break;
        }
        index_counter+=1;
        token = strtok(NULL, " \t\n");
      }
      if(quits == true)
      {
        break;
      }
      process_command(&hashtable, tokens, false); 
    }
    quit(hashtable);
  }
  else
  {
    char buffer[1024];
    char* tokens[5];
    bool quits = false;
    HashADT hashtable = ht_create(str_hash, str_equals, NULL, NULL);
    FILE* file = fopen( argv[1], "r" );
    if(file == NULL) 
    {
        perror(argv[1]);
        return(EXIT_FAILURE);
    }
    else
    {
      while (fgets(buffer, sizeof(buffer), file) != NULL)
      {
        char *token = strtok(buffer, " \t\n");
        int index_counter = 0;
        tokens[0] = NULL;
        tokens[1] = NULL;
        tokens[2] = NULL;
        tokens[3] = NULL;
        tokens[4] = NULL;
        while(token != NULL)
        {
          if(index_counter < 5)
          {
            tokens[index_counter] = token;
          }
          index_counter+=1;
          token = strtok(NULL, " \t\n");
        }
        process_command(&hashtable, tokens, true); 
      }
      fclose(file);
    }
    while (fgets(buffer, sizeof(buffer), stdin) != NULL)
    {
      char *token = strtok(buffer, " \t\n");
      int index_counter = 0;
      tokens[0] = NULL;
      tokens[1] = NULL;
      tokens[2] = NULL;
      tokens[3] = NULL;
      tokens[4] = NULL;
      while(token != NULL)
      {
        if(index_counter < 5)
        {
          tokens[index_counter] = token;
        }
        if(strcasecmp(tokens[0], "quit") == 0)
        {
          quits = true;
          break;
        }
        index_counter+=1;
        token = strtok(NULL, " \t\n");
      }
      if(quits == true)
      {
        break;
      }
      process_command(&hashtable, tokens, false); 
    }
    quit(hashtable);
  }
}