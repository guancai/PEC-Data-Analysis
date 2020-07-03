/**************************************************************
**                  process CV data
** This program is designed to output the voltage (V, vs. RHE)
** and its corresponding current (mA)in the EC/PEC 
** characterization of an electrode.
** Handle all txt files in the same folder of this code.
***************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <guancai.c>

#define N 1000  //maximum number of processed files*2
#define M 100000  //maximum lines of processed data in each file
char store[M][100];

int main()
{
	FILE *data,*out,*inter;
	int i,j,k,areapl,refi,spliti,filenum,rows,txtnum;
	double **temp; //2D dynamic array
    double ph,refed,area;
    char *path = NULL, fname[N][256],del[5]=".txt",houzui[10];
    char *list[10],*split,first[100];

    path = "./";
    //initialize the pH and used reference electrode
    //-----------------------------------------------
    printf ("Input the electrolyte pH: ");
    scanf("%lf",&ph);
    printf ("Choose the refrence electrode: 0-None, 1-Ag/AgCl, 2-SCE, 3-Hg/HgO\t");
    scanf("%d",&refi);
    switch (refi) {
    	case 0: refed = 0;break;
        case 1: refed = 0.197;break;
        case 2: refed = 0.244;break;
        case 3: refed = 0.098;
    }
    printf ("Do the electrodes have the same area? 0-Yes, 1-No\t");
    scanf("%d",&areapl);
    if(areapl==0){
        printf ("Input the electrode area(cm2): ");
        scanf("%lf",&area);
    }
    //-----------------------------------------------
    //Set the separater between the data in the data file
    printf ("Choose the data separator: 0-space, 1-comma, 2-tab, 3-;\t");
    scanf("%d",&spliti);
    switch (spliti) {
    	case 0: split = " ";break;
        case 1: split = ",";break;
        case 2: split = "\t";break;
        case 3: split = ";";
    }
    //-----------------------------------------------
    temp = (double **)malloc(M*sizeof(double *));
        for(j=0;j<M;j++)
        {
            temp[j] = (double *)malloc(N*sizeof(double *));
        }
    txtnum=trave_dir(path,filenum,del,fname);
    
    for(i=0;i<txtnum;i++){
		data = fopen(fname[i],"rt");
		inter = fopen("temp","w+");
		delexten(fname[i],del);
        strcpy(houzui,".cv");

        if(areapl==1){
            printf ("Input the electrode area(cm2) of %s: ",fname[i]);
            scanf("%lf",&area);
        }
        
        //------------------------------------------------------
		out = fopen(addexten(fname[i],houzui),"wt");

        j=0;
		while((fgets(store[j],100,data)) != NULL)
		{
			datalist(store[j],split,list);
            strcpy(first,list[0]);
            if(digjudge(first)) fprintf(inter,"%s\t%s",list[2],list[3]);
			j++;
        }

       fseek(inter,0L,SEEK_SET);
       k=0,j=0;
        while(1)
		{
			fscanf(inter,"%lf%lf",&temp[k][2*j],&temp[k][2*j+1]);
		 	if(feof(inter)) break;
            if(refi!=0) temp[k][2*j] += ph*0.059+refed;
            temp[k][2*j+1] = temp[k][2*j+1]*1000/area;
            if(k>0){ //删除测试开始时的重复数据点
                if(temp[k][2*j] == temp[k-1][2*j])
                    k -= 1;
            }
            if(k > 1){  //根据电压极小值判断CV圈数
                if(temp[k-1][2*j] < temp[k][2*j] && temp[k-1][2*j] < temp[k-2][2*j])
                    j++,k=-1;
            }
			k++;
		}
        filenum = k; rows = 2*(j+1);
        for(k=0;k<abs(rows/2);k++){
                fprintf(out,"\t%d",k+1);
                if(k<abs(rows/2)-1) fprintf(out,"\t");
        }
        fprintf(out,"\n\n\n");
        for(j=0;j<filenum;j++){
            for(k=0;k<rows;k++){
                fprintf(out,"%e",temp[j][k]);
                if(k<rows-1) fprintf(out,"\t");
            }
            if(j<filenum-1) fprintf(out,"\n");
        }

		printf("Finished!\n");

		fclose(data);
		fclose(inter);
		fclose(out);
	}

    unlink("temp");
    //free memory
    for(j=0;j<M;j++)
    {
        free(temp[j]);
    }
    free(temp);

    return 0;
}

