#ifndef _DATA_H_
#define _DATA_H_

#include <stdio.h>

/* CONSTANT DEFINITION */

#define MAX_STRING_LEN 128 
#define MAX_LINE_LEN 512

#define BLACK 1
#define GRAY 0

#define SW 0
#define NW 1
#define NE 2
#define SE 3

#define MAX_QT_CAPACITY 4

#define STEPSIZE 2

/* STRUCT DECLARATION */

typedef struct footpath {

    int footpath_id;
    char address[MAX_STRING_LEN + 1];
    char clue_sa[MAX_STRING_LEN + 1];
    char asset_type[MAX_STRING_LEN + 1];
    double deltaz;
    double distance;
    double grade1in;
    int mcc_id;
    int mccid_int;
    double rlmax;
    double rlmin;
    char segside[MAX_STRING_LEN + 1];
    int statusid;
    int streetid;
    int street_group;
    double start_lat;
    double start_lon;
    double end_lat;
    double end_lon;

} footpath_t;

typedef struct point2D{
    long double x;
    long double y;
} point2D_t;

typedef struct dataPoint{
    point2D_t location;
    footpath_t footpath;
    struct dataPoint *next;
} dataPoint_t;

typedef struct rectangle2D {
    point2D_t topRight;
    point2D_t botLeft;
} rectangle2D_t;

typedef struct quadtreeNode {

    int status;
    rectangle2D_t boundry;
    dataPoint_t *data;
    struct quadtreeNode *children[MAX_QT_CAPACITY];

} quadtreeNode_t;

typedef struct list{

    dataPoint_t **list;
    int size;
    int size_used;

} list_t;

/* FUNCTION PROTOTYPES */

/* Read footpath datafield from CSV file */
footpath_t *read_footpath(FILE *infile);

/* Read special string datafield */
void read_csv_string(FILE *infile, char *str);

/* Print data field from node to out file */
void print_result_outFile(FILE *outFile, footpath_t *record);

/* Sort list increasingly on footpath_id */
void uniquelist_sort(FILE *outFile, dataPoint_t *list[], int records);

/* ========== Quadtree Implementation ============== */

/* Construct and add new point to quadtree */
quadtreeNode_t *insertToQuadtree(FILE *input, 
quadtreeNode_t *root, rectangle2D_t *bound, int *numRecords);

/* Create and allocate memory for newly created datapoint */
dataPoint_t *createNewDataPoint(footpath_t *data, point2D_t *location); 

/* Check whether a given 2D point lies within the rectangle,
 return 1 (TRUE) if it does */
int inRectangle(rectangle2D_t *bound, point2D_t point);

/* Find the quadrant correspond to 
where the point should be located in */ 
int determineQuadrant(rectangle2D_t *bound, point2D_t *point);

/* Find new sub-area (returns rectangle bound) of a quadrant */
rectangle2D_t subDivide(int quadrant, rectangle2D_t *bound);

/* Add a datapoint given with its 2D coordinates to the quadtree */
quadtreeNode_t *addPoint(quadtreeNode_t *parent, 
rectangle2D_t *bound, dataPoint_t *datapoint);

/* Check whether 2 points lies in same location */
int check_same_location(point2D_t point1, point2D_t point2);

/* Tests whether a datapoint given by its 2D coordinates
 lies within the rectangle*/
void searchPoint(quadtreeNode_t *root, 
point2D_t query_point, list_t *query_list);

/* Tests whether two given rectangles overlap and 
returns 1 (TRUE) if they do.*/
int rectangleOverlap(rectangle2D_t *rec1, rectangle2D_t *rec2);

/* Takes a 2D rectangle as argument and append all datapoints in
 the PR quadtree whose coordinates lie within the query 
 rectangle to a list of found queries */
void rangeQuery(quadtreeNode_t *root, 
rectangle2D_t *bound, list_t *querylist);

/* Free quadtree nodes */
void freeQuadtree(quadtreeNode_t *root);

/* Check and ensure size used by the list to allocate enough memory */
void ensureSize(list_t *list);

#endif


