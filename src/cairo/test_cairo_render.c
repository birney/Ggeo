
#include "ggeo_cairo_render.h"

#include "ggeo_inmemory.h"

#include "cairo.h"




int main(int argc,char ** argv)
{
  Ggeo_Track_Renderer_Factory * rf;
  Ggeo_Render_User_Parameter_Set * gps;
  Ggeo_Track_Renderer * tr;

  Ggeo_Render_Object * geo;
  
  cairo_surface_t * surface;

  Ggeo_Slice_Factory * f;
  Ggeo_Slice * s;
  Ggeo_TopLevel * tl;

  rf =  new_cairo_Ggeo_Track_Renderer_Factory();

  f = Ggeo_Slice_Factory_from_bed_file(stdin);

  tl = (*f->get_toplevel_by_string)(f->data,"chr1");

  s = (*f->get_slice_by_chr_start_end_overlap)(f->data,tl,1000,5000);

  gps = (*rf->get_default_parameters)(rf->data,"simple_feature");

  tr = (*rf->make_renderer)(rf->data,gps,"simple_feature");

  
  geo = (*tr->render_track)(tr->data,(*s->get_leftmost_Ggeo_Feature)(s->data,0),1000,5000);

  surface = (cairo_surface_t *) geo->data;


  cairo_surface_write_to_png(surface,"test_cairo.png");
  

  return(0);

}
