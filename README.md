# internet-forum-ads-buy-sell
This is a client-server university systems programming project. 

## General info
This is a client-server university project in "Systems programming" discipline.

## Description
  The owners of an internet forum make daily reports for sections "Buy" and "Sell". In these sections different users, identified by their usernames, add the relevant ads. Every ad contains:
  
	  -Product type (predefined list of types of your choice - for example, if the program is about a computer forum, you can define types like “gsm”, “laptop”, “video card”, “tablet”, …; 
	  -Ad ID
	  -Ad title as free text: characteristic which distinguishes it from the others, for example "I sell new Nokia N90 mobile phone, great quality, located in Sofia,Bulgaria.
	  -Product price (at which it is sold or at which the user would like to buy it).
    
  The above mentioned reports are located in two files - requests.dat (for section "Buy") and offers.dat (for section "Sell"), in which the data is formated by your judgement. Implement a function, in which the program opens both of the files from the current directory and searches them for "possible matches". Matching ads are two ads - one from section "Buy" and one from section "Sell", which have the same product type and their prices vary in the range of not more than 10%. As a result, the function should print on the screen the full information for the possible matching ads.
  
## Compile

```
$ gcc server.c -o server
$ gcc client.c -o client
```

## Run

```
$ ./server
$ ./client
```
