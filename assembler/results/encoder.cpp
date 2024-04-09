#include <iostream>
#include <fstream>

using namespace std;

int main(int argC, char** argV){
    if(argC < 3){
        cout << "Usage: " << argV[0] << " <input_text_file> <output_filename>\n";
        return 2;
    }
    
    ifstream input(argV[1]);
    if(!input.is_open()){
        return 1;
    }
    
    ofstream output(argV[2], ifstream::binary);
    if(!input.is_open()){
        return 1;
    }
    
    unsigned int value = 0;
    while(input >> value){
        output.write((char*)&value, 1);
    }
    
    input.close();
    output.close();
    
    return 0;
}
