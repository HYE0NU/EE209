/* 20210604 정현우
   assignmnet #3
   customer_manager1.c */

#define _GNU_SOURCE
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "customer_manager.h"

#define UNIT_ARRAY_SIZE 1024

struct UserInfo {
  char *name;                // customer name
  char *id;                  // customer id
  int purchase;              // purchase amount (> 0)
};

struct DB {
  struct UserInfo *pArray;   // pointer to the array
  int curArrSize;            // current array size (max # of elements)
  int numItems;              // # of stored items, needed to determine
			     // # whether the array should be expanded
			     // # or not
};

/*-------------------------------------------------------------------*/
/* create and return a db structure */
DB_T
CreateCustomerDB(void)
{
  DB_T d;
  
  d = (DB_T) calloc(1, sizeof(struct DB));
  if (d == NULL) {
    fprintf(stderr, "Can't allocate a memory for DB_T\n");
    return NULL;
  }
  d->curArrSize = UNIT_ARRAY_SIZE; // start with 1024 elements
  d->pArray = 
  (struct UserInfo *)calloc(d->curArrSize, sizeof(struct UserInfo));
  if (d->pArray == NULL) {
    fprintf(stderr, "Can't allocate a memory for array of size %d\n",
	    d->curArrSize);   
    free(d);
    return NULL;
  }
  return d;
}
/*-------------------------------------------------------------------*/
/* checks if the UserInfo element is empty */
int
isEmptyUser(struct UserInfo user)
{
  return (user.name == NULL);
}
/*-------------------------------------------------------------------*/
/* destory db and its associated memory */
void
DestroyCustomerDB(DB_T d)
{
  if(d==NULL) return;
  
  assert(d->pArray!=NULL);

  for(int i=0;i<d->curArrSize;i++){
    if(!isEmptyUser(d->pArray[i])){
      free(d->pArray[i].id);
      free(d->pArray[i].name);
    }
  }
  free(d->pArray);
  free(d);
  return;
}
/*-------------------------------------------------------------------*/
/* register a customer with (name, id, purchase) */
int
RegisterCustomer(DB_T d, const char *id,
		 const char *name, const int purchase)
{
  if(d==NULL || id==NULL || name ==NULL || purchase<=0)
    return -1;
  
  /* check if same name or id exists */
  for(int i=0;i<d->curArrSize;i++){
    if(isEmptyUser(d->pArray[i])) continue;
    else if(strcmp(d->pArray[i].id, id)==0) return -1;
    else if(strcmp(d->pArray[i].name, name)==0) return -1;
  }

  for(int i=0;i<d->curArrSize;i++){
    /* search empty element and save data */
    if(isEmptyUser(d->pArray[i]))
    {
      d->pArray[i].id = strdup(id);
      if(d->pArray[i].id==NULL) return -1;
      d->pArray[i].name = strdup(name);
      if(d->pArray[i].name==NULL) return -1;
      d->pArray[i].purchase = purchase;
      d->numItems++;
      break;
    }
    /* expand DB memory if array is full */
    if(i==d->curArrSize-1){
      struct UserInfo* temp;
      temp = realloc(d->pArray, 
        (d->curArrSize+UNIT_ARRAY_SIZE)*sizeof(struct UserInfo));
        
      /* reallocation success */
      if(temp!=NULL){
        d->pArray = temp;
        memset(d->pArray + d->curArrSize, 0, 
          UNIT_ARRAY_SIZE*sizeof(struct UserInfo));
        d->curArrSize += UNIT_ARRAY_SIZE;
      }
      /* reallocation fail */
      else{
        fprintf(stderr, "Can't reallocate enough memory\n");
        return -1;
      }
    }
  }

  return 0;
}
/*-------------------------------------------------------------------*/
/* unregister a customer with 'id' */
int
UnregisterCustomerByID(DB_T d, const char *id)
{
  if(d==NULL || id==NULL) return -1;

  for(int i=0;i<d->curArrSize;i++){
    if(isEmptyUser(d->pArray[i])) continue;
    else if(strcmp(d->pArray[i].id, id)==0){
      free(d->pArray[i].id);
      free(d->pArray[i].name);
      memset(&(d->pArray[i]), 0, sizeof(struct UserInfo));
      d->numItems--;
      return 0;
    }
  }
  return (-1);
}

/*-------------------------------------------------------------------*/
/* unregister a customer with 'name' */
int
UnregisterCustomerByName(DB_T d, const char *name)
{
  if(d==NULL || name==NULL) return -1;

  for(int i=0;i<d->curArrSize;i++){
    if(isEmptyUser(d->pArray[i])) continue;
    else if(strcmp(d->pArray[i].name, name)==0){
      free(d->pArray[i].id);
      free(d->pArray[i].name);
      memset(&(d->pArray[i]), 0, sizeof(struct UserInfo));
      d->numItems--;
      return 0;
    }
  }
  return (-1);
}
/*-------------------------------------------------------------------*/
/* get the purchase amount of a user whose ID is 'id' */
int
GetPurchaseByID(DB_T d, const char* id)
{
  if(d==NULL || id==NULL) return -1;

  for(int i=0;i<d->curArrSize;i++){
    if(isEmptyUser(d->pArray[i])) continue;
    else if(strcmp(d->pArray[i].id, id)==0){
      return d->pArray[i].purchase;
    }
  }
  return (-1);
}
/*-------------------------------------------------------------------*/
/* get the purchase amount of a user whose name is 'name' */
int
GetPurchaseByName(DB_T d, const char* name)
{
  if(d==NULL || name==NULL) return -1;

  for(int i=0;i<d->curArrSize;i++){
    if(isEmptyUser(d->pArray[i])) continue;
    else if(strcmp(d->pArray[i].name, name)==0){
      return d->pArray[i].purchase;
    }
  }
  return (-1);
}
/*-------------------------------------------------------------------*/
/* iterate all valid user items once, evaluate fp for each valid user
   and return the sum of all fp function calls */
int
GetSumCustomerPurchase(DB_T d, FUNCPTR_T fp)
{
  if(d==NULL || fp==NULL) return -1;

  int sum=0;
  for(int i=0;i<d->curArrSize;i++){
    if(isEmptyUser(d->pArray[i])) continue;
    sum = sum + 
    fp(d->pArray[i].id, d->pArray[i].name, d->pArray[i].purchase);
  }
  return sum;
}
