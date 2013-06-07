#include <math.h>
#include <assert.h>

#include "eigen.h"

void eigen(float q1, float q2, float q3, float q4,
	   float * e1, float * e2, float * v) {

  /*
   * Takes matrix [[q1 q2][q3 q4]]. Eigenvector with highest
   * eigenvalue goes in [[e1][e2]]. Eigenvalue goes in v.
   */

  /* operation is (A - Iv == 0) */

  float a, b, c;

  a = 1.0;
  b = -(q1 + q4);		/* always negative */
  c = (q1 * q4 - q2 * q3);

  *v = quad(a, b, c);

  /*
   * OK, so this is a quick and dirty hack to get the eigenvalue;
   * both rows of Q - vI are multiples of the same vector (a, b),
   * so a proper eigenvector is any multiple of (-b, a).
   *
   * We use (-q2, q1) after Q => Q - vI.
   */

  q1 -= *v;
  q4 -= *v;

  *e1 = -q2;
  *e2 = q1;

  if (*v < 0) {
    *v = -(*v);
    *e1 = -(*e1);
    *e2 = -(*e2);
  }

  return;
}

float quad(float a, float b, float c) {
  /* returns greatest root, given that b is negative */

  assert(b <= 0.0);

  return ((-b + sqrt(b*b - 4*a*c)) / (2*a));
}

float det2x2(float m1, float m2, float m3, float m4) {
  return m1 * m4 - m2 * m3;
}
