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

char take_input(){
        HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);

    DWORD originalMode;
    GetConsoleMode(hIn, &originalMode);

    DWORD rawMode = originalMode;
    rawMode &= ~ENABLE_LINE_INPUT;   // don't wait for Enter
    rawMode &= ~ENABLE_ECHO_INPUT;   // don't echo typed characters
    SetConsoleMode(hIn, rawMode);

    std::cout << "Press any key...\n";

    char c;
    DWORD read;
    ReadConsole(hIn, &c, 1, &read, NULL);

    std::cout << "\nYou pressed: '" << c << "' (0x"
               << std::hex << (int)(unsigned char)c << ")\n";

    SetConsoleMode(hIn, originalMode); // restore

    return c;
}


std::optional<fs::directory_entry> navigation(fs::path my_path){
    char assigned_letters[26] = {'a','s','d','f','g','h','j','k','l','q','w','e','r','t','y','u','i','o','p','z','x','c','v','b','n','m'};
    int count = 0;
    std::vector<fs::directory_entry> files;

    for(const auto & entry : fs::directory_iterator(my_path)){
        std::cout << entry.path().filename()<< "\t";
        if (entry.is_regular_file()){
            std::cout << "Size: "<<(fs::file_size(entry))/1000 << "KB Last Write Time: ";
            auto ftime = fs::last_write_time(entry);
            time_t cftime = last_write_time(ftime);
            std::cout << std::asctime(std::localtime(&cftime));
                
        }
        else{
            std::cout << "Directory";
            }
        std::cout << ", " << assigned_letters[count];
        count++;
        std::cout << std::endl;
        files.push_back(entry);
    }

        char chosen = take_input();
        int index = -1;
   
        
        for (int i = 0; i<26;i++){
            if (chosen == assigned_letters[i]){
                index = i;
            }
        }
        
        if (index >= 0 && index < static_cast<int>(files.size())) {
            std::cout << "You chose " << files[index] << ".\n";
            return files[index];
        } 
        else if (chosen == '/'){
            return std::nullopt;   // <-- missing semicolon fixed too
        }
        else if (chosen == ','){
            fs::directory_entry parent_entry{my_path.parent_path()};
            return parent_entry;
        }
        else {
            std::cout << "Invalid selection.\n";
            return std::nullopt;   // treat "invalid" as stop too, rather than an empty entry
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
    std::string input;
    std::cout << "Enter a path: ";
    std::getline(std::cin,input);
    fs::path my_path = input;

    if(check_path(input)){
        std::optional<fs::directory_entry> result = navigation(my_path);
        
        while(result.has_value()){
            if(result->is_directory()){
                result = navigation(result->path());
            }
            else if(result->is_regular_file()){
                open_file(result->path());
            }
        }
    }
    return 0;
}

