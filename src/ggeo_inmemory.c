
#include "ggeo_feature.h"

#include <string.h>


typedef struct GgeoInmemory_TopLevel {
  Ggeo_TopLevel ** active_toplevels;
  int len;
  int maxlen;
} GgeoInmemory_TopLevel;

typedef struct GgeoInmemory_simple_store {
  GgeoInmemory_TopLevel * tl;
  Ggeo_Feature ** array;
  int len;
  int maxlen;
} GgeoInmemory_simple_store;

typedef struct GgeoInmemory_slice {
  Ggeo_Feature_LinkedList * head;
} GgeoInmemory_slice;
  


GgeoInmemory_TopLevel * new_GgeoInmemory_TopLevel(void) 
{
  GgeoInmemory_TopLevel * out;

  out = (GgeoInmemory_TopLevel *) malloc(sizeof(GgeoInmemory_TopLevel));

  assert(out != NULL);

  out->active_toplevels = calloc(50,sizeof(Ggeo_TopLevel *));

  assert(out->active_toplevels);

  out->len = 0;
  out->maxlen = 50;

  return(out);
}

const Ggeo_TopLevel * fetch_or_store_GgeoInmemory_TopLevel(GgeoInmemory_TopLevel * tl,const char * name)
{
  int i;
  
  assert(tl != NULL);
  assert(name != NULL);

  for(i=0;i<tl->len;i++) {
    if( strcmp(tl->active_toplevels[i]->name,name) == 0 ) {
      return (const Ggeo_TopLevel *) tl->active_toplevels[i];
    }
  }

  /* not in yet */

  if( tl->len >= tl->maxlen ) {
    tl->active_toplevels = realloc(tl->active_toplevels,sizeof(Ggeo_TopLevel *)*(tl->maxlen *2));
  }

  tl->active_toplevels[tl->len] = malloc(sizeof(Ggeo_TopLevel));
  tl->active_toplevels[tl->len]->name = calloc(strlen(name),sizeof(char));
  strcpy( tl->active_toplevels[tl->len]->name,name);

  tl->len++;

  return(tl->active_toplevels[tl->len-1]);

}

Ggeo_TopLevel * get_toplevel_by_string_GeoInmemory_TopLevel(GgeoInmemory_TopLevel * tl,char * name)
{
  int i;


  for(i=0;i<tl->len;i++) {
    if( strcmp(tl->active_toplevels[i]->name,name) == 0 ) {
      return(tl->active_toplevels[i]);
    }
  }
  
  return(NULL);
}


GgeoInmemory_simple_store * new_GgeoInmemory_simple_store(void)
{
  GgeoInmemory_simple_store * out;

  out = malloc(sizeof(GgeoInmemory_simple_store));
  out->array = calloc(1024,sizeof(Ggeo_Feature *));
  out->len = 0;
  out->maxlen = 1024;
  out->tl = new_GgeoInmemory_TopLevel();

  return(out);
}

int qsort_compare_Ggeo_Feature(const void * a,const void *b)
{
  Ggeo_Feature * af;
  Ggeo_Feature * bf;

  af = (Ggeo_Feature *) * (Ggeo_Feature **)a;
  bf = (Ggeo_Feature *) * (Ggeo_Feature **)b;

  if( af->seq_region == bf->seq_region ) {
    return(af->start - bf->start);
  } else {
    return (strcmp(af->seq_region->name,bf->seq_region->name));
  }
}

void sort_GgeoInmemory_simple_store(GgeoInmemory_simple_store * gmem)
{
  assert(gmem != NULL);
  qsort(gmem->array,gmem->len,sizeof(Ggeo_Feature*),qsort_compare_Ggeo_Feature);
}

void store_Ggeo_Feature_GgeoInmemory_simple_store(GgeoInmemory_simple_store * gmem,Ggeo_Feature * f)
{
  assert(f != NULL);
  assert(gmem != NULL);

  if( gmem->len >= gmem->maxlen ) {
    gmem->array = realloc(gmem->array,sizeof(Ggeo_Feature*)*(gmem->maxlen *2));
  }
  assert(gmem->array != NULL);

  gmem->array[gmem->len] = f;

  gmem->len++;
}

void no_frills_no_refcount_Ggeo_Feature_free(Ggeo_Feature * f)
{
  return;
}

Ggeo_Feature * no_frills_no_refcount_Ggeo_Feature(const Ggeo_TopLevel * tl,long int start,long int end) 
{
  Ggeo_Feature * out;
  assert(tl != NULL);
  
  out = malloc(sizeof(Ggeo_Feature));
  assert(out != NULL);

  out->seq_region = tl;

  out->start = start;
  out->end = end;
  out->free_data = &(no_frills_no_refcount_Ggeo_Feature_free);
  out->data = NULL;

  return (out);
}


/* no free at the linkedlist level as we will be freeing only
   when the slice is freed */

void GgeoInmemory_linkedlist_dummy_free(struct ggeo_feature_linked_list * l)
{
  return;
}

Ggeo_Feature_LinkedList * new_no_frills_Ggeo_Feature_LinkedList(void)
{
  Ggeo_Feature_LinkedList * out;
  
  out = malloc(sizeof(Ggeo_Feature_LinkedList));
  out->next = NULL;

  out->free_data = &GgeoInmemory_linkedlist_dummy_free;
  
  return(out);
}

GgeoInmemory_slice * new_GgeoInmemory_slice_chr_start_end(GgeoInmemory_simple_store * gmem,const Ggeo_TopLevel * tl,long int start,long int end)
{
  int i;
  GgeoInmemory_slice * out;
  Ggeo_Feature_LinkedList * curr;
  
  out = malloc(sizeof(GgeoInmemory_slice));

  assert(out != NULL);

  assert(gmem != NULL);
  assert(tl != NULL);
  assert(start > 0);
  assert(end > 0);


  /* we need to loop over array until we get to the first position 
     that could qualify, when end crossed into start. As end >= start, 
     this is done by handling on N
  */

  for(i=0;i<gmem->len;i++) {
    if( gmem->array[i]->seq_region != tl ) {
      continue;
    }
    if( gmem->array[i]->end >= start ) {
      break;
    }
  }

  /* 
     if we hit len, set to NULL, return
  */

  if( i >= gmem->len ) {
    out->head = NULL;
    return(out);
  }

  out->head = new_no_frills_Ggeo_Feature_LinkedList();

  out->head->f = gmem->array[i];

  curr = out->head;

  /* this is a bit more subtle. We've sorted on
     start, not end, so there can be features that don't
     overlap the region where f->start > head, but end is < coordinate start
     so, we continue through the array, noting when f->start > end
     (this terminates the search). However, we have to make sure that end
     is greater than our start to slice to put it into the linked list
  */

  for(i++;i< gmem->len && gmem->array[i]->start <= end && gmem->array[i]->seq_region == tl;i++) {
    if( gmem->array[i]->end > start ) {
      curr->next = new_no_frills_Ggeo_Feature_LinkedList();
      curr->next->f = gmem->array[i];
      curr = curr->next;
    }
  }
  
  return(out);
}

void recursive_linkedlist_free_GgeoInmemory_slice(Ggeo_Feature_LinkedList * l)
{
  if( l == NULL ) {
    return;
  }
  if( l->next != NULL) {
    recursive_linkedlist_free_GgeoInmemory_slice(l->next);
  }
  free(l);
}

void free_GgeoInmemory_slice(Ggeo_Slice * genericsl)
{
  GgeoInmemory_slice * sl;

  sl = (GgeoInmemory_slice *) genericsl->data;

  /* recursively free linked list */
  if( sl->head != NULL ) {
    recursive_linkedlist_free_GgeoInmemory_slice(sl->head);
  }
  
  free(sl);
  free(genericsl);
}

Ggeo_Feature_LinkedList * get_leftmost_Ggeo_Feature_inmemory(void * data)
{
  GgeoInmemory_slice * sl;

  sl = (GgeoInmemory_slice *) data;

  return (sl->head);
}

Ggeo_Slice * get_GgeoInmemory_slice_by_chr_start_end(void * data,const Ggeo_TopLevel * tl,long int start,long int end)
{
  GgeoInmemory_simple_store * gmem;
  GgeoInmemory_slice * sl;
  Ggeo_Slice * out;

  gmem = (GgeoInmemory_simple_store *) data;

  assert(gmem != NULL);
  assert(tl != NULL);

  sl = new_GgeoInmemory_slice_chr_start_end(gmem,tl,start,end);
  
  assert(sl != NULL);

  out = malloc(sizeof(Ggeo_Slice));

  out->data= (void*) sl;
  out->free_data = &free_GgeoInmemory_slice;
  out->get_leftmost_Ggeo_Feature = &get_leftmost_Ggeo_Feature_inmemory;

  return(out);

}

GgeoInmemory_simple_store * new_GgeoInmemory_simple_store_from_bed(FILE * ifp)
{
  GgeoInmemory_simple_store * out;
  Ggeo_Feature * f;
  Ggeo_TopLevel * tl = NULL;
  int i;
  char buffer[1024];

  char chr[1024];
  long int start;
  long int end;

  
  assert(ifp != NULL);

  out = new_GgeoInmemory_simple_store();

  while( fgets(buffer,1024,ifp) != NULL ) {
    if( buffer[0] == '#' ) {
      continue;
    }

    sscanf(buffer,"%s\t%lu\t%lu",chr,&start,&end);
    if( tl == NULL || strcmp(tl->name,chr) != 0 ) {
      tl = (Ggeo_TopLevel *) fetch_or_store_GgeoInmemory_TopLevel(out->tl,chr);
    }
    assert(tl != NULL);

    f = no_frills_no_refcount_Ggeo_Feature(tl,start,end);

    store_Ggeo_Feature_GgeoInmemory_simple_store(out,f);
  }
    
  sort_GgeoInmemory_simple_store(out);
  
  return(out);
}

void free_GeoInmemory_simple_store(Ggeo_Slice_Factory * sf)
{
  int i;
  GgeoInmemory_simple_store * gmem;

  gmem = (GgeoInmemory_simple_store *) sf->data;

  for(i=0;i<gmem->len;i++) {
    free(gmem->array[i]);
  }

  free(gmem->array);
  free(gmem);

  free(sf);
  
}

Ggeo_TopLevel * get_toplevel_by_string_GgeoInmemory(void * data,char * name)
{
  GgeoInmemory_simple_store * gmem;

  gmem = (GgeoInmemory_simple_store*) data;

  return(get_toplevel_by_string_GeoInmemory_TopLevel(gmem->tl,name));
}

Ggeo_Slice_Factory * Ggeo_Slice_Factory_from_GgeoInmemory_simple_store(GgeoInmemory_simple_store * gmem)
{
  Ggeo_Slice_Factory * out;

  assert(gmem != NULL);

  out = malloc(sizeof(Ggeo_Slice_Factory));

  out->data = (void *) gmem;
  out->free_data = &free_GeoInmemory_simple_store;
  out->get_toplevel_by_string = &get_toplevel_by_string_GgeoInmemory;
  out->get_slice_by_chr_start_end_overlap = & get_GgeoInmemory_slice_by_chr_start_end;
  
  return(out);
}

Ggeo_Slice_Factory * Ggeo_Slice_Factory_from_bed_file(FILE * ifp)
{
  GgeoInmemory_simple_store * gmem;

  gmem = new_GgeoInmemory_simple_store_from_bed(ifp);

  return(Ggeo_Slice_Factory_from_GgeoInmemory_simple_store(gmem));
}
