#include <stdio.h>
#include <stdlib.h> 
#include <assert.h>
#include <string.h> 
#include <ctype.h>
#include <math.h>
#include "data.h"

#define MIN_ARG 7
#define FLAG "3"

/* Sort unique list and print query result to outFile */
void output_query_search(FILE *output, 
quadtreeNode_t *root, point2D_t *location, int size);

int
main (int argc, char *argv[]){

    /* Ensure correct flag and arg */
    if (argc < MIN_ARG || (strcmp(argv[1], FLAG) != 0)){
        printf("Error, Invalid Input and/or Wrong flag");
        exit(EXIT_FAILURE);
    }

    /* Open CSV File */
    FILE *inFile = fopen(argv[2], "r");
    FILE *outFile = fopen(argv[3], "w");
    assert(inFile && outFile);

    /* Skip header */
    while(fgetc(inFile) != '\n');
    
    /* Read input from Stdin */
    char *tmp;
    long double startlon = strtold(argv[4], &tmp);
    long double startlat = strtold(argv[5], &tmp);
    long double endlon = strtold(argv[6], &tmp);
    long double endlat = strtold(argv[7], &tmp);

    /* Create empty quadtree */
    quadtreeNode_t *quadtree = NULL;

    /* Insert ARG, create bound */
    rectangle2D_t bound;
    bound.botLeft.x = startlon;
    bound.botLeft.y = startlat;
    bound.topRight.x = endlon;
    bound.topRight.y = endlat;

    /* Construct quadtree */
    int numRecords = 0;
    quadtree = insertToQuadtree(inFile, quadtree, &bound, &numRecords);

    /* Read input from queryfile and search point */
    char read_query[MAX_STRING_LEN + 1];
    char str_lon[MAX_STRING_LEN + 1], str_lat[MAX_STRING_LEN + 1];
    long double query_lon, query_lat;

    while (fgets(read_query, sizeof(read_query), stdin)){
        read_query[strcspn(read_query,"\n")] = '\0';

        char *token = strtok(read_query, " ");
        int count = 1;

        while (token){
            if (count == 1){
                strcpy(str_lon, token);
                query_lon = strtold(token, &tmp);
                count ++;
            }else if (count == 2){
                strcpy(str_lat, token);
                query_lat = strtold(token, &tmp);
            }

            token = strtok(NULL, " ");
        }

        // Insert query coordinate
        point2D_t query;
        query.x = query_lon;
        query.y = query_lat;

        // Print to outFile 
        fprintf(outFile, "%s %s\n", str_lon, str_lat);
        
        // Print to Stdout
        printf("%s %s --> ", str_lon, str_lat);
        output_query_search(outFile, quadtree, &query, numRecords);
        printf("\n");

    }

    /* Free quadtree */
    freeQuadtree(quadtree);

    /* Close all Files */
    fclose(inFile);
    fclose(outFile);

    return 0;
}

/* Sort unique list and print query result to outFile */
void output_query_search(FILE *outFile, 
quadtreeNode_t *root, point2D_t *location, int size){

    list_t querylist;

    querylist.size_used = 0;
    querylist.size = size;
    querylist.list = (dataPoint_t **)malloc(sizeof(dataPoint_t *) * size);
    assert(querylist.list);

	searchPoint(root, *location, &querylist);

  	uniquelist_sort(outFile, querylist.list, querylist.size_used);

	// free memory
	free(querylist.list);
}













