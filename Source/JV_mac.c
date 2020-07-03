/***********************************************************
**         process current-voltage (J-V) data
** This code is used to output the voltage (V vs. RHE) 
** and its corresponding current (mA) in the EC/PEC 
** characterization of an electrode.
** Handle all txt files in the same folder of this code.
***********************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <guancai.c>

#define N 1000  //maximum number of processed files*2
#define M 10000  //maximum lines of processed data in each file
char store[M][100];

int main()
{
	FILE *data,*out,*inter;
	int i,j,k,areapl,refi,spliti,filenum,txtnum;
	double **temp; //2D dynamic array
    double ph,refed,area;
	char *path = NULL,fname[N][256],del[5]=".txt",houzui[10];
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
    for(i=0;i<M;i++)
    {
        temp[i] = (double *)malloc(N*sizeof(double *));
    }
    txtnum=trave_dir(path,filenum,del,fname);
    
	for(i=0;i<txtnum;i++){
		data = fopen(fname[i],"rt");
		inter = fopen("temp","w+");

        //----------Input area of the electrode-----------------
        if(areapl==1){
            printf ("Input the electrode area(cm2) of %s: ",fname[i]);
            scanf("%lf",&area);
        }
        //------------------------------------------------------
        delexten(fname[i],del);
        strcpy(houzui,".jv");
		out = fopen(addexten(fname[i],houzui),"wt");

        j=0;
		while((fgets(store[j],100,data)) != NULL)
		{
			datalist(store[j],split,list);
            strcpy(first,list[0]);
            if(digjudge(first)) fprintf(inter,"%s\t%s\n",list[2],list[3]);
			j++;
        }

       fseek(inter,0L,SEEK_SET);
       k=0;
       while(1)
		{
			fscanf(inter,"%lf%lf",&temp[k][2*i],&temp[k][2*i+1]);
		 	if(feof(inter)) break;
            if(refi!=0)
                temp[k][2*i] += ph*0.059+refed;
            temp[k][2*i+1] = temp[k][2*i+1]*1000/area;
            fprintf(out,"%e\t%e\n",temp[k][2*i],temp[k][2*i+1]);
			k++;
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

