
#include "ggeo_feature.h"



void free_Ggeo_Slice(Ggeo_Slice * s)
{
  assert(s != NULL);
  
  (*s->free_data)(s);
    
  return;
}


void free_Ggeo_Feature(Ggeo_Feature * f) 
{
  assert(f != NULL);

  (*f->free_data)(f);

  return;
}

void write_simple_bed_Ggeo_Slice(Ggeo_Slice * s,FILE * ofp) 
{
  Ggeo_Feature_LinkedList * ll;
  int i;

  assert(s != NULL);
  assert(ofp != NULL);

  ll = (*s->get_leftmost_Ggeo_Feature)(s->data);

  while( ll != NULL ) {
    fprintf(ofp,"%s\t%ld\t%ld\n",ll->f->seq_region->name,ll->f->start,ll->f->end);
    ll = ll->next;
  }
}
