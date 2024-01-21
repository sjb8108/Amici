//author: Scott Bullock
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdbool.h>
#include "HashADT.h"
#include "realloc.h"

struct hashtab_s
{
  void **keys;
  void **values;
  size_t size;
  size_t capacity;
  size_t collisions;
  size_t rehashes;
  size_t (*hash)(const void *key);
  bool (*equals)(const void *key1, const void *key2);
  void (*print)(const void *key, const void *value);
  void (*delete)(void *key, void *value);
};

HashADT ht_create(size_t (*hash)( const void *key ), bool (*equals)( const void *key1, const void *key2 ), 
                  void (*print)( const void *key, const void *value ), void (*delete)( void *key, void *value ))
{
  HashADT t = (HashADT) malloc(sizeof(struct hashtab_s));
  t->keys = 0;
  t->values = 0;
  t->size = 0;
  t->capacity = INITIAL_CAPACITY;
  t->collisions = 0;
  t->rehashes = 0;
  t->hash = hash;
  t->equals = equals;
  t->print = print;
  t->delete = delete;
  return (t);
}

void ht_destroy( HashADT t )
{
  assert(t != NULL);
  if (t->delete != NULL)
  {
    for(size_t i = 0; i < t->capacity; i++)
    {
      if(t->keys[i] != NULL)
      {
        t->delete(t->keys[i], t->values[i]); 
      }
    }
  }
  free(t->keys);
  free(t->values);
  free(t);
}

void ht_dump( const HashADT t, bool contents )
{
  if(contents == true)
  {
    printf("Size: %ld\n", t->size);
    printf("Capacity: %ld\n", t->capacity);
    printf("Collisions: %ld\n", t->collisions);
    printf("Rehashes: %ld\n", t->rehashes);
    for(size_t i = 0; i < t->capacity; i++)
    {
      if(t->keys[i] != NULL)
      {
        printf("%ld: (", i);
        t->print(t->keys[i], t->values[i]);
        printf(")\n");
      }
      else
      {
        printf("%ld: null\n", i);
      }
    }
  }
  else
  {
    printf("Size: %ld\n", t->size);
    printf("Capacity: %ld\n", t->capacity);
    printf("Collisions: %ld\n", t->collisions);
    printf("Rehashes: %ld\n", t->rehashes);
  }
}

const void *ht_get( const HashADT t, const void *key )
{
  size_t hash_value = t->hash(key) % (int)t->capacity;
  while(true)
  {
    if(t->keys[hash_value] == NULL)
    {
      //do nothing
    }
    else if(t->equals(t->keys[hash_value], key) == true)
    {
      return(t->values[hash_value]);
    }
    else if(t->keys[hash_value] != NULL)
    {
      t->collisions+=1;
    }
    hash_value+=1;
    if(hash_value >= t->capacity)
    {
      hash_value = 0;
    }
  }
}

bool ht_has( const HashADT t, const void *key )
{
  size_t hash_value = t->hash(key) % t->capacity;
  size_t counter = 0;
  while(counter < t->capacity)
  {
    if(t->keys == 0)
    {
      return(false);
    }
    else if(t->keys[hash_value] == NULL)
    {
      return(false); 
    }
    else if(t->equals(t->keys[hash_value], key) == true)
    {
      return(true);
    }
    hash_value+=1;
    if(hash_value >= t->capacity)
    {
      hash_value = 0;
    }
    t->collisions+=1;
    counter+=1;
  }
  return(false);
}

void *ht_put( HashADT t, const void *key, const void *value )
{
  if( t->keys == 0 && t->values == 0) 
  {
		t->keys = calloc(t->capacity, sizeof(void *) * t->capacity );
    t->values = calloc(t->capacity, sizeof(void *) * t->capacity );
		assert(t->keys != 0);
    assert(t->values != 0);
	}
  size_t hash_value = t->hash(key) % t->capacity;
  while(true)
  {
    if(t->keys[hash_value] == NULL)
    {
      t->keys[hash_value] = (void *)key;
      t->values[hash_value] = (void *)value;
      t->size+=1;
      float rehash = (float)t->size / (float)t->capacity;
      if (rehash >= LOAD_THRESHOLD)
      {
        realloc_hash_table(t);
        t->rehashes+=1;
      }
      return(NULL);
    }
    else if(t->equals(t->keys[hash_value], key) == true) 
    {
      void* old_value = t->values[hash_value];
      t->values[hash_value] = (void *)value;
      return(old_value);
    }
    else
    {
      t->collisions+=1;
    }
    hash_value+=1;
    if(hash_value >= t->capacity)
    {
      hash_value = 0;
    }
  }
}

void **ht_keys( const HashADT t )
{
  if(t->keys == 0)
  {
    return NULL;
  }
  else
  {
    void **keys_copy = (void **) calloc(t->size, sizeof(void *));
    int index = 0;
    for(size_t i = 0; i < t->capacity; i++)
    {
      if(t->keys[i] != NULL)
      {
        keys_copy[index] = t->keys[i];
        index+=1;
      }
    }
    return keys_copy;
  }
}

void **ht_values( const HashADT t )
{
  if(t->values == 0)
  {
    return NULL;
  }
  else
  {
    void **values_copy = (void **) calloc(t->size, sizeof(void *));
    int index = 0;
    for(size_t i = 0; i < t->capacity; i++)
    {
      if(t->values[i] != NULL)
      {
        values_copy[index] = t->values[i];
        index+=1;
      }
    }
    return values_copy;
  }
}

void realloc_hash_table(HashADT t)
{
  size_t new_size = t->capacity * RESIZE_FACTOR;
  void** old_key_indexs = ht_keys(t);
  void** old_value_indexs = ht_values(t);
  free(t->keys);
  free(t->values);
  t->capacity = new_size; 
  t->keys = calloc(new_size, sizeof(void *));
  t->values = calloc(new_size, sizeof(void *));
  for(size_t i = 0; i < t->size; i++)
  {
    size_t hash_value = t->hash(old_key_indexs[i]) % t->capacity;
    while(true)
    {
      if(t->keys[hash_value] == NULL)
      {
        t->keys[hash_value] = old_key_indexs[i];
        t->values[hash_value] = old_value_indexs[i];
        break;
      }
      else if(t->equals(t->keys[hash_value], old_key_indexs[i]) == true) 
      {
        t->values[hash_value] = old_value_indexs[i];
        break;
      }
      else
      {
        t->collisions+=1;
      }
      hash_value+=1;
      if(hash_value >= t->capacity)
      {
        hash_value = 0;
      }
    }
  }
  free(old_key_indexs);
  free(old_value_indexs);
}
