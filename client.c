#include "types.h"
#define MAX 1024

char requests[] = "requests.dat";
char offers[] = "offers.dat";

//global constants
unsigned long long OfferID = DefaultValue;
int msg;

int baseFile = 0;

//private functions
static long long int generateID( void );
static error FillOfferObj(User* Obj);
static error ObjectWriter(User* Obj, int  fd);
static eItemPlaceHolder SelectItem(void);
static char* getDescription(FILE* fp, size_t size, size_t max);

//generate unique id for each offer
static long long int generateID( void )
{
	time_t t;
	srand((unsigned) time(&t));
	return (minID + rand() % (maxID+1 - minID));
}

//get the description from the standart input
static char* getDescription(FILE* fp, size_t size, size_t max)
{
    char *str;
    int ch = 0;
    size_t len = 0;
    str = malloc(size);
    if(!str)return str;
    while(EOF!=(ch=fgetc(fp)))
    {
    	if(ch != '\t' && ch != '\n')
    	{
    		str[len++]=ch;
    	}
        if(len==size)
        {
            str = realloc(str, sizeof(char)*(size = size + MaxSymbolsPerLine));
            if(!str)return str;
        }

        if(len == max)
        {
        	break;
        }
    }
    str[len++]='\0';
    return realloc(str, len);
}

//select item type from a list of options
static eItemPlaceHolder SelectItem(void)
{
	eItemPlaceHolder item = NoItem;
	printf("\nSelect item type:");
	printf("\n1: GSM");
	printf("\n2: Laptop");
	printf("\n3: Video card");
	printf("\n");
	
	scanf("\n%u",&item);
	if(item >= PlaceHolder4) //check if the entered type is correct
	{
		item = NoItem;
	}
	return item;
}

//fill the new offer object with proper information
static error FillOfferObj(User* Obj)
{
	error Error = OK;
	char temp[100];
	//get the userName
	printf("\nEnter User name: ");
	scanf("%s", temp);
	if(strlen(temp) > UserNameMaxLen)
	{
		Error = NameNotOk;
	}
	else
	{
		strcpy(Obj->UserName,temp);
		temp[0] = '\0'; // empty the temp array, faster than memset
	}

	//get the Item
	Obj->Offer.itemType = SelectItem();
	if(Obj->Offer.itemType == NoItem)
	{
		Error = NameNotOk; 
	}

	//set default id
	Obj->Offer.offerID = DefaultValue;

	//get the price
	printf("\nSet the price: ");
	scanf("%lf", &(Obj->Offer.price));
	if(Obj->Offer.price <= 0.0)
	{
		Error = PriceNotOK; 
	}

	printf("\nWrite something about the product: ");
	//get the description 
	Obj->Offer.description = getDescription(stdin, MaxSymbolsPerLine, MaxSymbolsDescription);
	if(Obj->Offer.description == NULL)
	{
		Error = DescriptionNotOK;
	} 
	return Error;
}

//write the object into a file descriptor
static error ObjectWriter(User* Obj, int  fd)
{
	error Error = OK;

	if(Obj->Offer.offerID == DefaultValue )
	{
		Obj->Offer.offerID = generateID();

		//snprintf removes the need to use strcat multiple times here in order to store the whole message in a buffer. It makes the code more readable and clean.
		char temp[1024];
		msg = snprintf(temp,1024,"%d\n{\n\tprice = %.2f\n\tid = %ld\n\tusername = %s\n\tdescription = %s\n}\n\n",
					 Obj->Offer.itemType,
					 Obj->Offer.price,
					 Obj->Offer.offerID,
					 Obj->UserName,
					 Obj->Offer.description);

		write(fd,temp,msg);

	}
	else
	{
		Error = OfferIDnotOK; // check if the offer id is ok
	}

	return Error;
}

//write object in a file
error writeOfferToFile(User* Obj, char* fileName)
{
	error Error = OK;
	int fd = open(fileName, O_RDWR|O_APPEND);

	if(0 <= fd) //check if the file is oppened correctly
	{
		Error = FillOfferObj(Obj);
		if(Error != OK)
		{
			return Error;
		}
		else
		{
			Error = ObjectWriter(Obj,fd);
		}
		close(fd);
	}
	else
	{
		Error = NullPointer;
	}

	return Error;
}

void menu(int sockfd){
	int choice;
	char buff[MAX]; 
    	int n; 
	char *msg = NULL;

// client chooses an option from the menu and sends it to the server  in order to receive the wanted result
	do {
		printf("\n\t\t C PROGRAM INTERNET FORUM\n");
		printf("\n________________________________________________________________");
		printf("\n\t\t\t WELCOME ");
		printf("\n\t\t\t    to ");
		printf("\n\t\t\tOUR FORUM ");
		printf("\n________________________________________________________________");
		printf("\n________________________________________________________________");
		printf("\n\t\t PLEASE SELECT AN OPTION:   ");
		printf("\n________________________________________________________________");
		printf("\n________________________________________________________________");
		printf("\n > Press 1 to add an offer.");
		printf("\n > Press 2 to add a request.");
		printf("\n > Press 3 to find possible matches from the opposite section.");
		printf("\n > Press 4 to exit             ");
		printf("\n________________________________________________________________\n\n");
		
		scanf("%d", &choice);
		//send client choice to server
		write(sockfd,&choice,sizeof(choice));
		printf("Client's choice is: %d \n",choice);

		switch (choice) {
		case 1: 
			printf("You selected option #1. Please fill in the following information to add an offer: \n");
			User newOffer;
			writeOfferToFile(&newOffer, offers); // write the offer to offers.dat file
			break;
		case 2:
			printf("You selected option #2. Please fill in the following information to add an request: \n");
			User newRequest;
			writeOfferToFile(&newRequest, requests); // write the offer to offers.dat file
			break;
		case 3:
			printf("You selected option #3. Server will display the possible matches: ");
			bzero(buff, sizeof(buff));
			read(sockfd,buff,sizeof(buff));
			printf("From server: %s\n",buff);
			break;
		case 4: 
			printf("Goodbye!\n");
			printf("Client Exit...\n"); 
			break;
		default:
			printf("Wrong choice. Try again.\n");
			break;
		}
	} while (choice != 4); //repeat until a correct command is written

getchar();
}

int main() 
{ 
	int sockfd, connfd; 
	struct sockaddr_in servaddr, cli; 

	// socket create and varification 
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) { 
		printf("socket creation failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully created..\n"); 
	bzero(&servaddr, sizeof(servaddr)); 

	// assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
	servaddr.sin_port = htons(PORT); 

	// connect the client socket to server socket 
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
		printf("connection with the server failed...\n"); 
		exit(0); 
	} 
	else
		printf("connected to the server..\n"); 

	// function for communication with server 
	menu(sockfd);

	// close the socket 
	close(sockfd); 
} 


