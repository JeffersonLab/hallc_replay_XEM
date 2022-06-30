#ifndef PARSE_UTILS_H
#define PARSE_UTILS_H

/*
  Author: C. Yero
  Date: Jan 22, 2022 
 */

//The pasrse_utils.h header file contains useful functions for C++ string parsing


//------------------------START UTILITIES FUNCTIONS----------------------------


//_______________________________________________________________________________
vector<string> parse_line(string my_str, char del)
{

  /*
    Brief: This method reads in a string and parses it 
    based on the specified delimiter (del). A parsed
    vector is returned, whose elements are the values
    previously separated by the delimiter
    
   */
  
  vector<string> parsed;
  stringstream s_stream(my_str); //create string stream from the string

  while(s_stream.good()) {

    string substr;
    getline(s_stream, substr, del); //get first string delimited by comma
    parsed.push_back(substr);

  }


  return parsed;
}

//_______________________________________________________________________________
string getString(char x)
{
  //method to convert a character to a string
  string s(1,x);
  return s;
}

//_______________________________________________________________________________
vector <string> split(string str, char del=':')
{
  
  //method to split a string into a vetor of strings separated by a delimiter del
  //Returns a vector of strings, whose elements are separated by the delimiter del.
  
  string temp1, temp2;
  
  vector <string> parse_word;
  int del_pos;  //delimiter position
  
  for (int i=0; i < str.size(); i++){
    
    //Get position of delimiter
    if(str[i]==del){
      del_pos = i;
    }
    
  }
  
  for (int i=0; i < str.size(); i++){
    
    //append char to a string for char left of the delimiter
    if(i<del_pos){
      temp1.append(getString(str[i]));
    }      
    
    //append char to a string for char right of the delimiter
    else if(i>del_pos){
      temp2.append(getString(str[i]));
    }
    
  }
  parse_word.push_back(temp1);
  parse_word.push_back(temp2);
  
  return parse_word;
}

//_______________________________________________________________________________
vector <string> FindString(string keyword, string fname, bool multi_line=false, int line_max=-1)
{
  
  //Method: Finds string keyword in a given txt file. 
  //Returns the lines (stored in a vector) in which the keyword was found. Lines are counted from 0. 

  /*
    DEFINITION OF INPUT FUNCTION PARAMETERS:
    'keyword'    : the actual keyword the user needs to find in the file
    'fname'      : the filename full path where the keyword can be found
    'multi_line' : boolean flag that gives the option to read not only the line where the keyword is found,
    but also the subsequent lines below the keyword line.
    'line_max' : maximum number of lines to be read, including the line where the keyword is found. 
    For example, if line_max = 10, the first 10 lines (including the keyword line) will be stored into the line_found vector
    
    Example of usage for:

    ---multi-line parameters---
    
    FindString("keyword_name", "path/to/filename.txt", true, 3)[0]
    FindString("keyword_name", "path/to/filename.txt", true, 3)[1]
    FindString("keyword_name", "path/to/filename.txt", true, 3)[2]
    
    --single-line parameter---
    FindString("keyword_name", "path/to/filename.txt")[0]
  
  */
  
  ifstream ifile(fname);
  
  vector <string> line_found; //vector to store in which lines was the keyword found

  int line_fnd = -1;
  int line_cnt = 0;
  string line;                  //line string to read
  
  int found = -1; //position of found keyword
  
  while(getline(ifile, line))
    {
      //Check 1st character of found string
      TString cmt = line[0];
      
      found = line.find(keyword);
      
      if(found<0||found>1000){found=-1;} //not found
      if(cmt==";" || cmt=="#" || cmt=="!") {found=-1;}  //Found commented line. So Skip (You can add more definition of comments)

      if(found!=-1){
	//cout << "position of line found = " << line_cnt <<  endl;
	line_fnd = line_cnt;  //store line number where the keyword was found
	//line_found.push_back(line);
	
      } //end if statement

      //C.Y. Jan 22 || potentially read subsequent lines (in case of multi-line paramerers)
      if(multi_line){
	// read only single line, where the keyword was found
	if ( (line_fnd!=-1) and (line_cnt<line_fnd+line_max) ){
	  //cout << "line_cnt = " << line_cnt << ", line_fnd = " << line_fnd << endl;     
	  //cout << "line --> " << line << endl;
	  line_found.push_back(line);
	}
      }
      
      // Only read single-line keyword 
      else if (line_fnd==line_cnt){
	line_found.push_back(line);
      }

      
      // increment line counter
      line_cnt++;
      
    } //end readlines

  return line_found;

}

//_______________________________________________________________________________
string& ltrim(std::string& s)
{
  auto it = std::find_if(s.begin(), s.end(),
			 [](char c) {
			   return !std::isspace<char>(c, std::locale::classic());
			 });
  s.erase(s.begin(), it);
  return s;
}

//_______________________________________________________________________________
string& rtrim(std::string& s)
{
  auto it = std::find_if(s.rbegin(), s.rend(),
						[](char c) {
			   return !std::isspace<char>(c, std::locale::classic());
			 });
  s.erase(it.base(), s.end());
  return s;
}

//_______________________________________________________________________________
string& trim(std::string& s)
{
  return ltrim(rtrim(s));
}

//--------------------------END UTILITIES FUNCTIONS----------------------------


#endif
