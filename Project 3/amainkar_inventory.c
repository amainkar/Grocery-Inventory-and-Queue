#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct Grocery_item
{
	//char grocery_product[40];
	char grocery_key[5];
	char grocery_amount[5];
	struct Grocery_item *next;
};
struct Customer
{
	char name[40];
	float balance;
	struct Customer *next_cust;
	struct Grocery_item *list;
} *head = NULL, *tail;

struct Item
{
	int key;
	char name[40];
	int threshhold;
	int stock;
	float price;
} itemInventory[100];

int arrKeys[200],keyCounter=0;

void addCustomer(struct Customer *current);
int hash_function (int key, int m);
int probe(int key, int i);
int hashing(int key);
void makeInventory(char line[]);
int checkoutReceipt(FILE *sp);
void checkout(char filename[]);
void inventorySubMenu();
void menu();
void getCustomer(char line[]);
void checkInventoryAfterQueue();
void writeBacktoInventory();



void addCustomer(struct Customer *current)
{
	if (head == NULL)
	{
		head = current;
		tail = head;
		head->next_cust = NULL;
	}
	else
	{
		tail->next_cust = current;
		tail = tail->next_cust;
		tail->next_cust = NULL;
	}
}

int hash_function (int key, int m) {
  return (key%m);
}

int probe(int key, int i){
	return ((hash_function(key,100)+i)%100);

}

int hashing(int key){
	int x, hashValue;
	for(x=0;x<100;x++){
		hashValue=probe(key,x);
		if (itemInventory[hashValue].key == key) {
			return hashValue;
      	}
	}
	return -1;
}

void makeInventory(char line[])
{
	char *token,temp[40],tempvar[100];
	char name[40];
	int key, threshold, stock, x=0,y=0,z=0,placed=0,hashValue=0,exists=0;
	float price;

	while (line[y + 1] != '\0')
	{
		if (line[y] != '{')
		{   //printf("%c",name[b][y]);
			tempvar[z] = line[y];
			z++;
		}
		y++;
	}

	token=strtok(tempvar, ", \"");
	key=atoi(token);
	token=strtok(NULL, "\", ");
	strcpy(name,token);
	token=strtok(NULL, "\", ");
	if(!isdigit(token[0])){
		strcat(name," ");
		strcat(name,token);
		token=strtok(NULL, ", ");
	}
	threshold=atoi(token);
	token=strtok(NULL, ", ");
	stock=atoi(token);
	token=strtok(NULL, "}");
	price=atof(token);
	exists=hashing(key);
	if(exists==-1){
		for(x=0;x<100 && placed!=1;x++){
			hashValue=probe(key,x);
			if (itemInventory[hashValue].key == -1) {
				itemInventory[hashValue].key = key;
				itemInventory[hashValue].stock = stock;
				itemInventory[hashValue].threshhold = threshold;
				itemInventory[hashValue].price = price;
				strcpy(itemInventory[hashValue].name,name);
				placed = 1;
				arrKeys[keyCounter]=key;
				keyCounter++;
			}
		}
	}
	//printf("%d %d\n",itemInventory[hashValue].key, hashValue);

}

int checkoutReceipt(FILE *sp)
{
	struct Customer *front;
	if (head == NULL)
	{
		//printf("\nSorry, queue is empty\n\n");
		return 0;
	}

	else
	{
		//Gets the Customer at the front of the line
		front = head;
		head = head->next_cust;
		front->next_cust = NULL;
		int hashValue;

		//Get the total_items and total_cost from the items
		struct Grocery_item *begin = front->list;
		int total_items = 0;
		float total_cost = 0;
		fprintf(sp,"Customer - %s\n",front->name);
		while (begin != 0x100000001000000 && begin != NULL)
		{
			hashValue=hashing(atoi(begin->grocery_key));
			//total_items += atoi(begin->grocery_amount);//Use hashmap to get price*******
			fprintf(sp,"\n %s x%d @ $%2f\n",itemInventory[hashValue].name, atoi(begin->grocery_amount), itemInventory[hashValue].price);
			total_cost +=  itemInventory[hashValue].price * atof(begin->grocery_amount);
			//itemInventory[hashing(begin->grocery_key)].stock-=atof(begin->grocery_amount);
			begin = begin->next;
		}

		//Finally, create the log for the current customer
		//First determine which line the customer goes into
		/*char lane[8] = "normal";
		if (total_items < 10)
		{
			lane[0] = 'e';
			lane[1] = 'x';
			lane[2] = 'p';
			lane[3] = 'r';
			lane[4] = 'e';
			lane[5] = 's';
			lane[6] = 's';
			lane[7] = '\0';
		}*/

		//Next determine if the customer got money
		if (total_cost > front->balance)
		{
			fprintf(sp,"\nTotal: $%2f\n", total_cost);
			fprintf(sp,"Customer did not have enough money and was REJECTED\n");
			fprintf(sp,"Thank you, come back soon!\n");
			fprintf(sp,"-------------------------------------------------------\n");
		}
		else
		{
			begin = front->list;
			fprintf(sp,"\nTotal: $%2f\n", total_cost);
			fprintf(sp,"Thank you, come back soon!\n");
			fprintf(sp,"-------------------------------------------------------\n");
			while (begin != NULL && begin != 0x100000001000000)
			{
				int tempIndex;
				total_items += atoi(begin->grocery_amount);//Use hashmap to get price*******
				tempIndex=hashing(atoi(begin->grocery_key));
				//total_cost +=  itemInventory[hashing(begin->grocery_key)].price * atof(begin->grocery_amount);
				itemInventory[tempIndex].stock=itemInventory[tempIndex].stock - atoi(begin->grocery_amount);
				begin = begin->next;
			}
			struct Grocery_item *bruh = front->list;
			struct Grocery_item *temp = front->list;
			char string[100];
			int index = 0;
			//Create the part of the log that specifies the groceries bought and the amount
			//Loop through all the groceries and keep adding to a character array
			/*while (bruh != NULL)
			{
				for (int i = 0; i < sizeof(bruh->grocery_amount) / sizeof(char); i++)
				{
					if (bruh->grocery_amount[i] == '\0')
					{
						break;
					}
					string[index] = bruh->grocery_amount[i];
					index++;
				}
				string[index] = ' ';
				index++;
				for (int sis = 0; sis < (sizeof(bruh->grocery_product) / sizeof(char)) - 1; sis++)
				{
					if (bruh->grocery_product[sis] == '\0')
					{
						break;
					}
					string[index] = bruh->grocery_product[sis];
					index++;
				}
				if (bruh->next != NULL)
				{
					string[index] = ',';
					index++;
					string[index] = ' ';
					index++;
				}

				bruh = bruh->next;
				free(temp);
				temp=bruh;
			}
			string[index++] = '\0';*/

			//Now enter in the generate string into the printf
			//fprintf(fp, "%s paid $%.2f for %s at the %s register\n", front->name, total_cost, string, lane);
			//printf("%s paid $%.2f for %s at the %s register\n", front->name, total_cost, string, lane);
			free(front);
		}
		return 0;
	}
}

void writeBacktoInventory(){
	FILE *fp;
	int x,hashValue;
	char filename1[100]="",filename[100]="inventory.txt";
	strcat(filename1,filename);
	fp=fopen(filename1,"w");
	for(x=0;x<200;x++){
		if(arrKeys[x]!=-1){
			hashValue=hashing(arrKeys[x]);
			fprintf(fp,"{%.3d, \"%s\", %d, %d, %.2f}\n",itemInventory[hashValue].key, itemInventory[hashValue].name, itemInventory[hashValue].threshhold, itemInventory[hashValue].stock, itemInventory[hashValue].price);
		}
	}
	fclose(fp);
}

void checkInventoryAfterQueue(){
	int x,hashValue,toggle=0;
	for(x=0;x<200;x++){
		if(arrKeys[x]!=-1){
			hashValue=hashing(arrKeys[x]);
			if(itemInventory[hashValue].stock<itemInventory[hashValue].threshhold){
				if(toggle==0){
					printf("Warning! The following Item(s) may need to be restocked:\n");
					toggle=1;
				}
				printf("%d (%s): %d remain in stock, replenishment threshold is %d\n",itemInventory[hashValue].key,itemInventory[hashValue].name,itemInventory[hashValue].stock,itemInventory[hashValue].threshhold);

			}
		}
	}
}

void checkout(char filename[]){
	char filename1[100]="",line0[100];
	char filename2[100]="";
	char *tok1;
	strcat(filename1,filename);
	tok1=strtok(filename,".");
	strcat(filename2,tok1);
	strcat(filename2,"_receipt.txt");
	FILE *xp,*wp;
	xp = fopen(filename1, "r");
	while (!feof(xp))
	{
		fgets(line0, 100, xp);
		getCustomer(line0);
	}
	fclose(xp);
	wp = fopen(filename2,"w+");
	while (head!=NULL)
	{
		checkoutReceipt(wp);
	}
	fclose(wp);
	checkInventoryAfterQueue();

}

void inventorySubMenu(){
	char inputLine[100], *token;
	int x,noWords=1,key,index,hashValue,placed,esc=0;
	while(esc==0){
		printf("\nadd item (syntax: add key name threshold stock price)\ndelete item (syntax: delete key)\nrestock item (syntax: restock key amount)\nrestock all (syntax: restock all)\nreturn to main menu (syntax: return)\n");
		gets(inputLine);
		for(x=0;x<200;x++){
			if(inputLine[x]==' '){
				noWords++;
			}
		}
		if(inputLine[0]=='d'){
			strcat(inputLine,"}");
			token=strtok(inputLine," ");
			token=strtok(NULL,"}");
			key=atoi(token);
			index=hashing(key);
			if(index!=-1){
				itemInventory[index].key=-1;
				strcpy(itemInventory[index].name,"\0");
				itemInventory[index].price=0;
				itemInventory[index].stock=0;
				itemInventory[index].threshhold=0;
				for(x=0;x<200;x++){
					if(arrKeys[x]==key){
						arrKeys[x]=-1;
					}
				}
				//printf("Deleted");
			}
			else{
				printf("Given key does not exist in inventory");
			}
		}
		else if(inputLine[0]=='a'){
			int threshold, stock;
			float price;
			char name[10],garb[10];

			token=strtok(inputLine," ");

			token=strtok(NULL," \"");
			key=atoi(token);
			token=strtok(NULL,"\" ");
			strcpy(name,token);
			token=strtok(NULL, "\", ");
			if(!isdigit(token[0])){
				strcat(name," ");
				strcat(name,token);
				token=strtok(NULL, ", ");
			}		
			threshold=atoi(token);
			token=strtok(NULL," ");
			stock=atoi(token);
			token=strtok(NULL,"\0");
			price=atof(token);
			placed=0;
			placed=hashing(key);
			if(placed!=-1){
				printf("Error: key %d is already taken\n",key);
				placed = 1;
			}
			for(x=0;x<100 && placed!=1;x++){
				hashValue=probe(key,x);
				if (itemInventory[hashValue].key == -1) {
					itemInventory[hashValue].key = key;
					itemInventory[hashValue].stock = stock;
					itemInventory[hashValue].threshhold = threshold;
					itemInventory[hashValue].price = price;
					strcpy(itemInventory[hashValue].name,name);
					placed = 1;
					arrKeys[keyCounter]=key;
					keyCounter++;
				}
			}
		}
		else if(inputLine[0]=='r'){
			int restock;
			token=strtok(inputLine," ");
			if(strcmp(token,"return")==0){
				break;
				//exit by breaking;
			}
			token=strtok(NULL," ");
			if(strcmp(token,"all")==0){
				//set stock=threshold for all products in inventory
				for(x=0;x<200;x++){
					if(arrKeys[x]!=-1){
						hashValue=hashing(arrKeys[x]);
						if(itemInventory[hashValue].stock<itemInventory[hashValue].threshhold){
							itemInventory[hashValue].stock=itemInventory[hashValue].threshhold;
						}
					}
				}
				//printf("Restocked");
			}
			else{
				key=atoi(token);
				token=strtok(NULL," ");
				restock=atoi(token);
				hashValue=hashing(key);
				itemInventory[hashValue].stock = itemInventory[hashValue].stock + restock;
				//printf("Restocked");
			}
		}
	}
}

void menu(){
	char line[100],*token;
	int esc=0;
	while(esc==0){
		printf("\n1. Check-out Queue of Customers (syntax: checkout filename)\n2. Manage Inventory (syntax: inventory)\n3. Close Program (syntax: quit)\n");
		gets(line);
		strcat(line,"}");
		if(line[0]=='c'){
			token=strtok(line," ");
			token=strtok(NULL,"}");
			//printf("%s",token);
			checkout(token);
		}
		else if(line[0]=='i'){
			inventorySubMenu();
		}
		else if(line[0]=='q'){
			writeBacktoInventory();
			esc=1;
			break;
		}
	}
}



void getCustomer(char line[])
{
	char tempvar[100], tempname[100], tempamount[100], tempGroceriesKey[10][50], tempGroceriesAmount[10][50];
	int x = 0, y = 0, z = 0, a = 0, b = 0, grosCounter = 0, arrayLineAmt = 0, lineIndex[15],commaCount=0;
    char *token;

	//printf("%s\n%s\n%s\n",name[0],name[1],name[2]);

	while (line[y + 1] != '\0')
	{
		if (line[y] != '{' && line[y] != '}' && line[y] != '[' && line[y] != ']')
		{   //printf("%c",name[b][y]);
            if(line[y]==','){
                commaCount++;
            }
			tempvar[z] = line[y];
			z++;
		}
		y++;
	}
	//tempvar[z] = ',', z++;
	tempvar[z] = '\0';
    commaCount= (commaCount-1)/2;
	grosCounter=commaCount;

	z = 0, y = 0;
    token=strtok(tempvar,"\"");
    strcpy(tempname,token);
    token=strtok(NULL,"\", ");
    strcpy(tempamount,token);

	while (commaCount>0)
	{
		token=strtok(NULL,", ");
        strcpy(tempGroceriesKey[x],token);
        token=token=strtok(NULL,", ");
        strcpy(tempGroceriesAmount[x],token);
        x++;
        commaCount--;
	}
    
	struct Customer *current = (struct Customer *)malloc(sizeof(struct Customer));

	struct Grocery_item *start = NULL;
	struct Grocery_item *fin;

	int index = 0;
	while (index < grosCounter)
	{
		if (start == NULL)
		{
			start = (struct Grocery_item *)malloc(sizeof(struct Grocery_item));
			start->next = NULL;
			fin = start;
			fin->next = NULL;
		}
		else
		{
			fin->next = (struct Grocery_item *)malloc(sizeof(struct Grocery_item));
			fin = fin->next;
			fin->next = NULL;
		}

		//Enter in the grocery's name
		strcpy(fin->grocery_key,tempGroceriesKey[index]);
		//printf("%s ",fin->grocery_key);

		//Enter in the Grocery's price
		//fin->grocery_cost = atof(tempGroceriesPrice[index]);

		//Enter in the Grocery amount
		strcpy(fin->grocery_amount,tempGroceriesAmount[index]);
		//printf("%s\n",fin->grocery_amount);
		index++;
	}

	current->list = start;
	current->balance = atof(tempamount);
	fin->next = NULL;
	strcpy(current->name,tempname);
	//printf("%f %s\n",current->balance,current->name);
	addCustomer(current);
	//printf(" ");
}

int main()
{
	FILE *fp,*xp;
	char line[100],line0[100],filename1[100]="";
	char filename[50]="inventory.txt";
	strcat(filename1,filename);
	int x;
	for(x=0;x<100;x++){
		itemInventory[x].key=-1;
	}
	for(x=0;x<200;x++){
		arrKeys[x]=-1;
	}
	xp = fopen(filename1, "r");
	while (!feof(xp))
	{
		fgets(line0, 100, xp);
		makeInventory(line0);
	}
	
	//printf("Type foo\n");
	menu();
	


    return 0;
}