#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG
#define MAX 200
#define MAXTOKEN 40

#define COMMAND_RETRIEVE 1
#define COMMAND_LEND 2
#define COMMAND_RETURN 3
#define COMMAND_EXIT 4

#define RETRIEVE_KEY_ID 1
#define RETRIEVE_KEY_AUTHOR 2
#define RETRIEVE_KEY_TITLE 3

struct book{
	int id;
	char title[MAXTOKEN];
	char author[MAXTOKEN];
	int num;
	int borrowed;
};

struct idx_tbl{
	int id;
	int idx;
};

int create_tbl(FILE *infp,struct book *book,struct idx_tbl *idx_tbl);
void sort_book(struct idx_tbl *idx_tbl,int left,int right);
int retrieve_by_id(int id,struct idx_tbl *idx_tbl,int n);
int retrieve_by_author(char *author,struct book *book,int b,int n);
int retrieve_by_title(char *title,struct book *book,int b,int n);
void lend(struct book *book,int i);
void ret(struct book *book,int i);
void proc_request(struct book *book, struct idx_tbl *idx_tbl,int n);

int main(int argc,char *argv[])
{
	int i,n,g,h,id;
	struct book Book[MAX];
	struct idx_tbl Idx_tbl[MAX];
	FILE *infp;
	
		if((infp=fopen("books.txt","r"))==NULL){
			printf("file not found.\n");
			exit(1);
		}
	
	n=create_tbl(infp,Book,Idx_tbl);
	sort_book(Idx_tbl,0,n-1);
	
	proc_request(Book, Idx_tbl, n);
	
	fclose(infp);
	return 0;
}

int create_tbl(FILE *infp,struct book *Book, struct idx_tbl *Idx_tbl)
{
	int i=0;
	int id,num;
	char title[MAXTOKEN],author[MAXTOKEN];
	
	while(fscanf(infp,"%d %s %s %d",&id,author,title,&num)!=EOF){
		Book[i].id=id;
		strcpy(Book[i].author,author);
		strcpy(Book[i].title,title);
		Book[i].num=num;
		Idx_tbl[i].id=id;
		Idx_tbl[i].idx=i;
		Book[i].borrowed=0;
		++i;
	}
	return i;
}

void sort_book(struct idx_tbl *idx_tbl,int left,int right)
{
	int i,j,p,k;
	i=left;
	j=right;
	p=i+(j-i)/2;
	struct idx_tbl temp;
	
	if(left>=right)
		return;
		
	while(i<=j)
	{
		while(idx_tbl[i].id<idx_tbl[p].id)
		{
			i++;
		}
		while(idx_tbl[j].id>idx_tbl[p].id)
		{
			j--;
		}
	
		if(i<=j)
		{
			k=idx_tbl[i].id;
			idx_tbl[i].id=idx_tbl[j].id;
			idx_tbl[j].id=k;

			k=idx_tbl[i].idx;
			idx_tbl[i].idx=idx_tbl[j].idx;
			idx_tbl[j].idx=k;

			i++;
			j--;
		}
	}
	sort_book(idx_tbl,left,j);
	sort_book(idx_tbl,i,right);
}

int retrieve_by_id(int id,struct idx_tbl *idx_tbl,int n)
{
	int i,r,low,high,mid;
	high=n;
	low=0;
	
	while(low != high)
	{
		mid=(high+low)/2;
		
		if(id == idx_tbl[mid].id)
		{
			return idx_tbl[mid].idx;
		}
		if(id<idx_tbl[mid].id)
		{
			high=mid-1;
		}
		if(id>idx_tbl[mid].id)
		{
			low=mid+1;
		}
	}
	
	if(id == idx_tbl[high].id)
	{
		return idx_tbl[high].idx;
	}
	
	return -1;
}

int retrieve_by_author(char *author,struct book *book,int b,int n)
{
	int i,m,p;
	for(i=b;i<=n;i++)
	{
		p= strcmp(author,book[i].author);
		if(p == 0)
		{
			return i;
		}
	}
	return -1;
}

int retrieve_by_title(char *title,struct book *book,int b,int n)
{
	int i,m,p;
	for(i=b;i<=n;i++)
	{
		m = strlen(title);
		p = strncmp(title,book[i].title,m);
		if(p == 0)
		{
			return i;
		}
	}
	return -1;
}

void lend(struct book *book,int i)
{
	int num,borrowed;
	if(book[i].num>=book[i].borrowed)
	{
		book[i].borrowed=book[i].borrowed+1;
		book[i].num=book[i].num-1;
		printf("現在の貸出数：%d\n",book[i].borrowed);
		printf("現在の残り冊数：%d\n",book[i].num);
	}
	else
	{
		printf("貸し出すことができません\n");
	}
}

void ret(struct book *book,int i)
{
	int num,borrowed;
	if(book[i].borrowed>0)
	{
		book[i].borrowed=book[i].borrowed-1;
		book[i].num=book[i].num+1;
		printf("現在の貸出数：%d\n",book[i].borrowed);
		printf("現在の残り冊数：%d\n",book[i].num);
	}
	else
	{
		printf("貸し出されていません\n");
	}
}

void proc_request(struct book *book, struct idx_tbl *idx_tbl, int n)
{
	int cmd,subcmd,id,i,b;
	char author[MAXTOKEN];
	char title[MAXTOKEN];
	while(1)
	{
		i=0;
		printf("コマンドを入力してください 検索(1)/貸出(2)/返却(3)/終了(4):");
		scanf("%d",&cmd);
		switch(cmd)
		{
			case COMMAND_RETRIEVE:
			printf("\t キーは何にしますか？ ID(1)/著者名(2)/タイトル(3):");
			scanf("%d",&subcmd);
			switch(subcmd)
			{
				case RETRIEVE_KEY_ID:
				printf("\t\tID を入力してください:");
				scanf("%d",&id);
				i=retrieve_by_id(id,idx_tbl,n);
				if(i == -1)
				{
					printf("見つかりませんでした\n");
				}
				else
				{
					printf("Id=%d Author=%s Title=%s Num=%d\n",book[i].id,book[i].author,book[i].title,book[i].num);
				}
				break;
				
				case RETRIEVE_KEY_AUTHOR:
				printf("\t\t著者名を入力してください:");
				scanf("%s",author);
				b=0;
				for(;;)
				{
				i=retrieve_by_author(author,book,b,n);
				if(i==-1)break;
				printf("Id=%d Author=%s Title=%s Num=%d\n",book[i].id,book[i].author,book[i].title,book[i].num);
				b=i+1;
				}
				if(b == 0)
				{
					printf("見つかりませんでした\n");
				}
				break;
				
				case RETRIEVE_KEY_TITLE:
				printf("\t\tタイトルを入力してください:");
				scanf("%s",title);
				b=0;
				for(;;)
				{
				i=retrieve_by_title(title,book,b,n);
				if(i==-1)break;
				printf("Id=%d Author=%s Title=%s Num=%d\n",book[i].id,book[i].author,book[i].title,book[i].num);
				b=i+1;
				}
				if(b == 0)
				{
					printf("見つかりませんでした\n");
				}
				break;
				default:
			}
			break;
			
			case COMMAND_LEND:
			printf("\t借りたい本のIDを入力してください：");
			scanf("%d",&id);
			i=retrieve_by_id(id,idx_tbl,n);
			if(i == -1)
			{
				printf("見つかりませんでした\n");
			}
			else
			{
				lend(book,i);
			}
			break;
			
			case COMMAND_RETURN:
			printf("\t返却したい本のIDを入力してください：");
			scanf("%d",&id);
			i=retrieve_by_id(id,idx_tbl,n);
			if(i == -1)
			{
				printf("見つかりませんでした\n");
			}
			else
			{
				ret(book,i);
			}
			break;
			case COMMAND_EXIT:
			printf("さようなら\n");
			return;
			default:
			printf("入力誤りです\n");
		}
	}
}
