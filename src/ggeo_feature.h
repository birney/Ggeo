
#ifndef GGEO_FEATURE
#define GGEO_FEATURE

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define GGEO_FEATURE_INTVALS 16
#define GGEO_FEATURE_DOUBLEVALS 16

/* these are always considered to be constant */

typedef struct ggeo_toplevel {
  char * name;
} Ggeo_TopLevel;


typedef struct ggeo_feature {
  void * data;
  const Ggeo_TopLevel * seq_region;
  long int start;
  long int end;
  short int strand;
  int int_vals[GGEO_FEATURE_INTVALS];
  double double_vals[GGEO_FEATURE_DOUBLEVALS];
} Ggeo_Feature;


typedef struct ggeo_feature_linked_list {
  void * data;
  void (*free_data)(struct ggeo_feature_linked_list *); /* could be as simple as freeing this structure */
  char * response_type;
  Ggeo_Feature * f;
  struct ggeo_feature_linked_list * next;
} Ggeo_Feature_LinkedList;


typedef struct ggeo_feature_slice {
  void * data;
  void (*free_data)(struct ggeo_feature_slice *);
  Ggeo_Feature_LinkedList * (*get_leftmost_Ggeo_Feature)(void *,int);
} Ggeo_Slice;

typedef struct ggeo_feature_slice_factory {
  void * data;
  char ** response_types;
  void (*free_data)(struct ggeo_feature_slice_factory *);
  Ggeo_TopLevel * (*get_toplevel_by_string)(void * data,char *);
  Ggeo_Slice * (*get_slice_by_chr_start_end_overlap)(void * data,const Ggeo_TopLevel *,long int,long int);
} Ggeo_Slice_Factory;



void free_Ggeo_Slice(Ggeo_Slice * s);

void write_simple_bed_Ggeo_Slice(Ggeo_Slice * s,FILE * ofp);



#endif
