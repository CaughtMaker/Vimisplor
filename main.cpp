#include <iostream>
#include <string>
#include <filesystem>
#include <windows.h>
#include <vector>
#include <optional>
#include <shellapi.h>


namespace fs = std::filesystem;

bool check_path(fs::path my_path){

    if(fs::exists(my_path)){
        //std:: cout << "Found";
        return true;
    }
    else {
        //std:: cout << "Cannot Find";
        return false;
    }
}

time_t last_write_time(auto p){
    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        p - fs::file_time_type::clock::now() + std::chrono::system_clock::now()
    );

    std::time_t cftime = std::chrono::system_clock::to_time_t(sctp);

    return cftime;
}

std::size_t count_entries(const fs::path& p){
    return std::distance(fs::directory_iterator(p), fs::directory_iterator{});
}

int page_count(fs::directory_entry folder){
    int entry_number = count_entries(folder.path());
    int result = (entry_number + 10 - 1) / 10;
    return result;
}

char take_input(){
        HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);

    DWORD originalMode;
    GetConsoleMode(hIn, &originalMode);

    DWORD rawMode = originalMode;
    rawMode &= ~ENABLE_LINE_INPUT;   // don't wait for Enter
    rawMode &= ~ENABLE_ECHO_INPUT;   // don't echo typed characters
    SetConsoleMode(hIn, rawMode);



    char c;
    DWORD read;
    ReadConsole(hIn, &c, 1, &read, NULL);
    

    SetConsoleMode(hIn, originalMode); // restore

    return c;
}

std::vector<fs::directory_entry> search_result(std::string target, fs::path path){
    std::vector<fs::directory_entry> matching_values;

    for(const auto & entry : fs::directory_iterator(path)){
        std::string name = entry.path().filename().string();
        for (int i = 0; i<name.length(); i++){
            if (i+target.length() >= name.length()){
                break;
            }
            //'std::cout << name.substr(i,i+(target.length()-1)) << std::endl;
            if (name.substr(i,target.length()) == target){
                matching_values.push_back(entry);
                break;
            }
        }
    }

    return matching_values;
}

std::optional<fs::directory_entry> search (fs::path path){
    std::string name;
    std::cout << "\033[2J\033[H";
    std::cout << "***********************************************************************************\n";
    for (int i = 0; i<15;i++){
        std::cout<<std::endl;
    }
    std::cout << "***********************************************************************************\n";
    std::cout<< "\n"<<"\033[32m"<<"Enter a key word:"<<"\033[0m";
    std::vector<fs::directory_entry> x;
    while(true){
        char c = take_input();
        
        if (c == '\''){
            break;
        }
        if (c == '\b'){
            name.pop_back();
        }
        else{
            name.push_back(c);
        }
        x = search_result(name,path);

        std::cout << "\033[2J\033[H";
        std::cout << "***********************************************************************************\n";
        for (int i = 0; i<x.size();i++){
            std::cout<< x[i].path().filename() << std::endl;
        }
        int remainder = 0;
        if (x.size() < 15){
            remainder = 15 - x.size();
        }
        for(int i = 0; i<remainder; i++){
            std::cout<< std:: endl;
        }
        std::cout << "***********************************************************************************\n";
        std::cout <<"\033[34m"<<std::endl<< "Press [\'] to enter" << std::endl;
        std::cout << "\033[32m"<< "Enter a key word: "<<"\033[0m";
        std::cout<< " " << name;
    }

    char assigned_letters[26] = {'a','s','d','f','g','h','j','k','l','q','w','e','r','t','y','u','i','o','p','z','x','c','v','b','n','m'};
    int count = 0;


    int current_page = 0;
    int page_numbers = (x.size() + 10 - 1) / 10;
    const int limit = 15;

    while(true){
        std::cout << "\033[2J\033[H";
        std::cout << "***********************************************************************************\n";

        int start = current_page * limit;
        int end = std::min(start + limit, static_cast<int>(x.size()));
        count = 0;
        for (int i = start; i<end;i++){
            std::cout <<"["<< "\033[32m" << assigned_letters[i] <<"\033[0m"<<"]";

            if (x[i].is_directory()){
               std::cout << "\033[33m" << x[i].path().filename() << std::endl <<"\033[0m";
            }
            else{
                std::cout << x[i].path().filename();
            }

            std::cout << std::endl;
        }
        int remainder = 0;
        if (count < 15){
            remainder = 15 - count;
        }
        for(int i = 0; i<remainder; i++){
            std::cout<< std:: endl;
        }

        std::cout << "***********************************************************************************\n";
        std::cout << "Page " << (current_page + 1) << "/" <<page_numbers<< "\n";
        std::cout <<"\033[34m" << "[.] next page  [,] prev page [;] prev folder [/] quit\n" <<"\033[0m";
    
        std::cout <<"Press any key... ";
        char chosen = take_input();
        int index = -1;
   
        
        for (int i = 0; i<26;i++){
            if (chosen == assigned_letters[i]){
                index = i;
            }
        }
        
        int global_index = start+index;
        
        if (index >= 0 && global_index < end) {
            std::cout << "You chose " << x[global_index] << ".\n";
            return x[global_index];
        } 
        else if (chosen == '.'){

            if (current_page + 1 < page_numbers) current_page++;
        }

        else if (chosen == ','){
            if (current_page > 0) current_page--;
        }
        else if (chosen == '/'){
            fs::directory_entry y{path};
            return y;   // <-- missing semicolon fixed too
        }

        else {
            std::cout << "Invalid selection.\n";
            
            return std::nullopt;   
        }
    }



}

fs::path custom_path(){
    std::cout << "\nEnter a path: ";
    std::string path;
    std::getline(std::cin, path);

    
    return fs::path{path}; 
}


std::optional<fs::directory_entry> navigation(fs::path my_path){
    std::cout << "\033[2J\033[H";
    
    char assigned_letters[26] = {'a','s','d','f','g','h','j','k','l','q','w','e','r','t','y','u','i','o','p','z','x','c','v','b','n','m'};
    int count = 0;
    std::vector<fs::directory_entry> files;

    int page_numbers = page_count(fs::directory_entry(my_path));
    int current_page = 0;

    const int limit = 15;
    for(const auto & entry : fs::directory_iterator(my_path)){
        files.push_back(entry);
    }

    while(true){
        std::cout << "\033[2J\033[H";
        std::cout << "***********************************************************************************\n";

        int start = current_page * limit;
        int end = std::min(start + limit, static_cast<int>(files.size()));
        count = 0;
        for (int i = start; i<end; ++i){

            const auto& entry = files[i];
            int letter_index = i-start;

            std::cout<<"\033[0m"<< "[" << "\033[32m" << assigned_letters[count] <<"\033[0m"<<"]" << "   ";
            if (entry.is_regular_file()){
                int l = 20;
                std::string name = entry.path().filename().string();
                if (name.length() > l){
                    int remainder = name.length() - l;
                    for(int i = 0; i < remainder+3; i++){
                        name.pop_back();
                    }
                    std::cout << name;
                    std::cout << "...";
                }
                else{
                    int remainder =  l - name.length();
                    std::cout<<name;
                    for(int i = 0; i < remainder; i++){
                        std::cout<<" ";
                    }

                }
                std::cout << "\tSize: "<<(fs::file_size(entry))/1000 << "KB Last Write Time: ";
                auto ftime = fs::last_write_time(entry);
                time_t cftime = last_write_time(ftime);
                std::string ts = std::asctime(std::localtime(&cftime));
                ts.pop_back();
                std::cout << ts;
            }
            else{
                int l = 20;
                std::string name = entry.path().filename().string();
                std::cout<<"\033[33m";
                if (name.length() > l){
                    int remainder = name.length() - l;
                    for(int i = 0; i < remainder+3; i++){
                        name.pop_back();
                    }
                    std::cout << name;
                    std::cout << "...";
                }
                else{
                    int remainder = l - name.length();
                    std::cout<<name;
                    for(int i = 0; i < remainder; i++){
                        std::cout<<" ";
                    }

                }
                //std::cout << "\033[33m" << entry.path().filename()<< "\t";
                std::cout << "\tDirectory";
                }
        
            count++;
            std::cout << "\033[0m"<<std::endl;
        }
        int remainder = 0;
        if (count < 15){
            remainder = 15 - count;
        }
        for(int i = 0; i<remainder; i++){
            std::cout<< std:: endl;
        }

        std::cout << "***********************************************************************************\n";
        std::cout << "Page " << (current_page + 1) << "/" << page_numbers << "\n";
        std::cout <<"\033[34m" << "[.] next page  [,] prev page [;] prev folder [/] quit ['] search [`] custom path\n" <<"\033[0m";

        

        std::cout <<"Press any key... ";
        char chosen = take_input();
        int index = -1;
   
        
        for (int i = 0; i<26;i++){
            if (chosen == assigned_letters[i]){
                index = i;
            }
        }
        
        int global_index = start+index;
        
        if (index >= 0 && global_index < end) {
            std::cout << "You chose " << files[global_index] << ".\n";
            return files[global_index];
        } 
        else if (chosen == '.'){
            if (current_page + 1 < page_numbers) current_page++;
        }
        else if (chosen == '\''){
            return(search(my_path));

        }
        else if (chosen == ','){
            if (current_page > 0) current_page--;
        }
        else if (chosen == '/'){
            
            return std::nullopt;   // <-- missing semicolon fixed too
        }
        else if (chosen == ';'){
            fs::directory_entry parent_entry{my_path.parent_path()};
           
            return parent_entry;
        }
        else if (chosen == '`'){
            fs::path custom_p = custom_path();

            if (check_path(custom_p)){
                fs::directory_entry x(custom_p);
                return x;
            }
            else{
                fs::directory_entry current_spot(my_path);
                return current_spot;
            }
            
        }
        else {
            std::cout << "Invalid selection.\n";
            
            fs::directory_entry current_spot(my_path);
            return current_spot;
        }
    }
}


void open_file(const fs::path& p){
    ShellExecuteW(
        NULL,                   // no parent window
        L"open",                // verb: open with default associated app
        p.wstring().c_str(),    // file path (wide string)
        NULL,                   // no parameters
        NULL,                   // default working directory
        SW_SHOWNORMAL           // show the app window normally
    );
}

int main() {
    std::string input = "C:\\Users\\Admins\\Desktop";
    
    fs::path my_path = input;

    

    if(check_path(input)){
        std::optional<fs::directory_entry> result = navigation(my_path);
        
        while(result.has_value()){
            if(result->is_directory()){
                result = navigation(result->path());
            }
            else if(result->is_regular_file()){
                std::cout << "\nAre you sure you want to open "<<result->path().filename() << "[" << "\033[32m" <<"Y"<<"\033[0m"<<"/"<< "\033[32m" <<"N"<< "\033[0m" << "]\n";
                char x = take_input();
                if (x == 'y'){
                    open_file(result->path());
                }
                else{
                    result = navigation(result->path().parent_path());
                }
                break;
            }
        }
    }

    return 0;
}

