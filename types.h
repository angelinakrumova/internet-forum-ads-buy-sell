#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h> 
#include <netdb.h> 
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>

typedef char BOOL;
#define False 0
#define True  1
#define MaxSymbolsPerLine 50U
#define MaxSymbolsDescription 500U

#define UserNameMaxLen 32U
#define DefaultValue  (-1)
#define MaxSymbolsIDNumber 9U

#define minID 100000000
#define maxID 999999999

#define MAX 1024 
#define PORT 8080 
#define SA struct sockaddr 

//item types enum
typedef enum eItemPlaceHolder
{
	NoItem,
	PlaceHolder,
	PlaceHolder2,
	PlaceHolder3,
	PlaceHolder4
}eItemPlaceHolder;

// pointer to char* - dynamic reading and writing 
typedef struct sOffer
{
	eItemPlaceHolder    itemType;
	unsigned long       offerID;
	double              price;
	char*              description;  
}sOffer;

//final structure which will be written into/read from the files
typedef struct User
{
	char  UserName[32];
	sOffer Offer;
}User;


//some error codes for debug
typedef enum error
{
	OK,
	NullPointer,
	NameNotOk,
	UserNameNotOK,
	OfferIDnotOK,
	PriceNotOK,
	NoItemSelected,
	DescriptionNotOK
}error;


//linked list structure 
  typedef struct llist{
	User usr;
	struct llist *next;
} List;

