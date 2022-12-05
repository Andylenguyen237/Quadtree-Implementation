#include <stdio.h>
#include <stdlib.h> 
#include <assert.h>
#include <string.h> 
#include <ctype.h>
#include <math.h>
#include "data.h"

#define MIN_ARG 7
#define FLAG "4"

/* Sort unique list and print query result to outFile */
void output_query_search(FILE *outFile, 
quadtreeNode_t *root, rectangle2D_t *bound, int size);

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
    long double bl_x, bl_y, tr_x, tr_y;
    while (fgets(read_query, sizeof(read_query), stdin)){
        read_query[strcspn(read_query,"\n")] = '\0';
        char *token = strtok(read_query, " ");
        int count = 1;
        char *tmp2;

        while(token){
            if (count == 1){
                printf("%s ", token);
                fprintf(outFile, "%s ", token);
                bl_x = strtold(token, &tmp2);
                count ++;
            }else if (count == 2){
                printf("%s ", token);
                fprintf(outFile, "%s ", token);
                bl_y = strtold(token, &tmp2);
                count ++;
            }else if (count == 3){
                printf("%s ", token);
                fprintf(outFile, "%s ", token);
                tr_x = strtold(token, &tmp2);
                count ++;
            }else{
                printf("%s ", token);
                fprintf(outFile, "%s ", token);
                tr_y = strtold(token, &tmp2);
            }
            
            token = strtok(NULL, " ");
        }
        
        printf("--> ");

        rectangle2D_t rec;
        rec.botLeft.x = bl_x;
        rec.botLeft.y = bl_y;
        rec.topRight.x = tr_x;
        rec.topRight.y = tr_y;

        // Print result to outFile 
        fprintf(outFile, "\n");
        
        // Print result to Stdout
        output_query_search(outFile, quadtree, &rec, numRecords);

        printf("\n");

    }

    /* Free Quadtree */
    freeQuadtree(quadtree);

    /* Close all Files */
    fclose(inFile);
    fclose(outFile);

    return 0;

}

/* Sort unique list and print query result to outFile */
void output_query_search(FILE *outFile, 
quadtreeNode_t *root, rectangle2D_t *bound, int size){

    list_t querylist;

    querylist.size_used = 0;
    querylist.size = size;
    querylist.list = (dataPoint_t **)malloc(sizeof(dataPoint_t *) * size);
    assert(querylist.list);
    
    rangeQuery(root, bound, &querylist);

	uniquelist_sort(outFile, querylist.list, querylist.size_used);

    // free memory
	free(querylist.list);

}
