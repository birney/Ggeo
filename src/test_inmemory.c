
#include "ggeo_inmemory.h"


int main(int argc,char ** argv)
{
  Ggeo_Slice_Factory * f;
  Ggeo_Slice * s;
  Ggeo_TopLevel * tl;

  f = Ggeo_Slice_Factory_from_bed_file(stdin);

  tl = (*f->get_toplevel_by_string)(f->data,"chr1");

  s = (*f->get_slice_by_chr_start_end_overlap)(f->data,tl,1000,5000);

  write_simple_bed_Ggeo_Slice(s,stdout);
}


