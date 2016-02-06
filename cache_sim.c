//Marina Kent 
//s1567270
//CS Assignment 2
//11/26/15

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <inttypes.h>
#include <math.h>

typedef enum {dm, fa} cache_map_t;
typedef enum {uc, sc} cache_org_t;
typedef enum {instruction, data} access_t;

typedef struct {
  uint32_t address;
  access_t accesstype;
} mem_access_t;

// declare caches and their respective arrays that keep track of valid bits
uint32_t *unifiedArray;
int *uavalid;
uint32_t *splitInstructionArray; 
int *sivalid;
uint32_t *splitDataArray;   
int *sdvalid;

// declare counters for stats
int UNumAccess = 0;
int UNumHit = 0;
double UHitRate; 

int INumAccess = 0;
int INumHit = 0;
double IHitRate; 

int DNumAccess = 0;
int DNumHit = 0;
double DHitRate; 

// declare variables to be used for calculations with cache accesses
int numBlocks, numIndexBits, numTagBits, FIFOcount, FIFOData, FIFOInst;
uint32_t tagVal, indexVal;
int offset = 6;

// findNumBlocks will return the number of blocks in a cache
int findNumBlocks (uint32_t capacity) { 
  return (int) (capacity / 64);
}

// will return the size of the index
int findNumIndexBits (int blocks) {
  return (log2 (blocks));
}

// will return the size of the tag
int findNumTagBits (int blocks, int indexBits) {
  return (32 - indexBits - offset);
}

// will return the value of the index
uint32_t findIndexVal (int address, int numTB) { 
  uint32_t temp;
  temp = (address << numTB);
  return (uint32_t) (temp >> (numTB + offset));
}

// will return the value of the tag
uint32_t findTagVal (int address, int numIB) {
  return (uint32_t) (address >> (offset + numIB));
}

uint32_t cache_size; 
uint32_t block_size = 64;
cache_map_t cache_mapping;
cache_org_t cache_org;

/* Reads a memory access from the trace file and returns
 * 1) access type (instruction or data access
 * 2) memory address
 */
mem_access_t read_transaction(FILE *ptr_file) {
  char buf[1000];
  char* token;
  char* string = buf;
  mem_access_t access;
  
  if (fgets(buf,1000, ptr_file)!=NULL) {
    
    /* Get the access type */
    token = strsep(&string, " \n");        
    if (strcmp(token,"I") == 0) {
      access.accesstype = instruction;
    } else if (strcmp(token,"D") == 0) {
      access.accesstype = data;
    } else {
      printf("Unkown access type\n");
      exit(0);
    }
    
    /* Get the access type */        
    token = strsep(&string, " \n");
    access.address = (uint32_t)strtol(token, NULL, 16);
    
    return access;
  }
  
  /* If there are no more entries in the file,  
   * return an address 0 that will terminate the infinite loop in main
   */
  access.address = 0;
  return access;
}


void main(int argc, char** argv)
{
  /* Read command-line parameters and initialize:
   * cache_size, cache_mapping and cache_org variables
   */
  
  if ( argc != 4 ) { /* argc should be 2 for correct execution */
    printf("Usage: ./cache_sim [cache size: 128-4096] [cache mapping: dm|fa] [cache organization: uc|sc]\n");
    exit(0);
  } else  {
    /* argv[0] is program name, parameters start with argv[1] */
    
    /* Set cache size */
    cache_size = atoi(argv[1]);
    
    /* Set Cache Mapping */
    if (strcmp(argv[2], "dm") == 0) {
      cache_mapping = dm;
    } else if (strcmp(argv[2], "fa") == 0) {
      cache_mapping = fa;
    } else {
      printf("Unknown cache mapping\n");
      exit(0);
    }
    
    /* Set Cache Organization */
    if (strcmp(argv[3], "uc") == 0) {
      cache_org = uc;
    } else if (strcmp(argv[3], "sc") == 0) {
      cache_org = sc;
    } else {
      printf("Unknown cache organization\n");
      exit(0); 
    }
  }
  
  
  /* Open the file mem_trace.txt to read memory accesses */
  FILE *ptr_file; 
  ptr_file =fopen("mem_trace.txt","r");  
  if (!ptr_file) {
    printf("Unable to open the trace file\n");
    exit(1); 
  } 
  
  // find the number of blocks per cache - used to determine how much memory needed
  int numBlock = findNumBlocks(cache_size);
  
  // make one cache for UC
  if (cache_org == uc) {

    // create cache and array to store valid bit. Start by filling each one with 0s.

    unifiedArray =  (uint32_t*) malloc (numBlock * sizeof(uint32_t));   
    uavalid = (int*) malloc (numBlock * sizeof(uint32_t));
    int i = 0;
    for (i; i < numBlock; i++){
      unifiedArray[i] = 0;
      uavalid[i] = 0;
    }
    
  } else {   // if split cache - create two caches of half the size of UC
    
    splitDataArray =  (uint32_t*) malloc (numBlock * sizeof(uint32_t) * 0.5);   
    sdvalid = (int*) malloc (numBlock * sizeof(uint32_t) * 0.5);
    
    splitInstructionArray =  (uint32_t*) malloc (numBlock * sizeof(uint32_t) * 0.5);   
    sivalid = (int*) malloc (numBlock * sizeof(uint32_t) * 0.5);
    
    // fill with 0 again
    int i = 0;
    for (i; i < (numBlock * 0.5 ); i++){
      splitDataArray[i] = 0;
      sdvalid[i] = 0;
      
      splitInstructionArray[i] = 0;
      sivalid[i] = 0;
    }
    
  }
  
  /* Loop until whole trace file has been read */
  mem_access_t access;
  while(1) {
    access = read_transaction(ptr_file);
    //If no transactions left, break out of loop
    if (access.address == 0)
      break;
    
    /* Do a cache access */
    
    if (cache_org == uc) {  //one cache - not split
      
      //increase counter
      UNumAccess++;
      
      // if direct mapping, unified cache
      if (cache_mapping == dm) {       
	
	// initialize these for calculations 
	numBlocks = findNumBlocks (cache_size);
	numIndexBits = findNumIndexBits (numBlocks); //will be one for fa
	numTagBits = findNumTagBits (numBlocks, numIndexBits);
	indexVal = findIndexVal (access.address, numTagBits);
	tagVal = findTagVal (access.address, numIndexBits);      
	int myIndex = ((int) indexVal % numBlocks);
	
	// check if tag is in cache at index. If also valid bit, will be a hit.
	if ((unifiedArray[myIndex] == tagVal) && (uavalid[myIndex] == 1)) {
	  UNumHit ++;
	} else {
	  
	  // else is a miss: will update cache, make bit valid.
	  unifiedArray [myIndex] = tagVal;
	  uavalid [myIndex] = 1;
	}
	
      } else { 	// FA cache mapping, unified cache
	
	//initialize for calculations
	numBlocks = findNumBlocks (cache_size);
	numIndexBits = findNumIndexBits (1); //will be 1 for fa - don't need an index
	numTagBits = findNumTagBits (numBlocks, numIndexBits);
	indexVal = findIndexVal (access.address, numTagBits);
	tagVal = findTagVal (access.address, numIndexBits);      
	
	// change is a "boolean" to keep track of if there is a hit
	int change = 0;
	int i = 0;
	
	// loop through entire cache, seeing if contains value of tag
	for (i; i < numBlocks; i++) {
	  if (unifiedArray [i] == tagVal) {
	    if (uavalid [i] == 1){
	      //is valid, so is hit
	      change = 1;
	    }
	    break;
	  }
	}
	
	if (change == 1) {  //is a hit
	  UNumHit ++;
	  //FIFOcount used to see which index to change if there is a miss
	  FIFOcount --;
	} else { 
	  // update cache if miss, make it a valid bit
	  int tempIndex = ((FIFOcount) % numBlocks); 
	  unifiedArray[tempIndex] = tagVal;
	  uavalid[tempIndex] = 1;
	}
	
      } 
      FIFOcount++; 
    } else {       //Split caches
      
      // same as before, but check if instruction type is data or instruction
      if (cache_mapping == dm) {
	
	//initialize variables again, this time at half the size per cache
	numBlocks = findNumBlocks (cache_size * 0.5);
	numIndexBits = findNumIndexBits (numBlocks);
	numTagBits = findNumTagBits (numBlocks, numIndexBits);
	indexVal = findIndexVal (access.address, numTagBits);
	tagVal = findTagVal (access.address, numIndexBits);      
	int myIndex = ((int) indexVal % numBlocks);
	
        if (access.accesstype == data) { //data instruction type
          DNumAccess ++;
	  
	  if ((splitDataArray [myIndex] == tagVal) && (sdvalid[myIndex] == 1)) {
	    DNumHit ++;
	  } else {
	    splitDataArray [myIndex] = tagVal;
	    sdvalid [myIndex] = 1;
	  }
	  
        } else { //instruction instruction type
          INumAccess ++;
	  
	  if ((splitInstructionArray [myIndex] == tagVal) && (sivalid[myIndex] == 1)) {
	    INumHit ++;
	  } else {
	    splitInstructionArray [myIndex] = tagVal;
	    sivalid [myIndex] = 1;
	  }
	  
        }
	
      } else { //FA cache mapping - again, same as before. Checking again what instruction type.
	
	//initialize variables again, this time at half the size per cache
	numBlocks = findNumBlocks (cache_size * 0.5);
	numIndexBits = findNumIndexBits (1); //again, for FA is 1 - no index needed
	numTagBits = findNumTagBits (numBlocks, numIndexBits);
	indexVal = findIndexVal (access.address, numTagBits);
	tagVal = findTagVal (access.address, numIndexBits);      
	
        if (access.accesstype == data) {  //data instruction type
          DNumAccess ++;
	  
	  int change = 0;
	  int i = 0;
	  for (i; i < numBlocks; i++) {
	    if (splitDataArray [i] == tagVal) {
              if (sdvalid [i] == 1){
		//is valid, so is hit
                change = 1;
	      }
	      break;
	    }
	  }
	  
	  if (change == 1) {
	    DNumHit ++;
	    FIFOData --;
	  } else { 
	    int tempIndex = ((FIFOData) % numBlocks); 
	    splitDataArray[tempIndex] = tagVal;
	    sdvalid[tempIndex] = 1;
	  }
	  FIFOData++; 
	  
	  
        } else {	  //instruction instruction type
          INumAccess ++;
	  
	  int change = 0;
	  int i = 0;
	  for (i; i < numBlocks; i++) {
	    if (splitInstructionArray [i] == tagVal) {
	      if (sivalid [i] == 1){
		//is valid, so is hit
		change = 1;
	      }
	      break;
	    }
	  }
	  
	  if (change == 1) {
	    INumHit ++;
	    FIFOInst --;
	  } else { 
	    int tempIndex = ((FIFOInst) % numBlocks); 
	    splitInstructionArray[tempIndex] = tagVal;
	    sivalid[tempIndex] = 1;
	  }
	  FIFOInst++; 	  	 	  
        }
	
      }
      
    }
    
  }
  
  /* Print the statistics */
  if (cache_org == uc) {
    printf("U.accesses: ");
    printf ("%i", UNumAccess);
    printf("\nU.hits: ");
    printf ("%i", UNumHit);
    
    UHitRate = (double) UNumHit / UNumAccess;
    printf("\nU.hit rate: ");
    printf ("%1.3f", UHitRate);
  } else {
    printf("I.accesses: ");
    printf ("%i", INumAccess);
    printf ("\nI.hits: ");
    printf ("%i", INumHit);

    IHitRate = (double) INumHit / INumAccess;
    printf("\nI.hit rate: ");
    printf("%1.3f", IHitRate);

    printf("\nD.accesses: "); 
    printf("%i", DNumAccess);
    printf("\nD.hits: ");
    printf("%i", DNumHit);

    DHitRate = (double) DNumHit / DNumAccess;
    printf("\nD.hit rate: ");
    printf("%1.3f", DHitRate);
  }
  
  // free the memory allocated to the caches and their respective valid bit arrays
  free (unifiedArray);
  free (uavalid);
  free (splitInstructionArray);
  free (sivalid);
  free (splitDataArray);   
  free (sdvalid);
  
  /* Close the trace file */
  fclose(ptr_file);
  
}
