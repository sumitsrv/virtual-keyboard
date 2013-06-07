/* imgCED.c */

#include "tools.h"
#include "eigen.h"
#include <math.h>

#define PRGNAME "CED"
#define ERROR(e) imgError(e,PRGNAME)

#define PI 3.14159265
#define PI_OVER_8 (PI / 8.0)

int main(int argc,char **argv)
{
  it_image *out1,*rin,*gin,*bin,*im;
  it_float q1,q2,q4;
  it_image *e1,*e2,*v;
  it_float fe1, fe2, fv;
  int width=0,height=0,x,y;
  int v_x=0,v_y=0,v_width=0,v_height=0,y_end=0;
  it_float *Rr0_ptr,*Rr1_ptr,*Rr2_ptr,*ptr1;
  it_float *Gr0_ptr,*Gr1_ptr,*Gr2_ptr;
  it_float *Br0_ptr,*Br1_ptr,*Br2_ptr;
  it_float *r0_ptr,*r1_ptr,*r2_ptr;
  double val,u1,u2,aa,bb,cc,dd,ee,ff,gg,hh,ii,rx,ry,gx,gy,bx,by;
  double min,max,angle;

  IFHELP
    {
      fprintf(stderr,"img%s - Perform Canny color edge detection\n",
	      PRGNAME);
      fprintf(stderr,"  stdin: Float\n");
      fprintf(stderr,"  stdout: Float\n");
      exit(0);
    }

  imgStart(PRGNAME);

  /* Loop for all images */
  do {
    rin=i_read_image_file(stdin,IT_FLOAT,IM_FRAGMENT);
    if(rin==NULL)
      ERROR("can't import image file");
    gin=i_read_image_file(stdin,IT_FLOAT,IM_FRAGMENT);
    if(gin==NULL)
      ERROR("can't import image file");
    bin=i_read_image_file(stdin,IT_FLOAT,IM_FRAGMENT);
    if(bin==NULL)
      ERROR("can't import image file");
    width=rin->width;
    height=rin->height;
    v_x=rin->valid_x+1;
    v_y=rin->valid_y+1;
    v_width=rin->valid_width-2;
    v_height=rin->valid_height-2;
    y_end=v_y+v_height;
	    
    out1=i_create_image(width,height,IT_FLOAT,IM_FRAGMENT);
    if(out1==NULL)
      ERROR("out of memory");

    im=i_create_image(width,height,IT_FLOAT,IM_FRAGMENT);
    if(im==NULL)
      ERROR("out of memory");


    e1=i_create_image(width,height,IT_FLOAT,IM_FRAGMENT);
    if(e1==NULL)
      ERROR("out of memory");
    e2=i_create_image(width,height,IT_FLOAT,IM_FRAGMENT);
    if(e2==NULL)
      ERROR("out of memory");
    v=i_create_image(width,height,IT_FLOAT,IM_FRAGMENT);
    if(v==NULL)
      ERROR("out of memory");  

    
    imgInitMinMax(min,max);
    for(y=v_y;y<y_end;y++) {
      Rr0_ptr=im_float_row(rin,y-1)+v_x-1;
      Rr1_ptr=im_float_row(rin,y)+v_x-1;
      Rr2_ptr=im_float_row(rin,y+1)+v_x-1;
      Gr0_ptr=im_float_row(gin,y-1)+v_x-1;
      Gr1_ptr=im_float_row(gin,y)+v_x-1;
      Gr2_ptr=im_float_row(gin,y+1)+v_x-1;
      Br0_ptr=im_float_row(bin,y-1)+v_x-1;
      Br1_ptr=im_float_row(bin,y)+v_x-1;
      Br2_ptr=im_float_row(bin,y+1)+v_x-1;

      for(x=0;x<v_width;x++) {
	Rr0_ptr++;
	bb= *Rr0_ptr++;
	Rr0_ptr--;
	dd= *Rr1_ptr++;
	Rr1_ptr++;
	ff= *Rr1_ptr--;
	Rr2_ptr++;
	hh= *Rr2_ptr++;
	Rr2_ptr--;
	rx=(ff-dd)/2.0;
	ry=(bb-hh)/2.0;
	    
	Gr0_ptr++;
	bb= *Gr0_ptr++;
	Gr0_ptr--;
	dd= *Gr1_ptr++;
	Gr1_ptr++;
	ff= *Gr1_ptr--;
	Gr2_ptr++;
	hh= *Gr2_ptr++;
	Gr2_ptr--;
	gx=(ff-dd)/2.0;
	gy=(bb-hh)/2.0;
	
	Br0_ptr++;
	bb= *Br0_ptr++;
	Br0_ptr--;
	dd= *Br1_ptr++;
	Br1_ptr++;
	ff= *Br1_ptr--;
	Br2_ptr++;
	hh= *Br2_ptr++;
	Br2_ptr--;
	bx=(ff-dd)/2.0;
	by=(bb-hh)/2.0;
	
	q1=rx*rx+gx*gx+bx*bx;
	q2=rx*ry+gx*gy+bx*by;
	q4=ry*ry+gy*gy+by*by;
	
	eigen(q1, q2, q2, q4, &fe1, &fe2, &fv);
	im_float_value(e1,x+v_x,y) = fe1;
	im_float_value(e2,x+v_x,y) = fe2;
	im_float_value(v,x+v_x,y) = sqrt(fv);

      }
    }
    
    /* i_write_image_file(stdout, v, IF_BINARY); */

    for(y=v_y;y<y_end;y++) {
      r0_ptr=im_float_row(v,y-1)+v_x-1;
      r1_ptr=im_float_row(v,y)+v_x-1;
      r2_ptr=im_float_row(v,y+1)+v_x-1;
      ptr1=im_float_row(out1,y)+v_x;
      for(x=0;x<v_width;x++) {
	aa= *r0_ptr++;
	bb= *r0_ptr++;
	cc= *r0_ptr--;
	dd= *r1_ptr++;
	ee= *r1_ptr++;
	ff= *r1_ptr--;
	gg= *r2_ptr++;
	hh= *r2_ptr++;
	ii= *r2_ptr--;
	gx=im_float_value(e1,x+v_x,y);
	gy=im_float_value(e2,x+v_x,y);
	angle = atan2(gy, gx);

	if ((angle < -7 * PI_OVER_8) ||
	    ((angle >= - PI_OVER_8) && (angle < PI_OVER_8)) ||
	    (angle >= 7 * PI_OVER_8)) {
	  u1=dd;
	  u2=ff;
	} else if (((angle >= -5 * PI_OVER_8) && (angle < -3 * PI_OVER_8)) ||
		   ((angle >= 3 * PI_OVER_8) && (angle < 5 * PI_OVER_8))) {
	  u1=bb;
	  u2=hh;
	} else if (((angle >= -7 * PI_OVER_8) && (angle < -5 * PI_OVER_8)) ||
		   ((angle >= PI_OVER_8) && (angle < 3 * PI_OVER_8))) {
	  u1=cc;
	  u2=gg;
	} else if (((angle >= -3 * PI_OVER_8) && (angle < - PI_OVER_8)) ||
		   ((angle >= 5 * PI_OVER_8) && (angle < 7 * PI_OVER_8))) {
	  u1=aa;
	  u2=ii; 
	} else {
	  printf("Big error. Angle = %f\n", angle);
	}
	val=0.0;
	if(ee>u1 &&ee>u2)
	  val=ee;
	*ptr1++=val;
	if(val>max) max=val;
	if(val<min) min=val;
      }
    }

    out1->min_value=min;
    out1->max_value=max;
    out1->valid_x=v_x;
    out1->valid_y=v_y;
    out1->valid_width=v_width;
    out1->valid_height=v_height;
    i_write_image_file(stdout, out1, IF_BINARY);

    i_destroy_image(e1);
    i_destroy_image(e2);
    i_destroy_image(v);
    i_destroy_image(out1);
    i_destroy_image(im);

  } while(!feof(stdin));

  imgFinish(PRGNAME);
  return(0);
}
