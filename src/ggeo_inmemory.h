
#ifndef GGEO_FEATURE
#define GGEO_FEATURE

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/* these are always considered to be constant */

typedef struct ggeo_toplevel {
  char * name;
} Ggeo_TopLevel;


typedef struct ggeo_feature {
  void * data;
  void (*free_data)(struct ggeo_feature *);
  const Ggeo_TopLevel * seq_region;
  long int start;
  long int end;
  short int strand;
} Ggeo_Feature;


typedef struct ggeo_feature_linked_list {
  void * data;
  void (*free_data)(struct ggeo_feature_linked_list *); /* could be as simple as freeing this structure */
  Ggeo_Feature * f;
  struct ggeo_feature_linked_list * next;
} Ggeo_Feature_LinkedList;

/* at first glance this datastructure looks redundant to the above
   and this might well be true for some implementations; however
   the split is because the get_leftmost_Ggeo_Feature might be called
   multiple times and the lists free'd in between, and yet the implementation
   might want to cache aspects of the slice.
*/

typedef struct ggeo_feature_slice {
  void * data;
  void (*free_data)(struct ggeo_feature_slice *);
  Ggeo_Feature_LinkedList * (*get_leftmost_Ggeo_Feature)(void *);
} Ggeo_Slice;

typedef struct ggeo_feature_slice_factory {
  void * data;
  void (*free_data)(struct ggeo_feature_slice_factory *);
  Ggeo_TopLevel * (*get_toplevel_by_string)(void * data,char *);
  Ggeo_Slice * (*get_slice_by_chr_start_end_overlap)(void * data,const Ggeo_TopLevel *,long int,long int);
} Ggeo_Slice_Factory;



/* helper functions to handle the pointers in a nice typesafe way */

void free_Ggeo_Feature(Ggeo_Feature * f);

void free_Ggeo_Slice(Ggeo_Slice * s);

void write_simple_bed_Ggeo_Slice(Ggeo_Slice * s,FILE * ofp);



#endif
