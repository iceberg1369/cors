/*------------------------------------------------------------------------------
 * test_read_vbsta.c
 *
 * author  : sujinglan
 * version : $Revision: 1.1 $ $Date: 2008/07/17 21:48:06 $
 * history : 2022/11/17 1.0  new
 *-----------------------------------------------------------------------------*/
#include "cors.h"

int main(int argc, const char *argv[])
{
    const char *file="vrsstationist.txt";
    const char *file_out=".\\vrs-sta.out";
    FILE *fp=fopen(file,"r");
    FILE *fp_out=fopen(file_out,"w");
    char buff[1024],*p,*q,*val[64];
    double pos[3],rr[3];
    int n;

    while (!feof(fp)) {
        if (!fgets(buff,sizeof(buff),fp)) continue;
        if (!(p=strstr(buff,"|"))) continue;
        *p='\0';

        for (n=0,p=buff;*p&&n<64;p=q+1) {
            if ((q=strchr(p,','))||(q=strchr(p,'\0'))) {val[n++]=p; *q='\0';}
            else break;
        }
        rr[0]=atof(val[2]);
        rr[1]=atof(val[4]);
        rr[2]=atof(val[6]);
        ecef2pos(rr,pos);

        fprintf(fp_out,"%s,%s,%s,%s,%.8lf,%.8lf,%.4lf,%d,%d\n",val[0],"","","",pos[0]*R2D,pos[1]*R2D,pos[2],2014,1);
        fflush(fp_out);
    }
    fclose(fp);
    fclose(fp_out);
    return 0;
}