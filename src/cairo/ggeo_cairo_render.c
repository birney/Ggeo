
#include "ggeo_cairo_render.h"

#include "cairo.h"
#include <string.h>

/* the render object is just a wrap about a cairo_surface */


/* Cairo renders do not need any ancillery data per renderer   */
/* but by having the generic cairo function take the Cr object */
/* as input some more complex compositing can occur in the future */

typedef struct ggeo_track_renderer_cairo {
  char * response_type;
  void (*cairo_render_track)(cairo_t * cr,Ggeo_Render_User_Parameter_Set * para,Ggeo_Feature_LinkedList *,long int start,long int end);
} Ggeo_Track_Renderer_Cairo;


/* This is the binding between a Renderer and a Parameter Set */
/* it implements renderer track by binding the gp to the render */

typedef struct active_ggeo_track_renderer {
  Ggeo_Render_User_Parameter_Set * gp;
  Ggeo_Track_Renderer_Cairo * renderer;
} Active_Cairo_Renderer;


/* this is the main work horse to associate renders with response_types */

#define MAX_CAIRO_RENDERER 1024

typedef struct ggeo_track_render_factory_cairo {
  Ggeo_Track_Renderer_Cairo * renderers[MAX_CAIRO_RENDERER];
  int len;
} Ggeo_Track_Renderer_Factory_Cairo;



void simple_feature_Cairo_render_track(cairo_t * cr,Ggeo_Render_User_Parameter_Set * para,Ggeo_Feature_LinkedList * fl,long int start,long int end)
{
  double bpstart;
  double bpwidth;

  long int length;

  assert(cr != NULL);
  assert(para != NULL);
  assert(fl != NULL);

  cairo_save(cr);

  length = end - start +1;


  cairo_set_source_rgb(cr,para->red,para->green,para->blue);


  for(;fl != NULL;fl = fl->next) {
    bpstart = (fl->f->start - start)*(para->tile_pixel_width)/(double)length ;
    bpwidth = (fl->f->end - fl->f->start+1) * (para->tile_pixel_width)/(double)length ;
    fprintf(stderr,"Looking at %f to %f\n",bpstart,bpwidth);
    cairo_rectangle(cr,bpstart,1,bpwidth,8);
    cairo_fill(cr);
    cairo_stroke(cr);
  }

  cairo_restore(cr);
}

Ggeo_Track_Renderer_Cairo * new_simple_cairo_renderer(void)
{
  Ggeo_Track_Renderer_Cairo * out;

  out = malloc(sizeof(Ggeo_Track_Renderer_Cairo));

  out->response_type = strdup("simple_feature");
  out->cairo_render_track = simple_feature_Cairo_render_track;
  return(out);

}


Ggeo_Track_Renderer_Factory_Cairo * new_blank_Ggeo_Track_Renderer_Factory_Cairo(void)
{
  Ggeo_Track_Renderer_Factory_Cairo * out;

  out = malloc(sizeof(Ggeo_Track_Renderer_Factory_Cairo));

  out->len = 0;

  return(out);
}

void add_Ggeo_Track_Renderer_Factory_Cairo(Ggeo_Track_Renderer_Factory_Cairo * f,Ggeo_Track_Renderer_Cairo * c)
{
  assert(f != NULL);
  assert(c != NULL);

  assert(f->len < MAX_CAIRO_RENDERER);

  f->renderers[f->len++] = c;
}

Ggeo_Track_Renderer_Factory_Cairo * new_standard_Cairo_Renderer_Factory(void)
{
  Ggeo_Track_Renderer_Factory_Cairo * out;

  out = new_blank_Ggeo_Track_Renderer_Factory_Cairo();

  add_Ggeo_Track_Renderer_Factory_Cairo(out,new_simple_cairo_renderer());
  
  return(out);
}


void free_cairo_ggeo_render_object(struct ggeo_render_object * o)
{
  cairo_surface_t * s;
  assert(o);
  
  s = (cairo_surface_t *) o->data;

  cairo_surface_destroy(s);
  free(o);

  return;
}

Ggeo_Render_Object * call_cairo_render_track(void * data,Ggeo_Feature_LinkedList * fl,long int start, long int end) 
{
  Ggeo_Render_Object * g;
  Active_Cairo_Renderer * ac;
  cairo_surface_t * surface;
  cairo_t * cr;

  assert(data != NULL);
  assert(fl != NULL);
  
  ac = (Active_Cairo_Renderer *) data;
  g = malloc(sizeof(Ggeo_Render_Object));

  surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, ac->gp->tile_pixel_width , 10);

  g->data = (void *)surface;
  g->free_data = free_cairo_ggeo_render_object;
  g->start = start;
  g->end   = end;

  cr = cairo_create(surface);

  cairo_rectangle(cr, 0.0, 0.0, ac->gp->tile_pixel_width, 10);
  cairo_set_source_rgb(cr, 0.1, 0.0, 0.0);
  cairo_fill(cr);

  (*ac->renderer->cairo_render_track)(cr,ac->gp,fl,start,end);

  cairo_show_page(cr);

  cairo_destroy(cr);


  return(g);
}


void free_active_cairo(Ggeo_Track_Renderer * go)
{
  assert(go != NULL);
  free(go->data);
  free(go);
}


Ggeo_Render_User_Parameter_Set * default_cairo_gps(void * data,char * response_type)
{
  Ggeo_Render_User_Parameter_Set * out;

  /* at the moment 1024 pixels, blue */
  out = malloc(sizeof(Ggeo_Render_User_Parameter_Set));

  out->red = 0.0;
  out->green = 0.0;
  out->blue = 1.0;
  out->alpha_t = 1.0;

  out->tile_pixel_width = 1024;
  
  return(out);
}


Ggeo_Track_Renderer * make_renderer_cairo_factory(void * data,Ggeo_Render_User_Parameter_Set * gp,char * response_type)
{
  int i;
  Ggeo_Track_Renderer_Factory_Cairo * f;
  Ggeo_Track_Renderer * out;
  Active_Cairo_Renderer * ac;

  f = (Ggeo_Track_Renderer_Factory_Cairo*) data;

  for(i=0;i<f->len;i++) {
    if( strcmp(response_type,f->renderers[i]->response_type) == 0 ) {
      ac = malloc(sizeof(Active_Cairo_Renderer));
      ac->gp = gp;
      ac->renderer = f->renderers[i];
      
      out = malloc(sizeof(Ggeo_Track_Renderer));

      out->data = (void *) ac;
      out->free_data = free_active_cairo;
      out->render_track = call_cairo_render_track;
      return(out);
    }
  }

  return(NULL);
}
 
void free_cairo_factory(Ggeo_Track_Renderer_Factory * f)
{
  assert(f != NULL);

  free(f->data);
  free(f);
} 

Ggeo_Track_Renderer_Factory * new_cairo_Ggeo_Track_Renderer_Factory(void)
{
  Ggeo_Track_Renderer_Factory * out;
  Ggeo_Track_Renderer_Factory_Cairo * c;

  out = malloc(sizeof(Ggeo_Track_Renderer_Factory));

  c = new_standard_Cairo_Renderer_Factory();

  out->data = c;
  out->free_data = free_cairo_factory;
  out->get_default_parameters = default_cairo_gps;
  out->make_renderer = make_renderer_cairo_factory;

  

  return(out);
}

