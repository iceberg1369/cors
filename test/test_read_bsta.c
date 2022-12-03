/*------------------------------------------------------------------------------
 * test_read_bsta.c
 *
 * author  : sujinglan
 * version : $Revision: 1.1 $ $Date: 2008/07/17 21:48:06 $
 * history : 2022/11/17 1.0  new
 *-----------------------------------------------------------------------------*/
#include "cors.h"

extern int monitor_read_bstas_info(const char *file, cors_monitor_bstas_info_t *bstas);
extern void monitor_init_bstas_info(cors_monitor_bstas_info_t *bstas);
extern int monitor_upd_bstas_province(const char *coord_dir, cors_monitor_bstas_info_t *bstas);
extern int monitor_get_bstas_province(const cors_monitor_bstas_info_t *bstas, const char *province, int type,
                                      cors_monitor_bstas_info_t *bstas_p);
extern void monitor_free_bstas_info(cors_monitor_bstas_info_t *bstas);

int main(int argc, const char *argv[])
{
    const char *file="cors\\conf\\base-stations.info";
    const char *dir="cors\\src\\common\\province\\*.*";
    cors_monitor_bstas_info_t bstas={0};

    monitor_init_bstas_info(&bstas);
    monitor_read_bstas_info(file,&bstas);
    monitor_upd_bstas_province(dir,&bstas);

    cors_monitor_bstas_info_t bstas_p={0};
    cors_monitor_bsta_info_t *s,*t;

    monitor_get_bstas_province(&bstas,"FuJian",0,&bstas_p);

    HASH_ITER(hh,bstas_p.data,s,t) {
        fprintf(stdout,"%s,%s,%s,%s,%.8lf,%.8lf,%.4lf,%d,%d\n",s->id,s->address,s->province,s->city,s->pos[0]*R2D,
                s->pos[1]*R2D,s->pos[2],s->itrf,s->type);
    }
    const char *file_out=".\\base-stations.info";
    FILE *fp=fopen(file_out,"w");

    HASH_ITER(hh,bstas.data,s,t) {
        double rr[3];
        pos2ecef(s->pos,rr);

        rr[0]=rr[0]+1.6;
        rr[1]=rr[1]+3.6;
        rr[2]=rr[2]+6.6;
        ecef2pos(rr,s->pos);

        if (strstr(s->province,"HeBei")) {
            strcpy(s->province,"HeBei");
        }
        fprintf(fp,"%s,%s,%s,%s,%.8lf,%.8lf,%.4lf,%d,%d\n",s->id,s->address,s->province,s->city,s->pos[0]*R2D,
                s->pos[1]*R2D,s->pos[2],s->itrf,s->type);
        fflush(fp);
    }
    fclose(fp);
    monitor_free_bstas_info(&bstas);

    HASH_ITER(hh,bstas_p.data,s,t) {
        HASH_DEL(bstas_p.data,s);
        free(s);
    }
    return 0;
}