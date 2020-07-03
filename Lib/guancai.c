#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

/*******************************************************
**                 定义常用的物理常数
*******************************************************/
#define PC 6.625e-34    //Plank constant
#define T 300          //room temperature[K]
#define pi 3.1415926  
#define BC 1.38e-23     //Boltzman constant
#define m0 9.108e-31   //electron mass
#define ee 8.854e-12  //真空介电常数
#define e0 1.602e-19  //元电荷量
#define cc 2.998e8    //光速

/*******************************************************
**       该函数用于删除文件扩展名
**  原型: char *delexten (char str[],char del[])
**  str为待处理文件名，del待删扩展名
**  返回修改后的文件名
*******************************************************/
char *delexten (char str[],char del[])
{
    char *p=NULL,*t=NULL;
    while((p=strstr(str,del))!=NULL){
            t=p+strlen(del);
            *p='\0';
            strcat(str,t);
        }

    return str;
}

/*******************************************************
**       该函数用于添加文件扩展名
**  原型: char *addexten (char str[],char add[])
**  str为待处理文件名，add待加扩展名
**  返回修改后的文件名
*******************************************************/
char *addexten (char str[],char add[])
{
    strncat(str,add,10);
    return str;
}

/*******************************************************
**       该函数用于修改文件扩展名
**  原型: char *reexten (char str[],char del[],char add[])
**  str为待处理文件名，del待删扩展名，add待加扩展名
**  返回修改后的文件名
*******************************************************/

char *reexten (char str[],char del[],char add[])
{
    char *p=NULL,*t=NULL;
    while((p=strstr(str,del))!=NULL){
            t=p+strlen(del);
            *p='\0';
            strcat(str,t);
        }
		strncat(str,add,10);

    return str;
}

/**********************************************************************
**       该函数用于得到某目录下的txt文件      ==该函数有待改善==
**  原型: static void trave_dir_txt(char* path, int filenum, char str[][256])
**  path为待处理目录,filenum为txt文件个数，str记录txt文件名
**  返回文件个数
**********************************************************************/

int trave_dir_txt(char* path,int filenum,char str[filenum][256]) {
  DIR *d = NULL;
  struct dirent *dp = NULL;
  struct stat st;    
  char p[256] = {0};
  filenum = 0;
  
  if(stat(path, &st) < 0 || !S_ISDIR(st.st_mode)) {
    printf("invalid path: %s\n", path);
    return 0;
  }
  if(!(d = opendir(path))) {
    printf("opendir[%s] error: %m\n", path);
    return 0;
  }
  
  while((dp = readdir(d)) != NULL) {
    /* 把当前目录.，上一级目录..及隐藏文件都去掉，避免死循环遍历目录 */
    if((!strncmp(dp->d_name, ".", 1)) || (!strncmp(dp->d_name, "..", 2)))
      continue;
    snprintf(p, sizeof(p) - 1, "%s/%s", path, dp->d_name);
    stat(p, &st);
    if(!S_ISDIR(st.st_mode)) {
      size_t len = strlen(dp->d_name);
      if(len > 4 && strcmp(dp->d_name + len - 4, ".txt") == 0){
        strcpy(str[filenum],dp->d_name);
        filenum++;
      }
    }
  }
  closedir(d);

  return filenum;
}

/**************************************************************************************
**       该函数用于得到某目录下的特定后缀名文件      ==该函数有待改善==
**  原型: static void trave_dir(char* path, int filenum, char hzstr[], char str[][256])
**  path为待处理目录,filenum为文件个数，hzstr为特定后缀名，str记录文件名
**  返回文件个数
***************************************************************************************/

int trave_dir(char* path,int filenum,char hzstr[256],char str[filenum][256]) {
  DIR *d = NULL;
  struct dirent *dp = NULL;
  struct stat st;    
  char p[256] = {0};
  filenum = 0;
  
  if(stat(path, &st) < 0 || !S_ISDIR(st.st_mode)) {
    printf("invalid path: %s\n", path);
    return 0;
  }
  if(!(d = opendir(path))) {
    printf("opendir[%s] error: %m\n", path);
    return 0;
  }
  
  while((dp = readdir(d)) != NULL) {
    /* 把当前目录.，上一级目录..及隐藏文件都去掉，避免死循环遍历目录 */
    if((!strncmp(dp->d_name, ".", 1)) || (!strncmp(dp->d_name, "..", 2)))
      continue;
    snprintf(p, sizeof(p) - 1, "%s/%s", path, dp->d_name);
    stat(p, &st);
    if(!S_ISDIR(st.st_mode)) {
      size_t len = strlen(dp->d_name);
      if(len > 4 && strcmp(dp->d_name + len - 4, hzstr) == 0){
        strcpy(str[filenum],dp->d_name);
        filenum++;
      }
    }
  }
  closedir(d);

  return filenum;
}

/***************************************************************
**       该函数用于获取多列数据中的某几列
**  原型: void datalist (char str[],char split[],char *list[])
**  str为待处理数据，split为分隔符，list存储分割后的各列
****************************************************************/
void datalist (char str[],char split[],char *list[])
{
    char *llist;
    int k = 0;
    list[0] = strtok(str,split);
    llist = list[0];
    while(llist != NULL){
        list[k] = llist;
        llist = strtok(NULL,split);
        k++;
    }

    return;
}

/*******************************************************
**       该函数用于判断一个字符串是否是数字
**  原型: int digjudge (char str[])
**  如果数字返回1，反之返回0
*******************************************************/
int digjudge (char str[])
{
    int i,k,startout;
    i = strlen(str);
    for(k=0;k<i;k++){
        if(isdigit(str[k])||str[k]=='e'||str[k]=='E'||str[k]=='-'||str[k]=='.'||str[k]=='+'){
            if(str[k] == 'e'){
                if(str[k+1] == '-'||str[k+1] == '+')
                    startout = 1;
                else
                    {startout = 0; break;}
            }
            else if(str[k] == 'E'){
              if(str[k+1] == '-'||str[k+1] == '+')
                startout = 1;
              else
                {startout = 0; break;}
            }
            else if(str[k] == '-'||str[k]=='+'){
                if(str[k-1] == 'e'||str[k-1] == 'E')
                    startout = 1;
                else if(k==0)
                    startout = 1;
                else
                    {startout = 0; break;}
                }
            else if(str[k]=='.'){
                if(isdigit(str[k-1])&&isdigit(str[k+1]))
                    startout = 1;
                else
                    {startout = 0; break;}
            }
            else
                startout = 1;
        }
        else
            {startout = 0; break;}
    }

    if(startout == 1)
        return 1;
    else
        return 0;
}

/*************************************************************
**       该函数用于直线的最小二乘拟合
**  原型: void lsf(double x[],double y[],int n,double a[3])
**  x,y为待拟合二维数据点，n为数据点个数，
** a[0]=截距，a[1]=斜率，a[2]=方差
**************************************************************/
void lsf(double x[],double y[],int n,double a[3])
  {
	double dx,dxy,xx,yy,sse,sst;
	int i;

	xx=0.0;yy=0.0;
	for(i=0;i<n;i++)
	{
		xx+=x[i]/(1.0*n);
		yy+=y[i]/(1.0*n);
	}
	dx=0.0;dxy=0.0;
	for(i=0;i<n;i++)
	{
		a[2]=x[i]-xx;
		dx+=a[2]*a[2];
		dxy+=a[2]*(y[i]-yy);
	}
	a[1]=dxy/dx;
	a[0]=yy-a[1]*xx;
	a[2]=0;
	sse=0.1;sst=0.0;
	for(i=0;i<n;i++)
	{
		sst+=(y[i]-yy)*(y[i]-yy);
		sse+=(a[1]*x[i]+a[0]-y[i])*(a[1]*x[i]+a[0]-y[i]);
	}
        a[2]=1-sse/sst;

	return;
  }






