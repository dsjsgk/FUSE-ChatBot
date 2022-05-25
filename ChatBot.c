#define FUSE_USE_VERSION 31
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
struct INode;
FILE * LOG;
struct INode {
	char * context;
	char * path;
	char * Name;
	int isdir;
	struct INode * nxt,*prev;
	struct INode * son, * father;
	
};

static struct INode * GetNewNode() {
	void * ptr = malloc(sizeof(struct INode));
	struct INode * newptr = (struct INode *) ptr;
	newptr->context=strdup("begin----------------------------------\n");
	newptr->path=strdup("begin----------------------------------\n");
	newptr->isdir=0;
	newptr->nxt=newptr->prev = NULL;
	newptr->son = NULL;
	newptr->Name = strdup("begin----------------------------------\n");
	newptr->father = NULL;
	return newptr;
}

static struct INode * root;

static struct INode * FindMyNode(const char *path) {
	struct INode * cur = root;
	if(strlen(path)==1) return cur;
	int ptr=1;
	while(ptr<strlen(path)) {
		char Name[100];
		int len=0;
		if(path[ptr]=='\0') return cur;
		while(path[ptr]!='/'&&path[ptr]!='\0') Name[len++]=path[ptr],ptr++;
		ptr++;
		Name[len]='\0';
		int flag=0;
		cur=cur->son;
		while(cur!=NULL) {
			if(strcmp((cur->Name),Name)!=0) cur=cur->nxt;
			else {flag=1;break;}
		}
		if(!flag) {return NULL;}
	}
	return cur;
}

static int My_getattr(const char *path, struct stat *stbuf,struct fuse_file_info *fi)
{
//	return 0;
	struct INode * ptr = FindMyNode(path);
	memset(stbuf,0,sizeof(struct stat));
	if(ptr==NULL) {
		return -ENOENT;
	}
	else {
		
		if(ptr->isdir) {
			stbuf->st_mode = S_IFDIR | 0755;
			stbuf->st_nlink = 2;
		}
		else {
			stbuf->st_mode=S_IFREG | 0444;
			stbuf->st_nlink = 1;
			stbuf->st_size=strlen(ptr->context);
		}
	}
	
	return 0;
}

static void AddSon(struct INode * Father,struct INode * newSon) {
	if(Father->son!=NULL)
	Father->son->prev = newSon,newSon->nxt = Father->son;
	Father->son = newSon;
	newSon->father = Father;
	return ;
}
//Release Memory
static void freeNode(struct INode * A) {
	if(A->context!=NULL) free(A->context);
	if(A->path!=NULL) free(A->path);
	if(A->Name!=NULL) free(A->Name);
	if(A->son!=NULL) {
		struct INode * ptr=A->son;
		while(ptr!=NULL) {
			struct INode * tmpptr = ptr;
			ptr=ptr->nxt;
			freeNode(tmpptr);
		}
	}
	free(A);
}

static void DeleteSon(struct INode * Father,struct INode * Son) {
	if(Father->son==Son) {
		if(Son->nxt!=NULL) Father->son = Son->nxt,Son->nxt->prev=NULL;
		else {
			Father->son=NULL;
		}
	}
	else {
		if(Son->nxt!=NULL) Son->nxt->prev=Son->prev,Son->prev->nxt=Son->nxt;
		else {
			Son->prev->nxt=NULL;
		}
	}
	//return ;
	freeNode(Son);
	return ;
}

static int My_mkdir(const char *path, mode_t mode) {
	//return 0;
	
	struct INode * ptr = NULL,* ptr_tmp = NULL;
	int las = strlen(path)-1;
	char newPath[100],Name[100]; 
	while(path[las]!='/') las--;
	//fwrite(path,sizeof(char)*strlen(path),1,LOG);
	if(las==0) {newPath[0]=path[0];newPath[1]='\0';}
	else 
	{	
		for(int i=0;i<las;++i) newPath[i]=path[i];
		newPath[las]='\0';
	}
	int lenname=0;
	for(int i=las+1;i<strlen(path);++i) Name[lenname++]=path[i];
	Name[lenname]='\0';
	//fwrite(newPath,sizeof(char)*strlen(newPath),1,LOG);
	//return 0;
	ptr=FindMyNode(newPath);
	ptr_tmp = FindMyNode(path);
	if(ptr==NULL) return -ENOENT;
	if(!(ptr->isdir)) return -ENOENT;
	if(ptr_tmp!=NULL) return 0;
	
	struct INode * newNode = GetNewNode();
	//return 0;
	newNode->path=strdup(path);
	
	newNode->Name=strdup(Name);
	
	AddSon(ptr,newNode);
	newNode->isdir = 1;
	return 0;
}
int mknodd(const char *path) {
	struct INode * ptr = NULL,* ptr_tmp = NULL;
	//fwrite()
	int las = strlen(path)-1;
	char newPath[100],Name[100];
	while(path[las]!='/') las--;
	if(las==0) {newPath[0]=path[0];newPath[1]='\0';}
	else 
	{	
		for(int i=0;i<las;++i) newPath[i]=path[i];
		newPath[las]='\0';
	}
	int lenname=0;
	for(int i=las+1;i<strlen(path);++i) Name[lenname++]=path[i];
	Name[lenname]='\0';
	ptr=FindMyNode(newPath);
	ptr_tmp = FindMyNode(path);
	if(ptr==NULL) return -ENOENT;
	if(!(ptr->isdir)) return -ENOENT;
	if(ptr_tmp!=NULL) return 0;
	struct INode * newNode = GetNewNode();
	newNode->path=strdup(path);
	newNode->Name=strdup(Name);
	AddSon(ptr,newNode);
	newNode->isdir = 0;
	return 0;
}
static int My_mknod(const char *path, mode_t mode, dev_t rdev)
{
	//fwrite(path,sizeof(char)*strlen(path),1,LOG);
	int las = strlen(path)-1;
	char newPath[100];
	char s[6][100];
	int cnt=0;
	int i=0;
	int curlen=0;
	while(1){
		if(path[i]=='\0') {
			s[cnt][curlen]='\0';
			break;
		}
		if(path[i]=='/') {
			cnt++;
			//s[cnt-1][curlen]='\n';
			s[cnt-1][curlen]='\0';
			curlen=0;
			++i;
			continue;
		}
		else s[cnt][curlen++]=path[i],++i;
	}
	//for(int i=1;i<=cnt;++i){
		
	//	fwrite(s[i],sizeof(char)*strlen(s[i]),1,LOG);
	//	fwrite("\n",sizeof(char)*strlen("\n"),1,LOG);
	//}
	int res1 = mknodd(path);
	int res2 = 0;
	int lenn=0;
	//fwrite
	if(cnt>=2) {
		int i=1;
		for(i=1;i<=cnt-2;++i) {
			newPath[lenn++]='/';
			for(int j=0;j<strlen(s[i]);++j) newPath[lenn++]=s[i][j];
		}
		newPath[lenn++]='/';
		for(int j=0;j<strlen(s[cnt]);++j) newPath[lenn++]=s[cnt][j];
		newPath[lenn++]='/';
		for(int j=0;j<strlen(s[cnt-1]);++j) newPath[lenn++]=s[cnt-1][j];
		newPath[lenn]='\0';
		
	//	fwrite(newPath,sizeof(char)*strlen(newPath),1,LOG);
		res2=mknodd(newPath);
		
	}
	return res1;
}

static int My_rmdir(const char *path)
{
	
    struct INode * ptr = FindMyNode(path);
    if(ptr==NULL) return -ENOENT;
    if(!(ptr->isdir)) return -ENOENT;
    if(strlen(path)==1) return -ENOENT;
    //return 0;
    DeleteSon(ptr->father,ptr);
    return 0;
}

static int My_unlink(const char *path)
{
    struct INode * ptr = FindMyNode(path);
    if(ptr==NULL) return -ENOENT;
    if(!(ptr->isdir)) return -ENOENT;
    if(strlen(path)==1) return -ENOENT;
    DeleteSon(ptr->father,ptr);
    return 0;
}
static int My_readdir(const char *path, void *buf, fuse_fill_dir_t filler,off_t offset, struct fuse_file_info *fi,enum fuse_readdir_flags flags)
{
	//return 0;
	struct INode * ptr = FindMyNode(path);
	
	if(ptr==NULL) return -ENOENT;
	filler(buf,".",NULL,0,0);
	filler(buf,"..",NULL,0,0);
	struct INode * cur = ptr->son;
	while(cur!=NULL) {
		filler(buf,cur->Name,NULL,0,0);
		cur=cur->nxt;
	}
	return 0;
}
int getmin(int a,int b) {
return a<b?a:b;
}
static int My_read(const char *path, char *buf, size_t size, off_t offset,struct fuse_file_info *fi) {
	struct INode * ptr = FindMyNode(path);
	if(ptr==NULL) return -ENOENT;
	if(ptr->isdir) return -ENOENT;
	//if(strlen(ptr->context)<size) return -ENOENT;
	for(int i=0;i<getmin(size,strlen(ptr->context));++i){
		buf[i]=(ptr->context)[i];
	}
	buf[size]='\0';
	return getmin(size,strlen(ptr->context));
}
static int My_open(const char *path, struct fuse_file_info *fi) {
	//return 0;
	struct INode * ptr = FindMyNode(path);
	if(ptr==NULL) return -ENOENT;
	else return 0;
}
static int My_write(const char *path, const char *buf, size_t size,off_t offset, struct fuse_file_info *fi){
	//return size;
	struct INode * ptr = FindMyNode(path);
	if(ptr==NULL) return -ENOENT;
	if(ptr->isdir) return -ENOENT;
	char s[1000],tmp[1000];
	strcpy(s,"I Say : \n");
	
	for(int i=0;i<size;++i) tmp[i]=buf[i];
	tmp[size]='\n';
	tmp[size+1]='\0';
	strcat(s,tmp);
	strcat(ptr->context,s);
	//fwrite(ptr->context,sizeof(char)*strlen(ptr->context),1,LOG);
	char newPath[100];
	char ss[6][100];
	int cnt=1;
	int i=1;
	int curlen=0;
	while(1){
		if(path[i]=='\0') {
			ss[cnt][curlen]='\0';
			break;
		}
		if(path[i]=='/') {
			cnt++;
			//s[cnt-1][curlen]='\n';
			ss[cnt-1][curlen]='\0';
			curlen=0;
			++i;
			continue;
		}
		else ss[cnt][curlen++]=path[i],++i;
	}
	if(cnt>=2) {
		int lenn=0;
		int i=1;
		for(i=1;i<=cnt-2;++i) {
			newPath[lenn++]='/';
			for(int j=0;j<strlen(ss[i]);++j) newPath[lenn++]=ss[i][j];
		}
		i=cnt;
		newPath[lenn++]='/';
		for(int j=0;j<strlen(ss[i]);++j) newPath[lenn++]=ss[i][j];
		i=cnt-1;
		newPath[lenn++]='/';
		for(int j=0;j<strlen(ss[i]);++j) newPath[lenn++]=ss[i][j];
		newPath[lenn]='\0';
		int o=0;
		for(int j=0;j<strlen(ss[cnt-1]);++j) {
			s[o++]=ss[cnt-1][j];
		}
		fwrite(newPath,sizeof(char)*strlen(newPath),1,LOG);
		struct INode * ptr2= FindMyNode (newPath);
		if(ptr2==NULL) return 0;
		s[o++]=' ';
		s[o++]='S';
		s[o++]='a';
		s[o++]='y';
		s[o++]=':';
		s[o++]='\n';
		s[o++]='\0';
		strcat(s,tmp);
		strcat(ptr2->context,s);
	}
	return size;
}
static void *My_init(struct fuse_conn_info *conn,struct fuse_config *cfg)
{
	(void) conn;
	cfg->kernel_cache = 0;
	return NULL;
}

static const struct fuse_operations My_oper = {
    .init       =   My_init,
    .getattr    =   My_getattr,
    .readdir    =   My_readdir,
    .open       =   My_open,
    .read       =   My_read,
    .mkdir      =   My_mkdir,
    .mknod      =   My_mknod,
    .write      =   My_write,
    .unlink     =   My_unlink,
    .rmdir      =   My_rmdir,
};


int main(int argc, char *argv[])
{
	LOG=fopen("log.txt","w");
	//fwrite("fuck\n",sizeof(char)*strlen("fuck\n"),1,LOG);
	int ret;
	root=GetNewNode();
	root->isdir = 1;
	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);


	/* Parse options */
	if (fuse_opt_parse(&args, NULL, NULL, NULL) == -1)
		return 1;

	ret = fuse_main(args.argc, args.argv,&My_oper, NULL);
	fuse_opt_free_args(&args);
	return ret;
}