
#include<bits/stdc++.h>
#include<pwd.h>
#include<grp.h>
#include <sys/stat.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <dirent.h>
#include<fstream>
using namespace std;

winsize terminal;
unsigned int TERMINAL_LEN,CURSOR,SCROLL_POINTS=0,pagesize=15,SCR=15;
stack<string> backstack,forwardstack;
termios original,raw_one,raw_timeout;
unsigned int FILE_POINTER,FILES_LEN;
vector<vector<string>> files_list;
string rootname;
void exitsystem();
void onend(void);
void onend(int);
void onend(string);
void Raw_on(void);
void Raw_off(void);
void getTerminalHeight(int);
void clear(void);
void display(void);
void cursor(void);
void checker(char *);
void copy_permissions(string &src,string &);
bool search_dir(string &path,string &,int );
bool search_dir_recursive_cmd(string &,string &);
void createfilesbuffer(string,vector<string>&);
void inline pathresolver(string &);
void commandmode(void);
void schecker(string );
void inline pathresolver(string &path)
{
	string cur=get_current_dir_name();
        
        char *resolvedpath ;
	if(path[0]=='~')
	{   
		unsigned int siz=rootname.size();
		if(rootname[siz-1]=='/')
		path=rootname+path.substr(2);
		else
		{
			path = rootname+path.substr(1);
		}
		

		
	}
	else if(path[0]=='/')
	{
		
         
	  if(path.size() > rootname.size())
	  {
		  if(path.substr(0,rootname.size()) == rootname)
		  return;
	  }
      onend("In pathresolver.....Invalid Path...");
		
	}
	else
	path = cur+'/'+path;

       resolvedpath= new char[4096];
       realpath(path.c_str(),resolvedpath);
       checker(resolvedpath);
        path = resolvedpath;	
        delete [] resolvedpath;
        resolvedpath=NULL;
}

void copy_permissions(string &src,string &desc)
{
  struct stat file_info;
  if(stat(src.c_str(),&file_info)==-1)
   {
	   onend("Copy Permissions...Reading Stats");
   }
   chown(desc.c_str(),file_info.st_uid,file_info.st_gid);
   chmod(desc.c_str(),file_info.st_mode);
}

bool search_dir(string &path,string &filename,int type)
{

DIR *dir =opendir(path.c_str());
if(dir == NULL)
{
  
  onend("Error..in..search...no directory found.\n");
}
dirent *current_read;

bool ans=false;
if(type==0)
{
while((current_read=readdir(dir)) != NULL)
{
  string name = current_read->d_name;
  if( current_read->d_type==DT_DIR&&filename==name)
  {
	 
		  ans=true;
		  break;
	 
  }
}
}
else  if(type==1)
{

while((current_read=readdir(dir)) != NULL)
{
  string name = current_read->d_name;
  if( current_read->d_type==DT_REG&&filename==name)
  {
	  
		  ans=true;
		  break;
	  
  }
}
}
else 
{
	while((current_read=readdir(dir)) != NULL)
{
        string name = current_read->d_name;
	  if(filename==name)
	  {
		  ans=true;
		  break;
	  }

}

}
closedir(dir);
return ans;	
}

bool search_dir_recursive_cmd(string &path,string &filename)
{


DIR *dir;
dirent *current_read;
dir = opendir(path.c_str());
char che[3]={0};
if(dir == NULL)
{
  
  onend("Error..in..search.recursive no directory found...\n");
}

while((current_read=readdir(dir)) != NULL)
{
  string name = current_read->d_name;
  if(name=="." )
   continue;
  if( name=="..")
     continue;

 
 schecker(name);
  if(current_read->d_type != DT_DIR)
  {    
       if(name==filename)
	{
	   closedir(dir);
	   return true;
	}
	else
	{
	       checker(che);
		int i;
		for( i=name.size()-1;i>-1;i--)
		{
			if(name[i]=='.')
			break;
		}
		if(name.substr(0,i)==filename)
		{
	      closedir(dir);
	      return true;
	    }
	}	
  
  
  }
  else
  {
        
	if(name==filename)
	{
	   closedir(dir);
	   return true;
	}
	 string newpath=path+'/'+name;
	 schecker(newpath);
	  if(search_dir_recursive_cmd(newpath,filename))
	   {
	   closedir(dir);
	   return true;
	   }
  

	
  }
}

closedir(dir);
return false;

}



void createfilesbuffer(string path,vector<string>&lists)
{

DIR *dir;
dirent *current_read;
dir = opendir(path.c_str());
if(dir == NULL)
{
  onend("Createfilesbuffer....No Dir Found..");
}

while((current_read=readdir(dir)) != NULL)
{
   string par=current_read->d_name;
  if( par== ".." || par==".")
    continue;

  lists.push_back(path+'/'+par);  
 
}
closedir(dir);

}
string get_filename(string &path)
{
	for(int i=path.size()-1;i>-1;i--)
	{
		if(path[i]=='/')
		{
			return path.substr(i);
		}
	}
	schecker(path);
	return path;
}

bool remove_cmd(vector<string>&source)
{
static string output="";

for(auto src:source)
   {  output+= " --1-- "+src;
	  pathresolver(src); 	
	  output+=" --2-- "+src;
	  struct stat file_info;
	  schecker(output);
	  if(stat(src.c_str(),&file_info)==-1)
	  {
		  onend("Remove_cmd...Error in reading file stat"+src+" ..... "+output);
	  }
	  if(!S_ISDIR(file_info.st_mode))
	  {
		 remove(src.c_str()); 
	  }
	  else
	  {
	          vector<string> lists;
		  createfilesbuffer(src,lists);		 
		  if(remove_cmd(lists)==0)
		  return false;
		  else
                 rmdir(src.c_str());
	      
	  }	  
	
   }
   return true;

}

bool remove_file_util(string &source)
{
    string temp=source,fname,parent;
    vector<string> src;
    pathresolver(source);
	fname = get_filename(source);
	parent = source.substr(0,source.size()-fname.size());
	fname=fname.substr(1);
  	if(search_dir(parent,fname,1)==0)
	    return false;
	else{
	schecker(temp);
	src.push_back(temp);
	return remove_cmd(src);
	}
}
bool remove_dir_util(string &source)
{   string temp=source;
    pathresolver(source);
	string fname = get_filename(source);
	string parent = source.substr(0,source.size()-fname.size());
	fname=fname.substr(1);
  	if(search_dir(parent,fname,0)==0)
	return false;
	vector<string> src;
	src.push_back(temp);
	return remove_cmd(src);
}
bool copy_cmd(vector<string>&source,string &destination)
{   

   for(auto src:source)
   {  
	  pathresolver(src);

	  cout<<src<<endl;
	  string newdest=destination+get_filename(src);

	  struct stat file_info;
	  if(stat(src.c_str(),&file_info)==-1)
	  {
		  onend("Copy cmd...Error in reading stat file...");
	  }
	  if(!S_ISDIR(file_info.st_mode))
	  {
	       
	  
	       ifstream  _src(src, ios::binary);
		   if(_src.fail())
		   {
			   return false;
		   }
               ofstream  _dst(newdest, ios::binary);
	       _dst << _src.rdbuf();
		   copy_permissions(src,newdest);
		 
	  }
	  else
	  {
	      vector<string> lists;
		  createfilesbuffer(src,lists);

		  if(mkdir(newdest.c_str(),file_info.st_mode)==-1)
		   return false;
		  copy_permissions(src,newdest);
		 if(copy_cmd(lists,newdest)==false)
		 return false;
	  
	 
	  }	  

	
   }
  return true; 
}

bool move_cmd(vector<string>&source,string &destination)
{
   
   if(copy_cmd(source,destination)!=0)
   return remove_cmd(source);
  
   else
       return false;
}

bool rename_cmd(string &from,string &to)
{
   pathresolver(from);
   int x=get_filename(from).size();
   string dest=from.substr(0,from.size()-x+1)+to;

   if(rename(from.c_str(),dest.c_str())!=-1)
   return true;
   else
   return false;
}


void schecker(string s)
{
    int len=s.length();
    //cout<<s<<" "<<s.length();

}
bool createfile_cmd(string &name,string &destination)
{   

	if(search_dir(destination,name,1))
	return false;
	name=destination+'/'+name;
	struct stat file_info;
	if(stat(destination.c_str(),&file_info)==-1)
	{
        onend("Create File..Error in reading file stat");
	}
	ofstream  _dst(name, ios::binary);
    chmod(name.c_str(),file_info.st_mode);
	return true;
}

bool create_dir_cmd(string &name,string &destination)
{

	if(search_dir(destination,name,0))
	return false;
	
	struct stat file_info;
	if(stat(destination.c_str(),&file_info)==-1)
	{
        onend("Create Dir..Error in reading file stat");
	}
    name=destination+'/'+name;
	if(mkdir(name.c_str(),file_info.st_mode)==-1)
	  return false;
	chmod(name.c_str(),file_info.st_mode);
	return true;

}

 void clear()
{
   printf("\033c");
}
int get_cursor_pos(int &len) {
 
 string str;
 int i;
 char c;
 i=len=c=0;
 write(1, "\033[6n", 4);
 
 while(c != 'R')
 {
    if(read(STDIN_FILENO,&c,1)==-1)
    {
        return 1;
    }
   i++;
   str+=c; 
 }
if(str.length()<2)
   return 1;
for(i=2;str[i]!=';';i++)
{
    len = len*10 ;
    len+=(str[i] - '0');
}
return 0;    
}

bool comp(const vector<string>&a,const vector<string>&b)
{
    bool val=a[5] <b[5];
     return val;
}

void onend(int n)
{
    clear();
    Raw_off();   
    exit(1);
}

void getTerminalHeight(int sig)
{
 clear();
 int worked=(ioctl(STDOUT_FILENO,TIOCGWINSZ,&terminal));
 if(worked==-1)
 {
     printf("Error.....\n");
     exit(1);
 }

 TERMINAL_LEN = terminal.ws_row;
 SCROLL_POINTS=0;
 if(TERMINAL_LEN -2 > 1)
{
	SCR=TERMINAL_LEN -2;
} 
 else{
 	SCR=1;
 
 }

 if(sig==SIGWINCH)
  {
   display();
     
  }
 

}
void Raw_on()
{

    tcflush(STDIN_FILENO, TCIFLUSH);
    int worked=(tcsetattr(STDIN_FILENO,TCSANOW,&raw_one));
    if( worked == -1)
    {
        onend("Raw moode on");
    }
}
void Raw_off()
{
    tcflush(STDIN_FILENO, TCIFLUSH);
    int worked=(tcsetattr(STDIN_FILENO,TCSANOW,&original));
    if(worked==-1)
    {
        cout<<"Raw off Error \n";
        exit(1);
    }
}

void onend(void)
{
    Raw_off();  

}

void onend(string error)
{
    clear();
    Raw_off();
    cout<<"Error is "<<error<<endl;   
    exit(1);
}

void Termios_init()
{
    
    int worked=(tcgetattr(STDIN_FILENO,&original));
    if( worked == -1)
     {
         cout<<"Error \n";
         exitsystem();
      }
      unsigned int siz=sizeof(raw_one);
     bzero(&raw_one,siz);
     raw_one = original;
     raw_one.c_lflag &= ~(ICANON | ECHO);
     siz=sizeof(raw_timeout);
     bzero(&raw_timeout, siz);
     raw_timeout = original;
     raw_timeout.c_lflag &= ~(ICANON | ECHO);
     siz=sizeof(raw_timeout);
     raw_timeout.c_cc[VTIME]=1;
     raw_timeout.c_cc[VMIN]=0;    
}
void exitsystem(){

     exit(1);
}

void createdirbuffer()
{
dirent *current_read;
files_list.clear();
SCROLL_POINTS=0;
CURSOR=1;
char path[]=".";
DIR *dir;
dir = opendir(path);
if(dir == NULL)
{
  onend("No directory Found");
}
string cdir = get_current_dir_name();
struct stat filedetail ;
while((current_read=readdir(dir)) != NULL)
{
   string par=current_read->d_name;
  if(  cdir == rootname && par== ".." )
      continue;


      int worked=stat(current_read->d_name,&filedetail);
      if(worked==-1)
      {
          onend("Failed to get stats from file");
      }
      struct passwd *tpuname = getpwuid(filedetail.st_uid);
      string perm="0123456789";
      struct group *tpgname = getgrgid(filedetail.st_gid);
      vector<string>temp; 
      perm[0]=( (S_ISDIR(filedetail.st_mode)) ? 'd' : '-');
      perm[1]=( (filedetail.st_mode & S_IRUSR) ? 'r' : '-');
      perm[2]=( (filedetail.st_mode & S_IWUSR) ? 'w' : '-');
      perm[3]=( (filedetail.st_mode & S_IXUSR) ? 'x' : '-');
      perm[4]=( (filedetail.st_mode & S_IRGRP) ? 'r' : '-');
      perm[5]=( (filedetail.st_mode & S_IWGRP) ? 'w' : '-');
      perm[6]=( (filedetail.st_mode & S_IXGRP) ? 'x' : '-');
      perm[7]=( (filedetail.st_mode & S_IROTH) ? 'r' : '-');
      perm[8]=( (filedetail.st_mode & S_IWOTH) ? 'w' : '-');
      perm[9]=( (filedetail.st_mode & S_IXOTH) ? 'x' : '-');
      temp.push_back(perm);
      temp.push_back(tpuname->pw_name);
      temp.push_back(tpgname->gr_name);    
      string t;
      temp.push_back(to_string(filedetail.st_size));
      t=ctime(&(filedetail.st_mtim.tv_sec));
      t.pop_back();
      temp.push_back(t);
      t=current_read->d_name;
      temp.push_back(t);  
      files_list.push_back(temp);  

  
}
closedir(dir);
FILES_LEN = files_list.size();
CURSOR=1;
sort(files_list.begin(),files_list.end(),comp);
}
void checker(char arr[])
{
   int a=10;
   a+=10;
  //cout<<"working"<<arr[0];

}
void forwardmotion()
{
 if(forwardstack.empty())
  return;
 unsigned int siz=forwardstack.top().size()+1;
 backstack.push(get_current_dir_name());
 char ar[siz];
 strcpy(ar,forwardstack.top().c_str());
 forwardstack.pop();
 checker(ar);
 chdir(ar);
 createdirbuffer();
 display();
}

void display()
{
 clear();
 
 pagesize=SCR;
 if(FILES_LEN<pagesize+SCROLL_POINTS )
 {
     pagesize = FILES_LEN - SCROLL_POINTS;
 }
 
 CURSOR=pagesize+1;
 int i=SCROLL_POINTS;
 while(i < SCROLL_POINTS+pagesize)
 {
    int len=files_list[i].size(),j=0;
   cout<<i+1<<".\t";
    for(auto &el:files_list[i])
    {
        if(j!=len-1)
          cout<<el<<'\t';
        else{
        if(el.length()>15)
        {
            string str=el.substr(0,15);
            cout<<str<<"...";
    
        }
        else{
        cout<<el<<'\t';

        
        }
          }
        j++;
    }
    cout<<endl;
    i++;
 }
 
 return;
}

void backmotion()
{
 if(backstack.empty())
  return;
 unsigned int siz=backstack.top().size()+1;
 char ar[siz];
 forwardstack.push(get_current_dir_name());
 strcpy(ar,backstack.top().c_str());
 backstack.pop();
 checker(ar);
 chdir(ar);
 createdirbuffer();
 display();
}

void gotohome()
{
    if(get_current_dir_name() == rootname)
       return;
    string temp = get_current_dir_name();
    stack<string>s;
    backstack.push(temp);
    forwardstack.swap(s);
    chdir(rootname.c_str());
    createdirbuffer();
    display();
}

bool goto_cmd(string &path)
{
    string temp ;
    pathresolver(path);
    temp=get_current_dir_name();
    if(temp == path)
        return true;
    else if(path.size()<rootname.size())
    return false;   
    
    else if(chdir(path.c_str())==-1)
    return false;

    backstack.push(temp);
    string t;
    if(forwardstack.size()!=0)
    {
    stack<string>s;
    forwardstack.swap(s);
    }
    createdirbuffer();
    display();
    return true;
}


void gotoparent()
{
    stack<string>s;
    string temp ;
    temp=get_current_dir_name(); 
    forwardstack.swap(s);
    string str="";
    if(files_list[1][5]=="..")
    {
      backstack.push(temp);
      chdir("..");
      createdirbuffer();
      display();
    }
    
}

vector<string> parse(string &input)
{
  
  vector<string> args;
  int i;
  string str="";
  for(i=0;i<input.size();i++)
  {
    if(input[i]==32)
    {
        while(i<input.size() && input[i]==32)
        i++;
        
    }
    string temp="";
    while(i<input.size() && input[i]!=' '&&str.size()==0)
    {
        temp+=input[i];
        i++;
    }
    i--;//exceeded
    unsigned int siz=temp.size();
    if(siz!=0)
    args.push_back(temp);
  }

 return args;
  
  
  }



string parsecommand(vector<string>&args)
{
   unsigned int siz=args.size();
   if(siz<2)
   return "INVALID COMMAND OR ARGUMENTS...";
   string cmd=args[0];
   string destination=args[siz-1];
   pathresolver(destination);
   args.erase(args.begin()); 
  
 if(cmd=="copy")
 {
   args.pop_back();
   siz=args.size();
   if(siz!=0)
   {
       if(copy_cmd(args,destination))
       return "Copied";
       else
       return "Failed";
   }
   return "Invalid arguments";
 }
 else if(cmd=="move")
 {
   args.pop_back();
   siz=args.size();
   if(siz!=0)
   {
       if(move_cmd(args,destination))   
       return "Moved";
       else
       return "Failed";
   }
   return "Invalid arguments";
 }
 else if(cmd=="rename")
 { 
   siz=args.size();
   
   if(siz==2)
   {
        if(rename_cmd(args[0],args[1]))
        return "Renamed";
        else
        return "Failed";
   }
   
   return "Invalid arguments";
 }
 else if(cmd=="create_file")
 {
   siz=args.size();
   if(siz==2)
   {
        if(createfile_cmd(args[0],destination))
        return "Success";
        else
        return "Failed";
   }
   else if(siz==1)
   {
        destination=get_current_dir_name();
        if(createfile_cmd(args[0],destination))
        return "Success";
        else
        return "Failed";
   }
   
   return "Invalid Arguments";
 }
 else if(cmd=="create_dir")
 {
   siz=args.size();
   if(siz==1)
   {
        destination = get_current_dir_name();
        if(create_dir_cmd(args[0],destination))
        return "Success";
        else
        return "Failed";
   }
   return "Invalid Arguments";
 }
 else if(cmd=="delete_file")
 {  
   siz=args.size();
   if(siz==1)
   {  
        if(remove_file_util(args[0]))
        return "Removed";
        else
        return "Failed";
   }
   return "Invalid Arguments";
 }
 else if(cmd=="delete_dir")
 {
   siz=args.size();
   if(siz==1)
   {
        if(remove_dir_util(args[0]))
        return "Removed";
        else
        return "Failed";
   }
   return "Invalid Arguments";
 }
 else if(cmd=="goto")
 { 
   siz=args.size();
   if(siz>=2)
   return "Invalid Arguments";

   string temp;
   if(goto_cmd(args[0]))
   return "DONE";
   else
   return "INVALID PATH";
 }
 else if(cmd=="search")
 {
   siz=args.size();
   if(siz>=2)
    return "Invalid Arguments";

    string currentpath=get_current_dir_name();
   if(search_dir_recursive_cmd(currentpath,args[0]))
    return "True";

    return "False";   
    
 }
 else
return "INVALID COMMAND OR ARGUMENTS...";
}

string cmodecursor()
{
char c[4]={0};

    string command;

while (true)
{
  string temp="";
  read(STDIN_FILENO, &c, 3);


  if(c[0]==10)
  {
    
     return command;
  }
  if(iscntrl(c[0])){
  checker(c);
  if(c[0]==127)
  {
      unsigned int siz=command.size();
      if(siz!=0)
      {
       command.pop_back();
       char ar[]="\b \b";
       write(STDOUT_FILENO,&ar,3);
      }
  }    
  if(c[0]==27)
  {  
      
         
     if(c[1]==c[2] && c[1]=='\0')   
     return "00";
   
  }
  
  }else
  {
     
      if(c[0]=='q'||c[0]=='Q')
     {  
      
         
        if(c[1]==c[2] && c[1]=='\0')   
        return "01";
   
       }
      command+=c[0];
      write(STDOUT_FILENO,&c,1);
      
  }
  
  
}


 




}





void cursor()
{  

char c[4]={0};
int c_Y;

while (true)
{
  c[1]=0;
  c[2]=0;
  read(STDIN_FILENO,&c,3);
  int flag=0;
  if(c[0]==10)
  {
     if(CURSOR < pagesize+1)
        flag=1;
  }
  if(flag==1)
  {
    return;
}
  if(c[0]==27)
  {  
     
     
     if(c[1]==91)//escape
     {
         get_cursor_pos(c_Y);
         if(c[2]==65){
                     if(c_Y>pagesize+1)
                     {
                      char ab[]="\x1b[A";
                      checker(ab);
                      write(STDOUT_FILENO,&ab,sizeof(ab)); 
                     }
                     if(CURSOR > 1)
                     {                            
                         CURSOR--;                    
                      
                      char ab[]="\x1b[A";
                      checker(ab);
                      write(STDOUT_FILENO,&ab,sizeof(ab));                          
                     }
                        
               }

         else if(c[2]==66)
         { 
                  if(CURSOR < pagesize+1)
                    {

                    CURSOR++;    
                   
                    char ab[]="\x1b[B";
                    checker(ab);
                      write(STDOUT_FILENO,&ab,sizeof(ab)); 

                    }
          }     
         else if(c[2]==67)
         { 
             forwardmotion();
            
         }

         else if(c[2]==68) 
         {
            backmotion();
           
         }
         
         
         
     }
  }



  if(c[0]=='k' || c[0]=='K')
  {
   if(SCROLL_POINTS>=SCR)
   SCROLL_POINTS-=SCR;
   display();
   continue;
  }

  if(c[0]=='l'|| c[0]=='L')
  {
    if(SCROLL_POINTS+SCR<=FILES_LEN)
    SCROLL_POINTS+=SCR;   
    display();
    continue;
  }
    

  if(c[0]=='h' || c[0]=='H')
  {
      gotohome();
      continue;
  }
   if(c[0]== 127)
  {
      gotoparent();
      continue;
  }

  if(c[0]==':')
  {
      commandmode();
      continue;
   
    
  }

}

}

void commandmode()
{
  while(true)
  {
  printf("\x1b[%d;1HEnter your Command:\n",TERMINAL_LEN-1);
  char arr[]=">>> ";
  unsigned int siz=sizeof(arr);
  string inp="";
  write(STDOUT_FILENO,&arr,siz);
  inp=cmodecursor();
  int flag=0;
  if(inp=="01")
  {
     exit(1);
  
  }
   if(inp=="00")
   {
    display();
    
   flag=1;
   }
   if(flag==1)
   {
   	return;
   }
   
   
   siz=inp.size();
   vector<string> args;
   if(siz==0)
   continue;
   
  
   args=parse(inp);
   string res;
   res=parsecommand(args);
   createdirbuffer();//reload
   display();
   printf("\x1b[%d;%dH Result:~ ",TERMINAL_LEN-1,25);
   cout<<res<<endl;
  }
}


int main()
{
    
    Termios_init();

 printf( "\x1b[8;999;999t" );

signal(SIGINT,onend);
signal(SIGWINCH,getTerminalHeight);
getTerminalHeight(0);
rootname=get_current_dir_name();
Raw_on();
string temp;
while(true)
{

start:
createdirbuffer();
display();
cursor();
unsigned int x=SCROLL_POINTS+CURSOR-1;
string temp = files_list[x][5];


string histdir=get_current_dir_name();
if(chdir(temp.c_str())==-1)
 {
 
    int pid = fork();
    
    if (pid == 0) 
    {
        execl("/usr/bin/xdg-open", "xdg-open",temp.c_str(), (char *)0);
        Raw_off();
        exit(1);
    }
     }
 else
 {
     backstack.push(histdir);

 
 }
 
}
Raw_off();
return 0;
}



