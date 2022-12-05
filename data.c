#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include "data.h"

/* FOOTPATH FUNCTIONS */

/* Read footpath datafield from CSV file */
footpath_t *read_footpath(FILE *infile){

    footpath_t *record = NULL;
    int footpath_id;

    if (fscanf(infile, "%d,", &footpath_id) == 1){

      record = malloc(sizeof(*record));
      assert(record);

      record->footpath_id = footpath_id;
      
      read_csv_string(infile, record->address);
      read_csv_string(infile, record->clue_sa);
      read_csv_string(infile, record->asset_type);

      double tmp1, tmp2, tmp3;
      fscanf(infile, "%lf,%lf,%lf,%lf,%lf,%lf,%lf,", &record->deltaz, 
      &record->distance, &record->grade1in, &tmp1, &tmp2,
      &record->rlmax, &record->rlmin);
      record->mcc_id = tmp1;
      record->mccid_int = tmp2;

      read_csv_string(infile, record->segside);
      fscanf(infile, "%lf,%lf,%lf,%lf,%lf,%lf,%lf", &tmp1, &tmp2, &tmp3, 
      &record->start_lat, &record->start_lon, 
      &record->end_lat, &record->end_lon);
      record->statusid = tmp1;
      record->streetid = tmp2;
      record->street_group = tmp3;  

    }

    return record;

}

/* Read special string datafield */
void read_csv_string(FILE *infile, char *str){
    
    if (fscanf(infile, " \"%[^\"]\",", str) == 1||
    fscanf(infile, " %[^,],", str) == 1){
        return;
    }

    if (fscanf(infile, " %[,]", str) == 1){
        strcpy(str, "");
        return;
    }

}

/* Print data field from node to out file */
void print_result_outFile(FILE *outFile, footpath_t *record){
    
    fprintf(outFile, "--> footpath_id: %d ", record->footpath_id);
    fprintf(outFile, "|| address: %s ", record->address);
    fprintf(outFile, "|| clue_sa: %s ", record->clue_sa);
    fprintf(outFile, "|| asset_type: %s ", record->asset_type);
    fprintf(outFile, "|| deltaz: %lf ", record->deltaz);
    fprintf(outFile, "|| distance: %lf ", record->distance);
    fprintf(outFile, "|| grade1in: %lf ", record->grade1in);
    fprintf(outFile, "|| mcc_id: %d ", record->mcc_id);
    fprintf(outFile, "|| mccid_int: %d ", record->mccid_int);
    fprintf(outFile, "|| rlmax: %lf ", record->rlmax);
    fprintf(outFile, "|| rlmin: %lf ", record->rlmin);
    fprintf(outFile, "|| segside: %s ", record->segside);
    fprintf(outFile, "|| statusid: %d ",record->statusid);
    fprintf(outFile, "|| streetid: %d ",  record->streetid);
    fprintf(outFile, "|| street_group: %d ", record->street_group);
    fprintf(outFile, "|| start_lat: %lf ",  record->start_lat);
    fprintf(outFile, "|| start_lon: %lf ", record->start_lon);
    fprintf(outFile, "|| end_lat: %lf ", record->end_lat);
    fprintf(outFile, "|| end_lon: %lf || \n", record->end_lon);
    
} 

/* Sort list increasingly on footpath_id */
void uniquelist_sort(FILE *outFile, dataPoint_t *list[], int records) {

    // sort 
    for (int i = 0; i < records; i++){

        for (int j = 0; j < records; j++){

            if (list[i]->footpath.footpath_id < list[j]->footpath.footpath_id){
                dataPoint_t *tmp = list[i];
                list[i] = list[j];
                list[j] = tmp;
            }
        }
    }

    // print to outFile unique path
    int dup_id = 0;

	for (int i = 0; i < records; i++){

		if (list[i]->footpath.footpath_id != dup_id) {
			print_result_outFile(outFile, &list[i]->footpath);
			dup_id = list[i]->footpath.footpath_id;
		}
	}

}

/* ====================================================== */

/* QUADTREE FUNCTIONS */

/* Construct and add new point to quadtree */
quadtreeNode_t *insertToQuadtree(FILE *input, quadtreeNode_t *root, 
rectangle2D_t *bound, int *numRecords) {
	
    footpath_t *record;

	while ((record = read_footpath(input)) != NULL) {

        (*numRecords)++;

		dataPoint_t *datapoint;
		point2D_t start, end;
		start.x = record->start_lon;
		start.y = record->start_lat;
        end.x = record->end_lon;
		end.y = record->end_lat;
		
        // Insert start point if it's in bound 
		if (inRectangle(bound, start)) {
			datapoint = createNewDataPoint(record, &start);
			root = addPoint(root, bound, datapoint);
		}

        // Insert end point if it's in bound 
		if (inRectangle(bound, end)) {
			datapoint = createNewDataPoint(record, &end);
			root = addPoint(root, bound, datapoint);
		}

        free(record);
	}

	return root;
}


/* Create and allocate memory for newly created datapoint */
dataPoint_t *createNewDataPoint(footpath_t *data, point2D_t *location) {

		dataPoint_t *datapoint = (dataPoint_t*)malloc(sizeof(*datapoint));
        assert(datapoint);
		datapoint->footpath = *data; 
		datapoint->location.x = location->x;
		datapoint->location.y = location->y;
		datapoint->next = NULL;

		return datapoint;

}

/* Check whether a given 2D point lies within the rectangle,
 return 1 (TRUE) if it does */
int inRectangle(rectangle2D_t *bound, point2D_t point) {

    if (point.x > bound->botLeft.x && point.x <= bound->topRight.x &&
    point.y >= bound->botLeft.y && point.y < bound->topRight.y){
        return 1;
    }

    return 0;
	
}

/* Find the quadrant correspond to where the point should be located in */ 
int determineQuadrant(rectangle2D_t *bound, point2D_t *point) {

	point2D_t center;
	center.x = bound->botLeft.x + (bound->topRight.x - bound->botLeft.x) / 2.0;
	center.y = bound->botLeft.y + (bound->topRight.y - bound->botLeft.y) / 2.0 ;
	
	if (point->x <= center.x && point->y < center.y) return SW;
	else if (point->x <= center.x && point->y >= center.y) return NW;
    else if (point->x > center.x && point->y >= center.y) return NE;
	else return SE;
	
}

/* Find new sub-bound (returns rectangle bound) of a quadrant */
rectangle2D_t subDivide(int quadrant, rectangle2D_t *bound) {

    point2D_t center;
	center.x = bound->botLeft.x + (bound->topRight.x - bound->botLeft.x) / 2.0;
	center.y = bound->botLeft.y + (bound->topRight.y - bound->botLeft.y) / 2.0 ;

    rectangle2D_t sub_bound;

    if (quadrant == SW){
        sub_bound.topRight = center;
        sub_bound.botLeft = bound->botLeft;
    }
    else if (quadrant == NW){
        sub_bound.topRight.x = center.x;
        sub_bound.topRight.y = bound->topRight.y;
        sub_bound.botLeft.x = bound->botLeft.x;
        sub_bound.botLeft.y = center.y;
    }
    else if (quadrant == NE){
        sub_bound.topRight = bound->topRight;
        sub_bound.botLeft = center;
    }
    else{
        sub_bound.topRight.x = bound->topRight.x;
		sub_bound.topRight.y = center.y;
		sub_bound.botLeft.x = center.x;
		sub_bound.botLeft.y = bound->botLeft.y;
    }
	
	return sub_bound;
}

/* Add a datapoint given with its 2D coordinates to the quadtree */
quadtreeNode_t *addPoint(quadtreeNode_t *parent, 
rectangle2D_t *bound, dataPoint_t *datapoint) {

    /* WHITE NODE -- assign new data and update status to BLACK */
	if (parent == NULL) {

		parent = (quadtreeNode_t*)malloc(sizeof(quadtreeNode_t));
        assert(parent);

        parent->boundry = *bound;
        parent->data = datapoint;

		for (int i=0; i< MAX_QT_CAPACITY; i++) {
			parent->children[i] = NULL;
		}

        parent->status = BLACK;

        return parent;	
	}
	
	int insertQuad = determineQuadrant(bound, &datapoint->location);
	rectangle2D_t sub_bound = subDivide(insertQuad, bound);
	
    /* BLACK NODE -- when the node contain a data point */
	if (parent->status == BLACK) { 

		if (check_same_location(parent->data->location, datapoint->location)) {
			dataPoint_t *tmp = parent->data;

			while (tmp->next) {
				tmp = tmp->next;
			}

			tmp->next = datapoint;

		} else {
            
			parent->children[insertQuad] = addPoint(parent->children[insertQuad],
            &sub_bound, datapoint);

			// Push data down to the tree and update status to GRAY 
			insertQuad = determineQuadrant(bound, &parent->data->location);
			sub_bound = subDivide(insertQuad, bound);
			parent->children[insertQuad] = addPoint(parent->children[insertQuad],
            &sub_bound, parent->data);
			parent->data = NULL;

            // update status 
			parent->status = GRAY;
		}
			
	} 

    /* GRAY NODE -- contains more than 1 data points ie. has children nodes */
    /* Recursively find the target quadrant to insert new datapoint */
    else {
		parent->children[insertQuad] = addPoint(parent->children[insertQuad],
        &sub_bound, datapoint);
	}

	return parent;
}

/* Check whether 2 points lies in same location */
int check_same_location(point2D_t point1, point2D_t point2) {

	if (fabs(point1.x - point2.x) < __DBL_EPSILON__ 
    && fabs(point1.y - point2.y) < __DBL_EPSILON__ ){
        return 1;
    }

    return 0;
}

/* Tests whether a datapoint given by its 2D coordinates lies within the rectangle*/
void searchPoint(quadtreeNode_t *root, 
point2D_t query_point, list_t *query_list) {

	if (root == NULL || !inRectangle(&root->boundry, query_point)) return;

	// Returns when the node status is BLACK -- leaf node */
	if (root->status == BLACK){
        
        if (check_same_location(query_point, root->data->location)) {
            dataPoint_t *current = root->data;

            while (current) {
                query_list->list[query_list->size_used] = current;
                query_list->size_used++;
                current = current->next;
            }
        }
        return;
    
	}

	int quadrant = determineQuadrant(&root->boundry, &query_point);

	if (quadrant == SW) printf("SW ");
	else if (quadrant == NW) printf("NW ");
	else if (quadrant == NE) printf("NE ");
	else printf("SE ");

	searchPoint(root->children[quadrant], query_point, query_list);
}

/* Tests whether two given rectangles overlap and returns 1 (TRUE) if they do.*/
int rectangleOverlap(rectangle2D_t *rec1, rectangle2D_t *rec2) {
   
    // when rec1 is set aside rec2 or the pther way
	if (rec1->topRight.x < rec2->botLeft.x || 
		rec1->botLeft.x > rec2->topRight.x) {
		return 0;
	}
        
    // when rec1 is set above rec2 or the otherway
    if (rec1->topRight.y < rec2->botLeft.y || 
		rec1->botLeft.y > rec2->topRight.y) {
		return 0;
	}
        
    return 1;
}

/* Takes a 2D rectangle as argument and append all datapoints in the PR quadtree 
   whose coordinates lie within the query rectangle to a list of found queries */
void rangeQuery(quadtreeNode_t *root, rectangle2D_t *bound, list_t *querylist) {

	if (root == NULL || !rectangleOverlap(&root->boundry, bound)) return;

	if (root->status == BLACK){
        if (inRectangle(bound, root->data->location)) {
            dataPoint_t *tmp = root->data;

            while (tmp) {
                ensureSize(querylist);
                querylist->list[querylist->size_used] = tmp;
                querylist->size_used++;
                tmp = tmp->next;
            }
        }
        return;
	}
	
	if (root->children[SW] && 
    rectangleOverlap(&root->children[SW]->boundry, bound)) {
		printf("SW ");
		rangeQuery(root->children[SW], bound, querylist);
	}
	
	if (root->children[NW] && 
    rectangleOverlap(&root->children[NW]->boundry, bound)) {
		printf("NW ");
		rangeQuery(root->children[NW], bound, querylist);
	}

	if (root->children[NE] && 
    rectangleOverlap(&root->children[NE]->boundry, bound)) {
		printf("NE ");
		rangeQuery(root->children[NE], bound, querylist);
	}
	
	if (root->children[SE] && 
    rectangleOverlap(&root->children[SE]->boundry, bound)) {
		printf("SE ");
		rangeQuery(root->children[SE], bound, querylist);
	}

}

/* Check and ensure size used by the list to allocate enough memory */
void ensureSize(list_t *list){

    if (list->size_used < list->size) return;

    list->size *= STEPSIZE;
    list->list = realloc(list->list, sizeof(dataPoint_t *) * list->size);
    assert(list->list);

}

/* Free quadtree nodes */
void freeQuadtree(quadtreeNode_t *root) {

	if (root == NULL) return;

	if (root->status == BLACK) {
		dataPoint_t *tmp, *prev;
        tmp = root->data;

		while (tmp) {
			prev = tmp;
			tmp = tmp->next;
			free(prev);
		}
	}
	
	freeQuadtree(root->children[SW]);
	freeQuadtree(root->children[NW]);
	freeQuadtree(root->children[NE]);
	freeQuadtree(root->children[SE]);

	free(root);
}

