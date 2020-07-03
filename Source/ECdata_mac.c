/*******************************************************
**        process EC/PEC data
** this programe is designed to output the data for plot
** for electrodes with unfixed areas
*******************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <guancai.c>

#define N 1000  //maximum number of processed files*2
#define M 100000  //maximum lines of processed data in each file
char store[M][100];

int main()
{
	FILE *data,*out,*inter;
	int i=0,j,k,areapl,refi,spliti,filenum,rows,txtnum,md;
	double **temp; //2D dynamic array
    double ph,refed,area;
	char *path = NULL,fname[N][256],del[5]=".txt",houzui[10];
    char *list[10],*split,first[100],*method;

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
    //-------------------------------------------------------------
	temp = (double **)malloc(M*sizeof(double *));
    for(i=0;i<M;i++)
    {
        temp[i] = (double *)malloc(N*sizeof(double *));
    }
    txtnum=trave_dir(path,filenum,del,fname);

	for(i=0;i<txtnum;i++)
	{
		data = fopen(fname[i],"rt");
		inter = fopen("temp","w+");

        j=0;
        //-----determine the characterization method of each file------
        while((fgets(store[j],100,data)) != NULL)
		{
            if(strstr(store[j],"Cyclic voltammogram")){
                method="cv";
                break;
            }
            else if(strstr(store[j],"Impedance samples")){
                method="nyquist";
                break;
            }
            else if(strstr(store[j],"polarization data")){
                method="ocp";
                break;
            }
            else if(strstr(store[j],"S-FILE")){
                method="jv";
                break;
            }
            else if(strstr(store[j],"Impedance - Potential")){
                method="ms";
                break;
            }
            j++;
        }        
        if(!method){  //skip the file if not above methods detected
            method="None";
            printf("Cannot detect the characterization method in %s!\t Skip!\n",fname[i]);
            goto next;
        }
        else
            printf("%s file is a/an %s characterization.\n",fname[i], method);
            
        //----------Input area of the electrode-----------------
        if(areapl==1){
            printf ("Input the electrode area(cm2) of %s: ",fname[i]);
            scanf("%lf",&area);
        }
        delexten(fname[i],del);
        
        //------------transfer method to inter for switch-case function-----------
        if(strcmp(method,"cv")==0)
            md=1,strcpy(houzui,".cv");
        else if(strcmp(method,"jv")==0)
        { //for various jv characterizations, determin the specific method by file name
            if(strstr(fname[i],"chop"))
                md=21,strcpy(houzui,".chop");
            else if(strstr(fname[i],"stav"))
                md=22,strcpy(houzui,".stav");
            else if(strstr(fname[i],"staj"))
                md=23,strcpy(houzui,".staj");
            else
                md=2,strcpy(houzui,".jv");
        }
        else if(strcmp(method,"nyquist")==0)
            md=3,strcpy(houzui,".eis");
        else if(strcmp(method,"ocp")==0)
            md=4,strcpy(houzui,".ocp");
        else if(strcmp(method,"ms")==0)
            md=5,strcpy(houzui,".ms");
        else
            md=6;
        //--------------determine the valid data in each file--------------  
        j=0;      
        while((fgets(store[j],100,data)) != NULL)
        {       
            datalist(store[j],split,list);
            strcpy(first,list[0]);
            if(digjudge(first)) {
                if(md==1 || md==3)
                    fprintf(inter,"%s\t%s\n",list[2],list[3]);
                else if(md==4)
                    fprintf(inter,"%s\t%s\n",list[1],list[2]);
                else
                    fprintf(inter,"%s\t%s\n",list[1],list[3]);
            }
			j++;
        }
       
        fseek(inter,0L,SEEK_SET);
    	out = fopen(addexten(fname[i],houzui),"wt");
        k=0,j=0;
        while(1)
        { 
            fscanf(inter,"%lf%lf",&temp[k][2*i],&temp[k][2*i+1]);
            if(feof(inter)) break;
            
            switch(md){
            case 1: //cv
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
                break;
            case 2: //jv
                if(refi!=0)
                    temp[k][2*i] += ph*0.059+refed;
                temp[k][2*i+1] = temp[k][2*i+1]*1000/area;
                fprintf(out,"%e\t%e\n",temp[k][2*i],temp[k][2*i+1]);
    			k++;
                break;
            case 21: //chop
                temp[k][2*i+1] = temp[k][2*i+1]*1000/area;
                fprintf(out,"%f\t%e\n",temp[k][2*i],temp[k][2*i+1]);
        		k++;
                break;
            case 22: //stability-constant V
                temp[k][2*i+1] = temp[k][2*i+1]*1000/area;
                fprintf(out,"%f\t%e\n",temp[k][2*i],temp[k][2*i+1]);
        		k++;
                break;
            /*case 23: //stability-constant J
                if(refi!=0)
                    temp[k][2*i] += ph*0.059+refed;
                temp[k][2*i+1] = temp[k][2*i+1]*1000/area;
                fprintf(out,"%e\t%e\n",temp[k][2*i],temp[k][2*i+1]);
    			k++;
                break;
            */
            case 3: //nyquist
                temp[k][2*i+1] = -temp[k][2*i+1]*area;
                temp[k][2*i] = temp[k][2*i]*area;
             	if(feof(inter)) break;
                fprintf(out,"%e\t%e\n",temp[k][2*i],temp[k][2*i+1]);
                k++;
                break;
            case 4: //ocp
                temp[k][2*i+1] += ph*0.059 + refed;
                fprintf(out,"%e\t%e\n",temp[k][2*i],temp[k][2*i+1]);
                k++;
                break;
            case 5: 
                break;
            case 6:
                break;
            }
        }  
       // CV related output 
        switch(md){
            case 1:
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
        }
        fclose(out);
		printf("Finished!\n");
next:
		fclose(data);
		fclose(inter);
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

