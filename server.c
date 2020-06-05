#include "types.h"

char requests[] = "requests.dat";
char offers[] = "offers.dat";

//global constants
unsigned long long OfferID = DefaultValue;
int msg;

int baseFile = 0;
off_t offsetSave;

List *offerList = NULL;
List *requestList = NULL;

//function declarations
static void getUserName(char* temp);
static void getDescriptionFromFile(char* temp);
static BOOL priceInRange(double current, double base);
static void printAD(User* Obj, int sockfd);
static BOOL readBaseFromFile(User* Obj,off_t offset);
ssize_t readLine(char *buffer,char *filename ,int size,off_t  *offset);
List * add(User,List*);
void menu(int sockfd);
void findPossibleMatches(List *offr, List *req, int sockfd);

static void getUserName(char* temp)
{	
	//get the username from the file
	if(temp != NULL)
	{
		char* result = malloc(sizeof(char)* 32); //32
		int counter = 0;
		while(temp[counter + 12] != '\0')
		{
		result[counter] = temp[counter +12]; //12 is offset
		counter++;
		}
		strcpy(temp,result);
	}
}

static void getDescriptionFromFile(char* temp)
{
	//get the product description from the file
	char* result = malloc(sizeof(char)* 64);
	int counter = 0;
	while(temp[counter + 15] != '.') //reads until "." symbol is encountered
	{
	    result[counter] = temp[counter + 15];
	    counter++;
	}
	strcpy(temp,result);
}

static BOOL priceInRange(double current, double base)
{
	//check if the price of the products is in the limit +- 10%
	BOOL result = False;
	double upper = base + ((base/100)*10);
	double lower = base - ((base/100)*10);
	double upper2 = current + ((current/100)*10);
	double lower2 = current - ((current/100)*10);
	if((current > lower) && (current < upper) && base>lower2 &&base<upper2)result = True;
	return result;
}

static void printAD(User* Obj, int sockfd)
{
	// print possible matches in server, create a message with this information and send it to the client
	char temp[1024];
	msg = snprintf(temp,1024,"\n\n__________________\nmatching AD\n\ttype = %d\n\tprice = %.2f\n\tid = %ld\n\tusername = %s\n\tdescription = %s\n__________________\n",
					 Obj->Offer.itemType,
					 Obj->Offer.price,
					 Obj->Offer.offerID,
					 Obj->UserName,
					 Obj->Offer.description);
	write(sockfd,temp,msg);

	printf("\n===========================================");
	printf("\nAD");
	printf("\n type = %d",Obj->Offer.itemType);
	printf("\n price = %.2f",Obj->Offer.price);
	printf("\n id = %lu",Obj->Offer.offerID);
	printf("\n username = %s",Obj->UserName);
	printf("\n description = %s",Obj->Offer.description);
	printf("\n===========================================");

}

//READ STRUCTURES FROM REQUESTS.DAT AND ADD TO LINKED LISTS
void readObjFromFile()
{
	int maxSize = INET_ADDRSTRLEN +20;
	off_t offset =0;
	char line[maxSize];
	int len =0;
	
	while((len = readLine(line,requests,maxSize,&offset))!= -1){
	    if(len==0)
		break; // nothing to read
		User base;
	    if(readBaseFromFile(&base, offsetSave))
		{
			User Obj;
			//get type
			Obj.Offer.itemType = atoi(line);

			readLine(line,requests,maxSize,&offset);

			//get price
			readLine(line,requests,maxSize,&offset);
			Obj.Offer.price = atof(line + 9);//9 is constant offset  

			//get the ID
			readLine(line,requests,maxSize,&offset);
			Obj.Offer.offerID = atoi(line + 6);//6 is constant offset

			//get the name
			readLine(line,requests,maxSize,&offset);
			getUserName(line);
			strcpy(Obj.UserName,line);

			//get description
			readLine(line,requests,maxSize,&offset);
			Obj.Offer.description = malloc(sizeof(char) * 100);
			getDescriptionFromFile(line);
			strcpy(Obj.Offer.description,line);
				
			//add possible matches to linked lists		
			offerList = add(base,offerList);
			requestList = add(Obj,requestList);

			//forward to next offer
			readLine(line,requests,maxSize,&offset);
			readLine(line,requests,maxSize,&offset);
		}
	}	
}

//READ STRUCRUTES FROM OFFERS.DAT
static BOOL readBaseFromFile(User* Obj,off_t offset)
{
	int maxSize = INET_ADDRSTRLEN +20;
	char line[maxSize];
			//get type
			readLine(line,offers,maxSize,&offset);
			Obj->Offer.itemType = atoi(line);
			readLine(line,offers,maxSize,&offset);

			//get price
			readLine(line,offers,maxSize,&offset);
			Obj->Offer.price = atof(line + 9);//9 is constant offset  

			//get the ID
			readLine(line,offers,maxSize,&offset);
			Obj->Offer.offerID = atoi(line + 6);//6 is constant offset

			//get the name
			readLine(line,offers,maxSize,&offset);
			getUserName(line);
			strcpy(Obj->UserName,line);

			//get description
			readLine(line,offers,maxSize,&offset);
			Obj->Offer.description = malloc(sizeof(char) * 100);
			getDescriptionFromFile(line);
			strcpy(Obj->Offer.description,line);

			// forward to next offer
			readLine(line,offers,maxSize,&offset);
			readLine(line,offers,maxSize,&offset);

			offsetSave = offset;
return True;
}

ssize_t readLine(char *buffer,char *filename ,int size,off_t *offset)  {
	 //open file in read only mode
         int fd =open(filename , O_RDONLY ) ;
     
	 //check if file is open corectly
         if(fd ==-1)  return - 1 ;
     
         ssize_t charCount = 0 ,counter = 0 ;
         char  *p  = NULL ;

	 //read the file data until the end is reached
         if((charCount =lseek(fd,*offset,SEEK_SET))!= -1){
         charCount = read(fd,buffer,size);
         }
	
	 //close file descriptor
         close(fd);

         if(charCount == -1)
	 return -1 ;

	 //read bytes until newline symbol is reached (in order to be able to read the file line by line)
         p = buffer ;
         while(counter < charCount && *p !='\n') p ++, counter ++;
     
	 //set the pointer to null after newline symbol is encountered and increase the offset.
         *p  =  0 ;
         if(counter == charCount){
             *offset+= charCount ;
     
             return charCount < (ssize_t )size ?charCount : 0;
         }
     
         *offset += counter + 1 ;
         return counter;
    }

//Add a new List to linked list
List * add(User user, List *head) {
    List *new_List;
    new_List = (List *) malloc(sizeof(List));
    new_List->usr = user;
    new_List->next= head;
    head = new_List;
return head;
}

//find possible matches in offers.dat and requests.dat by using linked lists
void findPossibleMatches(List *offr, List *req,int sockfd){
     List *current_offr = offr;
     List *current_req = req;

    while(current_req != NULL){
         while(current_offr != NULL){

	     //are the types equal and the price in the +-10% range
	     if((current_offr->usr.Offer.itemType == current_req->usr.Offer.itemType) && (priceInRange(current_offr->usr.Offer.price,current_req->usr.Offer.price))){
             printf("\nmatch in offers:");
	     printAD(&current_req->usr,sockfd);
	     printAD(&current_offr->usr,sockfd); 
		
	     //go to next 2 offers.
             current_req = current_req->next;
	     current_offr = current_offr->next;
	     }
	     else{
		 write(sockfd,"\nno matches!\n",sizeof("\nno matches!\n"));
	     }
     	}  
    }
}

void menu(int sockfd){
	int choice;
	int ch;
	do {
		read(sockfd, &choice, sizeof(choice)); // reads client's choice

		printf("\nClient's choice is: %d ",choice);
		switch (choice) {
		case 1: 
			printf("You selected option #1.\n");
			break;
		case 2:
			printf("You selected option #2.\n");
			break;
		case 3:
			printf("You selected option #3. Possible matches are: ");
			findPossibleMatches(offerList,requestList,sockfd);
			
			break;
		case 4: printf("\nGoodbye!\n");
			
			printf("\nServer Exit...\n"); 
			break;
		default:
			printf("Wrong choice. Try again.\n");
			break;
		}
	} while (choice != 4);
	
getchar();
}

int main(void) {
	readObjFromFile();
	error Result = OK;
 	int sockfd, connfd, len; 
    	struct sockaddr_in servaddr, cli; 
  
    	// socket create and verification 
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
    	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    	servaddr.sin_port = htons(PORT); 
  
    	// Binding newly created socket to given IP and verification 
    	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0){ 
        printf("socket bind failed...\n"); 
        exit(0); 
   	} 
    	else
        printf("Socket successfully binded..\n"); 
  
    	// Now server is ready to listen and verification 
    	if ((listen(sockfd, 5)) != 0) { 
        printf("Listen failed...\n"); 
        exit(0); 
    	} 
    	else
        printf("Server listening..\n"); 
    	len = sizeof(cli); 
  
   	// Accept the data packet from client and verification 
        connfd = accept(sockfd, (SA*)&cli, &len); 
    	if (connfd < 0) { 
        printf("server acccept failed...\n"); 
        exit(0); 
    	} 
    	else
        printf("server acccept the client...\n"); 
  
    	// Menu function for communication between client and server 
    	menu(connfd); 
  
    	// After chatting close the socket 
    	close(sockfd); 

    return Result;
}

