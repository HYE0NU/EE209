/* 20210604 정현우
   assignmnet #3
   customer_manager2.c */

#define _GNU_SOURCE
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "customer_manager.h"

#define UNIT_BUCKET_COUNT 1024
#define HASH_MULTIPLIER 65599
#define MAX_BUCKET_COUNT 1048576

/* User Information */
struct UserInfo {
  char *name;                // customer name
  char *id;                  // customer id
  int purchase;              // purchase amount (> 0)
  struct UserInfo *nextbyName, *nextbyID; // next customer in linkage
  struct UserInfo *prevbyName, *prevbyID; // prev customer in linkage
};

/* Database of USer Information */
struct DB {
  struct UserInfo **pNameTable, **pIDTable; 
  // array of pointer to user data
  int curBucketCount;        // current bucket count (max # of array)
  int numItems;              // # of stored items, needed to determine
			     // # whether the hashtable should be expanded
			     // # or not
};

/*-------------------------------------------------------------------*/
/* Return a hash code for pcKey that is between 0 and iBucketCount-1,
   inclusive. Adapted from the EE209 lecture notes. */
static int hash_function(const char *pcKey, int iBucketCount)
{
   int i;
   unsigned int uiHash = 0U;
   for (i = 0; pcKey[i] != '\0'; i++)
      uiHash = uiHash * (unsigned int)HASH_MULTIPLIER
               + (unsigned int)pcKey[i];
   return (int)(uiHash % (unsigned int)iBucketCount);
}

/*-------------------------------------------------------------------*/
/* create and return a db structure. Return NULL if failed */
DB_T
CreateCustomerDB(void)
{
  DB_T d;
  
  d = (DB_T) calloc(1, sizeof(struct DB));
  if (d == NULL) {
    fprintf(stderr, "Can't allocate a memory for DB_T\n");
    return NULL;
  }
  d->curBucketCount = UNIT_BUCKET_COUNT; // start with 1024 buckets
  d->pNameTable = (struct UserInfo **)
    calloc(d->curBucketCount, sizeof(struct UserInfo*));
  d->pIDTable = (struct UserInfo **)
    calloc(d->curBucketCount, sizeof(struct UserInfo*));
  if (d->pNameTable == NULL || d->pIDTable == NULL) {
    fprintf(stderr, "Can't allocate a memory for array of size %d\n",
	    d->curBucketCount);   
    free(d);
    return NULL;
  }
  d->numItems=0; //meaningless, but for clear expression
  return d;
}

/*-------------------------------------------------------------------*/
/* destory db and its associated memory */
void
DestroyCustomerDB(DB_T d)
{
  if(d==NULL) return;
  
  assert(d->pIDTable!=NULL && d->pNameTable!=NULL);

  struct UserInfo *u, *nextu;
  for(int i=0;i<d->curBucketCount;i++){
    for(u=d->pNameTable[i]; u!=NULL; u=nextu){
      free(u->name);
      free(u->id);
      nextu = u->nextbyID;
      free(u);
    }
  }
  free(d->pNameTable);
  free(d->pIDTable);
  free(d);
  return;
}

/*-------------------------------------------------------------------*/
/* expand db; double bucket numbers. Return 0 if success; -1 if not*/
void
ExpandCustomerDB(DB_T d)
{
  if(d==NULL) return;

  struct UserInfo **pNewNameTable, **pNewIDTable;
  struct UserInfo *u, *nextu;
  int newBucketCount = 2*d->curBucketCount, newnamehash, newidhash;
  
  /* create new NameTable and IDTable */
  pNewNameTable = calloc(newBucketCount, sizeof(struct UserInfo*));
  pNewIDTable = calloc(newBucketCount, sizeof(struct UserInfo*));
  if(pNewNameTable==NULL || pNewIDTable==NULL){
    fprintf(stderr,"Cannot expand Customer DB to size %d",
     newBucketCount);
    return;
  }
  
  /* move all UseInfo elements to new Table */
  for(int i=0;i<d->curBucketCount;i++){
    for(u=d->pNameTable[i]; u!=NULL; u=nextu){
      newnamehash = hash_function(u->name, newBucketCount);
      newidhash = hash_function(u->id, newBucketCount);
      nextu = u->nextbyName;

      /* link UserInfo to NameTable */
      u->nextbyName = pNewNameTable[newnamehash];
      u->prevbyName = NULL;
      if(pNewNameTable[newnamehash]!=NULL){
        pNewNameTable[newnamehash]->prevbyName = u;
      }
      pNewNameTable[newnamehash] = u;

      /* link UserInfo to IDTable */
      u->nextbyID = pNewIDTable[newidhash];
      u->prevbyID = NULL;
      if(pNewIDTable[newidhash]!=NULL){
        pNewIDTable[newidhash]->prevbyID = u;
      }
      pNewIDTable[newidhash] = u;
    }
  }

  /* Replace old tables to new tables */
  free(d->pNameTable);
  free(d->pIDTable);
  d->curBucketCount = newBucketCount;
  d->pNameTable = pNewNameTable;
  d->pIDTable = pNewIDTable;
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
  
  int namehash = hash_function(name, d->curBucketCount);
  int idhash = hash_function(id, d->curBucketCount);
  struct UserInfo* u;

  /* check if same name or id exists */
  for(u=d->pNameTable[namehash]; u!=NULL; u=u->nextbyName){
    if(strcmp(u->name, name)==0) return -1;
  }
  for(u=d->pIDTable[idhash]; u!=NULL; u=u->nextbyID){
    if(strcmp(u->id, id)==0) return -1;
  }

  /* no overlap; then register customer */
  d->numItems++;

  /* construct new UserInfo */
  struct UserInfo* new = calloc(1, sizeof(struct UserInfo));
  new->name = strdup(name);
  new->id = strdup(id);
  new->purchase = purchase;
  if(new->name==NULL && new->id==NULL) return -1;
  
  /* register to NameTable */
  new->nextbyName = d->pNameTable[namehash];
  new->prevbyName = NULL;
  if(d->pNameTable[namehash]!=NULL){
    d->pNameTable[namehash]->prevbyName = new;
  }
  d->pNameTable[namehash] = new;

  /* register to IDTable */
  new->nextbyID = d->pIDTable[idhash];
  new->prevbyID = NULL;
  if(d->pIDTable[idhash]!=NULL){
    d->pIDTable[idhash]->prevbyID = new;
  }
  d->pIDTable[idhash] = new;

  /* resize db if customer# exeeds 70% of bucket count */
  if(d->numItems > 0.75*d->curBucketCount 
      && d->curBucketCount<=MAX_BUCKET_COUNT){
    ExpandCustomerDB(d);
  }

  return 0;
}

/*-------------------------------------------------------------------*/

/* unregister a customer with 'id' */
int
UnregisterCustomerByID(DB_T d, const char *id)
{
  if(id==NULL) return -1;
  if(d==NULL) return -1;

  int idhash = hash_function(id, d->curBucketCount);
  struct UserInfo* u = d->pIDTable[idhash];

  for(; u!=NULL; u=u->nextbyID){
    if(strcmp(u->id, id)==0){
      /* unregister in IDTable */
      if(u->nextbyID!=NULL)
        u->nextbyID->prevbyID = u->prevbyID;
      if(u->prevbyID!=NULL) 
        u->prevbyID->nextbyID = u->nextbyID;
      else d->pIDTable[idhash] = u->nextbyID;
      /* unregister in NameTable */
      if(u->nextbyName!=NULL)
        u->nextbyName->prevbyName = u->prevbyName;
      if(u->prevbyName!=NULL)
        u->prevbyName->nextbyName = u->nextbyName;
      else{
        int namehash = hash_function(u->name, d->curBucketCount);
        d->pNameTable[namehash] = u->nextbyName; 
      }
      /* free memory */
      free(u->name);
      free(u->id);
      free(u);
      d->numItems--;
      return 0;
    }
  }
  return (-1);
}

/*--------------------------------------------------------------------*/

/* unregister a customer with 'name' */
int
UnregisterCustomerByName(DB_T d, const char *name)
{
  if(name==NULL) return -1;
  if(d==NULL) return -1;

  int namehash = hash_function(name, d->curBucketCount); 
  struct UserInfo* u = d->pNameTable[namehash];

  for(; u!=NULL; u=u->nextbyName){
    if(strcmp(u->name, name)==0){
      /* unregister in NameTable */
      if(u->nextbyName!=NULL) 
        u->nextbyName->prevbyName = u->prevbyName;
      if(u->prevbyName!=NULL) 
        u->prevbyName->nextbyName = u->nextbyName;
      else d->pNameTable[namehash] = u->nextbyName;
      /* unregister in IDTable */
      if(u->nextbyID!=NULL) 
        u->nextbyID->prevbyID = u->prevbyID;
      if(u->prevbyID!=NULL)
        u->prevbyID->nextbyID = u->nextbyID;
      else{
        int idhash = hash_function(u->id, d->curBucketCount);
        d->pIDTable[idhash] = u->nextbyID; 
      }
      /* free memory */
      free(u->name);
      free(u->id);
      free(u);
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

  int idhash = hash_function(id, d->curBucketCount);
  struct UserInfo* u;

  for(u=d->pIDTable[idhash]; u!=NULL; u=u->nextbyID){
    if(strcmp(u->id, id) == 0){
      return u->purchase;
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

  int namehash = hash_function(name, d->curBucketCount);
  struct UserInfo* u;

  for(u=d->pNameTable[namehash]; u!=NULL; u=u->nextbyName){
    if(strcmp(u->name, name) == 0){
      return u->purchase;
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
  struct UserInfo* u;

  for(int i=0;i<d->curBucketCount;i++){
    for(u=d->pNameTable[i]; u!=NULL; u=u->nextbyName){
      sum += fp(u->id, u->name, u->purchase);
    }
  }
  return sum;
}
