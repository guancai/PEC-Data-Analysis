/*******************************************************
**        process EIS-nyquist data
** this code is designed to output Z and Z' in the EC/PEC 
** characterization of an electrode.
** Handle all txt files in the same folder of this code.
*******************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <guancai.c>

#define N 1000  //maximum number of processed files*2
#define M 10000  //maximum lines of processed data in each file
char store[M][100];

int main()
{
	FILE *data,*out,*inter;
	int i=0,j,k,spliti,filenum,txtnum;
	double **temp; //2D dynamic array
    double area;
	char *path = NULL,fname[N][256],del[5]=".txt",houzui[10];
    char *list[10],*split,first[100];

	path = "./";
    //Set the separater between the data in the data file
    printf ("Choose the data separator: 0-space, 1-comma, 2-tab, 3-;\t");
    scanf("%d",&spliti);
    switch (spliti) {
    	case 0: split = " ";break;
        case 1: split = ",";break;
        case 2: split = "\t";break;
        case 3: split = ";";
    }
	temp = (double **)malloc(M*sizeof(double *));
    for(i=0;i<M;i++)
    {
        temp[i] = (double *)malloc(N*sizeof(double *));
    }
    txtnum=trave_dir(path,filenum,del,fname);
    
 	for(i=0;i<txtnum;i++){
		data = fopen(fname[i],"rt");
		inter = fopen("temp","w+");

        //----------Input area of the electrod-----------------
        printf ("Input the electrode area(cm2) of %s: ",fname[i]);
        scanf("%lf",&area);
        //------------------------------------------------------
        delexten(fname[i],del);
        strcpy(houzui,".eis");
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
			temp[k][2*i+1] = -temp[k][2*i+1]*area;
            temp[k][2*i] = temp[k][2*i]*area;
		 	if(feof(inter)) break;
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

