
#include "ggeo_feature.h"



void free_Ggeo_Slice(Ggeo_Slice * s)
{
  assert(s != NULL);
  
  (*s->free_data)(s);
    
  return;
}


void write_simple_bed_Ggeo_Slice(Ggeo_Slice * s,FILE * ofp) 
{
  Ggeo_Feature_LinkedList * ll;

  assert(s != NULL);
  assert(ofp != NULL);

  ll = (*s->get_leftmost_Ggeo_Feature)(s->data,0);

  while( ll != NULL ) {
    fprintf(ofp,"%s\t%ld\t%ld\n",ll->f->seq_region->name,ll->f->start,ll->f->end);
    ll = ll->next;
  }
}
