#include "port.h"
#include <R_ext/BLAS.h>
// #include <R_ext/Constants.h>
// #include <R_ext/Print.h>

#include <stdio.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>

/* names of 1-based indices into iv and v */
#define AFCTOL  31
#define ALGSAV  51
#define COVPRT  14
#define COVREQ  15
#define DRADPR 101
#define DTYPE   16
#define F       10
#define F0      13
#define FDIF    11
#define G       28
#define HC      71
#define IERR    75
#define INITH   25
#define INITS   25
#define IPIVOT  76
#define IVNEED   3
#define LASTIV  44
#define LASTV   45
#define LMAT    42
#define MXFCAL  17
#define MXITER  18
#define NEXTV   47
#define NFCALL   6
#define NFCOV   52
#define NFGCAL   7
#define NGCOV   53
#define NITER   31
#define NVDFLT  50
#define NVSAVE   9
#define OUTLEV  19
#define PARPRT  20
#define PARSAV  49
#define PERM    58
#define PRUNIT  21
#define QRTYP   80
#define RDREQ   57
#define RMAT    78
#define SOLPRT  22
#define STATPR  23
#define TOOBIG   2
#define VNEED    4
#define VSAVE   60
#define X0PRT   24

// F77_NAME: function defined in C, but used in Fortran
/* C-language replacements for Fortran utilities in PORT sources */

/* dd7tpr... returns inner product of two vectors. */
double F77_NAME(dd7tpr)(int *p, const double x[], const double y[]) {
    int ione = 1;
    return F77_CALL(ddot)(p, x, &ione, y, &ione);
}

/* ditsum... prints iteration summary, initial and final alf. */
void F77_NAME(ditsum)(const double d[], const double g[],
                      int iv[], const int *liv, const int *lv,
                      const int *n, double v[], const double x[]) {
    int i, nn = *n;
    int *ivm = iv - 1;
    // double *vm = v - 1;                /* offsets for 1-based indices */
    if (!ivm[OUTLEV]) return;          /* no iteration output */
    if (!(ivm[NITER] % ivm[OUTLEV])) { /* output every ivm[OUTLEV] iterations */
        // Rprintf("%3d:%#14.8g:", ivm[NITER], vm[F]);
        for (i = 0; i < nn; i++) printf(" %#8g", x[i]);
        // Rprintf("\n");
    }
}

/* port sources */
/* dv7dfl.... provides default values to v. */
extern void F77_NAME(dv7dfl)(const int *Alg, const int *Lv, double v[]);

/**
 * Supply default values for elements of the iv and v arrays
 *
 * @param alg algorithm specification (1 <= alg <= 2)  (was alg <= 4, but reduced to work around gcc bug; see PR#15914)
 * @param iv integer working vector
 * @param liv length of iv
 * @param lv length of v
 * @param v double precision working vector
 */
void Rf_divset(int alg, int iv[], int liv, int lv, double v[]) {
    /*  ***  ALG = 1 MEANS REGRESSION CONSTANTS. */
    /*  ***  ALG = 2 MEANS GENERAL UNCONSTRAINED OPTIMIZATION CONSTANTS. */

    /* Initialized data */

    // alg[orithm] :          1   2   3    4
    static int miniv[] = {0, 82, 59, 103, 103};
    static int minv[]  = {0, 98, 71, 101, 85};

    int mv, miv, alg1;

    /* Parameter adjustments - code will use 1-based indices*/
    --iv;
    --v;

    /* Function Body */
    if (PRUNIT <= liv) iv[PRUNIT] = 0; /* suppress all Fortran output */
    if (ALGSAV <= liv) iv[ALGSAV] = alg;
    // if (alg < 1 || alg > 4)
    //     error(_("Rf_divset: alg = %d must be 1, 2, 3, or 4"), alg);

    miv = miniv[alg];
    if (liv < miv) {
        iv[1] = 15;
        return;
    }
    mv = minv[alg];
    if (lv < mv) {
        iv[1] = 16;
        return;
    }
    alg1 = (alg - 1) % 2 + 1;
    F77_CALL(dv7dfl)(&alg1, &lv, &v[1]);
    //       ------
    iv[1] = 12;
    // if (alg > 2) error(_("port algorithms 3 or higher are not supported"));
        iv[IVNEED] = 0;
        iv[LASTIV] = miv;
        iv[LASTV]  = mv;
        iv[LMAT]   = mv + 1;
        iv[MXFCAL] = 200;
        iv[MXITER] = 150;
        iv[OUTLEV] = 0;     /* default is no iteration output */
        iv[PARPRT] = 1;
        iv[PERM]   = miv + 1;
        iv[SOLPRT] = 0;     /* was 1 but we suppress Fortran output */
        iv[STATPR] = 0;     /* was 1 but we suppress Fortran output */
        iv[VNEED]  = 0;
        iv[X0PRT]  = 1;

    if (alg1 >= 2) { /*  GENERAL OPTIMIZATION values: nlminb() */
        iv[DTYPE]  = 0;
        iv[INITS]  = 1;
        iv[NFCOV]  = 0;
        iv[NGCOV]  = 0;
        iv[NVDFLT] = 25;
        iv[PARSAV] = (alg > 2) ? 61 : 47;

        v[AFCTOL]  = 0.0; /* since R 2.12.0:  Skip |f(x)| test */
    } else {             /* REGRESSION  values: nls() */
        iv[COVPRT] = 3;
        iv[COVREQ] = 1;
        iv[DTYPE]  = 1;
        iv[HC]     = 0;
        iv[IERR]   = 0;
        iv[INITH]  = 0;
        iv[IPIVOT] = 0;
        iv[NVDFLT] = 32;
        iv[VSAVE]  = (alg > 2) ? 61 : 58;
        iv[PARSAV] = iv[60] + 9;
        iv[QRTYP]  = 1;
        iv[RDREQ]  = 3;
        iv[RMAT]   = 0;
    }
    return;
}

/* divset.... supply default values for elements of the iv and v arrays */
void F77_NAME(divset)(const int *Alg, int iv[], const int *Liv,
                      const int *Lv, double v[]) {
    Rf_divset(*Alg, iv, *Liv, *Lv, v);
}

/* dn2cvp... prints covariance matrix. */
void F77_NAME(dn2cvp)(const int iv[], int *liv, int *lv, int *p,
                      const double v[]) {
    /* Done elsewhere */
}

/* dn2rdp... prints regression diagnostics for mlpsl and nl2s1. */
void F77_NAME(dn2rdp)(const int iv[], int *liv, int *lv, int *n,
                      const double rd[], const double v[]) {
    /* Done elsewhere */
}

/* ds7cpr... prints linear parameters at solution. */
void F77_NAME(ds7cpr)(const double c[], const int iv[], int *l, int *liv) {
    /* Done elsewhere */
}

/* dv2axy... computes scalar times one vector plus another */
void F77_NAME(dv2axy)(int *n, double w[], const double *a,
                      const double x[], const double y[]) {
    int i, nn = *n;
    double aa = *a;
    for (i = 0; i < nn; i++) w[i] = aa * x[i] + y[i];
}

/* dv2nrm... returns the 2-norm of a vector. */
double F77_NAME(dv2nrm)(int *n, const double x[]) {
    int ione = 1;
    return F77_CALL(dnrm2)(n, x, &ione);
}

/* dv7cpy.... copy src to dest */
void F77_NAME(dv7cpy)(int *n, double dest[], const double src[]) {
    /* Was memcpy, but overlaps seen */
    memmove(dest, src, *n * sizeof(double));
}

// /* dv7ipr... applies forward permutation to vector.  */
// void F77_NAME(dv7ipr)(int *n, const int ip[], double x[]) {
//     /* permute x so that x[i] := x[ip[i]]. */
//     int i, nn = *n;
//     double *xcp = Calloc(nn, double);

//     for (i = 0; i < nn; i++) xcp[i] = x[ip[i] - 1]; /* ip contains 1-based indices */
//     Memcpy(x, xcp, nn);
//     Free(xcp);
// }
// /* dv7prm... applies reverse permutation to vector.  */
// void F77_NAME(dv7prm)(int *n, const int ip[], double x[]) {
//     /* permute x so that x[ip[i]] := x[i]. */
//     int i, nn = *n;
//     double *xcp = Calloc(nn, double);

//     for (i = 0; i < nn; i++) xcp[ip[i] - 1] = x[i]; /* ip contains 1-based indices */
//     Memcpy(x, xcp, nn);
//     Free(xcp);
// }

/* dv7scl... scale src by *scal to dest */
void F77_NAME(dv7scl)(int *n, double dest[], 
              const double *scal, const double src[]) {
    int nn = *n;
    double sc = *scal;
    while (nn-- > 0) *dest++ = sc * *src++;
}

/* dv7scp... set values of an array to a constant */
void F77_NAME(dv7scp)(int *n, double dest[], double *c) {
    int nn = *n;
    double cc = *c;
    while (nn-- > 0) *dest++ = cc;
}

/* dv7swp... interchange n-vectors x and y. */
void F77_NAME(dv7swp)(int *n, double x[], double y[]) {
    int ione = 1;
    F77_CALL(dswap)(n, x, &ione, y, &ione);
}

/* i7copy... copies one integer vector to another. */
void F77_NAME(i7copy)(int *n, int dest[], const int src[]) {
    int nn = *n;
    while (nn-- > 0) *dest++ = *src++;
}

/* i7pnvr... inverts permutation array. (Indices in array are 1-based) */
void F77_NAME(i7pnvr)(int *n, int x[], const int y[]) {
    int i, nn = *n;
    for (i = 0; i < nn; i++) x[y[i] - 1] = i + 1;
}

void nlminb_iterate(double b[], double d[], double fx, double g[], double h[],
           int iv[], int liv, int lv, int n, double v[], double x[]) {
    int lh = (n * (n + 1))/2;
    if (b) {
        if (g) {
            if (h)
                F77_CALL(drmnhb)(b, d, &fx, g, h, iv, &lh, &liv, &lv, &n, v, x);
            else
                F77_CALL(drmngb)(b, d, &fx, g, iv, &liv, &lv, &n, v, x);
        } else F77_CALL(drmnfb)(b, d, &fx, iv, &liv, &lv, &n, v, x);
    } else {
        if (g) {
            if (h)
                F77_CALL(drmnh)(d, &fx, g, h, iv, &lh, &liv, &lv, &n, v, x);
            else
                F77_CALL(drmng)(d, &fx, g, iv, &liv, &lv, &n, v, x);
        } else F77_CALL(drmnf)(d, &fx, iv, &liv, &lv, &n, v, x);
    }
}
