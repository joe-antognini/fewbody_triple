/* -*- linux-c -*- */
/* fewbody.h

   Copyright (C) 2002-2004 John M. Fregeau
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef _FEWBODY_H
#define _FEWBODY_H 1

#include <stdio.h>
#include <gsl/gsl_nan.h>
#include <gsl/gsl_rng.h>

/* version information */
#define FB_VERSION "0.22-pn"
#define FB_NICK "for Joe"
#define FB_DATE "Tue Jul 25 14:49:21 CDT 2006"

/* dimensionless constants */
#define FB_CONST_PI 3.141592653589793238462643

/* constants, in cgs units */
#define FB_CONST_MSUN 1.989e+33
#define FB_CONST_RSUN 6.9599e+10
#define FB_CONST_C 2.99792458e+10
#define FB_CONST_G 6.67384e-8 // JMA 6-8-12 -- Changed to 2010 CODATA rec.
#define FB_CONST_AU 1.496e+13
#define FB_CONST_PARSEC 3.0857e+18
#define FB_CONST_YR 3.155693e+7

/* these usually shouldn't need to be changed */
#define FB_H 1.0e-2
#define FB_SSTOP GSL_POSINF
#define FB_AMIN GSL_POSINF
#define FB_RMIN GSL_POSINF
#define FB_ROOTSOLVER_MAX_ITER 100
#define FB_ROOTSOLVER_ABS_ACC 1.0e-11
#define FB_ROOTSOLVER_REL_ACC 1.0e-11
#define FB_MAX_STRING_LENGTH 2048
#define FB_MAX_LOGENTRY_LENGTH (32 * FB_MAX_STRING_LENGTH)

/* a struct containing the units used */
typedef struct{
  double v; /* velocity */
  double l; /* length */
  double t; /* time */
  double m; /* mass */
  double E; /* energy */
} fb_units_t;

/* the fundamental object */
typedef struct fb_obj{
  int ncoll; /* total number of stars collided together in this star */
  long *id; /* numeric id array */
  char idstring[FB_MAX_STRING_LENGTH]; /* string id */
  double m; /* mass */
  double R; /* radius */
  double Eint; /* internal energy (used to check energy conservation) */
  double Lint[3]; /* internal ang mom (used to check ang mom conservation) */
  double x[3]; /* position */
  double v[3]; /* velocity */
  int n; /* total number of stars in hierarchy */
  struct fb_obj *obj[2]; /* pointers to children */
  double a; /* semimajor axis */
  double e; /* eccentricity */
  double Lhat[3]; /* angular momentum vector */
  double Ahat[3]; /* Runge-Lenz vector */
  double t; /* time at which node was upsynced */
  double mean_anom; /* mean anomaly when node was upsynced */
} fb_obj_t;

/* parameters for the K-S integrator */
typedef struct{
  int nstar; /* number of actual stars */
  int kstar; /* nstar*(nstar-1)/2, number of separations */
  double *m; /* m[nstar] */
  double *M; /* M[kstar] */
  double **amat; /* amat[nstar][kstar] */
  double **Tmat; /* Tmat[kstar][kstar] */
  double Einit; /* initial energy used in integration scheme */
} fb_ks_params_t;

/* parameters for the non-regularized integrator */
typedef struct{
  int nstar; /* number of actual stars */
  double *m; /* m[nstar] */
  int PN1;
  int PN2;
  int PN25;
  int PN3;
  int PN35;
  fb_units_t units;
} fb_nonks_params_t;

/* JMA 8-16-2012 -- Knowledge about the PN terms we are interested in is
 * necessary for the downsync function to properly calculate the energy
 * (and hence the semi-major axis). 
 */
typedef struct{
  int PN1;
  int PN2;
  int PN25;
  int PN3;
  int PN35;
  fb_units_t units;
} fb_PN_params_t;

/* the hierarchy data structure */
typedef struct{
  int nstarinit; /* initial number of stars (may not equal nstar if there are collisions) */
  int nstar; /* number of stars */
  int nobj; /* number of binary trees */
  int *hi; /* hierarchical index array */
  int *narr; /* narr[i] = number of hierarchical objects with i elements */
  fb_obj_t *hier; /* memory location of hierarchy information */
  fb_obj_t **obj; /* array of pointers to top nodes of binary trees */
} fb_hier_t;

/* input parameters */
typedef struct{
  int ks; /* 0=no regularization, 1=K-S regularization */
  double tstop; /* stopping time, in units of t_dyn */
  int Dflag; /* 0=don't print to stdout, 1=print to stdout */
  double dt; /* time interval between printouts will always be greater than this value */
  double tcpustop; /* cpu stopping time, in units of seconds */
  double absacc; /* absolute accuracy of the integrator */
  double relacc; /* relative accuracy of the integrator */
  int ncount; /* number of integration steps between each call to fb_classify() */
  int outfreq; /* number of integration steps between each call to fb_classify() */
  double tidaltol; /* tidal tolerance */
  double speedtol; /* v/c tolerance */
  char firstlogentry[FB_MAX_LOGENTRY_LENGTH]; /* first entry to put in printout log */
  double fexp; /* expansion factor for a merger product: R = f_exp (R_1+R_2) */
  int PN1;
  int PN2;
  int PN25;
  int PN3;
  int PN35;
} fb_input_t;

/* return parameters */
typedef struct{
  long count; /* number of integration steps */
  int retval; /* return value */
  long iclassify; /* number of times classify was called */
  double tcpu; /* cpu time taken */
  double DeltaE; /* change in energy */
  double DeltaEfrac; /* change in energy, as a fraction of initial energy */
  double DeltaL; /* change in ang. mom. */
  double DeltaLfrac; /* change in ang. mom., as a fraction of initial ang. mom. */
  double Rmin; /* minimum distance of close approach during interaction */
  int Rmin_i; /* index of star i participating in minimum close approach */
  int Rmin_j; /* index of star j participating in minimum close approach */
  int Nosc; /* number of oscillations of the quantity s^2 (McMillan & Hut 1996) (Nosc=Nmin-1, so resonance if Nosc>=1) */
} fb_ret_t;

/* fewbody.c */
fb_ret_t fewbody(fb_input_t input, fb_units_t units, fb_hier_t *hier, double *t, gsl_rng *rng);

/* fewbody_classify.c */
int fb_classify(fb_hier_t *hier, double t, double tidaltol, double speedtol, fb_units_t units, fb_input_t params);
int fb_is_stable(fb_obj_t *obj, double speedtol, fb_units_t units);
int fb_is_stable_binary(fb_obj_t *obj, double speedtol, fb_units_t units);
int fb_is_stable_triple(fb_obj_t *obj);
int fb_is_stable_quad(fb_obj_t *obj);
int fb_mardling(fb_obj_t *obj, int ib, int is);

/* fewbody_coll.c */
int fb_is_collision(double r, double R1, double R2);
int fb_collide(fb_hier_t *hier, double f_exp, fb_units_t units, gsl_rng *rng, double *t);
void fb_merge(fb_obj_t *obj1, fb_obj_t *obj2, int nstarinit, double f_exp, fb_units_t units, gsl_rng *rng);
double fb_vkick(double m1, double m2);

/* fewbody_hier.c */
void fb_malloc_hier(fb_hier_t *hier);
void fb_init_hier(fb_hier_t *hier);
void fb_free_hier(fb_hier_t hier);
void fb_trickle(fb_hier_t *hier, double t);
void fb_elkcirt(fb_hier_t *hier, double t, fb_input_t params, fb_units_t units);
int fb_create_indices(int *hi, int nstar);
int fb_n_hier(fb_obj_t *obj);
char *fb_sprint_hier(fb_hier_t hier, char string[FB_MAX_STRING_LENGTH]);
char *fb_sprint_hier_hr(fb_hier_t hier, char string[FB_MAX_STRING_LENGTH]);
void fb_upsync(fb_obj_t *obj, double t, fb_input_t params, fb_units_t units);
double fb_incpartition(fb_obj_t *obj[1], double inc);
void fb_binaryorient(fb_obj_t *obj, gsl_rng *rng, double cosi, double peri, double ascnode);
void fb_randorient(fb_obj_t *obj, gsl_rng *rng);
void fb_downsync(fb_obj_t *obj, double t);
void fb_objcpy(fb_obj_t *obj1, fb_obj_t *obj2);

/* fewbody_int.c */
void fb_malloc_ks_params(fb_ks_params_t *ks_params);
void fb_init_ks_params(fb_ks_params_t *ks_params, fb_hier_t hier);
void fb_free_ks_params(fb_ks_params_t ks_params);
void fb_malloc_nonks_params(fb_nonks_params_t *nonks_params);
void fb_init_nonks_params(fb_nonks_params_t *nonks_params, fb_hier_t hier);
void fb_free_nonks_params(fb_nonks_params_t nonks_params);

/* fewbody_io.c */
void fb_print_version(FILE *stream);
void fb_print_story(fb_obj_t *star, int nstar, double t, char *logentry);

/* fewbody_isolate.c */
int fb_collapse(fb_hier_t *hier, double t, double tidaltol, double speedtol, fb_units_t units, fb_input_t input);
int fb_expand(fb_hier_t *hier, double t, double tidaltol);

/* fewbody_ks.c */
inline double fb_ks_dot(double x[4], double y[4]);
inline double fb_ks_mod(double x[4]);
void fb_calc_Q(double q[4], double Q[4]);
void fb_calc_ksmat(double Q[4], double Qmat[4][4]);
void fb_calc_amat(double **a, int nstar, int kstar);
void fb_calc_Tmat(double **a, double *m, double **T, int nstar, int kstar);
int fb_ks_func(double s, const double *y, double *f, void *params);
double fb_ks_Einit(const double *y, fb_ks_params_t params);
void fb_euclidean_to_ks(fb_obj_t **star, double *y, int nstar, int kstar);
void fb_ks_to_euclidean(double *y, fb_obj_t **star, int nstar, int kstar);

/* fewbody_nonks.c */
int fb_nonks_func(double t, const double *y, double *f, void *params);
int fb_nonks_jac(double t, const double *y, double *dfdy, double *dfdt, void *params);
void fb_euclidean_to_nonks(fb_obj_t **star, double *y, int nstar);
void fb_nonks_to_euclidean(double *y, fb_obj_t **star, int nstar);

/* fewbody_scat.c */
void fb_init_scattering(fb_obj_t *obj[2], double vinf, double b, double rtid);
void fb_normalize(fb_hier_t *hier, fb_units_t units);

/* fewbody_utils.c */
inline double *fb_malloc_vector(int n);
inline double **fb_malloc_matrix(int nr, int nc);
inline void fb_free_vector(double *v);
inline void fb_free_matrix(double **m);
inline double fb_sqr(double x);
inline double fb_cub(double x);
inline double fb_qrt(double x);
inline double fb_dot(double x[3], double y[3]);
inline double fb_mod(double x[3]);
int fb_cross(double x[3], double y[3], double z[3]);
int fb_angmom(fb_obj_t *star, int nstar, double L[3]);
void fb_angmomint(fb_obj_t *star, int nstar, double L[3]);
double fb_einttot(fb_obj_t *star, int nstar);
double fb_petot(fb_obj_t *star, int nstar);
double fb_ketot(fb_obj_t *star, int nstar);
double fb_outerpetot(fb_obj_t **obj, int nobj);
double fb_outerketot(fb_obj_t **obj, int nobj);
double fb_kepler(double e, double mean_anom);
double fb_keplerfunc(double mean_anom, void *params);
double fb_reltide(fb_obj_t *bin, fb_obj_t *single, double r);

/* macros */
/* The variadic macro syntax here conforms to the C99 standard, but for some
   reason won't compile on Mac OSX with gcc. */
/* #define fb_dprintf(...) if (fb_debug) fprintf(stderr, __VA_ARGS__) */
/* The variadic macro syntax here is the old gcc standard, and compiles on
   Mac OSX with gcc. */
#define fb_dprintf(args...) if (fb_debug) fprintf(stderr, args)
#define FB_MIN(a, b) ((a)<=(b)?(a):(b))
#define FB_MAX(a, b) ((a)>=(b)?(a):(b))
#define FB_DELTA(i, j) ((i)==(j)?1:0)
#define FB_KS_K(i, j, nstar) ((i)*(nstar)-((i)+1)*((i)+2)/2+(j))

/* there is just one global variable */
extern int fb_debug;

/* radiation rocket canonical kick speed */
#define FB_VKICK 120.0e5

#define FB_REFF_BH2 10

#endif /* fewbody.h */
