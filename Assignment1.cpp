// **************************************************** //
// Derek Prince											//
// 18/1/15												//
// An exercise in ancient c++							//
// Craigslist-like sorting dealio						//
// **************************************************** //

/*
Not terribly thrilled with how rigid the code is, but with flexibility comes weight it would seem.
It would have been possible to prune and modify strings until any 3-sequence input would output formatted data
but that would have added many operations every pass.
*/

#include<iostream>	//std i/o
#include<cstdlib>	//std lib
#include<fstream>	//std::ifstream
#include<string>	//std::string
#include<sstream>	//std::stringstream
#include<algorithm>	//std::remove

using std::cout;
using std::cin;
using std::endl;
using std::string;

const size_t control = 100;	//just to change and limit the array in one place

int counter = 0;		//operation tracker
int made = 0;			//# of items created, global to skip passing to every argument, used as bookkeeping for delete as well as index

struct item	//like a class, just slightly less useful
{
	bool sale_want;	//true = sale | false = want
	double price;	//price
	string type;	//name
};

void fetchFileName(string &file_name);					//ask user for file name
void sortingHat(item *item_ptr[], std::ifstream &ifile);//big batch function. sorts the file and then calls other functions to do the sorting
void del_items(item *item_ptr[]);						//memory management to prevent mem leaks
void determine_need(item *item_ptr[], string &buffer);	//parses forsale/want into bool true/false
void check(item *item_ptr[]);							//checks for need and availability before committing to array and rebuffering
void swap(item *item_ptr[], int &del_i);				//removes sold items and places new one in old slot
void print_sale(item *item_ptr[]);						//it was just easier to split the function, it's not very different all-in-all
void print_want(item *item_ptr[]);						//print wanted --> end functions


int main()
{
	item *item_ptr[control];//pointer to create list items
	string file_name;		//user pointing to the file by name
	std::ifstream ifile;	//functional ifstream file

	fetchFileName(file_name);	//get file name
	ifile.open(file_name.c_str(), std::ifstream::in);		//open file, '.c_str()' because of lack of -std=c++11x or c++0x
//	ifile.open("messageBoard.txt", std::ifstream::in); //for testing purposes
	if (ifile.is_open())	//check
	{
		sortingHat(item_ptr, ifile);	//start main process
		ifile.close();					//close file after read
	}
	else
	{
		while (!ifile.is_open())
		{
			cout << "\n The file could not be opened.";
			ifile.clear();              //clear fail bit
			fetchFileName(file_name);	//fetch again
			ifile.open(file_name.c_str(), std::ifstream::in);		//update conditions
		}
		sortingHat(item_ptr, ifile);	//start the main process
		ifile.close();					//close file after read
	}

	print_sale(item_ptr);	//print items still for sale
	print_want(item_ptr);	//print items still wanted

	cout << "operations:" << counter << endl;

	del_items(item_ptr);	//delete array structs before closure
	cout << endl
		<< endl;
	return 0;	//END
}

void fetchFileName(string &file_name)
{
	cout << "\n File name:\n <name>.txt:";
	getline(cin, file_name);
	file_name.append(".txt");	//just to amke sure it can find the .txt extension. It is not worth it to write a series of loops to check for files. probably.
}

void sortingHat(item *item_ptr[], std::ifstream &ifile)
{
	string buffer;
	std::stringstream ssmcbuff;
	while (getline(ifile, buffer))//I chose to reuse the old buffer for risk of losing it's contents, but those are already loaded in ssmcbuff
	{
		counter++;	//read line

		item_ptr[made] = new item;	//creates item on pointer array location given by i

		buffer.erase(std::remove(buffer.begin(), buffer.end(), ' '), buffer.end());	//string shuffle followed by erasing the excess rm -r ' '
		ssmcbuff << buffer;	//load stringstream buffer after trimming
		getline(ssmcbuff, buffer, ',');	//transfer type
		item_ptr[made]->type = buffer;	//set type

		getline(ssmcbuff, buffer, ','); //transfer sale_want
		determine_need(item_ptr, buffer);		//translate sell/want to bool

		getline(ssmcbuff, buffer);				//transfer price
		//item_ptr[made]->price = stod(buffer);	//stod the stud (aka string-to-double)
		item_ptr[made]->price = atof(buffer.c_str());   //just kidding - we can't use -std=c++11x

		check(item_ptr);	//check for matches

		//cout << item_ptr[made]->type << " " << item_ptr[made]->sale_want << " " << item_ptr[made]->price << endl;

		ssmcbuff.clear();	//clear for next pass
		counter++;

		made++;
		counter++;
		//cout << made << endl;
	}
}

void determine_need(item *item_ptr[], string &buffer)
{
	counter++;
	if (buffer == "forsale")
		item_ptr[made]->sale_want = true;
	else
		item_ptr[made]->sale_want = false;
}

void check(item *item_ptr[])
{
	counter++;	//board search function
	for (int i = 0; i < made; i++)	//start at the beginning of the array
	{
		if (item_ptr[made]->type == item_ptr[i]->type)	//start with most specific, check if same
		{
			if (item_ptr[made]->sale_want != item_ptr[i]->sale_want)	//if so, move on to check if it is for sale/wanted
			{
				if (item_ptr[made]->sale_want == false)	//check to see if new listing is a buyer or seller. false = buyer
				{
					if (item_ptr[made]->price >= item_ptr[i]->price)	//if newest is buyer
					{
						cout << item_ptr[i]->type << " " << item_ptr[i]->price << endl;	//SOLD to newer listing at want/seller price
						swap(item_ptr, i);	//'i' will always be the deeper listing
					}
				}
				else if (item_ptr[i]->price >= item_ptr[made]->price)	//else newest is a seller, query price
				{
					cout << item_ptr[made]->type << " " << item_ptr[made]->price << endl;	//SOLD to older listing at want/seller price
					swap(item_ptr, i);	//'i' will always be the deeper listing
				}//with both buy/sell if's above, they share the same type, but not necessarily price so the second cout is significant not the first
			}
		}
	}
}

 void swap(item *item_ptr[], int &del_i)	//swap now empty with would-be new listing
 {
	 item_ptr[del_i]->type = item_ptr[made]->type;				//exchange type
	 item_ptr[del_i]->price = item_ptr[made]->price;			//exchange price
	 item_ptr[del_i]->sale_want = item_ptr[made]->sale_want;	//exchange sale_want

	 counter += 3;

	 delete item_ptr[made];	    //delete the latest addition, allowing it to be created again without mem leak
//	 item_ptr[made] = nullptr;  //not sure if this is actually necessary since it will be used again in a moment
                                    //just kidding, it's a mofo c++11x term.
	 made--;				    //keep current

	 counter++;
 }

 void print_sale(item *item_ptr[])	//print the items still up for sale
 {
	 for (int i = 0; i < made; i++)	//limited on the total number made to reduce the cycles
	 {
		 if (item_ptr[i]->sale_want == true)	//if it is indeed for sale
			 cout << item_ptr[i]->type << ", " << "for sale" << ", " << item_ptr[i]->price << endl;
	 }
 }

 void print_want(item *item_ptr[])	//print the items still up for sale
 {
	 for (int i = 0; i < made; i++)
	 {
		 if (item_ptr[i]->sale_want == false)
			 cout << item_ptr[i]->type << ", " << "wanted" << ", " << item_ptr[i]->price << endl;
	 }
 }

void del_items(item *item_ptr[])	//mem leak management
{
	for (int i = 0; i < made; i++)
	{
		delete item_ptr[i];
	}
}
